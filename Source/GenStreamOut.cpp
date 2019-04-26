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

#include "stdafx.h"
#include <GenStreamOut.h>
#include <fileapi.h>
//#include <Crc.h>


genio::IOutputStream *genio::IOutputStream::Create(HANDLE h)
{
	return (genio::IOutputStream *)(new COutputStream(h));
}

// ************************************************************************
// Output Stream Methods

COutputStream::COutputStream()
{
	m_hFile = NULL;
	m_OwnsFile = true;
}


COutputStream::COutputStream(HANDLE h)
{
	m_hFile = h;
	m_OwnsFile = (m_hFile == NULL) ? true : false;

	if (!m_hFile)
	{
		TCHAR path[MAX_PATH];
		GetFinalPathNameByHandle(m_hFile, path, MAX_PATH, 0);
		m_Filename = path;
	}
}


COutputStream::~COutputStream()
{
	Close();
}


void COutputStream::Release()
{
	delete this;
}


bool COutputStream::Assign(const TCHAR *filename)
{
	Close();

	m_Filename = filename;

	return true;
}


bool COutputStream::Open()
{
	if (!m_Filename.empty())
	{
		Close();

		m_hFile = CreateFile(m_Filename.c_str(), GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		m_OwnsFile = true;
	}

	return (m_hFile != NULL);
}


size_t COutputStream::Write(const void *data, size_t size, size_t number)
{
	if (!m_hFile)
		return 0;

	// fwrite returns the number of BLOCKS written, not the number of bytes, so
	// we have to fix up this number by multiplying it by size
	DWORD ret = 0, nwritten = 0;
	while (number)
	{
		WriteFile(m_hFile, data, (DWORD)size, &nwritten, NULL);
		ret += nwritten;
		number--;
	}

	if (!m_StreamBlockStack.empty())
	{
		SStreamBlockEntry &sbe = m_StreamBlockStack.back();

		size_t total = (size * number);

		//sbe.runningcrc = C2Crc32Calculate((UINT8 *)data, total, sbe.runningcrc);

		(&sbe)->m_Info.m_Length += total;
		//sbe.info.crc = sbe.runningcrc;
	}

	return ret;
}


bool COutputStream::Append()
{
	bool ret = Open();
	this->Seek(genio::IStream::SEEK_MODE::SM_END, 0);

	return ret;
}


void COutputStream::Close()
{
	if (!m_hFile)
		return;

	// End all blocks so that headers are up to date
	while (!m_StreamBlockStack.empty())
	{
		EndBlock();
	}

	if (m_OwnsFile)
		CloseHandle(m_hFile);

	m_hFile = NULL;
	m_OwnsFile = false;
}


void COutputStream::Flush()
{
	if (!m_hFile)
		return;

	FlushFileBuffers(m_hFile);
}


void COutputStream::Seek(genio::IStream::SEEK_MODE mode, int64_t count)
{
	if (m_hFile)
	{
		LARGE_INTEGER i;
		i.QuadPart = count;
		SetFilePointerEx(m_hFile, i, NULL, mode);
	}
}


size_t COutputStream::Pos() const
{
	if (m_hFile)
	{
		return SetFilePointer(m_hFile, 0, NULL, FILE_CURRENT);
	}

	return 0;
}


bool COutputStream::CanAccess() const
{
	return (m_hFile != NULL);
}


bool COutputStream::BeginBlock(genio::FOURCHARCODE id)
{
	SStreamBlockEntry sbe;

	// Set the id, and initialize length to 0
	sbe.m_Info.m_ID = htonl(id);
	sbe.m_Info.m_Length = 0;
	sbe.m_Info.m_Flags = 0;

	// Reset the block's crc... we'll calculate this as we add data to the block
	//sbe.runningcrc = CRC32_INITVALUE;

	DWORD nwritten = 0;
	WriteFile(m_hFile, &sbe.m_Info, sizeof(SStreamBlockInfo), &nwritten, NULL);

	// Store the start of this block after we have written the block header to the file
	sbe.m_BlockStart = Pos();

	m_StreamBlockStack.push_back(sbe);

	return true;
}


void COutputStream::EndBlock()
{
	if (!m_StreamBlockStack.empty())
	{
		SStreamBlockEntry &sbe = m_StreamBlockStack.back();

		size_t tmppos = Pos();

		// the length of the block when we end it, is the current position, minus the position we started it at
		sbe.m_Info.m_Length = tmppos - sbe.m_BlockStart;

		// Go to the start of the current block...  and back a few bytes to it's block header
		this->Seek(genio::IStream::SEEK_MODE::SM_BEGIN, sbe.m_BlockStart - sizeof(SStreamBlockInfo));

		// Write the updated header (this now includes the block crc and length)
		DWORD nwritten = 0;
		WriteFile(m_hFile, &sbe.m_Info, sizeof(SStreamBlockInfo), &nwritten, NULL);

		// From there, skip over the block to where we left off...
		this->Seek(genio::IStream::SEEK_MODE::SM_BEGIN, tmppos);

		m_StreamBlockStack.pop_back();
	}
}


void COutputStream::WriteINT64(int64_t d)
{
	Write((void *)&d, sizeof(d));
}


void COutputStream::WriteUINT64(uint64_t d)
{
	Write((void *)&d, sizeof(d));
}


void COutputStream::WriteINT32(int32_t d)
{
	Write((void *)&d, sizeof(d));
}


void COutputStream::WriteUINT32(uint32_t d)
{
	Write((void *)&d, sizeof(d));
}


void COutputStream::WriteDWORD(DWORD d)
{
	Write((void *)&d, sizeof(d));
}


void COutputStream::WriteINT16(int16_t d)
{
	Write((void *)&d, sizeof(d));
}


void COutputStream::WriteUINT16(uint16_t d)
{
	Write((void *)&d, sizeof(d));
}


void COutputStream::WriteINT8(int8_t d)
{
	Write((void *)&d, sizeof(d));
}


void COutputStream::WriteUINT8(uint8_t d)
{
	Write((void *)&d, sizeof(d));
}


void COutputStream::WriteDouble(double d)
{
	Write((void *)&d, sizeof(d));
}


void COutputStream::WriteFloat(float d)
{
	Write((void *)&d, sizeof(d));
}


void COutputStream::WriteStringA(const char *d)
{
	if (d)
		Write((void *)d, sizeof(char), strlen(d) + 1);
}


void COutputStream::WriteStringW(const wchar_t *d)
{
	if (d)
		Write((void *)d, sizeof(TCHAR), wcslen(d) + 1);
}
