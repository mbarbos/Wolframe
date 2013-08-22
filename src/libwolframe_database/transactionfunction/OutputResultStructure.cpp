/************************************************************************

 Copyright (C) 2011 - 2013 Project Wolframe.
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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
///\brief Implementation of output data structure for transaction function
///\file transactionfunction/OutputResultStructure.hpp
#include "transactionfunction/OutputResultStructure.hpp"

using namespace _Wolframe;
using namespace _Wolframe::db;

TransactionFunctionOutput::ResultStructure::ResultStructure()
{
	m_strings.push_back( '\0');
}
TransactionFunctionOutput::ResultStructure::ResultStructure( const ResultStructure& o)
	:m_ar(o.m_ar),m_strings(o.m_strings){}

TransactionFunctionOutput::ResultStructure::ContentElement::ContentElement()
	:value(0),type(ResultElement::CloseTag),idx(0){}

TransactionFunctionOutput::ResultStructure::ContentElement::ContentElement( const ContentElement& o)
	:value(o.value),type(o.type),idx(o.idx){}

TransactionFunctionOutput::ResultStructure::const_iterator::const_iterator( const ResultStructure* struct_)
	:m_struct(struct_)
{
	if (m_struct)
	{
		m_itr = m_struct->m_ar.begin();
		init();
	}
}

TransactionFunctionOutput::ResultStructure::const_iterator::const_iterator( const const_iterator& o)
	:m_content(o.m_content)
	,m_itr(o.m_itr)
	,m_struct(o.m_struct){}

bool TransactionFunctionOutput::ResultStructure::const_iterator::operator==(const const_iterator& o) const
{
	if (!o.m_struct)
	{
		if (!m_struct) return true;
		if (m_itr == m_struct->m_ar.end()) return true;
	}
	else if (!m_struct)
	{
		if (!o.m_struct) return true;
		if (o.m_itr == o.m_struct->m_ar.end()) return true;
	}
	else if (m_struct == o.m_struct && m_itr == o.m_itr)
	{
		return true;
	}
	return false;
}


void TransactionFunctionOutput::ResultStructure::const_iterator::init()
{
	if (m_itr != m_struct->m_ar.end())
	{
		m_content.value = (m_itr->type() != ResultElement::OpenTag)?0:(m_itr->idx()==0?0:m_struct->m_strings.c_str()+m_itr->idx());
		m_content.idx = m_itr->idx();
		m_content.type = m_itr->type();
	}
	else
	{
		m_content.value = 0;
		m_content.idx = 0;
		m_content.type = ResultElement::CloseTag;
	}
}

void TransactionFunctionOutput::ResultStructure::openTag( const std::string& name)
{
	m_ar.push_back( ResultElement( ResultElement::OpenTag, m_strings.size()));
	m_strings.append( name);
	m_strings.push_back( '\0');
}

void TransactionFunctionOutput::ResultStructure::closeTag()
{
	m_ar.push_back( ResultElement( ResultElement::CloseTag, 0));
}

void TransactionFunctionOutput::ResultStructure::addValueReference( std::size_t functionidx)
{
	m_ar.push_back( ResultElement( ResultElement::Value, functionidx));
}

void TransactionFunctionOutput::ResultStructure::addMark( ResultElement::Type mrk, std::size_t functionidx)
{
	m_ar.push_back( ResultElement( mrk, functionidx));
}

void TransactionFunctionOutput::ResultStructure::addEmbeddedResult( const ResultStructure& o, std::size_t functionidx)
{
	const_iterator ri = o.begin(), re = o.end();
	for (; ri != re; ++ri)
	{
		switch (ri->type)
		{
			case ResultElement::Value:
				addValueReference( functionidx + ri->idx);
				break;
			case ResultElement::OpenTag:
				openTag( ri->value);
				break;
			case ResultElement::CloseTag:
				closeTag();
				break;
			case ResultElement::FunctionStart:
			case ResultElement::FunctionEnd:
			case ResultElement::IndexStart:
			case ResultElement::IndexEnd:
			case ResultElement::OperationStart:
			case ResultElement::OperationEnd:
				addMark( ri->type, functionidx + ri->idx);
				break;
		}
	}
}

std::string TransactionFunctionOutput::ResultStructure::tostring() const
{
	std::ostringstream rt;
	const_iterator ri = begin(), re = end();
	for (; ri != re; ++ri)
	{
		if (ri->type == ResultElement::OpenTag)
		{
			rt << ResultElement::typeName(ri->type) << " '" << (ri->value?ri->value:"") << "'; ";
		}
		else if (ri->type == ResultElement::CloseTag)
		{
			rt << ResultElement::typeName(ri->type) << "; ";
		}
		else
		{
			rt << ResultElement::typeName(ri->type) << " " << ri->idx<< "; ";
		}
	}
	return rt.str();
}

