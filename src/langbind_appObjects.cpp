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
///\file langbind_appObjects.cpp
///\brief Implementation of scripting language objects
#include "langbind/appObjects.hpp"
#include "serialize/ddl/filtermapDDLParse.hpp"
#include "serialize/ddl/filtermapDDLSerialize.hpp"
#include "ddl/compiler/simpleFormCompiler.hpp"
#include "filter/filter.hpp"
#include "utils/miscUtils.hpp"
#include "filter/typingfilter.hpp"
#include "filter/tostringfilter.hpp"
#include "filter/blob_filter.hpp"
#include "filter/char_filter.hpp"
#include "filter/line_filter.hpp"
#include "filter/token_filter.hpp"
#include <boost/algorithm/string.hpp>
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <string>
#include <fstream>
#include <iostream>
#include <stdexcept>

#ifdef WITH_LIBXML2
#include "filter/libxml2_filter.hpp"
#endif
#include "filter/textwolf_filter.hpp"

using namespace _Wolframe;
using namespace langbind;

namespace //anonymous
{
template <class Object>
void defineObject( std::map<std::string,Object>& m_map, const std::string& name, const Object& obj)
{
	std::string nam( name);
	std::transform( nam.begin(), nam.end(), nam.begin(), ::tolower);
	m_map[ nam] = obj;
}

template <class Object>
bool getObject( const std::map<std::string,Object>& m_map, const std::string& name, Object& obj)
{
	std::string nam( name);
	std::transform( nam.begin(), nam.end(), nam.begin(), ::tolower);
	typename std::map<std::string,Object>::const_iterator ii=m_map.find( nam),ee=m_map.end();
	if (ii == ee)
	{
		return false;
	}
	else
	{
		obj = ii->second;
		return true;
	}
}
}//anonymous namespace

InputFilterR& Input::getIterator()
{
	if (m_used) throw std::runtime_error( "try to read input twice");
	m_used = true;
	return m_inputfilter;
}


void FilterMap::defineFilter( const std::string& name, CreateFilterFunc f)
{
	defineObject( m_map, name, f);
}

bool FilterMap::getFilter( const std::string& name, const std::string& arg, Filter& rt) const
{
	CreateFilterFunc f;
	if (!getObject( m_map, name, f)) return false;
	rt = f( name, arg);
	return true;
}

FilterMap::FilterMap()
{
	defineFilter( "blob", createBlobFilter);
	defineFilter( "char", createCharFilter);
	defineFilter( "line" ,createLineFilter);
	defineFilter( "token", createTokenFilter);
	defineFilter( "xml:textwolf", createTextwolfXmlFilter);
#ifdef WITH_LIBXML2
	defineFilter( "xml", createLibxml2Filter);
	defineFilter( "xml:libxml2", createLibxml2Filter);
#else
	defineFilter( "xml", createTextwolfXmlFilter);
#endif
}


RedirectFilterClosure::RedirectFilterClosure()
	:m_state(0)
	,m_taglevel(0)
	,m_elemtype(InputFilter::Value){}

RedirectFilterClosure::RedirectFilterClosure( const TypedInputFilterR& i, const TypedOutputFilterR& o)
	:m_state(0)
	,m_taglevel(0)
	,m_inputfilter(i)
	,m_outputfilter(o)
	,m_elemtype(InputFilter::Value)
	{}

RedirectFilterClosure::RedirectFilterClosure( const RedirectFilterClosure& o)
	:m_state(o.m_state)
	,m_taglevel(o.m_taglevel)
	,m_inputfilter(o.m_inputfilter)
	,m_outputfilter(o.m_outputfilter)
	,m_elemtype(o.m_elemtype)
	,m_elem(o.m_elem)
	{}

void RedirectFilterClosure::init( const TypedInputFilterR& i, const TypedOutputFilterR& o)
{
	m_inputfilter = i;
	m_outputfilter = o;
	m_state = 0;
	m_taglevel = 0;
	m_elemtype = InputFilter::Value;
}


bool RedirectFilterClosure::call()
{
	if (!m_inputfilter.get()) throw std::runtime_error( "no input defined for redirecting filter");
	if (!m_outputfilter.get()) throw std::runtime_error( "no output defined for redirecting filter");

	for (;;) switch (m_state)
	{
		case 0:
			if (!m_inputfilter->getNext( m_elemtype, m_elem))
			{
				switch (m_inputfilter->state())
				{
					case InputFilter::Open:
						m_state = 2;
						return true;

					case InputFilter::EndOfMessage:
						return false;

					case InputFilter::Error:
						throw std::runtime_error( m_inputfilter->getError());
				}
			}
			m_state = 1;
			if (m_elemtype == InputFilter::OpenTag)
			{
				++m_taglevel;
			}
			else if (m_elemtype == InputFilter::CloseTag)
			{
				--m_taglevel;
				if (m_taglevel < 0)
				{
					m_state = 2;
					return true;
				}
			}
			/*no break here!*/
		case 1:
			if (!m_outputfilter->print( m_elemtype, m_elem))
			{
				switch (m_outputfilter->state())
				{
					case OutputFilter::Open:
						throw std::runtime_error( "unknown error in output filter");

					case OutputFilter::EndOfBuffer:
						return false;

					case OutputFilter::Error:
						throw std::runtime_error( m_outputfilter->getError());
				}
			}
			m_state = 0;
			continue;
		default:
			return true;
	}
}

