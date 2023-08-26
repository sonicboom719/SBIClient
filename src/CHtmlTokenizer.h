//////////////////////////////////////////////////////////////////////////////
//	CHtmlTokenizer:	HTMLトークナイザー
//////////////////////////////////////////////////////////////////////////////

#include "CBuffer.h"
#include <stdio.h>
#include <string>

//////////////////////////////////////////////////////////////////////////////
//	CHtmlTokenizer

class CHtmlTokenizer 
{
public:
	typedef enum enTOKENTYPE {
		DOCTYPE,
		TAG_START,
		TAG_END,
		ATTR_NAME,
		ATTR_VALUE,
		COMMENT,
		OTHER_TEXT
	} TOKENTYPE;

public:
	CHtmlTokenizer();
	virtual ~CHtmlTokenizer();

	bool OpenBuffer( const CBuffer* buffer );
	void CloseBuffer();
	bool GetNextToken( TOKENTYPE* type, std::string* token );

private:
	const CBuffer*	m_buffer;
	CBuffer::HANDLE	m_stream;
	int				m_status;
};

