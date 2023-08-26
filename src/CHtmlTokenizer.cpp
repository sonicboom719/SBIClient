//////////////////////////////////////////////////////////////////////////////
//	CHtmlTokenizer
//		HTMLファイルをトークン(文字列)に分割するクラス
//////////////////////////////////////////////////////////////////////////////

#include "CHtmlTokenizer.h"

using namespace std;

//////////////////////////////////////////////////////////////////////////////
//	CHtmlTokenizer 構築と破棄

CHtmlTokenizer::CHtmlTokenizer()
	: m_stream( NULL ),
	  m_buffer( NULL )
{
}

CHtmlTokenizer::~CHtmlTokenizer()
{
	if (m_stream != NULL)
		CloseBuffer();
}

//////////////////////////////////////////////////////////////////////////////
//	CHtmlTokenizer 操作

bool CHtmlTokenizer::OpenBuffer( const CBuffer* buffer )
{
	if (m_stream != NULL) CloseBuffer();

	m_buffer = buffer;
	m_stream = m_buffer->OpenRead();
	m_status = 0;

	return (m_stream != NULL);
}

void CHtmlTokenizer::CloseBuffer()
{
	if (m_stream != NULL) {
		m_buffer->Close( m_stream );
		m_stream = NULL;
	}
}

bool CHtmlTokenizer::GetNextToken( CHtmlTokenizer::TOKENTYPE* type, string* token )
{
	bool result = true;

	int c;
	bool bTokenObtained = false;
	bool bTagEnd = false;
	token->clear();

	while ((!bTokenObtained) && (c = m_buffer->Getc(m_stream))!= EOF)
	{
		switch (c){
		case '\n':
		case '\r': continue;
		case '\t': c = ' ';
		}
		switch (m_status){
		case 0:
			if (c == '<')
				m_status = 1;
			else if (c != ' '){
				*token += c;
				m_status = 2;
			}
			break;
		case 1:
			switch (c){
			case '/': bTagEnd = true; break;
			case '!': m_status = 4; break;
			case ' ': break;
			default:
				if (isalpha(c)){
					*token += c;
					m_status = 3;
				}else{
					*token += c;
					m_status = 2;
				}
			}
			break;
		case 2:
			if (c == '<'){
				*type = OTHER_TEXT;
				bTokenObtained = true;
				m_status = 0;
				//	character '<' push back 
				m_buffer->Ungetc( m_stream );
			}else *token += c;
			break;
		case 3:
			if (isalnum(c))
				*token += c;
			else if (c == '>'){
				*type = (bTagEnd)? TAG_END : TAG_START;
				bTokenObtained = true;
				m_status = 0;
			}else if (c == ' ') {
				*type = TAG_START;
				bTokenObtained = true;
				m_status = 5;
			}else{
				*token += c;
				m_status = 2;
			}
			break;
		case 4:
			switch (c){
			case '-':
			case ' ':
				break;
			case '>':
				*token += c;
				m_status = 2;
				break;
			case 'D':
				//	Dから始まったらDOCTYPE宣言とみなす
				//	【注意！】<!-- D... ←こういうコメントもそれとみなされてしまうという穴があります！
				*token += c;
				m_status = 200;
				break;
			default:
				*token += c;
				m_status = 7;
			}
			break;
		case 5:
			switch (c) {
			case '>':
				m_status = 0;
				break;
			case ' ':
				m_status = 5;
				break;
			default:
				*token += c;
				m_status = 100;
			}
			break;
		case 7:
			if (c == '-'){
				*token += c;
				m_status = 8;
			}else
				*token += c;
			break;
		case 8:
			if (c == '-'){
				*token += c;
				m_status = 9;
			}else{
				*token += c;
				m_status = 7;
			}
			break;
		case 9:
			switch (c) {
			case '>':
				*type = COMMENT;
				bTokenObtained = true;
				m_status = 0;
				//	trim left
				*token = token->substr( 0, token->find_last_not_of("- ") + 1 );
				break;
			case '-':
				*token += c;
				m_status = 9;
				break;
			default:
				*token += c;
				m_status = 7;
			}
			break;

		case 100:
			switch (c) {
			case '>':
				*type = ATTR_NAME;
				bTokenObtained = true;
				m_status = 0;
				break;
			case ' ':
				*type = ATTR_NAME;
				bTokenObtained = true;
				m_status = 101;
				break;
			case '=':
				*type = ATTR_NAME;
				bTokenObtained = true;
				m_status = 102;
				break;
			default:
				*token += c;
				m_status = 100;
			}
			break;
		case 101:
			switch (c) {
			case '>':
				m_status = 0;
				break;
			case '=':
				m_status = 102;
				break;
			case ' ':
				m_status = 101;
				break;
			default:
				*token += c;
				m_status = 100;
			}
			break;
		case 102:
			switch (c) {
			case '>':
				m_status = 0;
				break;
			case ' ':
				m_status = 102;
				break;
			case '"':
			case '\'':
				m_status = 103;
				break;
			default:
				*token += c;
				m_status = 104;
			}
			break;
		case 103:
			switch (c) {
			case '"':
			case '\'':
				*type = ATTR_VALUE;
				bTokenObtained = true;
				m_status = 5;
				break;
			default:
				*token += c;
				m_status = 103;
			}
			break;
		case 104:
			switch (c) {
			case '>':
				*type = ATTR_VALUE;
				bTokenObtained = true;
				m_status = 0;
				break;
			case ' ':
				*type = ATTR_VALUE;
				bTokenObtained = true;
				m_status = 5;
				break;
			default:
				*token += c;
				m_status = 104;
			}
			break;

		case 200:
			if (c == '>'){
				*type = DOCTYPE;
				bTokenObtained = true;
				m_status = 0;
			}else{
				*token += c;
				m_status = 200;
			}
			break;

		}
	}

	if (m_buffer->Eof(m_stream) && !bTokenObtained)	//	EOF and no token
		result = false;
	else if (m_buffer->Eof(m_stream) && m_status > 0)	//	EOF and text token
		*type = OTHER_TEXT;

	return result;
}