DDLForm DDLForm::copy() const
{
	DDLForm rt;
	if (m_structure.get())
	{
		rt.m_structure = ddl::StructTypeR( new ddl::StructType( *m_structure));
	}
	return rt;
}

std::string DDLForm::tostring() const
{
	ToStringFilter* flt = new ToStringFilter;
	TypedOutputFilterR out( flt);
	serialize::DDLStructSerializer ser( m_structure);
	ser.init( out, serialize::Context::SerializeWithIndices);
	if (!ser.call())
	{
		if (out->state() == OutputFilter::EndOfBuffer)
		{
			throw std::logic_error( "internal: tostring serialization with yield");
		}
		else
		{
			throw std::runtime_error( ser.getError());
		}
	}
	if (m_structure->doctype())
	{
		std::string rt = "!DOCTYPE \"";
		rt.append( m_structure->doctype());
		rt.append( "\" ");
		rt.append( flt->content());
		return rt;
	}
	return flt->content();
}


void DDLFormMap::defineForm( const std::string& name, const DDLForm& f)
{
	defineObject( m_map, name, f.copy());
}

bool DDLFormMap::getForm( const std::string& name, DDLForm& rt) const
{
	if (!getObject( m_map, name, rt)) return false;
	rt = rt.copy();
	return true;
}


ApiFormData::ApiFormData( const serialize::StructDescriptionBase* descr_)
	:m_descr(descr_)
	,m_data(std::calloc( descr_->size(), 1), std::free)
{
	void* ptr = m_data.get();
	if (!ptr) throw std::bad_alloc();
	if (!m_descr->init( ptr))
	{
		m_descr->done( ptr);
		m_data.reset();
		throw std::bad_alloc();
	}
}

ApiFormData::~ApiFormData()
{
	void* ptr = m_data.get();
	if (ptr) m_descr->done( ptr);
}


FormFunctionClosure::FormFunctionClosure( const FormFunction& f)
	:m_func(f)
	,m_state(0)
	,m_param_data(f.api_param())
	,m_result_data(f.api_result())
	,m_result(m_result_data.data(),m_result_data.descr())
	,m_parser(m_param_data.data(),m_param_data.descr()){}

FormFunctionClosure::FormFunctionClosure( const FormFunctionClosure& o)
	:m_func(o.m_func)
	,m_state(0)
	,m_param_data(o.m_param_data)
	,m_result_data(o.m_result_data)
	,m_result(o.m_result)
	,m_parser(o.m_parser)
	{}

void FormFunctionClosure::init( const TypedInputFilterR& i, serialize::Context::Flags flags)
{
	m_parser.init(i,flags);
}

bool FormFunctionClosure::call()
{
	void* param_struct = m_param_data.get();
	void* result_struct = m_result_data.get();
	switch (m_state)
	{
		case 0:
			if (!m_parser.call()) return false;
			m_state = 1;
		case 1:
			int rt = m_func.call( result_struct, param_struct);
			if (rt != 0)
			{
				std::ostringstream msg;
				msg << "error in call of form function (return code " << rt << ")";
				throw std::runtime_error( msg.str());
			}
			m_state = 2;
	}
	return true;
}

void FormFunctionMap::defineFormFunction( const std::string& name, const FormFunction& f)
{
	defineObject( m_map, name, f);
}

bool FormFunctionMap::getFormFunction( const std::string& name, FormFunction& rt) const
{
	return getObject( m_map, name, rt);
}

TransactionFunctionClosure::TransactionFunctionClosure( const TransactionFunctionR& f)
	:m_func(f)
	,m_state(0)
	,m_inputstruct(f->getInput()){}

TransactionFunctionClosure::TransactionFunctionClosure( const TransactionFunctionClosure& o)
	:m_func(o.m_func)
	,m_state(o.m_state)
	,m_input(o.m_input)
	,m_inputstruct(o.m_inputstruct)
	,m_result(o.m_result){}

bool TransactionFunctionClosure::call()
{
	switch (m_state)
	{
		case 0:
			throw std::runtime_error( "input not initialized");
		case 1:
			if (!m_input.call()) return false;
			m_state = 2;
		case 2:
			m_result = m_func->execute( m_inputstruct.get());
			m_state = 3;
			return true;
		default:
			return true;
	}
}

void TransactionFunctionClosure::init( const TypedInputFilterR& i)
{
	m_inputstruct = m_func->getInput();
	m_input.init( i, m_inputstruct);
	m_state = 1;
}

