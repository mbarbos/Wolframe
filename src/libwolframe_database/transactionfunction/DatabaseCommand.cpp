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
///\brief Internal interface for compiled database commands in the transaction function
///\file transactionfunction/DatabaseCommand.cpp
#include "transactionfunction/DatabaseCommand.hpp"

using namespace _Wolframe;
using namespace _Wolframe::db;

std::string DatabaseCommand::tostring( const TagTable* tagmap) const
{
	std::ostringstream rt;
	rt << "[L" << m_level << "] ";
	if (m_resultsetidx >= 0)
	{
		rt << "FOREACH "<< "RESULT[" << m_resultsetidx << "]" << " DO '" << m_statement << "'( ";
	}
	else if (m_selector.size())
	{
		rt << "FOREACH '"<< m_selector.tostring(tagmap) << "' DO '" << m_statement << "'( ";
	}
	else
	{
		rt << "DO '" << m_statement << "'( ";
	}
	std::vector<Path>::const_iterator ai = m_arg.begin(), ae = m_arg.end();
	int ii = 0;
	for (; ai != ae; ++ai,++ii)
	{
		if (ii) rt << ", ";
		rt << ai->tostring( tagmap);
	}
	rt << " )";
	return rt.str();
}

void DatabaseCommand::rewriteResultReferences( int scope_functionidx_incr)
{
	std::vector<Path>::iterator ai = m_arg.begin(), ae = m_arg.end();
	int ii = 0;
	for (; ai != ae; ++ai,++ii)
	{
		ai->rewriteResultReferences( scope_functionidx_incr);
	}
	m_selector.rewriteResultReferences( scope_functionidx_incr);
	if (m_resultsetidx >= 0)
	{
		m_resultsetidx += scope_functionidx_incr;
	}
}

void DatabaseCommand::rewriteResultReferences( const std::map<int,int>& addrtab)
{
	std::vector<Path>::iterator ai = m_arg.begin(), ae = m_arg.end();
	int ii = 0;
	for (; ai != ae; ++ai,++ii)
	{
		ai->rewriteResultReferences( addrtab);
	}
	m_selector.rewriteResultReferences( addrtab);
	if (m_resultsetidx >= 0)
	{
		std::map<int,int>::const_iterator ti = addrtab.find( m_resultsetidx);
		if (ti == addrtab.end())
		{
			throw std::logic_error( "TDL compiler error: cannot relocate result reference address");
		}
		else
		{
			m_resultsetidx = ti->second;
		}
	}
}
