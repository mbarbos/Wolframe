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
// PostgreSQL configuration parser
//

#include "PostgreSQL.hpp"
#include "config/valueParser.hpp"
#include "config/ConfigurationTree.hpp"

#include <boost/algorithm/string.hpp>
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>
#include "utils/miscUtils.hpp"

using namespace _Wolframe::utils;

static const unsigned short DEFAULT_POSTGRESQL_CONNECTIONS = 4;
static const unsigned short DEFAULT_CONNECTION_TIMEOUT = 30;
static const unsigned short DEFAULT_STATEMENT_TIMEOUT = 30000;

namespace _Wolframe {
namespace db {

//***  PostgreSQL configuration functions  **********************************
PostgreSQLconfig::PostgreSQLconfig( const char* cfgName, const char* logParent, const char* logName )
	: config::NamedConfiguration( cfgName, logParent, logName )
{
	port = 0;
	connections = 0;
	acquireTimeout = 0;
}

bool PostgreSQLconfig::parse( const config::ConfigurationTree& pt, const std::string& /*node*/,
			      const module::ModulesDirectory* /*modules*/ )
{
	using namespace _Wolframe::config;
	bool retVal = true;
	bool portDefined, connDefined, aTdefined, cTdefined, sTdefined;
	portDefined = connDefined = aTdefined = cTdefined = sTdefined = false;

	for ( boost::property_tree::ptree::const_iterator L1it = pt.begin(); L1it != pt.end(); L1it++ )	{
		if ( boost::algorithm::iequals( L1it->first, "identifier" ))	{
			bool isDefined = ( !m_ID.empty() );
			if ( !Parser::getValue( logPrefix().c_str(), *L1it, m_ID, &isDefined ))
				retVal = false;
		}
		else if ( boost::algorithm::iequals( L1it->first, "host" ))	{
			bool isDefined = ( !host.empty());
			if ( !Parser::getValue( logPrefix().c_str(), *L1it, host, &isDefined ))
				retVal = false;
		}
		else if ( boost::algorithm::iequals( L1it->first, "port" ))	{
			if ( !Parser::getValue( logPrefix().c_str(), *L1it, port,
						Parser::RangeDomain<unsigned short>( 1 ), &portDefined ))
				retVal = false;
		}
		else if ( boost::algorithm::iequals( L1it->first, "database" ))	{
			bool isDefined = ( !dbName.empty());
			if ( !Parser::getValue( logPrefix().c_str(), *L1it, dbName, &isDefined ))
				retVal = false;
		}
		else if ( boost::algorithm::iequals( L1it->first, "user" ))	{
			bool isDefined = ( !user.empty());
			if ( !Parser::getValue( logPrefix().c_str(), *L1it, user, &isDefined ))
				retVal = false;
		}
		else if ( boost::algorithm::iequals( L1it->first, "password" ))	{
			bool isDefined = ( !password.empty());
			if ( !Parser::getValue( logPrefix().c_str(), *L1it, password, &isDefined ))
				retVal = false;
		}
		else if ( boost::algorithm::iequals( L1it->first, "sslMode" ))	{
			bool isDefined = ( !sslMode.empty());
			if ( !Parser::getValue( logPrefix().c_str(), *L1it, sslMode, &isDefined ))
				retVal = false;
			if ( boost::algorithm::iequals( sslMode, "disable" ))
				sslMode = "disable";
			else if ( boost::algorithm::iequals( sslMode, "allow" ))
				sslMode = "allow";
			else if ( boost::algorithm::iequals( sslMode, "prefer" ))
				sslMode = "prefer";
			else if ( boost::algorithm::iequals( sslMode, "require" ))
				sslMode = "require";
			else if ( boost::algorithm::iequals( sslMode, "verify-ca" ))
				sslMode = "verify-ca";
			else if ( boost::algorithm::iequals( sslMode, "verify-full" ))
				sslMode = "verify-full";
			else	{
				MOD_LOG_FATAL << logPrefix() << "unknown SSL mode: '" << sslMode << "'";
				retVal = false;
			}
		}
		else if ( boost::algorithm::iequals( L1it->first, "sslCert" ))	{
			bool isDefined = ( !sslCert.empty());
			if ( !Parser::getValue( logPrefix().c_str(), *L1it, sslCert, &isDefined ))
				retVal = false;
			else	{
				if ( ! boost::filesystem::path( sslCert ).is_absolute() )
					MOD_LOG_WARNING << logPrefix() << "certificate file path is not absolute: "
							<< sslCert;
			}
		}
		else if ( boost::algorithm::iequals( L1it->first, "sslKey" ))	{
			bool isDefined = ( !sslKey.empty());
			if ( !Parser::getValue( logPrefix().c_str(), *L1it, sslKey, &isDefined ))
				retVal = false;
			else	{
				if ( ! boost::filesystem::path( sslKey ).is_absolute() )
					MOD_LOG_WARNING << logPrefix() << "key file path is not absolute: "
							<< sslKey;
			}
		}
		else if ( boost::algorithm::iequals( L1it->first, "sslRootCert" ))	{
			bool isDefined = ( !sslRootCert.empty());
			if ( !Parser::getValue( logPrefix().c_str(), *L1it, sslRootCert, &isDefined ))
				retVal = false;
			else	{
				if ( ! boost::filesystem::path( sslRootCert ).is_absolute() )
					MOD_LOG_WARNING << logPrefix() << "root CA file path is not absolute: "
							<< sslRootCert;
			}
		}
		else if ( boost::algorithm::iequals( L1it->first, "sslCRL" ))	{
			bool isDefined = ( !sslCRL.empty());
			if ( !Parser::getValue( logPrefix().c_str(), *L1it, sslCRL, &isDefined ))
				retVal = false;
			else	{
				if ( ! boost::filesystem::path( sslCRL ).is_absolute() )
					MOD_LOG_WARNING << logPrefix() << "CRL file path is not absolute: "
							<< sslCRL;
			}
		}
		else if ( boost::algorithm::iequals( L1it->first, "connectionTimeout" ))	{
			if ( !Parser::getValue( logPrefix().c_str(), *L1it, connectTimeout, &cTdefined ))
				retVal = false;
		}
		else if ( boost::algorithm::iequals( L1it->first, "connections" ))	{
			if ( !Parser::getValue( logPrefix().c_str(), *L1it, connections,
						Parser::RangeDomain<unsigned short>( 0 ), &connDefined ))
				retVal = false;
		}
		else if ( boost::algorithm::iequals( L1it->first, "acquireTimeout" ))	{
			if ( !Parser::getValue( logPrefix().c_str(), *L1it, acquireTimeout, &aTdefined ))
				retVal = false;
		}
		else if ( boost::algorithm::iequals( L1it->first, "statementTimeout" ))	{
			if ( !Parser::getValue( logPrefix().c_str(), *L1it, statementTimeout, &sTdefined ))
				retVal = false;
		}
		else if ( boost::algorithm::iequals( L1it->first, "programFile" ))	{
			std::string programFile;
			if ( !Parser::getValue( logPrefix().c_str(), *L1it, programFile ))
				retVal = false;
			else	{
				if ( ! boost::filesystem::path( programFile ).is_absolute() )
					MOD_LOG_WARNING << logPrefix() << "program file path is not absolute: "
							<< programFile;
				programFiles.push_back( programFile );
			}
		}
		else	{
			MOD_LOG_WARNING << logPrefix() << "unknown configuration option: '"
					<< L1it->first << "'";
		}
	}
	if ( !sslCert.empty() && sslKey.empty() )	{
		MOD_LOG_FATAL << logPrefix() << "SSL certificate configured but no SSL key specified";
		retVal = false;
	}
	if ( !sslCert.empty() && sslKey.empty() )	{
		MOD_LOG_FATAL << logPrefix() << "SSL key configured but no SSL certificate specified";
		retVal = false;
	}
	if ( boost::algorithm::iequals( sslMode, "verify-ca" ) ||
	     boost::algorithm::iequals( sslMode, "verify-full" ))	{
		MOD_LOG_FATAL << logPrefix() << "server SSL certificate requested but no root CA specified";
		retVal = false;
	}
	if ( ! connDefined )
		connections = DEFAULT_POSTGRESQL_CONNECTIONS;
	if ( ! cTdefined )
		connectTimeout = DEFAULT_CONNECTION_TIMEOUT;
	if ( ! sTdefined )
		statementTimeout = DEFAULT_STATEMENT_TIMEOUT;
	if ( sslMode.empty())
		sslMode = "prefer";

	return retVal;
}

void PostgreSQLconfig::setCanonicalPathes( const std::string& refPath )
{
	using namespace boost::filesystem;

	if ( ! sslCert.empty() )	{
		if ( ! path( sslCert ).is_absolute() )
			sslCert = resolvePath( absolute( sslCert,
							 path( refPath ).branch_path()).string());
		else
			sslCert = resolvePath( sslCert );
	}
	if ( ! sslKey.empty() )	{
		if ( ! path( sslKey ).is_absolute() )
			sslKey = resolvePath( absolute( sslKey,
							path( refPath ).branch_path()).string());
		else
			sslKey = resolvePath( sslKey );
	}
	if ( ! sslRootCert.empty() )	{
		if ( ! path( sslRootCert ).is_absolute() )
			sslRootCert = resolvePath( absolute( sslRootCert,
							     path( refPath ).branch_path()).string());
		else
			sslRootCert = resolvePath( sslRootCert );
	}
	if ( ! sslCRL.empty() )	{
		if ( ! path( sslCRL ).is_absolute() )
			sslCRL = resolvePath( absolute( sslCRL,
							path( refPath ).branch_path()).string());
		else
			sslCRL = resolvePath( sslCRL );
	}
	for ( std::list< std::string >::iterator it = programFiles.begin();
						it != programFiles.end(); it++ )	{
		if ( ! path( *it ).is_absolute() )
			*it = resolvePath( absolute( *it, path( refPath ).branch_path()).string());
		else
			*it = resolvePath( *it );
	}
}

void PostgreSQLconfig::print( std::ostream& os, size_t indent ) const
{
	std::string indStr( indent, ' ' );

	os << indStr << sectionName() << ":" << std::endl;
	if ( ! m_ID.empty() )
		os << indStr << "   ID: " << m_ID << std::endl;
	if ( host.empty())
		os << indStr << "   Database host: local unix domain socket" << std::endl;
	else
		os << indStr << "   Database host: " << host << ":" << port << std::endl;
	os << indStr << "   Database name: " << (dbName.empty() ? "(not specified - server user default)" : dbName) << std::endl;
	os << indStr << "   Database user: " << (user.empty() ? "(not specified - same as server user)" : user)
	   << ", password: " << (password.empty() ? "(not specified - no password used)" : password) << std::endl;
	if ( ! sslMode.empty())
		os << indStr << "   Database connection SSL mode: " << sslMode << std::endl;
	if ( ! sslCert.empty())	{
		os << indStr << "   Client SSL certificate file: " << sslCert << std::endl;
		if ( ! sslMode.empty())
			os << indStr << "   Client SSL key file: " << sslKey << std::endl;
	}
	if ( ! sslRootCert.empty())
		os << indStr << "   SSL root CA file: " << sslRootCert << std::endl;
	if ( ! sslCRL.empty())
		os << indStr << "   SSL CRL file: " << sslCRL << std::endl;

	if ( connectTimeout == 0 )
		os << indStr << "   Connect timeout: 0 (wait indefinitely)" << std::endl;
	else
		os << indStr << "   Connect timeout: " << connectTimeout << "s" << std::endl;
	os << indStr << "   Database connections: " << connections << std::endl;
	if ( acquireTimeout == 0 )
		os << indStr << "   Acquire database connection timeout: 0 (wait indefinitely)" << std::endl;
	else
		os << indStr << "   Acquire database connection timeout: " << acquireTimeout << "s" << std::endl;
	if ( statementTimeout == 0 )
		os << indStr << "   Default statement execution timeout: 0 (wait indefinitely)" << std::endl;
	else
		os << indStr << "   Default statement execution timeout: " << statementTimeout << "ms" << std::endl;

	if ( programFiles.size() == 0 )
		os << indStr << "   Program file: none" << std::endl;
	else if ( programFiles.size() == 1 )
		os << indStr << "   Program file: " << programFiles.front() << std::endl;
	else	{
		std::list< std::string >::const_iterator it = programFiles.begin();
		os << indStr << "   Program files: " << *it << std::endl;
		while ( it != programFiles.end() )
			os << indStr << "                  " << *it++ << std::endl;
	}
}

bool PostgreSQLconfig::check() const
{
	if ( connections == 0 )	{
		MOD_LOG_ERROR << logPrefix() << "number of database connections cannot be 0";
		return false;
	}
	return true;
}

}} // namespace _Wolframe::config
