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
///\file ddl_compiler_SimpleFormCompiler.cpp
///\brief Implementation of a compiler for a self defined form DDL called 'simple form DDL'

#include "ddl/compiler/simpleFormCompiler.hpp"
#include <string>
#include <vector>
#include <map>
#include <cstring>
#include <cstddef>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/info_parser.hpp>

using namespace _Wolframe;
using namespace ddl;

namespace
{

class FRMAttribute
{
public:
	enum Type
	{
		float_=AtomicType::float_,
		int_=AtomicType::int_,
		uint_=AtomicType::uint_,
		string_=AtomicType::string_,
		form_
	};

	static const char* typeName( Type tp)
	{
		static const char* ar[] = {"float","int","uint","string","form",0};
		return ar[ (int)tp];
	}

	static bool getType( const char* name, Type& tp)
	{
		const char* rt;
		unsigned int ii;
		for (ii=0,rt=typeName((Type)(ii)); rt!=0; ii++,rt=typeName((Type)(ii)))
		{
			if (boost::algorithm::iequals( rt, name))
			{
				tp = (Type)ii;
				return true;
			}
		}
		return false;
	}

public:
	FRMAttribute( const FRMAttribute& o)
		:m_isVector(o.m_isVector)
		,m_isAttribute(o.m_isAttribute)
		,m_type(o.m_type)
		,m_value(o.m_value){}

	explicit FRMAttribute( const std::string& item)
		:m_isVector(false),m_isAttribute(false),m_type(string_)
	{
		enum State
		{
			ParseStart,
			ParseName,
			ParseEndName,
			ParseArOpen,
			ParseDfOpen,
			ParseDfValue,
			ParseEnd
		};
		State st = ParseStart;
		std::string vv;

		std::string::const_iterator ii=item.begin(), ee=item.end();
		for (; ii != ee; ++ii)
		{
			switch (st)
			{
				case ParseStart:
					if (*ii == '@')
					{
						++ii;
						m_isAttribute = true;
					}
					st = ParseName;
					/* no break here !*/
				case ParseName:
					if (((*ii|32) >= 'a' && (*ii|32) <= 'z') || (*ii >= '0' && *ii <= '9') || *ii == '_')
					{
						vv.push_back( *ii);
						continue;
					}
					parseType( vv);
					st = ParseEndName;
					/* no break here !*/
				case ParseEndName:
					if (*ii <= ' ' && *ii >= 0)
					{
						break;
					}
					if (*ii == '[')
					{
						m_isVector = true;
						st = ParseArOpen;
						break;
					}
					if (*ii == '(')
					{
						st = ParseDfOpen;
						break;
					}
					throw std::runtime_error( "Syntax error in Simple Form Attribute: '[' or '(' expected");

				case ParseArOpen:
					if (*ii <= ' ' && *ii > 0)
					{
						break;
					}
					if (*ii == ']')
					{
						st = ParseEnd;
						break;
					}
					throw std::runtime_error( "Syntax error in Simple Form Attribute: ']' expected");

				case ParseDfOpen:
					if (*ii <= ' ' && *ii > 0)
					{
						break;
					}
					if (*ii == ')')
					{
						st = ParseEnd;
						break;
					}
					st = ParseDfValue;
					vv.clear();
					/* no break here !*/

				case ParseDfValue:
					if (*ii == ')')
					{
						m_value = vv;
						st = ParseEnd;
						break;
					}
					else
					{
						vv.push_back( *ii);
					}
				break;
				case ParseEnd:
					if (*ii <= ' ' && *ii > 0)
					{
						break;
					}
					throw std::runtime_error( "Syntax error in Simple Form Attribute: Illegal character after end of attribute");
			}
		}
		if (st == ParseName)
		{
			parseType( vv);
		}
		else if (st != ParseEnd)
		{
			throw std::runtime_error( "Syntax error in Simple Form Attribute: Incomplete attribute");
		}
	}

