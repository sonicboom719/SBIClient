#include "CBuffer.h"

class CBufferString : public CBuffer
{
public:
	CBufferString();
	virtual ~CBufferString();

	bool Load( const char* filename );
	bool Save( const char* filename ) const;
	const char* GetCPointer() const { return m_buff; }

	virtual HANDLE OpenRead() const;
	virtual HANDLE OpenWrite();
	virtual void Close( HANDLE stream ) const;
	virtual bool Write( HANDLE stream, const void* data, int size );
	virtual int Getc( HANDLE stream ) const;
	virtual bool Gets( HANDLE stream, std::string* str ) const;
	virtual bool Eof( HANDLE stream ) const;
	virtual void Ungetc( HANDLE stream ) const;

private:
	char* ExpandSize();

private:
	char* m_buff;
	int m_buff_size;
};
