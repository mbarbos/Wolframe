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
///\file serialize/ddl/FiltermapDDLSerialize.hpp
///\brief Defines the DDL structure serialization for filters

#ifndef _Wolframe_SERIALIZE_DDL_FILTERMAP_SERIALIZE_HPP_INCLUDED
#define _Wolframe_SERIALIZE_DDL_FILTERMAP_SERIALIZE_HPP_INCLUDED
#include "filter/typedfilter.hpp"
#include "serialize/mapContext.hpp"
#include "serialize/ddl/filtermapDDLSerializeStack.hpp"
#include "ddl/structType.hpp"
#include <cstddef>

namespace _Wolframe {
namespace serialize {

class DDLStructSerializer
{
public:
	DDLStructSerializer( const ddl::StructType& st, Context::Flags flags=Context::None)
		:m_st(&st)
		,m_ctx(flags)
	{
		m_stk.push_back( FiltermapDDLSerializeState( &st, langbind::TypedFilterBase::Element()));
	}

	DDLStructSerializer( const DDLStructSerializer& o)
		:m_ctx(o.m_ctx)
		,m_stk(o.m_stk){}

	void init()
	{
		m_ctx.clear();
		m_stk.clear();
		m_stk.push_back( FiltermapDDLSerializeState( m_st, langbind::TypedFilterBase::Element()));
	}

	enum CallResult
	{
		Ok,
		Error,
		Yield
	};

	CallResult print( langbind::TypedOutputFilter& out);

	bool getNext( langbind::FilterBase::ElementType& type, langbind::TypedFilterBase::Element& value);

	const char* getLastError() const			{return m_ctx.getLastError();}
	const char* getLastErrorPos() const			{return m_ctx.getLastErrorPos();}

private:
	const ddl::StructType* m_st;
	Context m_ctx;
	FiltermapDDLSerializeStateStack m_stk;
	int hugo;
};

}}//namespace
#endif

