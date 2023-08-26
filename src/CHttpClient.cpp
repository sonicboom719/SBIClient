//////////////////////////////////////////////////////////////////////////////
//	CHttpClient クラス
//		HTTP(S)で要求送信と応答受信を行うクラス
//////////////////////////////////////////////////////////////////////////////

#pragma comment(lib, "wininet.lib")

#include "CHttpClient.h"
#include <windows.h>
#include <wininet.h>
#include <stdio.h>

using namespace std;

//////////////////////////////////////////////////////////////////////////////
//	CCGIParams クラス定義

CHttpClient::CCGIParams::CCGIParams()
{
}

CHttpClient::CCGIParams::~CCGIParams()
{
}

string CHttpClient::CCGIParams::Get( const string& name ) const
{
	const_iterator iter;
	for (iter = begin(); iter != end(); iter++)
		if (iter->first == name)
			return iter->second;
	return "";
}

void CHttpClient::CCGIParams::Add( const std::string& name, const std::string& value )
{
	push_back( pair<string, string>(name, value) );
}

void CHttpClient::CCGIParams::Set( const std::string& name, const std::string& value )
{
	//	既に同名のパラメータがあれば上書きする
	CCGIParams::iterator iter;
	for (iter = begin(); iter != end(); iter++) {
		if (iter->first == name) {
			iter->second = value;
			return;
		}
	}
	//	なければ新たに追加
	Add( name, value );
}

void CHttpClient::CCGIParams::Dump()
{
	const_iterator iter;
	for (iter = begin(); iter != end(); iter++)
		printf("%s=%s\n", iter->first.c_str(), iter->second.c_str() );
}

//////////////////////////////////////////////////////////////////////////////
//	CHttpClient 構築と破棄

CHttpClient::CHttpClient() :
	m_userAgent("CHttpClient"),
	m_retryCount(1),
	m_retryInterval(3000)
{
}

CHttpClient::~CHttpClient()
{
}

//////////////////////////////////////////////////////////////////////////////
//	CHttpClient 操作

int CHttpClient::MethodGet( const char* url, CBuffer* buffer )
{
	int result;

	for (unsigned int count = 0; count < m_retryCount; count++)
	{
		result = Request( url, NULL, NULL, buffer );
		if (!IsRetryableCode( result )) break;
		Sleep( m_retryInterval );
	}

	return result;
}

int CHttpClient::MethodPost( const char* url, const CHttpClient::CHeaders* headers, const CHttpClient::CCGIParams& params, CBuffer* buffer )
{
	int result;

	for (unsigned int count = 0; count < m_retryCount; count++)
	{
		result = Request( url, headers, &params, buffer );
		if (!IsRetryableCode( result )) break;
		Sleep( m_retryInterval );
	}

	return result;
}

