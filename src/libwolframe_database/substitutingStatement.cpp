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
///\brief Implements substitution of parameters in embedded SQL statements
///\file substitutingStatement.cpp

#include "database/substitutingStatement.hpp"

#include <stdexcept>
#include <boost/lexical_cast.hpp>

using namespace _Wolframe;
using namespace _Wolframe::db;

SubstitutingStatement::SubstitutingStatement( )
	: BaseStatement( )
{
}

SubstitutingStatement::SubstitutingStatement( const SubstitutingStatement &o )
	: BaseStatement( o ),
	m_bind( o.m_bind ),
	m_data( o.m_data )
{
}

void SubstitutingStatement::init( const std::string &stmtStr )
{
	BaseStatement::init( stmtStr );
}

void SubstitutingStatement::clear( )
{
	BaseStatement::clear( );
	
	m_bind.clear( );
	m_data.clear();
}

void SubstitutingStatement::bind( unsigned int idx, const types::Variant &value )
{
	// does boundary check
	BaseStatement::bind( idx, value );
	
	m_bind[idx] = convert( value );
}

const std::string SubstitutingStatement::nativeSQL( ) const
{
	// doesn't work this way!
	std::string rt;
	//~ std::vector<Element>::const_iterator di = m_data.begin( ), de = m_data.end( );
	//~ for( ; di != de; di++ ) {
		//~ if( di->first ) {
			//~ std::map<unsigned int, std::string>::const_iterator bi = m_bind.find( di->first );
			//~ if( bi == m_bind.end( ) ) {
				//~ throw std::runtime_error( std::string( "parameter $" ) + boost::lexical_cast<std::string>( di->first ) + " undefined" );
			//~ }
			//~ rt.append( bi->second );
		//~ } else {
			//~ rt.append( di->second );
		//~ }
	//~ }
	return rt;
}

//~ void BaseStatement::init( const std::string& stmstr)
//~ {
		//~ if (*si == '$')
		//~ {
			//~ if (si > chunkstart)
			//~ {
				//~ m_data.push_back( Element( 0, std::string( chunkstart, si)));
			//~ }
			//~ std::string idxstr;
			//~ for (++si; si != se && *si >= '0' && *si <= '9'; ++si)
			//~ {
				//~ idxstr.push_back( *si);
			//~ }
			//~ chunkstart = si;
			//~ if (idxstr.empty()) throw std::runtime_error( "only parameters referenced by index supported until now in database statements");
			//~ unsigned int idx = boost::lexical_cast<unsigned int>( idxstr);
			//~ if (idx == 0) throw std::runtime_error( "parameter index out of range");
			//~ if (idx > m_maxparam) m_maxparam = idx;
			//~ m_data.push_back( Element( idx, ""));
			//~ if (si == se) break;
		//~ }
	//~ }
	//~ if (si > chunkstart)
	//~ {
		//~ m_data.push_back( Element( 0, std::string( chunkstart, si)));
	//~ }
