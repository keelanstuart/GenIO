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

