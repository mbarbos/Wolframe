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
///\file modules/filter/textwolf/mod_filter_textwolf.cpp
///\brief Module for textwolf XML filters
#include "modules/filter/template/filterBuilder.hpp"
#include "filter/textwolf_filter.hpp"
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

struct TextwolfXMLFilterObject
{
	static SimpleBuilder* builder()
		{return new FilterBuilder<lb::createTextwolfXmlFilterPtr>("xml:textwolf");}
};
struct TextwolfXMLFilterObject_UTF8
{
	static SimpleBuilder* builder()
		{return new FilterBuilder<lb::createTextwolfXmlFilterPtr>("xml:textwolf:UTF-8");}
};
struct TextwolfXMLFilterObject_UTF16BE
{
	static SimpleBuilder* builder()
		{return new FilterBuilder<lb::createTextwolfXmlFilterPtr>("xml:textwolf:UTF-16BE");}
};
struct TextwolfXMLFilterObject_UTF16LE
{
	static SimpleBuilder* builder()
		{return new FilterBuilder<lb::createTextwolfXmlFilterPtr>("xml:textwolf:UTF-16LE");}
};
struct TextwolfXMLFilterObject_UCS2BE
{
	static SimpleBuilder* builder()
		{return new FilterBuilder<lb::createTextwolfXmlFilterPtr>("xml:textwolf:UCS-2BE");}
};
struct TextwolfXMLFilterObject_UCS2LE
{
	static SimpleBuilder* builder()
		{return new FilterBuilder<lb::createTextwolfXmlFilterPtr>("xml:textwolf:UCS-2LE");}
};
struct TextwolfXMLFilterObject_UCS4BE
{
	static SimpleBuilder* builder()
		{return new FilterBuilder<lb::createTextwolfXmlFilterPtr>("xml:textwolf:UCS-4BE");}
};
struct TextwolfXMLFilterObject_UCS4LE
{
	static SimpleBuilder* builder()
		{return new FilterBuilder<lb::createTextwolfXmlFilterPtr>("xml:textwolf:UCS-4LE");}
};

}//anonymous namespace

enum {NofObjects=8};
static createBuilderFunc objdef[ NofObjects] =
{
	TextwolfXMLFilterObject::builder,
	TextwolfXMLFilterObject_UTF8::builder,
	TextwolfXMLFilterObject_UTF16BE::builder,
	TextwolfXMLFilterObject_UTF16LE::builder,
	TextwolfXMLFilterObject_UCS2BE::builder,
	TextwolfXMLFilterObject_UCS2LE::builder,
	TextwolfXMLFilterObject_UCS4BE::builder,
	TextwolfXMLFilterObject_UCS4LE::builder
};


ModuleEntryPoint entryPoint( 0, "textwolf XML filter", setModuleLogger, 0, 0, NofObjects, objdef);
