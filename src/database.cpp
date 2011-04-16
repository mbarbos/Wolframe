/************************************************************************

 Copyright (C) 2011 Project Wolframe.
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
//
// database.cpp
//

#include "database.hpp"
#include "standardConfigs.hpp"
#include "logger.hpp"

#include <boost/algorithm/string.hpp>

namespace _Wolframe	{
namespace db	{

Database::Database( Configuration& /* config */ )
{
	LOG_TRACE << "Database object created";
}

DatabaseType Database::strToType( const char *str )
{
	if ( boost::algorithm::iequals( str, "PostgreSQL" ))	return DBTYPE_POSTGRESQL;
	else if ( boost::algorithm::iequals( str, "SQLite" ))	return DBTYPE_SQLITE;
	else return DBTYPE_UNKNOWN;
}

std::string& Database::typeToStr( DatabaseType type )
{
	static std::string	retVal[ 3 ] = { "PostgreSQL", "SQLite", "UNKNOWN" };
	switch ( type )	{
	case DBTYPE_POSTGRESQL:	return retVal[0];
	case DBTYPE_SQLITE:	return retVal[1];
	case DBTYPE_UNKNOWN:
	default:		return retVal[2];
	}
}

DatabaseStrategy Database::strToStrategy( const char *str )
{
	if ( boost::algorithm::iequals( str, "round-robin" ))		return DBSTRATEGY_ROUND_ROBIN;
	else if ( boost::algorithm::iequals( str, "roundrobin" ))	return DBSTRATEGY_ROUND_ROBIN;
	else if ( boost::algorithm::iequals( str, "failover" ))		return DBSTRATEGY_FAILOVER;
	else return DBSTRATEGY_UNKNOWN;
}

std::string& Database::strategyToStr( DatabaseStrategy strategy )
{
	static std::string	retVal[ 3 ] = { "round-robin", "failover", "UNKNOWN" };
	switch ( strategy )	{
	case DBSTRATEGY_ROUND_ROBIN:	return retVal[0];
	case DBSTRATEGY_FAILOVER:	return retVal[1];
	case DBSTRATEGY_UNKNOWN:
	default:			return retVal[2];
	}
}
}} // namespace _Wolframe::db