void TransactionFunctionMap::defineTransactionFunction( const std::string& name, const TransactionFunctionR& f)
{
	defineObject( m_map, name, f);
}

bool TransactionFunctionMap::getTransactionFunction( const std::string& name, TransactionFunctionR& rt) const
{
	return getObject( m_map, name, rt);
}

void TransactionFunctionMap::defineTransactionFunctionType( const std::string& name, CreateTransactionFunction f)
{
	defineObject( m_typemap, name, f);
}

bool TransactionFunctionMap::getTransactionFunctionType( const std::string& name, CreateTransactionFunction& rt) const
{
	return getObject( m_typemap, name, rt);
}


PrintFunctionClosure::PrintFunctionClosure( const prnt::PrintFunctionR& f)
	:m_func(f)
	,m_state(0)
	,m_inputstruct(f->getInput()){}

PrintFunctionClosure::PrintFunctionClosure( const PrintFunctionClosure& o)
	:m_func(o.m_func)
	,m_state(o.m_state)
	,m_input(o.m_input)
	,m_inputstruct(o.m_inputstruct)
	,m_result(o.m_result){}

bool PrintFunctionClosure::call()
{
	switch (m_state)
	{
		case 0:
			throw std::runtime_error( "input not initialized");
		case 1:
			if (!m_input.call()) return false;
			m_state = 2;
		case 2:
			m_result = m_func->execute( m_inputstruct.get());
			m_state = 3;
			return true;
		default:
			return true;
	}
}

void PrintFunctionClosure::init( const TypedInputFilterR& i)
{
	m_inputstruct = m_func->getInput();
	m_input.init( i, m_inputstruct);
	m_state = 1;
}

void PrintFunctionMap::definePrintFunction( const std::string& name, const prnt::PrintFunctionR& f)
{
	defineObject( m_map, name, f);
}

bool PrintFunctionMap::getPrintFunction( const std::string& name, prnt::PrintFunctionR& rt) const
{
	return getObject( m_map, name, rt);
}

void PrintFunctionMap::definePrintFunctionType( const std::string& name, prnt::CreatePrintFunction f)
{
	defineObject( m_typemap, name, f);
}

bool PrintFunctionMap::getPrintFunctionType( const std::string& name, prnt::CreatePrintFunction& rt) const
{
	return getObject( m_typemap, name, rt);
}

void PreparedStatementHandlerMap::definePreparedStatementHandler( const std::string& name, const std::string& dbname, db::CreatePreparedStatementHandlerFunc f)
{
	std::pair<db::CreatePreparedStatementHandlerFunc,std::string> func(f,dbname);
	defineObject( m_map, name, func);
}

bool PreparedStatementHandlerMap::getPreparedStatementHandler( const std::string& name, db::PreparedStatementHandlerR& rt) const
{
	std::pair<db::CreatePreparedStatementHandlerFunc,std::string> func;
	if (!getObject( m_map, name, func)) return false;
	rt = func.first( func.second);
	return true;
}

DDLCompilerMap::DDLCompilerMap()
{
	ddl::CompilerInterfaceR simpleformCompiler( new ddl::SimpleFormCompiler());
	m_map[ simpleformCompiler->ddlname()] = simpleformCompiler;
}

void DDLCompilerMap::defineDDLCompiler( const std::string& name, const ddl::CompilerInterfaceR& f)
{
	defineObject( m_map, name, f);
}

bool DDLCompilerMap::getDDLCompiler( const std::string& name, ddl::CompilerInterfaceR& rt) const
{
	return getObject( m_map, name, rt);
}


bool Output::print( const char* tag, unsigned int tagsize, const char* val, unsigned int valsize)
{
	if (!m_outputfilter.get())
	{
		throw std::runtime_error( "no output sink defined (output ignored)");
	}
	if (tag)
	{
		if (val)
		{
			switch (m_state)
			{
				case 0:
					if (!m_outputfilter->print( OutputFilter::Attribute, tag, tagsize)) break;
					m_state ++;
				case 1:
					if (!m_outputfilter->print( OutputFilter::Value, val, valsize)) break;
					m_state ++;
				case 2:
					m_state = 0;
					return true;
			}
			const char* err = m_outputfilter->getError();
			if (err) throw std::runtime_error( err);
			return false;
		}
		else
		{
			if (!m_outputfilter->print( OutputFilter::OpenTag, tag, tagsize))
			{
				const char* err = m_outputfilter->getError();
				if (err) throw std::runtime_error( err);
				return false;
			}
			return true;
		}
	}
	else if (val)
	{
		if (!m_outputfilter->print( OutputFilter::Value, val, valsize))
		{
			const char* err = m_outputfilter->getError();
			if (err) throw std::runtime_error( err);
			return false;
		}
		return true;
	}
	else
	{
		if (!m_outputfilter->print( OutputFilter::CloseTag, 0, 0))
		{
			const char* err = m_outputfilter->getError();
			if (err) throw std::runtime_error( err);
			return false;
		}
		return true;
	}
}





