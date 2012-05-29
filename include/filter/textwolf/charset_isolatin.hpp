/*
---------------------------------------------------------------------
    The template library textwolf implements an input iterator on
    a set of XML path expressions without backward references on an
    STL conforming input iterator as source. It does no buffering
    or read ahead and is dedicated for stream processing of XML
    for a small set of XML queries.
    Stream processing in this context refers to processing the
    document without buffering anything but the current result token
    processed with its tag hierarchy information.

    Copyright (C) 2010 Patrick Frey

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

--------------------------------------------------------------------

	The latest version of textwolf can be found at 'http://github.com/patrickfrey/textwolf'
	For documentation see 'http://patrickfrey.github.com/textwolf'

--------------------------------------------------------------------
*/
#ifndef __TEXTWOLF_CHARSET_ISOLATIN_HPP__
#define __TEXTWOLF_CHARSET_ISOLATIN_HPP__
#include "textwolf/char.hpp"
#include "textwolf/charset_interface.hpp"
#include "textwolf/exception.hpp"
#include <cstddef>

namespace textwolf {
namespace charset {

///\class IsoLatin1
///\brief Character set IsoLatin-1 (ISO-8859-1)
struct IsoLatin1
{
	enum {MaxChar=0xFF};

	///\brief See template<class Iterator>Interface::skip(char*,unsigned int&,Iterator&)
	template <class Iterator>
	static void skip( char*, unsigned int& bufpos, Iterator& itr)
	{
		if (bufpos==0)
		{
			++itr;
			++bufpos;
		}
	}

	///\brief See template<class Iterator>Interface::asciichar(char*,unsigned int&,Iterator&)
	template <class Iterator>
	static char asciichar( char* buf, unsigned int& bufpos, Iterator& itr)
	{
		if (bufpos==0)
		{
			buf[0] = *itr;
			++itr;
			++bufpos;
		}
		return ((unsigned char)(buf[0])>127)?-1:buf[0];
	}

	///\brief See template<class Iterator>Interface::value(char*,unsigned int&,Iterator&)
	template <class Iterator>
	static UChar value( char* buf, unsigned int& bufpos, Iterator& itr)
	{
		if (bufpos == 0)
		{
			buf[0] = *itr;
			++itr;
			++bufpos;
		}
		return (unsigned char)buf[0];
	}

	///\brief See template<class Buffer>Interface::print(UChar,Buffer&)
	template <class Buffer_>
	static void print( UChar chr, Buffer_& buf)
	{
		char chr_ = (char)(unsigned char)chr;
		if (chr > 255)
		{
			char tb[ 32];
			char* cc = tb;
			Encoder::encode( chr, tb, sizeof(tb));
			while (*cc) buf.push_back( *cc++);
		}
		buf.push_back( chr_);
	}
};

}//namespace
}//namespace
#endif
