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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
///\file outputfilterImpl.hpp
///\brief Output filter abstraction for the libxml2 library

#ifndef _Wolframe_LIBXML2_OUTPUT_FILTER_HPP_INCLUDED
#define _Wolframe_LIBXML2_OUTPUT_FILTER_HPP_INCLUDED
#include "documentWriter.hpp"
#include "xsltMapper.hpp"
#include "types/countedReference.hpp"
#include "types/doctype.hpp"
#include "filter/outputfilter.hpp"
#include "libxml/parser.h"
#include "libxml/tree.h"
#include "libxml/encoding.h"
#include "libxml/xmlwriter.h"
#include "libxml/xmlsave.h"
#include <cstdlib>
#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace langbind {

class OutputFilterImpl :public OutputFilter
{
public:
	OutputFilterImpl( const types::CountedReference<std::string>& enc, const XsltMapper& xsltMapper_)
		:types::TypeSignature("langbind::OutputFilterImpl (libxml2)", __LINE__)
		,m_encoding(enc)
		,m_xsltMapper(xsltMapper_)
		,m_nofroot(0)
		,m_taglevel(0)
		,m_elemitr(0){}

	OutputFilterImpl( const types::CountedReference<std::string>& enc)
		:types::TypeSignature("langbind::OutputFilterImpl (libxml2)", __LINE__)
		,m_encoding(enc)
		,m_nofroot(0)
		,m_taglevel(0)
		,m_elemitr(0){}

	OutputFilterImpl( const OutputFilterImpl& o)
		:types::TypeSignature("langbind::OutputFilterImpl (libxml2)", __LINE__)
		,OutputFilter(o)
		,m_encoding(o.m_encoding)
		,m_doc(o.m_doc)
		,m_xsltMapper(o.m_xsltMapper)
		,m_nofroot(o.m_nofroot)
		,m_taglevel(o.m_taglevel)
		,m_attribname(o.m_attribname)
		,m_valuestrbuf(o.m_valuestrbuf)
		,m_elembuf(o.m_elembuf)
		,m_elemitr(o.m_elemitr){}

	virtual ~OutputFilterImpl(){}

	///\brief Implementation of OutputFilter::copy()
	virtual OutputFilterImpl* copy() const
	{
		return new OutputFilterImpl( *this);
	}

	///\brief Implementation of OutputFilter::setDocType( const std::string&)
	virtual void setDocType( const std::string& value);

	///\brief Implementation of OutputFilter::print( ElementType, const void*,std::size_t)
	virtual bool print( ElementType type, const void* element, std::size_t elementsize);

private:
	static const xmlChar* getXmlString( const std::string& aa)
	{
		return (const xmlChar*)aa.c_str();
	}

	const xmlChar* getElement( const void* element, std::size_t elementsize)
	{
		m_valuestrbuf.clear();
		m_valuestrbuf.append( (const char*)element, elementsize);
		return (const xmlChar*)m_valuestrbuf.c_str();
	}

	bool flushBuffer();

private:
	types::CountedReference<std::string> m_encoding;	//< character set encoding
	DocumentWriter m_doc;					//< document writer structure
	XsltMapper m_xsltMapper;				//< optional XSLT mapper
	int m_nofroot;						//< number of root elements parsed
	int m_taglevel;						//< tag hierarchy level
	std::string m_attribname;				//< attribute name buffer
	std::string m_valuestrbuf;				//< value buffer
	std::string m_elembuf;					//< buffer for current element
	std::size_t m_elemitr;					//< iterator on current element
	std::string m_doctype_root;				//< !DOCTYPE root element (1)
	std::string m_doctype_public;				//< !DOCTYPE public element (2)
	std::string m_doctype_system;				//< !DOCTYPE system element (3)
};

}}//namespace
#endif