int CHttpClient::Request( const char* url, const CHttpClient::CHeaders* headers, const CHttpClient::CCGIParams* params, CBuffer* buffer )
{
	HINTERNET hInet, hConn, hReq;
	DWORD dwSize;
	BOOL bret;
	URLPARTS url_parts;
	char databuf[1024];
	string entity;
	LPVOID entityPtr;
	int entityLength;
	void* stream;
	vector<string>::const_iterator iter;

	//	URLを分解する
	if (!ParseUrl( url, &url_parts ))
		return RET_PARSE_URL;

    // Internetに接続する
	hInet = InternetOpen( 
				m_userAgent.c_str(),
				INTERNET_OPEN_TYPE_PRECONFIG, // レジストリの設定に従う
				NULL,
				NULL,
				0 );
	
	if (hInet == NULL)
        return RET_INET_OPEN;

	// HTTPサーバーに接続
	hConn = InternetConnect(
				hInet,
				url_parts.host.c_str(),
				url_parts.port,
				url_parts.user.c_str(),
				url_parts.password.c_str(),
				INTERNET_SERVICE_HTTP,
				0,
				NULL );

    if (hConn == NULL)
	{
		InternetCloseHandle( hInet);
		return RET_INET_CONNECT;
	}

	// サーバー上で欲しいURLを指定する
	hReq = HttpOpenRequest(
				hConn,
				(params == NULL) ? "GET" : "POST",
				(url_parts.path + url_parts.extra).c_str(),
				NULL,	// デフォルトのHTTPバージョン
				NULL,	// 履歴を追加しない
				NULL,	//	Accept-Typeは text/* 
				url_parts.ssl ? INTERNET_FLAG_SECURE : 0,
				NULL );

	if (hReq == NULL)
	{
		InternetCloseHandle( hConn );
		InternetCloseHandle( hInet );
        return RET_OPEN_REQUEST;
    }

	//	POSTなら引数リストからURLコーディングされたエンティティの文字列を作成する
	if (params == NULL)
	{
		//	GET
		entityPtr = NULL;
		entityLength = 0;
	}
	else
	{
		//	POST
		if (!HttpAddRequestHeaders( hReq, "Content-Type: application/x-www-form-urlencoded\r\n",
									-1, HTTP_ADDREQ_FLAG_ADD_IF_NEW ))
		{
			InternetCloseHandle( hReq );
		    InternetCloseHandle( hConn );
			InternetCloseHandle( hInet );
			return RET_ADD_HEADERS;
		}

		entity = URLEncodeParams( *params );
		entityPtr = (LPVOID)entity.c_str();
		entityLength = entity.size();
	}

	//	追加ヘッダーがあれば設定する
	if (headers != NULL)
	{
		for (iter = headers->begin(); iter != headers->end(); iter++)
		{
			if (!HttpAddRequestHeaders( hReq, iter->c_str(),
											-1, 
											HTTP_ADDREQ_FLAG_ADD_IF_NEW ))
			{
				InternetCloseHandle( hReq );
				InternetCloseHandle( hConn );
				InternetCloseHandle( hInet );
				return RET_ADD_HEADERS;
			}
		}
	}

	//	ヘッダーを要求する
	if (HttpSendRequest( hReq, NULL, 0, entityPtr, entityLength) != TRUE)
	{
		InternetCloseHandle( hReq );
        InternetCloseHandle( hConn );
		InternetCloseHandle( hInet );
        return RET_SEND_REQUEST;
    }
	
	//	出力ファイルを開く
	if ((stream = buffer->OpenWrite()) == NULL)
	{
		InternetCloseHandle( hReq );
		InternetCloseHandle( hConn );
		InternetCloseHandle( hInet );
		return RET_CANNOT_OPEN_FILE;
	}

	// ファイルの内容を全て受信する
	do {
		bret = InternetReadFile( hReq, databuf, sizeof(databuf), &dwSize );
		if (bret == TRUE)
			buffer->Write( stream, (unsigned char*)databuf, dwSize );
	} while (bret == TRUE && dwSize > 0);

	buffer->Close( stream );

	InternetCloseHandle( hReq );
	InternetCloseHandle( hConn );
	InternetCloseHandle( hInet );

	return RET_OK;
}

//////////////////////////////////////////////////////////////////////////////
//	CHttpClient ヘルパー関数

