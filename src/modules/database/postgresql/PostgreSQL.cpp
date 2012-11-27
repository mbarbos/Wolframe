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
// Wolframe PostgreSQL client view implementation
//

#include "logger-v1.hpp"
#include "PostgreSQL.hpp"
#include "PostgreSQLpreparedStatement.hpp"
#include <string>
#include <sstream>

#include <boost/filesystem.hpp>

namespace _Wolframe {
namespace db {

//***  PostgreSQL database functions  ***************************************
static std::string escConnElement( std::string element )
{
	std::string esc;
	for ( std::string::const_iterator it = element.begin(); it != element.end(); it++ )	{
		if ( *it == '\'' )
			esc += "\\'";
		else if ( *it == '\\' )
			esc += "\\\\";
		else
			esc += *it;
	}
	return esc;
}

static std::string buildConnStr( const std::string& host, unsigned short port, const std::string& dbName,
				 const std::string& user, const std::string& password,
				 const std::string& sslMode, const std::string& sslCert, const std::string& sslKey,
				 const std::string& sslRootCert, const std::string& sslCRL,
				 unsigned short connectTimeout )
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
		ss << "dbname = '" << escConnElement( dbName ) << "'";
	}
	if ( ! user.empty())	{
		if ( ! ss.str().empty())
			ss << " ";
		ss << "user = '" << escConnElement( user ) << "'";
		if ( ! password.empty())	{
			if ( ! ss.str().empty())
				ss << " ";
			ss << "password = '" << escConnElement( password ) << "'";
		}
	}
	if ( ! sslMode.empty())	{
		if ( ! ss.str().empty())
			ss << " ";
		ss << "sslmode = '" << escConnElement( sslMode ) << "'";
	}
	if ( ! sslCert.empty())	{
		if ( ! ss.str().empty())
			ss << " ";
		ss << "sslcert = '" << escConnElement( sslCert ) << "'";
		if ( ! sslKey.empty())	{
			if ( ! ss.str().empty())
				ss << " ";
			ss << "sslkey = '" << escConnElement( sslKey ) << "'";
		}
	}
	if ( ! sslRootCert.empty())	{
		if ( ! ss.str().empty())
			ss << " ";
		ss << "sslrootcert = '" << escConnElement( sslRootCert ) << "'";
	}
	if ( ! sslCRL.empty())	{
		if ( ! ss.str().empty())
			ss << " ";
		ss << "sslcrl = '" << escConnElement( sslCRL ) << "'";
	}
	if ( connectTimeout != 0 )	{
		if ( ! ss.str().empty())
			ss << " ";
		ss << "connect_timeout = " << connectTimeout;
	}
	return ss.str();
}

void PostgreSQLdbUnit::noticeProcessor( void* this_void, const char * message)
{
	struct LogMsgMap :public std::map<std::string,_Wolframe::log::LogLevel::Level>
	{
		LogMsgMap()
		{
			typedef _Wolframe::log::LogLevel LV;
			(*this)["WARNING"] = LV::LOGLEVEL_WARNING;
			(*this)["ERROR"] = LV::LOGLEVEL_ERROR;
			(*this)["FATAL"] = LV::LOGLEVEL_FATAL;
			(*this)["PANIC"] = LV::LOGLEVEL_ALERT;
			(*this)["NOTICE"] = LV::LOGLEVEL_NOTICE;
			(*this)["DETAIL"] = LV::LOGLEVEL_INFO;
			(*this)["INFO"] = LV::LOGLEVEL_INFO;
			(*this)["DEBUG"] = LV::LOGLEVEL_DEBUG;
			(*this)["LOG"] = LV::LOGLEVEL_TRACE;
		}
	};
	static LogMsgMap logMsgMap;

	PostgreSQLdbUnit* this_ = (PostgreSQLdbUnit*)this_void;
	std::size_t ii=0;
	for (; message[ii] && message[ii] != ':'; ++ii);
	if (message[ii])
	{
		LogMsgMap::const_iterator li = logMsgMap.find( std::string( message, ii));
		if (li != logMsgMap.end())
		{
			_Wolframe::log::Logger(logBackendPtr).Get( li->second) << "PostgreSQL database '" << ((this_)?this_->ID():"") << "': " << (message + ii + 1);
			return;
		}
	}
	LOG_ERROR << "Unknown log message type from PostgreSQL database '" << ((this_)?this_->ID():"") << "': " << message;
}

