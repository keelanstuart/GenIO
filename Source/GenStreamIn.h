/*
	GenIO Library Source File

	Copyright © 2009-2019, Keelan Stuart. All rights reserved.

	GenIO is an I/O library, providing classes that stream data in and out
	in a way that forward- and backward-compatible de/serialization is easy.
	Additionally, text streams that support indentation and a C-syntax
	tokenizing parser are provided.

	GenIO is free software; you can redistribute it and/or modify it under
	the terms of the MIT License:

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/


#pragma once


#include <GenIO.h>
#include <GenIOPrivate.h>


// Implements input file streaming class


class CInputStream : public genio::IInputStream
{

public:

	CInputStream();
	CInputStream(HANDLE h);
	virtual ~CInputStream();

	virtual void Release();

	virtual bool Assign(const TCHAR *filename);
	virtual bool Open();
	virtual void Close();
	virtual void Flush();
	virtual void Seek(genio::IStream::SEEK_MODE mode, int64_t count);
	virtual size_t Pos() const;

	virtual bool CanAccess() const;

	virtual genio::FOURCHARCODE NextBlockId();
	virtual bool BeginBlock(genio::FOURCHARCODE id);
	virtual void EndBlock();

	virtual size_t Read(void *data, size_t size, size_t number = 1);

	virtual void ReadINT64		(int64_t	&d);
	virtual void ReadUINT64		(uint64_t	&d);
	virtual void ReadINT32		(int32_t	&d);
	virtual void ReadUINT32		(uint32_t	&d);
	virtual void ReadINT16		(int16_t	&d);
	virtual void ReadUINT16		(uint16_t	&d);
	virtual void ReadINT8		(int8_t		&d);
	virtual void ReadUINT8		(uint8_t	&d);
	virtual void ReadDouble		(double		&d);
	virtual void ReadFloat		(float		&d);
	virtual void ReadDWORD		(DWORD		&d);

	// If you use this, store the string length in the stream and pre-allocate space
	virtual void ReadStringA	(char		*d);
	virtual void ReadStringW	(wchar_t	*d);

protected:
	tstring m_Filename;
	bool m_OwnsFile;
	HANDLE m_hFile;

	TStreamBlockStack m_StreamBlockStack;

};