bool CHttpClient::ParseUrl( const char* url, CHttpClient::URLPARTS* parts )
{
	URL_COMPONENTS	compo;
	char szScheme[256], szHostName[256], szUserName[256];
	char szPassword[256], szUrlPath[256], szExtraInfo[2048];

	//	InternetCrackUrlのコール前に領域確保する
	compo.dwStructSize = sizeof(URL_COMPONENTSA);
    compo.nScheme = INTERNET_SCHEME_DEFAULT;
    compo.lpszScheme = szScheme;
	compo.dwSchemeLength = sizeof(szScheme);
    compo.lpszHostName = szHostName;
	compo.dwHostNameLength = sizeof(szHostName);
	compo.lpszUserName = szUserName;
	compo.dwUserNameLength = sizeof(szUserName);
	compo.lpszPassword = szPassword;
	compo.dwPasswordLength = sizeof(szPassword);
	compo.lpszUrlPath = szUrlPath;
	compo.dwUrlPathLength = sizeof(szUrlPath);
	compo.lpszExtraInfo = szExtraInfo;
	compo.dwExtraInfoLength = sizeof(szExtraInfo);

	if (!InternetCrackUrl( url, strlen(url), ICU_DECODE, &compo ))
		return false;

	//	スキームをチェックhttpとhttps以外はサポート外
	if (_stricmp(compo.lpszScheme, "http") == 0)
		parts->ssl = false;
	else if (_stricmp(compo.lpszScheme, "https") == 0)
		parts->ssl = true;
	else
		return false;

	parts->user = compo.lpszUserName;
	parts->password = compo.lpszPassword;
	parts->host = compo.lpszHostName;
	parts->port = compo.nPort;
	parts->path = compo.lpszUrlPath;
	parts->extra = compo.lpszExtraInfo;

	//	extraに日本語等の％エンコードがあるとInternetCrackUrlで日本語(SJIS)
	//	に変更されてしまい都合が悪いのでextraはそのまま使う 2017.3.17
	if (strlen(compo.lpszExtraInfo))
		parts->extra = strchr(url, '?');

	return true;
}

//////////////////////////////////////////////////////////////////////////////
//	CHttpClient 操作(静的)

string CHttpClient::URLEncodeParams( const CCGIParams& params )
{
	string result;

	//	引数リストを'&'で全て連結した文字列を作成する
	CCGIParams::const_iterator iter;
	for (iter = params.begin(); iter != params.end(); iter++) {
		if (iter != params.begin()) result += '&';
		result += URLEncode( iter->first );
		result += '=';
		result += URLEncode( iter->second );
	}

	return result;
}

string CHttpClient::URLEncode( const string& src )
{
	string result;

	string::const_iterator iter;
	for (iter = src.begin(); iter != src.end(); iter++) {
		if (*iter == ' ')
			result += '+';
		else if (isalnum((unsigned char)*iter) || strchr("*-.@_", *iter) != NULL)
			result += *iter;
		else {
			char buf[10];
			sprintf( buf, "%%%02X", (unsigned char)*iter );
			result += buf;
		}
	}

	return result;
}

string CHttpClient::URLDecode( const string& src )
{
	string result;
	int	status = 0;
	string hexStr;
	string::const_iterator iter;

	for (iter = src.begin(); iter != src.end(); iter++)
	{
		switch (status) {
		case 0:
			switch (*iter) {
			case '%': status = 1; break;
			case '+': result += ' '; break;
			default: result += *iter;
			}
			break;
		case 1:
			if (isxdigit( *iter )) {
				hexStr = *iter;
				status = 2;
			}	//	16進文字以外なら本来はエラーだが無視します
			break;
		case 2:
			if (isxdigit( *iter )) {
				int c;
				hexStr += *iter;
				sscanf( hexStr.c_str(), "%2x", &c );
				result += (char)c;
				status = 0;
			}	//	16進文字以外なら本来はエラーだが無視します
			break;
		}
	}

	return result;
}

void CHttpClient::URLDecodeParams( const string& src, CCGIParams* params )
{
	int	status = 0;
	string param_name, param_value;
	string::const_iterator iter;

	params->clear();

	for (iter = src.begin(); iter != src.end(); iter++)
	{
		switch (status) {
		case 0:
			if (*iter == '?')
				status = 1;
			break;
		case 1:
			if (*iter == '=') {
				param_value.clear();
				status = 2;
			} else if (*iter == '&')
				param_name.clear();
			else
				param_name += *iter;
			break;
		case 2:
			if (*iter == '&') {
				params->Add( URLDecode(param_name), URLDecode(param_value) );
				param_name.clear();
				status = 1;
			}
			else
				param_value += *iter;
			break;
		}
	}

	if (status == 2)
		params->Add( URLDecode(param_name), URLDecode(param_value) );
}

bool CHttpClient::IsRetryableCode( int retcode )
{
	switch (retcode) 
	{
	case RET_INET_CONNECT:
	case RET_SEND_REQUEST:
		return true;
	default:
		return false;
	}
}