// This function also needs a lot of work
PostgreSQLdbUnit::PostgreSQLdbUnit(const std::string& id,
				    const std::string& host, unsigned short port,
				    const std::string& dbName,
				    const std::string& user, const std::string& password,
				    std::string sslMode, std::string sslCert, std::string sslKey,
				    std::string sslRootCert, std::string sslCRL ,
				    unsigned short connectTimeout,
				    size_t connections, unsigned short acquireTimeout,
				    unsigned statementTimeout,
				    const std::list<std::string>& programFiles_)
	: m_ID( id ), m_noConnections( 0 ), m_connPool( acquireTimeout ),
	  m_statementTimeout( statementTimeout ), m_programFiles(programFiles_)
{
	m_connStr = buildConnStr( host, port,  dbName, user, password,
				  sslMode, sslCert, sslKey, sslRootCert, sslCRL,
				  connectTimeout );
	MOD_LOG_DATA << "PostgreSQL database '" << m_ID << "' connection string <" << m_connStr << ">";

	for ( size_t i = 0; i < connections; i++ )	{
		PGconn* conn = PQconnectdb( m_connStr.c_str() );
		if ( conn == NULL )
			MOD_LOG_ALERT << "Failed to connect to PostgreSQL database '" << m_ID << "'";
		else	{
			PQsetNoticeProcessor( conn, &noticeProcessor, this);

			ConnStatusType stat = PQstatus( conn );
			switch( stat )	{
				case CONNECTION_OK:
					MOD_LOG_TRACE << "PostgreSQL database '" << m_ID << "' constructor: connection "
						      << i << " connected OK.";
					break;
				case CONNECTION_BAD:	{
					std::string err = PQerrorMessage( conn );
					if ( err.compare( 0, 6, "FATAL:" ) == 0 )	{
						MOD_LOG_ALERT << "PostgreSQL database '" << m_ID << "' (connection "
							      << i << ") error: '" << err << "'";
						throw std::runtime_error( "Fatal error connecting to the PostgreSQL database" );
					}
					MOD_LOG_WARNING << "PostgreSQL database '" << m_ID << "' connection "
							<< i << " error: '" << err << "'";
					break;
				}
				case CONNECTION_STARTED:
					MOD_LOG_ALERT << "PostgreSQL database '" << m_ID << "' constructor: connection "
						      << i << " connection STARTED: '" << PQerrorMessage( conn ) << "'";
					throw std::range_error( "PostgreSQL database: CONNECTION_STARTED in synchronous connection" );
					break;
				case CONNECTION_MADE:
					MOD_LOG_ALERT << "PostgreSQL database '" << m_ID << "' constructor: connection "
						      << i << " connection MADE: '" << PQerrorMessage( conn ) << "'";
					throw std::range_error( "PostgreSQL database: CONNECTION_MADE in synchronous connection" );
					break;
				case CONNECTION_AWAITING_RESPONSE:
					MOD_LOG_ALERT << "PostgreSQL database '" << m_ID << "' constructor: connection "
						      << i << " connection AWAITING RESPONSE: '" << PQerrorMessage( conn ) << "'";
					throw std::range_error( "PostgreSQL database: CONNECTION_AWAITING_RESPONSE in synchronous connection" );
					break;
				case CONNECTION_AUTH_OK:
					MOD_LOG_ALERT << "PostgreSQL database '" << m_ID << "' constructor: connection "
						      << i << " connection AUTH OK: '" << PQerrorMessage( conn ) << "'";
					throw std::range_error( "PostgreSQL database: CONNECTION_AUTH_OK in synchronous connection" );
					break;
				case CONNECTION_SSL_STARTUP:
					MOD_LOG_ALERT << "PostgreSQL database '" << m_ID << "' constructor: connection "
						      << i << " connection SSL start: '" << PQerrorMessage( conn ) << "'";
					throw std::range_error( "PostgreSQL database: CONNECTION_SSL_STARTUP in synchronous connection" );
					break;
				case CONNECTION_SETENV:
					MOD_LOG_ALERT << "PostgreSQL database '" << m_ID << "' constructor: connection "
						      << i << " connection SETENV: '" << PQerrorMessage( conn ) << "'";
					throw std::range_error( "PostgreSQL database: CONNECTION_SETENV in synchronous connection" );
					break;
				case CONNECTION_NEEDED:
					MOD_LOG_ALERT << "PostgreSQL database '" << m_ID << "' constructor: connection "
						      << i << " connection NEEDED: '" << PQerrorMessage( conn ) << "'";
					throw std::range_error( "PostgreSQL database: CONNECTION_NEEDED in synchronous connection" );
					break;
			}
			m_connPool.add( conn );
			m_noConnections++;
		}
	}

	m_db.setUnit( this );

	MOD_LOG_DEBUG << "PostgreSQL database '" << m_ID << "' created with a pool of " << m_noConnections << " connections";
}


