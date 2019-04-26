/*
	GenIO Library Source File

	Copyright © 2009-2019, Keelan Stuart. All rights reserved.

	GenIO is an I/O library, providing classes that stream data in and out
	in a way that forward- and backward-compatible de/serialization is easy.
	Additionally, text streams that support indentation and a C-syntax
	tokenizing parser are provided.

    GenIO is free software; you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    GenIO is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.
	See <http://www.gnu.org/licenses/>.
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

	virtual bool Assign(const TCHAR *filename);
	virtual bool Open();
	virtual void Close();
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
