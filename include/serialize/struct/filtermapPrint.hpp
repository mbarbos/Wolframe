/************************************************************************
Copyright (C) 2011 Project Wolframe.
All rights reserved.

This file is part of Project Wolframe.

Commercial Usage
Licensees holding valid Project Wolframe Commercial licenses may
use this file in accordance with the Project Wolframe
Commercial License Agreement provided with the Software or,
alternatively, in accordance with the terms contained
in a written agreement between the licensee and Project Wolframe.

GNU General Public License Usage
Alternatively, you can redistribute this file and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Wolframe is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
///\file serialize/struct/filtermapPrint.hpp
///\brief Defines the intrusive implementation of the printing part of serialization of filters

#ifndef _Wolframe_SERLIALIZE_STRUCT_FILTERMAP_PRINT_HPP_INCLUDED
#define _Wolframe_SERLIALIZE_STRUCT_FILTERMAP_PRINT_HPP_INCLUDED
#include "serialize/struct/filtermapTraits.hpp"
#include "serialize/struct/filtermapBase.hpp"
#include <stdexcept>
#include <string>
#include <vector>
#include <map>

namespace _Wolframe {
namespace serialize {

static bool printElem( protocol::OutputFilter::ElementType tp, const void* elem, std::size_t elemsize, protocol::OutputFilter*& out, Context& ctx)
{
	if (!out->print( tp, elem, elemsize))
	{
		ctx.append( out->charptr(), out->pos());
		out->release();
		if (!out->print( tp, elem, elemsize))
		{
			protocol::OutputFilter* ff = out->createFollow();
			if (ff)
			{
				delete out;
				out = ff;
			}
			if (!out->print( tp, elem, elemsize))
			{
				ctx.setError( out->getError());
				return false;
			}
		}
	}
	return true;
}

template <typename T>
static bool printObject( const char* tag, const T& obj, protocol::OutputFilter*& out, Context& ctx);


template <typename T>
bool print_( const char* tag, const void* obj, const struct_&, protocol::OutputFilter*& out, Context& ctx)
{
	static const FiltermapDescriptionBase* descr = T::getFiltermapDescription();
	bool isContent = true;

	if (tag && !printElem( protocol::OutputFilter::OpenTag, tag, std::strlen(tag), out, ctx)) return false;

	FiltermapDescriptionBase::Map::const_iterator itr = descr->begin(),end = descr->end();
	for (std::size_t idx=0; itr != end; ++itr,++idx)
	{
		if (itr->second.isAtomic() && !isContent && idx < descr->nof_attributes())
		{
			if (!printElem( protocol::OutputFilter::Attribute, itr->first, std::strlen(itr->first), out, ctx)) return false;
			if (!itr->second.print()( 0, const_cast<char*>((const char *)obj+itr->second.ofs()), out, ctx)) return false;
		}
		else
		{
			isContent = true;
			if (!itr->second.print()( itr->first, const_cast<char*>((const char *)obj+itr->second.ofs()), out, ctx)) return false;
		}
	}
	if (tag && !printElem( protocol::OutputFilter::CloseTag, "", 0, out, ctx)) return false;
	return true;
}

template <typename T>
bool print_( const char* tag, const void* obj, const arithmetic_&, protocol::OutputFilter*& out, Context& ctx)
{
	if (tag && !printElem( protocol::OutputFilter::OpenTag, tag, std::strlen(tag), out, ctx)) return false;
	std::string value( boost::lexical_cast<std::string>( *(const_cast<T*>((const T*)obj))));
	if (!printElem( protocol::OutputFilter::Value, value.c_str(), value.size(), out, ctx)) return false;
	if (tag && !printElem( protocol::OutputFilter::CloseTag, "", 0, out, ctx)) return false;
	return true;
}

template <typename T>
bool print_( const char* tag, const void* obj, const bool_&, protocol::OutputFilter*& out, Context& ctx)
{
	if (tag && !printElem( protocol::OutputFilter::OpenTag, tag, std::strlen(tag), out, ctx)) return false;
	if (!printElem( protocol::OutputFilter::Value, (*((T*)obj))?"t":"f", 1, out, ctx)) return false;
	if (tag && !printElem( protocol::OutputFilter::CloseTag, "", 0, out, ctx)) return false;
	return true;
}

template <typename T>
bool print_( const char* tag, const void* obj, const vector_&, protocol::OutputFilter*& out, Context& ctx)
{
	if (!tag)
	{
		ctx.setError( 0, "non printable structure");
		return false;
	}
	for (typename T::const_iterator itr=(const_cast<T*>((const T*)obj))->begin(); itr!=(const_cast<T*>((const T*)obj))->end(); itr++)
	{
		printElem( protocol::OutputFilter::OpenTag, tag, strlen(tag), out, ctx);
		if (!printObject<typename T::value_type>( 0, *itr, out, ctx)) return false;
		printElem( protocol::OutputFilter::CloseTag, "", 0, out, ctx);
	}
	return true;
}

template <typename T>
static bool printObject( const char* tag, const T& obj, protocol::OutputFilter*& out, Context& ctx)
{
	return print_<T>( tag, (const void*)&obj, getCategory(obj), out, ctx);
}

template <typename T>
struct IntrusivePrinter
{
	static bool print( const char* tag, const void* obj, protocol::OutputFilter*& out, Context& ctx)
	{
		if (!print_<T>( tag, obj, getCategory(*const_cast<T*>((const T*)obj)), out, ctx)) return false;
		ctx.append( out->charptr(), out->pos());
		out->release();
		return true;
	}
};

}}//namespace
#endif