// This function needs a lot of work and thinking...
PostgreSQLdbUnit::~PostgreSQLdbUnit()
{
	size_t connections = 0;

	m_db.setUnit( NULL );
	m_connPool.timeout( 3 );

	while ( m_connPool.available() )	{
		PGconn* conn = m_connPool.get();
		if ( conn == NULL )	{
			MOD_LOG_ALERT << "PostgreSQL database '" << m_ID << "' destructor: NULL connection from pool";
			throw std::logic_error( "PostgreSQL database destructor: NULL connection from pool" );
		}
		PGTransactionStatusType stat = PQtransactionStatus( conn );
		switch( stat )	{
			case PQTRANS_IDLE:
				PQfinish( conn );
				MOD_LOG_TRACE << "PostgreSQL database '" << m_ID << "' destructor: Connection " << connections << " idle";
				m_noConnections--, connections++;
				break;
			case PQTRANS_ACTIVE:
				PQfinish( conn );
				MOD_LOG_TRACE << "PostgreSQL database '" << m_ID << "' destructor: Connection " << connections << " active";
				m_noConnections--, connections++;
				break;
			case PQTRANS_INTRANS:
				PQfinish( conn );
				MOD_LOG_TRACE << "PostgreSQL database '" << m_ID << "' destructor: Connection " << connections << " in transaction";
				m_noConnections--, connections++;
				break;
			case PQTRANS_INERROR:
				PQfinish( conn );
				MOD_LOG_TRACE << "PostgreSQL database '" << m_ID << "' destructor: Connection " << connections << " in transaction error";
				m_noConnections--, connections++;
				break;
			case PQTRANS_UNKNOWN:
				PQfinish( conn );
				MOD_LOG_TRACE << "PostgreSQL database '" << m_ID << "' destructor: Connection " << connections << " status unknown";
				m_noConnections--, connections++;
				break;
		}
	}
	if ( m_noConnections != 0 )	{
		MOD_LOG_ALERT << "PostgreSQL database unit '" << m_ID << "' destructor: "
			      << m_noConnections << " connections not destroyed";
		throw std::logic_error( "PostgreSQL database unit destructor: not all connections destroyed" );
	}
	MOD_LOG_TRACE << "PostgreSQL database unit '" << m_ID << "' destroyed, " << connections << " connections destroyed";
}

void PostgreSQLdbUnit::loadProgram( const std::string& filename )
{
	// No program file, do nothing
	if ( filename.empty())
		return;
	if ( !boost::filesystem::exists( filename ))	{
		MOD_LOG_ALERT << "Program file '" << filename
			      << "' does not exist (PostgreSQL database '" << m_ID << "')";
		return;
	}
	try
	{
		addProgram( utils::readSourceFileContent( filename));
	}
	catch (const std::runtime_error& e)
	{
		throw std::runtime_error( std::string("error in program '") + utils::getFileStem(filename) + "':" + e.what());
	}
}

void PostgreSQLdbUnit::loadAllPrograms()
{
	std::list<std::string>::const_iterator pi = m_programFiles.begin(), pe = m_programFiles.end();
	for (; pi != pe; ++pi)
	{
		MOD_LOG_DEBUG << "Load Program '" << *pi << "' for PostgreSQL database unit '" << m_ID << "'";
		loadProgram( *pi);
	}
	MOD_LOG_DEBUG << "Programs for PostgreSQL database unit '" << m_ID << "' loaded";
}


Database* PostgreSQLdbUnit::database()
{
	return m_db.hasUnit() ? &m_db : NULL;
}



/*****  PostgreSQL database  ******************************************/
const std::string& PostgreSQLdatabase::ID() const
{
	if ( m_unit )
		return m_unit->ID();
	else
		throw std::runtime_error( "PostgreSQL database unit not initialized" );
}

void PostgreSQLdatabase::loadProgram( const std::string& filename )
{
	if ( !m_unit )
		throw std::runtime_error( "loadProgram: PostgreSQL database unit not initialized" );
	m_unit->loadProgram( filename );
}

