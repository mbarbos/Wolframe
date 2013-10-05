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
///\file xsltProgramType.cpp
///\brief Function to create a program type object for xslt filters
#include "xsltProgramType.hpp"
#include "xsltMapper.hpp"
#include "inputfilterImpl.hpp"
#include "outputfilterImpl.hpp"
#include "processor/procProvider.hpp"
#include "module/filterBuilder.hpp"
#include "utils/fileUtils.hpp"
#include "logger-v1.hpp"
#include <boost/shared_ptr.hpp>
#include <libxslt/xslt.h>
#include <libxslt/xsltInternals.h>
#include <libxslt/transform.h>
#include <libxslt/xsltutils.h>
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>

using namespace _Wolframe;
using namespace _Wolframe::langbind;

namespace {

struct XsltFilter :public Filter
{
	XsltFilter( const xsltStylesheetPtr stylesheet_, const std::vector<langbind::FilterArgument>& arg)
	{
		XsltMapper xsltmapper( stylesheet_, arg);
		InputFilterImpl impl( xsltmapper);
		m_inputfilter.reset( new BufferingInputFilter( &impl));
		m_outputfilter.reset( new OutputFilterImpl( xsltmapper));
	}
};


class XsltFilterConstructor
	:public module::FilterConstructor
{
public:
	XsltFilterConstructor( const std::string& sourcefile_)
		:module::FilterConstructor( utils::getFileStem(sourcefile_), 0)
	{
		xsltStylesheetPtr pp = xsltParseStylesheetFile( (const xmlChar *)sourcefile_.c_str());
		if (!pp)
		{
			xmlError* err = xmlGetLastError();
			throw std::runtime_error( std::string("error loading XSLT program '") + sourcefile_ + "': '" + (err?err->message:"unspecified XSLT error") + "'");
		}
		m_ptr = boost::shared_ptr<xsltStylesheet>( pp, xsltFreeStylesheet);
	}

	virtual ~XsltFilterConstructor(){}

	virtual langbind::Filter* object( const std::vector<langbind::FilterArgument>& arg) const
	{
		return new XsltFilter( m_ptr.get(), arg);
	}

private:
	boost::shared_ptr<xsltStylesheet> m_ptr;
};


class XsltProgramType
	:public prgbind::Program
{
public:
	XsltProgramType()
		:prgbind::Program( prgbind::Program::Function){}

	virtual ~XsltProgramType(){}

	virtual bool is_mine( const std::string& filename) const
	{
		boost::filesystem::path p( filename);
		return p.extension().string() == ".xslt";
	}

	virtual void loadProgram( prgbind::ProgramLibrary& library, db::Database* /*transactionDB*/, const std::string& filename)
	{
		module::FilterConstructorR fc( new XsltFilterConstructor( filename));
		library.defineFilterConstructor( fc);
	}
};
}//anonymous namespace

prgbind::Program* langbind::createXsltProgramType()
{
	return new XsltProgramType;
}
