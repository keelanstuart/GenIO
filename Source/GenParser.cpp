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
#include "GenParser.h"
#include <string.h>


CGenParserA::CGenParserA()
{
	m_data = nullptr;
	m_datalen = 0;
	m_pos = 0;

	m_curType = genio::IParser::TT_NONE;
}


CGenParserA::~CGenParserA()
{
}


void CGenParserA::SetSourceData(const char *data, size_t datalen)
{
	m_data = (char *)data;
	m_datalen = datalen;
	m_pos = 0;

	m_curType = genio::IParser::TT_NONE;
	m_curStr.clear();
}


bool CGenParserA::NextToken()
{
	if (!m_data || !m_datalen)
		return false;

	m_curType = genio::IParser::TT_NONE;
	m_curStr.clear();
	char strdelim = '\0';

	while (m_pos < m_datalen)
	{
		// skip whitespace
		if (strchr("\n\r\t ", m_data[m_pos]))
		{
			if (m_curType == genio::IParser::TT_NONE)
			{
				// skip whitespace
				m_pos++;
			}
			else if (m_curType == genio::IParser::TT_SHORTCOMMENT)
			{
				// short comments end at EOL
				if (strchr("\n\r", m_data[m_pos]))
					break;

				m_curStr += m_data[m_pos];
				m_pos++;
			}
			else if ((m_curType == genio::IParser::TT_STRING) || (m_curType == genio::IParser::TT_LONGCOMMENT))
			{
				m_curStr += m_data[m_pos];
				m_pos++;
			}
			else
			{
				break;
			}

			continue;
		}

		if (strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", m_data[m_pos]))
		{
			if (m_curType == genio::IParser::TT_NONE)
			{
				// nothing yet?  this is an identifier
				m_curType = genio::IParser::TT_IDENT;

				m_curStr += m_data[m_pos];
				m_pos++;
			}
			else if (m_curType == genio::IParser::TT_HEXNUMBER)
			{
				if (strchr("abcdefABCDEF", m_data[m_pos]))
				{
					m_curStr += m_data[m_pos];
					m_pos++;
				}
				else
					break;
			}
			else if ((m_curType == genio::IParser::TT_STRING) || (m_curType == genio::IParser::TT_IDENT) || (m_curType == genio::IParser::TT_SHORTCOMMENT) || (m_curType == genio::IParser::TT_LONGCOMMENT))
			{
				// already an identifier or string? just add and move on
				m_curStr += m_data[m_pos];
				m_pos++;
			}
			else
			{
				// if we're a number or a symbol and hit one of these characters, get out
				break;
			}

			continue;
		}

		if (strchr("0123456789", m_data[m_pos]))
		{
			if (m_curType == genio::IParser::TT_NONE)
			{
				// nothing yet?  this is a number
				m_curType = genio::IParser::TT_NUMBER;

				if ((m_data[m_pos + 1] == 'x') || (m_data[m_pos + 1] == 'X'))
				{
					m_curType = genio::IParser::TT_HEXNUMBER;
					m_pos += 2;
				}
				else
				{
					m_curStr += m_data[m_pos];
					m_pos++;
				}
			}
			else if ((m_curType == genio::IParser::TT_NUMBER) || (m_curType == genio::IParser::TT_STRING) || (m_curType == genio::IParser::TT_IDENT) || (m_curType == genio::IParser::TT_HEXNUMBER) || (m_curType == genio::IParser::TT_SHORTCOMMENT) || (m_curType == genio::IParser::TT_LONGCOMMENT))
			{
				// already an identifier or string? just add and move on
				m_curStr += m_data[m_pos];
				m_pos++;
			}
			else
			{
				// if we're a number or a symbol and hit one of these characters, get out
				break;
			}

			continue;
		}

		// symbols of every kind
		if (strchr("`~!@#$%^&*()-=+[]{}\\|;:,.<>/?", m_data[m_pos]))
		{
			if (m_curType == genio::IParser::TT_NONE)
			{
				if (m_data[m_pos] == '-')
				{
					if (((m_pos + 1) < m_datalen) && strchr("0123456789", m_data[m_pos + 1]))
					{
						// nothing yet?  this is a number
						m_curType = genio::IParser::TT_NUMBER;

						m_curStr += m_data[m_pos];
						m_pos++;

						continue;
					}
				}
				else if (m_data[m_pos] == '/')
				{
					if (((m_pos + 1) < m_datalen) && strchr("/*", m_data[m_pos + 1]))
					{
						// nothing yet?  this is a comment
						m_curType = (m_data[m_pos + 1] == '/') ? genio::IParser::TT_SHORTCOMMENT : genio::IParser::TT_LONGCOMMENT;

						m_curStr += m_data[m_pos];
						m_curStr += m_data[m_pos + 1];
						m_pos += 2;

						continue;
					}
				}

				// nothing yet?  this is a number
				m_curType = genio::IParser::TT_SYMBOL;

				m_curStr += m_data[m_pos];
				m_pos++;
			}
			else if (m_curType == genio::IParser::TT_NUMBER)
			{
				// a decimal point can only be inserted if it's already a number and there's no pre-existing decimal point
				if ((m_data[m_pos] == '.') && !strchr(m_curStr.c_str(), '.'))
				{
					m_curStr += m_data[m_pos];
					m_pos++;

					continue;
				}
			}
			else if ((m_curType == genio::IParser::TT_STRING) || (m_curType == genio::IParser::TT_SHORTCOMMENT))
			{
				m_curStr += m_data[m_pos];
				m_pos++;

				continue;
			}
			else if (m_curType == genio::IParser::TT_LONGCOMMENT)
			{
				bool ast = (m_data[m_pos] == '*');

				m_curStr += m_data[m_pos];
				m_pos++;

				if (ast && (m_pos < m_datalen) && (m_data[m_pos] == '/'))
				{
					m_curStr += m_data[m_pos];
					m_pos++;
					break;
				}

				continue;
			}

			break;
		}

		if (strchr("\"'", m_data[m_pos]) != nullptr)
		{
			if (m_curType == genio::IParser::TT_NONE)
			{
				m_curType = genio::IParser::TT_STRING;
				strdelim = m_data[m_pos];
				m_pos++;
			}
			else if (m_curType == genio::IParser::TT_STRING)
			{
				if (strdelim != m_data[m_pos])
				{
					m_curStr += m_data[m_pos];
					m_pos++;
				}
				else
				{
					m_pos++;
					strdelim = '\0';
					break;
				}
			}
			else if ((m_curType != genio::IParser::TT_LONGCOMMENT) && (m_curType != genio::IParser::TT_SHORTCOMMENT))
				continue;
		}

		if ((m_curType == genio::IParser::TT_STRING) || (m_curType == genio::IParser::TT_SHORTCOMMENT) || (m_curType == genio::IParser::TT_LONGCOMMENT))
		{
			m_curStr += m_data[m_pos];
		}

		m_pos++;
	}

	if (m_curStr.empty())
	{
		return false;
	}

	return true;
}


bool CGenParserA::NextLine()
{
	if (!m_data || !m_datalen)
		return false;

	m_curType = genio::IParser::TT_NONE;
	m_curStr.clear();

	while (m_pos < m_datalen)
	{
		// skip over everything except EOLs
		if (strchr("\n\r", m_data[m_pos]))
		{
			break;
		}

		m_pos++;
	}

	while (m_pos < m_datalen)
	{
		// skip over all EOLs until we hit something else
		if (!strchr("\n\r", m_data[m_pos]))
		{
			return true;
		}

		m_pos++;
	}

	return false;
}

bool CGenParserA::ToEndOfLine()
{
	if (!m_data || !m_datalen)
		return false;

	m_curType = genio::IParser::TT_NONE;
	m_curStr.clear();

	while (m_pos < m_datalen)
	{
		// skip over everything except EOLs
		if (strchr("\n\r", m_data[m_pos]))
		{
			return true;
		}

		m_curStr += m_data[m_pos];
		m_pos++;
	}

	return false;
}

genio::IParser::TOKEN_TYPE CGenParserA::GetCurrentTokenType() const
{
	return m_curType;
}


const char *CGenParserA::GetCurrentTokenString() const
{
	return m_curStr.c_str();
}

bool CGenParserA::IsToken(const char *s, bool case_sensitive) const
{
	if (case_sensitive)
		return !strcmp(m_curStr.c_str(), s);

	return !_stricmp(m_curStr.c_str(), s);
}

CGenParserW::CGenParserW()
{
	m_data = nullptr;
	m_datalen = 0;
	m_pos = 0;

	m_curType = genio::IParser::TT_NONE;
}


CGenParserW::~CGenParserW()
{
}


void CGenParserW::SetSourceData(const wchar_t *data, size_t datalen)
{
	m_data = (wchar_t *)data;
	m_datalen = datalen;
	m_pos = 0;

	m_curType = genio::IParser::TT_NONE;
	m_curStr.clear();
}


bool CGenParserW::NextToken()
{
	if (!m_data || !m_datalen)
		return false;

	m_curType = genio::IParser::TT_NONE;
	m_curStr.clear();
	wchar_t strdelim = L'\0';

	while (m_pos < m_datalen)
	{
		// skip whitespace
		if (wcschr(L"\n\r\t ", m_data[m_pos]))
		{
			if (m_curType == genio::IParser::TT_NONE)
			{
				// skip whitespace
				m_pos++;
			}
			else if (m_curType == genio::IParser::TT_SHORTCOMMENT)
			{
				// short comments end at EOL
				if (wcschr(L"\n\r", m_data[m_pos]))
					break;

				m_curStr += m_data[m_pos];
				m_pos++;
			}
			else if ((m_curType == genio::IParser::TT_STRING) || (m_curType == genio::IParser::TT_LONGCOMMENT))
			{
				m_curStr += m_data[m_pos];
				m_pos++;
			}
			else
			{
				break;
			}

			continue;
		}

		if (wcschr(L"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", m_data[m_pos]))
		{
			if (m_curType == genio::IParser::TT_NONE)
			{
				// nothing yet?  this is an identifier
				m_curType = genio::IParser::TT_IDENT;

				m_curStr += m_data[m_pos];
				m_pos++;
			}
			else if (m_curType == genio::IParser::TT_HEXNUMBER)
			{
				if (wcschr(L"abcdefABCDEF", m_data[m_pos]))
				{
					m_curStr += m_data[m_pos];
					m_pos++;
				}
				else
					break;
			}
			else if ((m_curType == genio::IParser::TT_STRING) || (m_curType == genio::IParser::TT_IDENT) || (m_curType == genio::IParser::TT_SHORTCOMMENT) || (m_curType == genio::IParser::TT_LONGCOMMENT))
			{
				// already an identifier or string? just add and move on
				m_curStr += m_data[m_pos];
				m_pos++;
			}
			else
			{
				// if we're a number or a symbol and hit one of these wchar_tacters, get out
				break;
			}

			continue;
		}

		if (wcschr(L"0123456789", m_data[m_pos]))
		{
			if (m_curType == genio::IParser::TT_NONE)
			{
				// nothing yet?  this is a number
				m_curType = genio::IParser::TT_NUMBER;

				if ((m_data[m_pos + 1] == L'x') || (m_data[m_pos + 1] == L'X'))
				{
					m_curType = genio::IParser::TT_HEXNUMBER;
					m_pos += 2;
				}
				else
				{
					m_curStr += m_data[m_pos];
					m_pos++;
				}
			}
			else if ((m_curType == genio::IParser::TT_NUMBER) || (m_curType == genio::IParser::TT_STRING) || (m_curType == genio::IParser::TT_IDENT) || (m_curType == genio::IParser::TT_HEXNUMBER) || (m_curType == genio::IParser::TT_SHORTCOMMENT) || (m_curType == genio::IParser::TT_LONGCOMMENT))
			{
				// already an identifier or string? just add and move on
				m_curStr += m_data[m_pos];
				m_pos++;
			}
			else
			{
				// if we're a number or a symbol and hit one of these wchar_tacters, get out
				break;
			}

			continue;
		}

		// symbols of every kind
		if (wcschr(L"`~!@#$%^&*()-=+[]{}\\|;:,.<>/?", m_data[m_pos]))
		{
			if (m_curType == genio::IParser::TT_NONE)
			{
				if (m_data[m_pos] == L'-')
				{
					if (((m_pos + 1) < m_datalen) && wcschr(L"0123456789", m_data[m_pos + 1]))
					{
						// nothing yet?  this is a number
						m_curType = genio::IParser::TT_NUMBER;

						m_curStr += m_data[m_pos];
						m_pos++;

						continue;
					}
				}
				else if (m_data[m_pos] == L'/')
				{
					if (((m_pos + 1) < m_datalen) && wcschr(L"/*", m_data[m_pos + 1]))
					{
						// nothing yet?  this is a comment
						m_curType = (m_data[m_pos + 1] == L'/') ? genio::IParser::TT_SHORTCOMMENT : genio::IParser::TT_LONGCOMMENT;

						m_curStr += m_data[m_pos];
						m_curStr += m_data[m_pos + 1];
						m_pos += 2;

						continue;
					}
				}

				// nothing yet?  this is a number
				m_curType = genio::IParser::TT_SYMBOL;

				m_curStr += m_data[m_pos];
				m_pos++;
			}
			else if (m_curType == genio::IParser::TT_NUMBER)
			{
				// a decimal point can only be inserted if it's already a number and there's no pre-existing decimal point
				if ((m_data[m_pos] == L'.') && !wcschr(m_curStr.c_str(), L'.'))
				{
					m_curStr += m_data[m_pos];
					m_pos++;

					continue;
				}
			}
			else if ((m_curType == genio::IParser::TT_STRING) || (m_curType == genio::IParser::TT_SHORTCOMMENT))
			{
				m_curStr += m_data[m_pos];
				m_pos++;

				continue;
			}
			else if (m_curType == genio::IParser::TT_LONGCOMMENT)
			{
				bool ast = (m_data[m_pos] == L'*');

				m_curStr += m_data[m_pos];
				m_pos++;

				if (ast && (m_pos < m_datalen) && (m_data[m_pos] == L'/'))
				{
					m_curStr += m_data[m_pos];
					m_pos++;
					break;
				}

				continue;
			}

			break;
		}

		if (wcschr(L"\"'", m_data[m_pos]) != nullptr)
		{
			if (m_curType == genio::IParser::TT_NONE)
			{
				m_curType = genio::IParser::TT_STRING;
				strdelim = m_data[m_pos];
				m_pos++;
			}
			else if (m_curType == genio::IParser::TT_STRING)
			{
				if (strdelim != m_data[m_pos])
				{
					m_curStr += m_data[m_pos];
					m_pos++;
				}
				else
				{
					m_pos++;
					strdelim = L'\0';
					break;
				}
			}
			else if ((m_curType != genio::IParser::TT_LONGCOMMENT) && (m_curType != genio::IParser::TT_SHORTCOMMENT))
				continue;
		}

		if ((m_curType == genio::IParser::TT_STRING) || (m_curType == genio::IParser::TT_SHORTCOMMENT) || (m_curType == genio::IParser::TT_LONGCOMMENT))
		{
			m_curStr += m_data[m_pos];
		}

		m_pos++;
	}

	if (m_curStr.empty())
	{
		return false;
	}

	return true;
}


bool CGenParserW::NextLine()
{
	if (!m_data || !m_datalen)
		return false;

	m_curType = genio::IParser::TT_NONE;
	m_curStr.clear();

	while (m_pos < m_datalen)
	{
		// skip over everything except EOLs
		if (wcschr(L"\n\r", m_data[m_pos]))
		{
			break;
		}

		m_pos++;
	}

	while (m_pos < m_datalen)
	{
		// skip over all EOLs until we hit something else
		if (!wcschr(L"\n\r", m_data[m_pos]))
		{
			return true;
		}

		m_pos++;
	}

	return false;
}

bool CGenParserW::ToEndOfLine()
{
	if (!m_data || !m_datalen)
		return false;

	m_curType = genio::IParser::TT_NONE;
	m_curStr.clear();

	while (m_pos < m_datalen)
	{
		// skip over everything except EOLs
		if (wcschr(L"\n\r", m_data[m_pos]))
		{
			return true;
		}

		m_curStr += m_data[m_pos];
		m_pos++;
	}

	return false;
}

genio::IParser::TOKEN_TYPE CGenParserW::GetCurrentTokenType() const
{
	return m_curType;
}


const wchar_t *CGenParserW::GetCurrentTokenString() const
{
	return m_curStr.c_str();
}

bool CGenParserW::IsToken(const wchar_t *s, bool case_sensitive) const
{
	if (case_sensitive)
		return !wcscmp(m_curStr.c_str(), s);

	return !_wcsicmp(m_curStr.c_str(), s);
}

genio::IParser *genio::IParser::Create(CHAR_MODE mode)
{
	switch (mode)
	{
		case CM_ASCII:
			return new CGenParserA();
			break;

		case CM_UNICODE:
			return new CGenParserW();
			break;
	}

	return nullptr;
}