void PostgreSQLdatabase::loadAllPrograms()
{
	if ( !m_unit )
		throw std::runtime_error( "loadAllPrograms: PostgreSQL database unit not initialized" );
	m_unit->loadAllPrograms();
}

void PostgreSQLdatabase::addProgram( const std::string& program )
{
	if ( !m_unit )
		throw std::runtime_error( "addProgram: PostgreSQL database unit not initialized" );
	m_unit->addProgram( program );
}

void PostgreSQLdatabase::addStatements( const types::keymap<std::string>& stmmap_)
{
	if ( !m_unit )
		throw std::runtime_error( "addStatements: PostgreSQL database unit not initialized" );
	m_unit->addStatements( stmmap_);
}

const std::string* PostgreSQLdatabase::getProgram( const std::string& name) const
{
	if ( !m_unit )
		throw std::runtime_error( "getProgram: PostgreSQL database unit not initialized" );
	return m_unit->getProgram( name );
}

Transaction* PostgreSQLdatabase::transaction( const std::string& /*name*/ )
{
	return new PostgreSQLtransaction( *this );
}

void PostgreSQLdatabase::closeTransaction( Transaction *t )
{
	delete t;
}

/*****  PostgreSQL transaction  ***************************************/
PostgreSQLtransaction::PostgreSQLtransaction( PostgreSQLdatabase& database )
	: m_db( database ), m_unit( database.dbUnit() )
{
}

const std::string& PostgreSQLtransaction::databaseID() const
{
	return m_unit.ID();
}

void PostgreSQLtransaction::execute_statement( const char* statement)
{
	if (!m_conn.get()) throw std::runtime_error( "executing transaction statement without transaction context");
	std::ostringstream msg;
	bool success = true;
	PGresult* res = PQexec( **m_conn, statement);
	if (PQresultStatus( res) != PGRES_COMMAND_OK)
	{
		const char* statusType = PQresStatus( PQresultStatus( res));
		const char* errmsg = PQresultErrorMessage( res);
		success = false;
		msg << "PostgreSQL status " << (statusType?statusType:"unknown");
		msg << "; message: '" << (errmsg?errmsg:"unknown") << "'";
	}
	PQclear( res);
	if (!success) throw std::runtime_error( msg.str());
}

void PostgreSQLtransaction::begin()
{
	m_conn.reset( new PoolObject<PGconn*>( m_unit.m_connPool));
	execute_statement( "BEGIN;");
}

void PostgreSQLtransaction::commit()
{
	execute_statement( "COMMIT;");
	m_conn.reset();
}

void PostgreSQLtransaction::rollback()
{
	execute_statement( "ROLLBACK;");
	m_conn.reset();
}

void PostgreSQLtransaction::execute_with_autocommit()
{
	try
	{
		PoolObject<PGconn*> conn( m_unit.m_connPool);
		PreparedStatementHandler_postgres ph( *conn, m_unit.stmmap());
		try
		{
			if (!ph.begin()
			||  !ph.doTransaction( m_input, m_output)
			||  !ph.commit())
			{
				const char* err = ph.getLastError();
				throw std::runtime_error(err?err:"unknown error");
			}
		}
		catch (const std::runtime_error& e)
		{
			MOD_LOG_ERROR << "error in postgres database transaction: " << e.what();
			ph.rollback();
			throw std::runtime_error( std::string("transaction failed: ") + e.what());
		}
	}
	catch ( _Wolframe::ObjectPoolTimeout )
	{
		throw std::runtime_error("timeout in database connection pool object allocation");
	}
}

void PostgreSQLtransaction::execute_transaction_operation()
{
	PreparedStatementHandler_postgres ph( **m_conn.get(), m_unit.stmmap(), true);
	try
	{
		if (!ph.doTransaction( m_input, m_output))
		{
			const char* err = ph.getLastError();
			throw std::runtime_error(err?err:"unknown error");
		}
	}
	catch (const std::runtime_error& e)
	{
		MOD_LOG_ERROR << "error in postgres database transaction operation: " << e.what();
		ph.rollback();
		throw std::runtime_error( std::string("transaction operation failed: ") + e.what());
	}
}

void PostgreSQLtransaction::execute()
{
	if (m_conn.get())
	{
		execute_transaction_operation();
	}
	else
	{
		execute_with_autocommit();
	}
}

void PostgreSQLtransaction::close()
{
	if (m_conn.get()) MOD_LOG_ERROR << "closed transaction without 'begin' or rollback";
	m_conn.reset();
	m_db.closeTransaction( this );
}

}} // _Wolframe::db

