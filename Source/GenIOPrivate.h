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

