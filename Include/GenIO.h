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

#if !defined(GENIO_STATIC)

#ifdef GENIO_EXPORTS
#define GENIO_API __declspec(dllexport)
#else
#define GENIO_API __declspec(dllimport)
#endif

#else

#define GENIO_API

#endif


#include <Windows.h>
#include <stdint.h>
#include <tchar.h>


namespace genio
{

	typedef uint32_t FOURCHARCODE;

#if !defined(GENIO_EXPORTS)
#if defined(UNICODE) || defined(_UNICODE)
#define IParserT IParserW
#define CM_TCHAR CM_UNICODE
#else
#define IParserT IParserA
#define CM_TCHAR CM_ASCII
#endif
#endif

	class IParser
	{

	public:

		enum TOKEN_TYPE
		{
			TT_NONE = 0,

			TT_IDENT,			/// [a-z,A-Z,_]+[0-9,a-z,A-Z,_]*
			TT_STRING,			/// ["'][...]*["']
			TT_NUMBER,			/// [-][0-9]+[.][0-9]*
			TT_SYMBOL,			/// [!@#$%^&*()`~-=+[]{}<>/?;':",.|\]
			TT_HEXNUMBER,		/// [0][xX][0-9,a-f,A-F]+
			TT_SHORTCOMMENT,	/// "//"[...]*[\n]
			TT_LONGCOMMENT,		/// "/*"[...]*"*/"
			TT_NEWLINE,			/// [\n\r]+

			TT_NUMTOKENTYPES
		};

		enum CHAR_MODE
		{
			CM_ASCII = 0,
			CM_UNICODE
		};

#define PARSEFLAG_TOKENIZE_NEWLINES		0x0001

		/// Consumes the next C-style token in the stream
		virtual bool NextToken() = NULL;

		/// Consumes all the tokens in the stream until a line break and then further
		/// until a non-whitespace token is found
		virtual bool NextLine() = NULL;

		/// Captures the data in the stream until the next detected EOL
		virtual bool ToEndOfLine() = NULL;

		/// Returns the current token type that has been parsed out of the stream
		virtual TOKEN_TYPE GetCurrentTokenType() const = NULL;

		/// Returns the resources allocated by the parser to the system
		virtual void Release() = NULL;

		/// Sets the flags that control parse operations
		virtual void SetModeFlags(uint64_t flags) = NULL;

		/// Gets the flags that control parse operations
		virtual uint64_t GetModeFlags() = NULL;

		/// Creates an IParser in the mode you desire, ASCII or Unicode. Specify CM_TCHAR to choose based on your program configuration
		GENIO_API static IParser *Create(CHAR_MODE mode);

	};

	class IParserW : public IParser
	{

	public:

		/// Sets the source data that will be parsed
		virtual void SetSourceData(const wchar_t *data, size_t datalen) = NULL;

		/// Returns the actual string out of the stream for the currently parsed token
		virtual const wchar_t *GetCurrentTokenString() const = NULL;

		/// Returns true if the current token matches the given string, optionally
		/// checking case
		virtual bool IsToken(const wchar_t *s, bool case_sensitive = false) const = NULL;

		/// Captures the data in the stream until the one of the characters in the delimiter_set is detected 
		virtual bool ReadUntil(const wchar_t *delimiter_set, bool end_ok = false, bool multiline = false) = NULL;

	};

	class IParserA : public IParser
	{

	public:

		/// Sets the source data that will be parsed
		virtual void SetSourceData(const char *data, size_t datalen) = NULL;

		/// Returns the actual string out of the stream for the currently parsed token
		virtual const char *GetCurrentTokenString() const = NULL;

		/// Returns true if the current token matches the given string, optionally
		/// checking case
		virtual bool IsToken(const char *s, bool case_sensitive = false) const = NULL;

		/// Captures the data in the stream until the one of the characters in the delimiter_set is detected 
		virtual bool ReadUntil(const char *delimiter_set, bool end_ok = false, bool multiline = false) = NULL;

	};

#if defined(UNICODE)
#define IParserT		IParserW
#define	CM_TCHAR		CM_UNICODE
#else
#define IParserT		IParserA
#define	CM_TCHAR		CM_ASCII
#endif


	class ITextOutput
	{

	public:

		enum
		{
			MAXINDENT = 255
		};

		virtual void Flush() = NULL;

		/// Write a string to the stream without formatting; allows the string to be optionally repeated
		/// a number of times
		virtual void Print(const TCHAR *d, size_t number = 1) = NULL;

