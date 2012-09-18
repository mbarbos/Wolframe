/************************************************************************
Copyright (C) 2011, 2012 Project Wolframe.
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
///\file module/ddlcompilerBuilder.hpp
///\brief Interface template for object builder of form DDL compilers
#ifndef _Wolframe_MODULE_DDL_COMPILER_OBJECT_BUILDER_TEMPLATE_HPP_INCLUDED
#define _Wolframe_MODULE_DDL_COMPILER_OBJECT_BUILDER_TEMPLATE_HPP_INCLUDED
#include "ddl/compilerInterface.hpp"
#include "moduleInterface.hpp"
#include "constructor.hpp"

namespace _Wolframe {
namespace module {

class DDLCompilerConstructor :public SimpleObjectConstructor< ddl::DDLCompiler >
{
public:
	DDLCompilerConstructor( const char* name_, ddl::CreateDDLCompilerFunc createFunc_ )
		: m_name(name_)
		, m_createFunc(createFunc_) {}

	virtual ~DDLCompilerConstructor(){}

	virtual ObjectConstructorBase::ObjectType objectType() const
	{
		return DDL_COMPILER_OBJECT;
	}
	virtual const char* objectClassName() const
	{
		return m_name.c_str();
	}
	virtual ddl::DDLCompiler* object() const
	{
		return m_createFunc();
	}

private:
	const std::string m_name;
	ddl::CreateDDLCompilerFunc m_createFunc;
};

class DDLCompilerBuilder :public SimpleBuilder
{
public:
	DDLCompilerBuilder( const char* name_, ddl::CreateDDLCompilerFunc createFunc_)
		:SimpleBuilder( name_)
		,m_createFunc(createFunc_){}

	virtual ~DDLCompilerBuilder(){}

	virtual ObjectConstructorBase::ObjectType objectType() const
	{
		return ObjectConstructorBase::DDL_COMPILER_OBJECT;
	}
	virtual ObjectConstructorBase* constructor()
	{
		return new DDLCompilerConstructor( m_className, m_createFunc);
	}
private:
	ddl::CreateDDLCompilerFunc m_createFunc;
};

}}//namespace

#endif

