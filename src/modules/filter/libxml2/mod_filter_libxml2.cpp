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
///\file mod_filter_libxml2.cpp
///\brief Module for libxml2 XML filters
#include "appdevel/filterModuleMacros.hpp"
#include "libxml2_filter.hpp"
#if WITH_LIBXSLT
#include "xsltProgramType.hpp"
#endif

namespace lb = _Wolframe::langbind;

#if WITH_LIBXSLT
FILTER_WITH_SOURCE_MODULE( "libxml2 filter and xslt program type", 
				libxml2, _Wolframe::langbind::createLibxml2FilterType,
				xslt, _Wolframe::langbind::createXsltProgramType)
#else
FILTER_MODULE( "libxml2 filter", libxml2, _Wolframe::langbind::createLibxml2FilterType)
#endif

