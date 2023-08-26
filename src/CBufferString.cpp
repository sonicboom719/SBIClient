#include "CBufferString.h"
#include <stdio.h>
#include <string.h>
#include <string>

#define INITIAL_BUFFER_SIZE		(256 * 1024)

using namespace std;


CBufferString::CBufferString()
{
	m_buff_size = INITIAL_BUFFER_SIZE;
	m_buff = new char [m_buff_size];
	*m_buff = '\0';
}

CBufferString::~CBufferString()
{
	delete [] m_buff;
}

char* CBufferString::ExpandSize()
{
	int new_buff_size = m_buff_size * 2;
	char* new_buff = new char [new_buff_size];
	memcpy( new_buff, m_buff, m_buff_size );
	delete [] m_buff;
	char* append_ptr = new_buff + m_buff_size;
	m_buff = new_buff;
	m_buff_size = new_buff_size;
	return append_ptr;
}


CBuffer::HANDLE CBufferString::OpenRead() const
{
	char** stream = new char*;
	*stream = m_buff;
	return stream;
}

CBuffer::HANDLE CBufferString::OpenWrite()
{
	char** stream = new char*;
	*stream = m_buff;
	*m_buff = '\0';
	return stream;
}

void CBufferString::Close( HANDLE stream ) const
{
	delete stream;
}

bool CBufferString::Write( HANDLE stream, const void* data, int size )
{
	char** localStream = (char**)stream;
	const unsigned char* p = (unsigned char*)data;

	for (int i = 0; i < size; i++, p++, (*localStream)++) {
		if (*localStream == m_buff + m_buff_size)
			*localStream = ExpandSize();
		**localStream = (*p != '\0') ? *p : '.';	//	ヌル文字はピリオドに変換
	}

	if (*localStream == m_buff + m_buff_size)
		*localStream = ExpandSize();
	**localStream = '\0';	//	ヌル終端にする

	return true;
}

int CBufferString::Getc( HANDLE stream ) const
{
	char** localStream = (char**)stream;
	char c = **localStream;
	if (c == '\0') return EOF;
	(*localStream)++;
	return c;
}

bool CBufferString::Gets( HANDLE stream, string* str ) const
{
	char** localStream = (char**)stream;
	str->clear();
	while (1) {
		char c = **localStream;
		if (c == '\0') return !str->empty();
		(*localStream)++;
		if (c == '\r' || c == '\n') return true;
		*str += c;
	}
}

bool CBufferString::Eof( HANDLE stream ) const
{
	char** localStream = (char**)stream;
	return (**localStream == '\0');
}

void CBufferString::Ungetc( HANDLE stream ) const
{
	char** localStream = (char**)stream;
	(*localStream)--;
}

bool CBufferString::Save( const char* filename ) const
{
	FILE* stream;

	stream = fopen( filename, "wb" );
	if (stream == NULL) return false;

	fwrite( m_buff, 1, strlen(m_buff), stream );
	fclose( stream );

	return true;
}

bool CBufferString::Load( const char* filename )
{
	FILE* stream;
	char readbuf[1024 * 512];
	int readsize;
	HANDLE handle;
	bool result;

	stream = fopen( filename, "rb" );
	if (stream == NULL) return false;

	readsize = fread( readbuf, 1, sizeof(readbuf), stream );
	fclose( stream );
	if (readsize < 1) return false;

	handle = OpenWrite();
	result = Write( handle, readbuf, readsize );
	Close( handle );

	return result;
}
