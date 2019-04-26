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



// ************************************************************************

/// SFlagset is a helper to manage bit flags
template <typename T> struct SFlagset
{
	SFlagset() { flags = 0; }
	SFlagset(T f) { flags = f; }

	inline void SetAll(T f) { flags = f; }

	inline void Set(T f) { flags |= f; }

	inline void Toggle(T f) { if (IsSet(f)) Clear(f); else Set(f); }

	inline void Clear(T f) { flags &= ~f; }

	inline operator T() { return flags; }
	inline T Get() { return flags; }

	inline bool IsSet(T f) { return (((flags & f) == f) ? true : false); }

	inline SFlagset &operator =(T f) { flags = f; return *this; }

	inline bool operator ==(T f) { return (f == flags); }

	inline bool operator !=(T f) { return (f != flags); }

	inline T operator ^(T f) { return (flags ^ f); }
	inline SFlagset &operator ^=(T f) { flags ^= f; return *this; }

	inline T operator |(T f) { return (flags | f); }
	inline SFlagset &operator |=(T f) { flags |= f; return *this; }

	inline T operator &(T f) { return (flags & f); }
	inline SFlagset &operator &=(T f) { flags &= f; return *this; }

protected:
	T flags;
};


// ************************************************************************

// Pack this structure the same, regardless of platform since it's going to be written to, and read from, our streams!
#pragma pack(push, streamblockinfo_pack)

#pragma pack(1)

struct SStreamBlockInfo
{
	genio::FOURCHARCODE m_ID;			// the identifier of the block
	size_t m_Length;
	uint32_t m_Crc;
	SFlagset<uint32_t> m_Flags;
};

#pragma pack(pop, streamblockinfo_pack)


struct SStreamBlockEntry
{
	SStreamBlockInfo m_Info;
	size_t m_BlockStart;
	uint32_t m_RunningCrc;
};

typedef class std::deque<SStreamBlockEntry> TStreamBlockStack;