		/// Writes a string of formatted text to the stream (see the standard c printf documentation for usage)
		virtual void PrintF(const TCHAR *format, ...) = NULL;

		/// Begins the next line
		/// Allows you to skip over a number of lines
		/// Also allows you to optionally disable indenting
		virtual void NextLine(size_t skip = 0, bool indent = true) = NULL;

		/// Indentation character functions
		/// The indentation character is the character that will be inserted n times (where n is the
		/// current indentation level) before each new line that is written.
		virtual void SetIndentChar(TCHAR ch = _T('\t')) = NULL;

		virtual TCHAR GetIndentChar() = NULL;

		/// Indentation level increase
		virtual void IncIndent(size_t inc = 1) = NULL;

		/// Indentation level decrease
		virtual void DecIndent(size_t dec = 1) = NULL;

		virtual void Release() = NULL;

		GENIO_API static ITextOutput *Create(HANDLE h);
		GENIO_API static ITextOutput *Create(const TCHAR *filename);
	};


#define STRMFLG_BIGENDIAN		0x00000001		// the data in the stream is big endian if this is set, otherwise it is little endian
#define STRMFLG_COMPRESSED		0x00000002		// the data has been lz-style compressed

	class IStream
	{

	public:

		enum SEEK_MODE
		{
			SM_BEGIN = 0,
			SM_CURRENT,
			SM_END,

			SM_NUMMODES
		};

		enum
		{
			ENDBLOCKID = 0
		};

		virtual bool Assign(const TCHAR *filename) = NULL;
		virtual bool Open() = NULL;
		virtual void Close() = NULL;
		virtual void Flush() = NULL;
		virtual void Seek(SEEK_MODE mode, int64_t count) = NULL;
		virtual size_t Pos() const = NULL;

		virtual bool CanAccess() const = NULL;

		virtual bool BeginBlock(FOURCHARCODE id) = NULL;
		virtual void EndBlock() = NULL;

		virtual void Release() = NULL;

	};

	class IInputStream : public IStream
	{

	public:

		virtual FOURCHARCODE NextBlockId() = NULL;

		virtual size_t Read(void *data, size_t size, size_t number = 1) = NULL;

		virtual void ReadINT64		(int64_t	&d) = NULL;
		virtual void ReadUINT64		(uint64_t	&d) = NULL;
		virtual void ReadINT32		(int32_t	&d) = NULL;
		virtual void ReadUINT32		(uint32_t	&d) = NULL;
		virtual void ReadINT16		(int16_t	&d) = NULL;
		virtual void ReadUINT16		(uint16_t	&d) = NULL;
		virtual void ReadINT8		(int8_t		&d) = NULL;
		virtual void ReadUINT8		(uint8_t	&d) = NULL;
		virtual void ReadDouble		(double		&d) = NULL;
		virtual void ReadFloat		(float		&d) = NULL;
		virtual void ReadDWORD		(DWORD		&d) = NULL;

		// If you use this, store the string length in the stream and pre-allocate space
		virtual void ReadStringA	(const char		*d) = NULL;
		virtual void ReadStringW	(const wchar_t	*d) = NULL;

		GENIO_API static IInputStream *Create(HANDLE h = NULL);

	};

	class IOutputStream : public IStream
	{

	public:

		virtual size_t Write(const void *data, size_t size, size_t number = 1) = NULL;

		virtual void WriteINT64		(int64_t	d) = NULL;
		virtual void WriteUINT64	(uint64_t	d) = NULL;
		virtual void WriteINT32		(int32_t	d) = NULL;
		virtual void WriteUINT32	(uint32_t	d) = NULL;
		virtual void WriteINT16		(int16_t	d) = NULL;
		virtual void WriteUINT16	(uint16_t	d) = NULL;
		virtual void WriteINT8		(int8_t		d) = NULL;
		virtual void WriteUINT8		(uint8_t	d) = NULL;
		virtual void WriteDouble	(double		d) = NULL;
		virtual void WriteFloat		(float		d) = NULL;
		virtual void WriteDWORD		(DWORD		d) = NULL;

		virtual void WriteStringA	(const char		*d) = NULL;
		virtual void WriteStringW	(const wchar_t	*d) = NULL;

		GENIO_API static IOutputStream *Create(HANDLE h = NULL);

	};


#if defined(UNICODE)

#define ReadString ReadStringW
#define WriteString WriteStringW

#else

#define ReadString ReadStringA
#define WriteString WriteStringA

#endif

};
