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

#include <PowerProps.h>

class CGenParserA : public genio::IParserA
{
public:
	CGenParserA();

	virtual ~CGenParserA();

	virtual void SetSourceData(const char *data, size_t datalen);

	virtual bool NextToken();
	virtual bool NextLine();
	virtual bool ReadUntil(const char *delimiter_set, bool end_ok = false, bool multiline = false);
	virtual bool ToEndOfLine();

	virtual genio::IParser::TOKEN_TYPE GetCurrentTokenType() const;
	virtual const char *GetCurrentTokenString() const;
	virtual bool GetCurrentTokenRange(size_t &token_start, size_t &token_end) const;

	virtual bool IsToken(const char *s, bool case_sensitive = false) const;

	virtual void Release() { delete this; }

	virtual void SetModeFlags(uint64_t flags) { m_flags = flags; }
	virtual uint64_t GetModeFlags() { return m_flags; }

protected:
	char *m_data;
	size_t m_datalen;
	size_t m_pos;
	size_t m_start, m_end;

	std::basic_string<char> m_curStr;
	genio::IParser::TOKEN_TYPE m_curType;

	props::TFlags64 m_flags;
};

class CGenParserW : public genio::IParserW
{
public:
	CGenParserW();

	~CGenParserW();

	virtual void SetSourceData(const wchar_t *data, size_t datalen);

	virtual bool NextToken();
	virtual bool NextLine();
	virtual bool ReadUntil(const wchar_t *delimiter_set, bool end_ok = false, bool multiline = false);
	virtual bool ToEndOfLine();

	virtual genio::IParser::TOKEN_TYPE GetCurrentTokenType() const;
	virtual const wchar_t *GetCurrentTokenString() const;
	virtual bool GetCurrentTokenRange(size_t &token_start, size_t &token_end) const;

	virtual bool IsToken(const wchar_t *s, bool case_sensitive = false) const;

	virtual void Release() { delete this; }

	virtual void SetModeFlags(uint64_t flags) { m_flags = flags; }
	virtual uint64_t GetModeFlags() { return m_flags; }

protected:
	wchar_t *m_data;
	size_t m_datalen;
	size_t m_pos;
	size_t m_start, m_end;

	std::basic_string<wchar_t> m_curStr;
	genio::IParser::TOKEN_TYPE m_curType;

	props::TFlags64 m_flags;
};

