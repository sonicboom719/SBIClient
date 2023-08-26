//////////////////////////////////////////////////////////////////////////////
//	CHttpClient クラス
//		HTTP(S)通信をして要求送信と応答受信を行う簡易HTTPクライアント
//////////////////////////////////////////////////////////////////////////////

#pragma once

#include <windows.h>
#include <wininet.h>
#include <string>
#include <list>
#include <vector>
#include "CBuffer.h"


class CHttpClient
{
public:
	enum enRETCODE {
		RET_OK = 0,
		RET_CANNOT_OPEN_FILE = -100,
		RET_INET_OPEN = -101,
		RET_INET_CONNECT = -102,
		RET_OPEN_REQUEST = -103,
		RET_SEND_REQUEST = -104,
		RET_PARSE_URL = -105,
		RET_ADD_HEADERS = -106
	};

	typedef struct stURLPARTS {
		bool ssl;
		int port;
		std::string host;
		std::string path;
		std::string user;
		std::string password;
		std::string extra;
	} URLPARTS;

	class CHeaders : public std::vector<std::string>
	{
	};

	class CCGIParams : public std::list<std::pair<std::string,std::string>>
	{
	public:
		CCGIParams();
		virtual ~CCGIParams();
		std::string Get( const std::string& name ) const;
		void Add( const std::string& name, const std::string& value );
		void Set( const std::string& name, const std::string& value );
		void Dump();
	};

public:
	CHttpClient();
	virtual ~CHttpClient();

	void SetUserAgent( const char* userAgent ) { m_userAgent = userAgent; }
	void SetRetry( unsigned int count, unsigned int interval ) { m_retryCount = count; m_retryInterval = interval; }

	int MethodGet( const char* url, CBuffer* buffer ); 
	int MethodPost( const char* url, const CHeaders* headers, 
					const CCGIParams& params, CBuffer* buffer );

	static std::string URLEncodeParams( const CCGIParams& params );
	static void URLDecodeParams( const std::string& src, CCGIParams* params );
	static std::string URLEncode( const std::string& src );
	static std::string URLDecode( const std::string& src );
	static bool IsRetryableCode( int retcode );

private:
	static bool ParseUrl( const char* url, URLPARTS* parts );
	int Request( const char* url, const CHeaders* headers,
					const CCGIParams* params, CBuffer* buffer );

private:
	std::string	m_userAgent;
	unsigned int m_retryCount;
	unsigned int m_retryInterval;
};
