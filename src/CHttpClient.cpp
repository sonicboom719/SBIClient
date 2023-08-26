//////////////////////////////////////////////////////////////////////////////
//	CHttpClient �N���X
//		HTTP(S)�ŗv�����M�Ɖ�����M���s���N���X
//////////////////////////////////////////////////////////////////////////////

#pragma comment(lib, "wininet.lib")

#include "CHttpClient.h"
#include <windows.h>
#include <wininet.h>
#include <stdio.h>

using namespace std;

//////////////////////////////////////////////////////////////////////////////
//	CCGIParams �N���X��`

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
	//	���ɓ����̃p�����[�^������Ώ㏑������
	CCGIParams::iterator iter;
	for (iter = begin(); iter != end(); iter++) {
		if (iter->first == name) {
			iter->second = value;
			return;
		}
	}
	//	�Ȃ���ΐV���ɒǉ�
	Add( name, value );
}

void CHttpClient::CCGIParams::Dump()
{
	const_iterator iter;
	for (iter = begin(); iter != end(); iter++)
		printf("%s=%s\n", iter->first.c_str(), iter->second.c_str() );
}

//////////////////////////////////////////////////////////////////////////////
//	CHttpClient �\�z�Ɣj��

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
//	CHttpClient ����

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

	//	URL�𕪉�����
	if (!ParseUrl( url, &url_parts ))
		return RET_PARSE_URL;

    // Internet�ɐڑ�����
	hInet = InternetOpen( 
				m_userAgent.c_str(),
				INTERNET_OPEN_TYPE_PRECONFIG, // ���W�X�g���̐ݒ�ɏ]��
				NULL,
				NULL,
				0 );
	
	if (hInet == NULL)
        return RET_INET_OPEN;

	// HTTP�T�[�o�[�ɐڑ�
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

	// �T�[�o�[��ŗ~����URL���w�肷��
	hReq = HttpOpenRequest(
				hConn,
				(params == NULL) ? "GET" : "POST",
				(url_parts.path + url_parts.extra).c_str(),
				NULL,	// �f�t�H���g��HTTP�o�[�W����
				NULL,	// ������ǉ����Ȃ�
				NULL,	//	Accept-Type�� text/* 
				url_parts.ssl ? INTERNET_FLAG_SECURE : 0,
				NULL );

	if (hReq == NULL)
	{
		InternetCloseHandle( hConn );
		InternetCloseHandle( hInet );
        return RET_OPEN_REQUEST;
    }

	//	POST�Ȃ�������X�g����URL�R�[�f�B���O���ꂽ�G���e�B�e�B�̕�������쐬����
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

	//	�ǉ��w�b�_�[������ΐݒ肷��
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

	//	�w�b�_�[��v������
	if (HttpSendRequest( hReq, NULL, 0, entityPtr, entityLength) != TRUE)
	{
		InternetCloseHandle( hReq );
        InternetCloseHandle( hConn );
		InternetCloseHandle( hInet );
        return RET_SEND_REQUEST;
    }
	
	//	�o�̓t�@�C�����J��
	if ((stream = buffer->OpenWrite()) == NULL)
	{
		InternetCloseHandle( hReq );
		InternetCloseHandle( hConn );
		InternetCloseHandle( hInet );
		return RET_CANNOT_OPEN_FILE;
	}

	// �t�@�C���̓��e��S�Ď�M����
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
//	CHttpClient �w���p�[�֐�

bool CHttpClient::ParseUrl( const char* url, CHttpClient::URLPARTS* parts )
{
	URL_COMPONENTS	compo;
	char szScheme[256], szHostName[256], szUserName[256];
	char szPassword[256], szUrlPath[256], szExtraInfo[2048];

	//	InternetCrackUrl�̃R�[���O�ɗ̈�m�ۂ���
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

	//	�X�L�[�����`�F�b�Nhttp��https�ȊO�̓T�|�[�g�O
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

	//	extra�ɓ��{�ꓙ�́��G���R�[�h�������InternetCrackUrl�œ��{��(SJIS)
	//	�ɕύX����Ă��܂��s���������̂�extra�͂��̂܂܎g�� 2017.3.17
	if (strlen(compo.lpszExtraInfo))
		parts->extra = strchr(url, '?');

	return true;
}

//////////////////////////////////////////////////////////////////////////////
//	CHttpClient ����(�ÓI)

string CHttpClient::URLEncodeParams( const CCGIParams& params )
{
	string result;

	//	�������X�g��'&'�őS�ĘA��������������쐬����
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
			}	//	16�i�����ȊO�Ȃ�{���̓G���[�����������܂�
			break;
		case 2:
			if (isxdigit( *iter )) {
				int c;
				hexStr += *iter;
				sscanf( hexStr.c_str(), "%2x", &c );
				result += (char)c;
				status = 0;
			}	//	16�i�����ȊO�Ȃ�{���̓G���[�����������܂�
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