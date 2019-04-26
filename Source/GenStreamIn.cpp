/*
	GenIO Library Source File

	Copyright � 2009-2019, Keelan Stuart. All rights reserved.

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

#include "stdafx.h"
#include <GenStreamIn.h>
//#include <Crc.h>


// ************************************************************************
// Input Stream Methods

CInputStream::CInputStream()
{
	m_OwnsFile = true;
	m_hFile = NULL;
}


CInputStream::CInputStream(HANDLE h)
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


CInputStream::~CInputStream()
{
	Close();
}


bool CInputStream::Assign(const TCHAR *filename)
{
	Close();

	m_Filename = filename;

	return true;
}


bool CInputStream::Open()
{
	if (!m_Filename.empty())
	{
		Close();

		m_hFile = CreateFile(m_Filename.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		m_OwnsFile = true;
	}

	return (m_hFile != NULL);
}


void CInputStream::Close()
{
	if (!m_hFile)
		return;

	if (m_OwnsFile)
		CloseHandle(m_hFile);

	m_hFile = NULL;
	m_OwnsFile = false;
}


size_t CInputStream::Read(void *data, size_t size, size_t number)
{
	size_t ret = 0;

	if (m_hFile)
	{
		assert(size * number < MAXDWORD);

		// fread returns the number of BLOCKS written, not the number of bytes, so
		// we have to fix up this number by multiplying it by size
		DWORD nread = 0;
		ReadFile(m_hFile, data, DWORD(size * number), &nread, NULL);
		ret = nread;

		if (!m_StreamBlockStack.empty())
		{
			SStreamBlockEntry &sbe = m_StreamBlockStack.back();

			// Calculate the running crc value
			//sbe.runningcrc = C2Crc32Calculate((const UINT8 *)data, size * number, sbe.runningcrc);

			// If we've passed the end of our current block, go to the next one... if it exists!
			if (Pos() > (sbe.m_BlockStart + sbe.m_Info.m_Length))
			{
				//assert(sbe.runningcrc == sbe.info.crc);

				//sbe.runningcrc = CRC32_INITVALUE;
			}
		}
	}

	return ret;
}


void CInputStream::Seek(genio::IStream::SEEK_MODE mode, int64_t count)
{
	if (m_hFile)
	{
		LARGE_INTEGER i;
		i.QuadPart = count;
		SetFilePointer(m_hFile, i.LowPart, &i.HighPart, mode);
	}
}


size_t CInputStream::Pos() const
{
	if (m_hFile)
	{
		LARGE_INTEGER z;
		z.QuadPart = 0;

		LARGE_INTEGER ret;
		if (SetFilePointerEx(m_hFile, z, &ret, FILE_CURRENT))
			return (size_t)ret.QuadPart;
	}

	return 0;
}


bool CInputStream::CanAccess() const
{
	return (m_hFile != NULL);
}


UINT32 CInputStream::NextBlockId()
{
	if (m_hFile)
	{
		genio::FOURCHARCODE tmpid;
		DWORD nread = 0;
		ReadFile(m_hFile, &tmpid, sizeof(genio::FOURCHARCODE), &nread, NULL);

		// Go back the size of one UINT32...
		Seek(genio::IStream::SEEK_MODE::SM_CURRENT, -((int64_t)sizeof(genio::FOURCHARCODE)));

		return ntohl(tmpid);
	}

	return 0;
}


bool CInputStream::BeginBlock(genio::FOURCHARCODE id)
{
	if (m_hFile)
	{
		SStreamBlockEntry sbe;

		DWORD nread = 0;
		ReadFile(m_hFile, &sbe.m_Info, sizeof(SStreamBlockInfo), &nread, NULL);
		if (nread != sizeof(SStreamBlockInfo))
			return false;

		sbe.m_Info.m_ID = ntohl(sbe.m_Info.m_ID);

		if (sbe.m_Info.m_ID == id)
		{
			sbe.m_BlockStart = Pos();

			//sbe.m_RunningCrc = CRC32_INITVALUE;

			m_StreamBlockStack.push_back(sbe);

			return true;
		}

		this->Seek(genio::IStream::SEEK_MODE::SM_CURRENT, -((int64_t)sizeof(SStreamBlockInfo)));
	}

	return false;
}


void CInputStream::EndBlock()
{
	if (m_hFile)
	{
		SStreamBlockEntry &sbe = m_StreamBlockStack.back();

		Seek(genio::IStream::SEEK_MODE::SM_BEGIN, sbe.m_BlockStart + sbe.m_Info.m_Length);

		m_StreamBlockStack.pop_back();
	}
}


void CInputStream::ReadINT64(int64_t &d)
{
	Read((void *)&d, sizeof(d));
}


void CInputStream::ReadUINT64(uint64_t &d)
{
	Read((void *)&d, sizeof(d));
}


void CInputStream::ReadINT32(int32_t &d)
{
	Read((void *)&d, sizeof(d));
}


void CInputStream::ReadUINT32(uint32_t &d)
{
	Read((void *)&d, sizeof(d));
}


void CInputStream::ReadDWORD(DWORD &d)
{
	Read((void *)&d, sizeof(d));
}


void CInputStream::ReadINT16(int16_t &d)
{
	Read((void *)&d, sizeof(d));
}


void CInputStream::ReadUINT16(uint16_t &d)
{
	Read((void *)&d, sizeof(d));
}


void CInputStream::ReadINT8(int8_t &d)
{
	Read((void *)&d, sizeof(d));
}


void CInputStream::ReadUINT8(uint8_t &d)
{
	Read((void *)&d, sizeof(d));
}


void CInputStream::ReadDouble(double &d)
{
	Read((void *)&d, sizeof(d));
}


void CInputStream::ReadFloat(float &d)
{
	Read((void *)&d, sizeof(d));
}


void CInputStream::ReadStringA(char  *d)
{
	for (;;)
	{
		Read((void *)d, sizeof(char));
		if (!(*d))
			break;

		d++;
	}
}


void CInputStream::ReadStringW(wchar_t  *d)
{
	for (;;)
	{
		Read((void *)d, sizeof(wchar_t));
		if (!(*d))
			break;

		d++;
	}
}