	bool isVector() const			{return m_isVector;}
	bool isAttribute() const		{return m_isAttribute;}
	Type type() const			{return m_type;}
	const std::string& value() const	{return m_value;}

private:
	void parseType( const std::string& typestr)
	{
		if (typestr.size() == 0)
		{
			m_type = form_;
		}
		else if (!getType( typestr.c_str(), m_type))
		{
			throw std::runtime_error( (std::string( "Unknown type: '") += typestr) += "'");
		}
	}
private:
	bool m_isVector;
	bool m_isAttribute;
	Type m_type;
	std::string m_value;
};

}///anonymous namespace

static bool isIdentifier( const std::string& name)
{
	std::string::const_iterator ii=name.begin(), ee=name.end();
	for (;ii!=ee; ++ii)
	{
		if (!(((*ii|32) >= 'a' && (*ii|32) <= 'z') || (*ii >= '0' && *ii <= '9') || *ii == '_'))
		{
			break;
		}
	}
	return (ii==ee);
}

static void compile_ptree( const boost::property_tree::ptree& pt, StructType& result)
{
	boost::property_tree::ptree::const_iterator itr=pt.begin(),end=pt.end();
	for (;itr != end; ++itr)
	{
		if (!isIdentifier( itr->first))
		{
			throw std::runtime_error( "Semantic error: Identifier expected as variable name");
		}
		if (itr->second.begin() == itr->second.end() && itr->second.data().size())
		{
			FRMAttribute fa( itr->second.data());
			if (fa.type() == FRMAttribute::form_)
			{
				throw std::runtime_error( "Semantic error: illegal type specifier");
			}
			AtomicType at( (AtomicType::Type)fa.type());
			at.set( fa.value());
			StructType val;
			if (fa.isVector())
			{
				val.defineAsVector( at);
			}
			else
			{
				val = at;
			}
			if (fa.isAttribute())
			{
				if (itr->first == "_") throw std::runtime_error( "empty attribute name is illegal");
				result.defineAttribute( itr->first, val);
			}
			else
			{
				if (itr->first == "_")
				{
					result.defineContent( "", val);
				}
				else
				{
					result.defineContent( itr->first, val);
				}
			}
		}
		else
		{
			if (itr->second.data().size())
			{
				FRMAttribute fa( itr->second.data());
				if (fa.type() != FRMAttribute::form_)
				{
					throw std::runtime_error( "Semantic error: Atomic type declared as structure");
				}
				if (fa.isAttribute())
				{
					throw std::runtime_error( "Syntax error: Form declared as attribute");
				}
				StructType st;
				if (fa.isVector())
				{
					StructType prototype;
					compile_ptree( itr->second, prototype);
					st.defineAsVector( prototype);
				}
				else
				{
					if (fa.type() == FRMAttribute::form_)
					{
						throw std::runtime_error( "Semantic error: Form declared with default value");
					}
					compile_ptree( itr->second, st);
				}
				result.defineContent( itr->first, st);
			}
			else
			{
				StructType st;
				compile_ptree( itr->second, st);
				result.defineContent( itr->first, st);
			}
		}
	}
}

static std::string getDoctype( boost::property_tree::ptree& pt)
{
	std::string rt;
	boost::property_tree::ptree::iterator itr=pt.begin(),end=pt.end();
	while (itr != end)
	{
		if (itr->second.begin() == itr->second.end() && itr->second.data().size())
		{
			if (itr->first[0] == '!')
			{
				if (boost::algorithm::iequals( itr->first, "!DOCTYPE"))
				{
					if (rt.size()) throw std::runtime_error( "attribute !DOCTYPE defined twice");
					rt = itr->second.data();
				}
				boost::property_tree::ptree::iterator dd = itr;
				itr++;
				pt.erase( dd);
				continue;
			}
		}
		++itr;
	}
	return rt;
}

StructType SimpleFormCompiler::compile( const std::string& srcstring) const
{
	StructType rt;
	std::istringstream src( srcstring);
	boost::property_tree::ptree pt;
	boost::property_tree::info_parser::read_info( src, pt);
	std::string doctype = getDoctype( pt);
	if (doctype.size()) rt.defineDoctype( doctype.c_str());
	compile_ptree( pt, rt);
	return rt;
}

DDLCompiler* _Wolframe::ddl::createSimpleFormCompilerFunc()
{
	return new SimpleFormCompiler();
}


