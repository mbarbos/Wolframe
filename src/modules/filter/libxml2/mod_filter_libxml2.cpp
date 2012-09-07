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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
///\file modules/filter/libxml2/mod_filter_libxml2.cpp
///\brief Module for libxml2 XML filters
#include "modules/filter/template/filterBuilder.hpp"
#include "filter/libxml2_filter.hpp"
#include "logger-v1.hpp"
#include <cstring>

_Wolframe::log::LogBackend* logBackendPtr;

using namespace _Wolframe;
using namespace _Wolframe::module;
namespace lb = _Wolframe::langbind;

static void setModuleLogger( void* logger )
{
	logBackendPtr = reinterpret_cast< _Wolframe::log::LogBackend*>( logger);
}

namespace {

struct Libxml2FilterObject
{
	static SimpleBuilder* builder()
		{return new FilterBuilder<lb::createLibxml2FilterPtr>("xml:libxml2");}
};
struct Libxml2FilterObject_UTF8
{
	static SimpleBuilder* builder()
		{return new FilterBuilder<lb::createLibxml2FilterPtr>("xml:libxml2:UTF-8");}
};
struct Libxml2FilterObject_UTF16BE
{
	static SimpleBuilder* builder()
		{return new FilterBuilder<lb::createLibxml2FilterPtr>("xml:libxml2:UTF-16BE");}
};
struct Libxml2FilterObject_UTF16LE
{
	static SimpleBuilder* builder()
		{return new FilterBuilder<lb::createLibxml2FilterPtr>("xml:libxml2:UTF-16LE");}
};
struct Libxml2FilterObject_UCS2BE
{
	static SimpleBuilder* builder()
		{return new FilterBuilder<lb::createLibxml2FilterPtr>("xml:libxml2:UCS-2BE");}
};
struct Libxml2FilterObject_UCS2LE
{
	static SimpleBuilder* builder()
		{return new FilterBuilder<lb::createLibxml2FilterPtr>("xml:libxml2:UCS-2LE");}
};
struct Libxml2FilterObject_UCS4BE
{
	static SimpleBuilder* builder()
		{return new FilterBuilder<lb::createLibxml2FilterPtr>("xml:libxml2:UCS-4BE");}
};
struct Libxml2FilterObject_UCS4LE
{
	static SimpleBuilder* builder()
		{return new FilterBuilder<lb::createLibxml2FilterPtr>("xml:libxml2:UCS-4LE");}
};

}//anonymous namespace


enum {NofObjects=8};
static createBuilderFunc objdef[ NofObjects] =
{
	Libxml2FilterObject::builder,
	Libxml2FilterObject_UTF8::builder,
	Libxml2FilterObject_UTF16BE::builder,
	Libxml2FilterObject_UTF16LE::builder,
	Libxml2FilterObject_UCS2BE::builder,
	Libxml2FilterObject_UCS2LE::builder,
	Libxml2FilterObject_UCS4BE::builder,
	Libxml2FilterObject_UCS4LE::builder
};


ModuleEntryPoint entryPoint( 0, "libxml2 XML filter", setModuleLogger, 0, 0, NofObjects, objdef);


