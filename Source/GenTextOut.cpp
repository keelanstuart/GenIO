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
