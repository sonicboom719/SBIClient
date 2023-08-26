#pragma once

#include <string>

class CBuffer 
{
public:
	typedef void* HANDLE;

public:
	CBuffer() {}
	virtual ~CBuffer() {}

	virtual HANDLE OpenRead() const = 0;
	virtual HANDLE OpenWrite() = 0;
	virtual void Close( HANDLE stream ) const = 0;
	virtual bool Write( HANDLE stream, const void* data, int size ) = 0;
	virtual int Getc( HANDLE stream ) const = 0;
	virtual bool Gets( HANDLE stream, std::string* str ) const = 0;
	virtual bool Eof( HANDLE stream ) const = 0;
	virtual void Ungetc( HANDLE stream ) const = 0;
};

