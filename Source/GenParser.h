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

class CGenParser
{
public:
	CGenParser();

	~CGenParser();

	enum TOKEN_TYPE
	{
		TT_NONE = 0,

		TT_IDENT,			// [a-z,A-Z,_]+[0-9,a-z,A-Z,_]*
		TT_STRING,			// ["'][...]*["']
		TT_NUMBER,			// [-][0-9]+[.][0-9]*
		TT_SYMBOL,			// [!@#$%^&*()`~-=+[]{}<>/?;':",.|\]
		TT_HEXNUMBER,		// [0][xX][0-9,a-f,A-F]+
		TT_SHORTCOMMENT,	// "//"[...]*[\n]
		TT_LONGCOMMENT,		// "/*"[...]*"*/"

		TT_NUMTOKENTYPES
	};

	void SetSourceData(const TCHAR *data, UINT datalen);

	bool NextToken();
	bool NextLine();
	bool ToEndOfLine(); // captures the data in the stream until the next detected EOL but does not move the current stream position

	TOKEN_TYPE GetCurrentTokenType();
	TCHAR *GetCurrentTokenString();

	bool IsToken(TCHAR *s, bool case_sensitive = false);

protected:
	TCHAR *m_data;
	UINT m_datalen;
	UINT m_pos;

	tstring m_curStr;
	TOKEN_TYPE m_curType;
};

