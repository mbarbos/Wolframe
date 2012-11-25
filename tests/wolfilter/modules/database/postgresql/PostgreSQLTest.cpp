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
//
// Wolframe PostgreSQL test client implementation
//
#include "logger-v1.hpp"
#include "PostgreSQLTest.hpp"
#include "utils/miscUtils.hpp"
#include <boost/system/error_code.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include "libpq-fe.h"

using namespace _Wolframe;
using namespace _Wolframe::db;

static std::string buildConnStr( const std::string& host, unsigned short port,
				 const std::string& user, const std::string& password,
				 const std::string& dbName )
{
	std::stringstream ss;

	if ( ! host.empty())
		ss << "host = '" << host << "'";
	if ( port != 0 )	{
		if ( ! ss.str().empty())
			ss << " ";
		ss << "port = " << port;
	}
	if ( ! dbName.empty())	{
		if ( ! ss.str().empty())
			ss << " ";
		ss << "dbname = '" << dbName << "'";
	}
	if ( ! user.empty())	{
		if ( ! ss.str().empty())
			ss << " ";
		ss << "user = '" << user << "'";
		if ( ! password.empty())	{
			if ( ! ss.str().empty())
				ss << " ";
			ss << "password = '" << password << "'";
		}
	}

	return ss.str();
}

static void createTestDatabase( const std::string& host, unsigned short port,
				const std::string& user, const std::string& password,
				const std::string& dbName, const std::string& inputfile )
{
	std::string connStr = buildConnStr( host, port, user, password, dbName );
	PGconn* conn = PQconnectdb( connStr.c_str() );

	if ( PQstatus( conn ) != CONNECTION_OK )	{
		std::string msg = std::string( "Connection to database failed: " ) + PQerrorMessage( conn );
		PQfinish( conn );
		throw std::runtime_error( msg );
	}
	// Get a list of tables (in the public schema)
	PGresult* res = PQexec( conn, "SELECT table_name FROM information_schema.tables WHERE table_schema = 'public'" );
	if ( PQresultStatus( res ) != PGRES_TUPLES_OK )	{
		std::string msg = std::string( "Failed to list tables: " ) + PQerrorMessage( conn );
		PQclear( res );
		PQfinish( conn );
		throw std::runtime_error( msg );
	}
	std::list< std::string > tables;
	for ( int i = 0; i < PQntuples( res ); i++ )
		tables.push_back( PQgetvalue( res, i, 0 ));
	// Drop the tables
	for ( std::list< std::string >::const_iterator it = tables.begin();
							it != tables.end(); it++ )	{
		std::string query = "DROP TABLE " + *it;
		PQclear( res );
		res = PQexec( conn, query.c_str() );
		if ( PQresultStatus( res ) != PGRES_TUPLES_OK )	{
			std::string msg = std::string( "Failed to delete table " ) + *it
					  + ": " + PQerrorMessage( conn );
			PQfinish( conn );
			throw std::runtime_error( msg );
		}
	}

	// Now create the test database
	std::string dbsource = utils::readSourceFileContent( inputfile );
	res = PQexec( conn, dbsource.c_str() );
	if ( PQresultStatus( res ) != PGRES_TUPLES_OK )	{
		std::string msg = std::string( "Failed to create PostgrSQL test database: " ) + PQerrorMessage( conn );
		PQclear( res );
		PQfinish( conn );
		throw std::runtime_error( msg );
	}

	PQclear( res );
	PQfinish( conn );
}

static void dumpDatabase( const std::string& host, unsigned short port,
			  const std::string& user, const std::string& password,
			  const std::string& dbName, const std::string& outputfile )
{
	FILE *fh = fopen( outputfile.c_str(), "w" );
	if ( fh == NULL )
		throw std::runtime_error( std::string( "failed to open file for database dump (" )
					  + boost::lexical_cast<std::string>( errno ) + "), file '"
					  + outputfile + "'");

	std::string connStr = buildConnStr( host, port, user, password, dbName );
	PGconn* conn = PQconnectdb( connStr.c_str() );

	if ( PQstatus( conn ) != CONNECTION_OK )	{
		std::string msg = std::string( "Connection to database failed: " ) + PQerrorMessage( conn );
		PQfinish( conn );
		throw std::runtime_error( msg );
	}
	// Get a list of tables (in the public schema)
	PGresult* res = PQexec( conn, "SELECT table_name FROM information_schema.tables WHERE table_schema = 'public'" );
	if ( PQresultStatus( res ) != PGRES_TUPLES_OK )	{
		std::string msg = std::string( "Failed to list tables: " ) + PQerrorMessage( conn );
		PQclear( res );
		PQfinish( conn );
		throw std::runtime_error( msg );
	}
	std::list< std::string > tables;
	for ( int i = 0; i < PQntuples( res ); i++ )
		tables.push_back( PQgetvalue( res, i, 0 ));
	// Dump the tables
	for ( std::list< std::string >::const_iterator it = tables.begin();
							it != tables.end(); it++ )	{
		std::string query = "SELECT * FROM " + *it;
		PQclear( res );
		res = PQexec( conn, query.c_str() );
		if ( PQresultStatus( res ) != PGRES_TUPLES_OK )	{
			std::string msg = std::string( "Failed to dump table " ) + *it
					  + ": " + PQerrorMessage( conn );
			PQfinish( conn );
			throw std::runtime_error( msg );
		}

		fprintf( fh, "%s:\n", it->c_str() );
		int nFields = PQnfields( res );
		for ( int i = 0; i < PQntuples( res ); i++ )	{
			for ( int j = 0; j < nFields; j++ )
				fprintf( fh, j ? ", %s" : "%s", PQgetvalue( res, i, j ) );
			fprintf( fh, "\n" );
		}
	}

	PQclear( res );
	PQfinish( conn );
}

void PostgreSQLTestConstructor::createTestDatabase( const PostgreSQLTestConfig& cfg )
{
}

config::ConfigurationTree PostgreSQLTestConfig::extractMyNodes( const config::ConfigurationTree& pt )
{
	boost::property_tree::ptree rt;
	boost::property_tree::ptree::const_iterator pi = pt.begin(), pe = pt.end();

	for ( ; pi != pe; ++pi )	{
		if ( boost::algorithm::iequals( pi->first, "inputfile" ))
			m_input_filename = pi->second.data();
		else if ( boost::algorithm::iequals( pi->first, "dumpfile" ))
			m_dump_filename = pi->second.data();
		else
			rt.add_child( pi->first, pi->second );
	}
	return rt;
}

void PostgreSQLTestConfig::setMyCanonicalPathes( const std::string& referencePath )
{
	if ( !m_input_filename.empty() )
		m_input_filename = utils::getCanonicalPath( m_input_filename, referencePath );
	if (!m_dump_filename.empty())
		m_dump_filename = utils::getCanonicalPath( m_dump_filename, referencePath );
}

void PostgreSQLTestConfig::dump_database()
{
}


