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


class CGenTextOutput : public genio::ITextOutput
{
public:
	CGenTextOutput(FILE *f);
	virtual ~CGenTextOutput();

	virtual void Flush();

	// Write a string to the stream without formatting; allows the string to be optionally repeated
	// a number of times
	virtual void Print(const TCHAR *d, size_t number = 1);

	// Writes a string of formatted text to the stream (see the standard c printf documentation for usage)
	virtual void PrintF(const TCHAR *format, ...);

	// Begins the next line
	// Allows you to skip over a number of lines
	// Also allows you to optionally disable indenting
	virtual void NextLine(size_t skip = 0, bool indent = true);

	// Indentation character functions
	// The indentation character is the character that will be inserted n times (where n is the
	// current indentation level) before each new line that is written.
	virtual void SetIndentChar(TCHAR ch = _T('\t'));
	virtual TCHAR GetIndentChar();

	// Indentation level increase
	virtual void IncIndent(size_t inc = 1);

	// Indentation level decrease
	virtual void DecIndent(size_t dec = 1);


protected:
	size_t indentlvl;
	TCHAR indentchar;

	UINT modeflags;

	tstring m_Filename;
	bool m_OwnsFile;
	FILE *m_f;

};
