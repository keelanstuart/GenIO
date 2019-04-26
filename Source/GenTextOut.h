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
