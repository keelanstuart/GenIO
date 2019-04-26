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
#include <GenIO.h>
#include "GenTextOut.h"
#include <io.h>
#include <fcntl.h>

genio::ITextOutput *genio::ITextOutput::Create(HANDLE h)
{
	FILE *f = NULL;
	if (h)
	{
		int fd = _open_osfhandle((intptr_t)h, _O_TEXT);
		f = _fdopen(fd, "tw");
	}
	return (genio::ITextOutput *)(new CGenTextOutput(f));
}

// ************************************************************************
// Text Output Stream Methods

CGenTextOutput::CGenTextOutput(FILE *f)
{
	m_f = f;
	indentchar = _T('\t');
	indentlvl = 0;
}


CGenTextOutput::~CGenTextOutput()
{
}


void CGenTextOutput::Flush()
{
	if (!m_f)
		return;

	fflush(m_f);
}


void CGenTextOutput::NextLine(size_t skip, bool indent)
{
	if (!m_f)
		return;

	// end the current line...
	PrintF(_T("\n"));

	// then skip over the specified number of lines...
	while (skip)
	{
		PrintF(_T("\n"));
		skip--;
	}

	// then indent that line...
	if (indent)
		for (size_t i = 0; i < indentlvl; i++)
			PrintF(_T("%c"), indentchar);
}


void CGenTextOutput::SetIndentChar(TCHAR ch)
{
	indentchar = ch;
}


TCHAR CGenTextOutput::GetIndentChar()
{
	return indentchar;
}


void CGenTextOutput::IncIndent(size_t inc)
{
	indentlvl += inc;
	if (indentlvl > genio::ITextOutput::MAXINDENT)
		indentlvl = genio::ITextOutput::MAXINDENT;
}


void CGenTextOutput::DecIndent(size_t dec)
{
	if (dec <= indentlvl)
		indentlvl -= dec;
}


void CGenTextOutput::Print(const TCHAR *d, size_t number)
{
	if (!m_f)
		return;

	while (number)
	{
		PrintF(_T("%s"), d);
		number--;
	}
}


void CGenTextOutput::PrintF(const TCHAR *format, ...)
{
	if (!m_f)
		return;

	va_list marker;
	va_start(marker, format);
	_vftprintf(m_f, format, marker);
}
