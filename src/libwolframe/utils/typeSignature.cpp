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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
/// \file utils/typeSignature.cpp
/// \brief Implementation of signature for structures to detect memory problems
#include "utils/typeSignature.hpp"
#include <boost/lexical_cast.hpp>

using namespace _Wolframe;
using namespace _Wolframe::utils;

#ifdef _Wolframe_LOWLEVEL_DEBUG
void TypeSignature::verify() const
{
	if (m_objid != m_objid || m_objinvid != getStmp(m_objid) || m_chk != chk())
	{
		throw std::runtime_error( std::string( "internal: memory curruption detected for object ") + m_objname + " line:" + boost::lexical_cast<std::string>(m_objid) + ")");
	}
}
#endif

#ifdef BOOST_MSVC
// avoids LNK4221 on Windows (see #69)
namespace { char dummy; }
#endif
