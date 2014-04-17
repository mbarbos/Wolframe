/************************************************************************

 Copyright (C) 2011 - 2014 Project Wolframe.
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
///\brief Definition of a preprocessing function call parameter parsing result
///\file tdl/preprocElementReference.hpp
#ifndef _DATABASE_TDL_PREPROC_ELEMENT_REFERENCE_HPP_INCLUDED
#define _DATABASE_TDL_PREPROC_ELEMENT_REFERENCE_HPP_INCLUDED
#include "database/databaseLanguage.hpp"
#include <string>
#include <vector>

namespace _Wolframe {
namespace db {
namespace tdl {

struct PreProcElementReference
{
	enum Type 
	{
		SelectorPath,
		LoopCounter,
		Constant
	};
	Type type;
	std::string name;
	std::string value;

	PreProcElementReference()
		:type(SelectorPath){}
	explicit PreProcElementReference( Type t)
		:type(t){}
	PreProcElementReference( Type t, const std::string& v)
		:type(t),value(v){}
	PreProcElementReference( Type t, const std::string& n, const std::string& v)
		:type(t),name(n),value(v){}
	PreProcElementReference( const PreProcElementReference& o)
		:type(o.type),name(o.name),value(o.value){}

	static PreProcElementReference parse( const LanguageDescription* langdescr, std::string::const_iterator& si, const std::string::const_iterator& se);
};

}}}//namespace
#endif
