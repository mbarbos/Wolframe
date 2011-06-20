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
// database.hpp - Wolframe base database class
//

#ifndef _DATABASE_HPP_INCLUDED
#define _DATABASE_HPP_INCLUDED

#include <list>
#include "config/configurationBase.hpp"
#include "logger.hpp"

namespace _Wolframe {
namespace db {


class DatabaseConfig : public config::ConfigurationBase
{
public:
	DatabaseConfig( const char* name, const char* logParent, const char* logName )
		: ConfigurationBase( name, logParent, logName ){}
	virtual ~DatabaseConfig()			{}

	virtual const char* type() const = 0;
	void ID( const std::string& id )		{ m_ID = id; }
	const std::string& ID() const			{ return m_ID; }
private:
	std::string		m_ID;
};


/// database reference class
/// note that this is a configuration class only
class ReferenceConfig : public DatabaseConfig
{
	friend class config::ConfigurationParser;
public:
	const char* type() const			{ return "DB Reference"; }

	ReferenceConfig( const char* name, const char* logParent, const char* logName )
		: DatabaseConfig( name, logParent, logName )	{}

	bool check() const;
	void print( std::ostream& os, size_t indent ) const;

	const std::string& dbName() const		{ return m_ref; }
private:
	std::string	m_ref;
};


/// database base classes
class _DatabaseRequest_
{
	virtual ~_DatabaseRequest_()			{}
};

class _DatabaseAnswer_
{
	virtual ~_DatabaseAnswer_()			{}
};


class _DatabaseChannel_
{
public:
	_DatabaseChannel_()				{}
	~_DatabaseChannel_()				{}

	void close()					{}
	void doDBstuff( _DatabaseRequest_&, _DatabaseAnswer_&){}
};


class Database
{
public:
	Database( const std::string id ) : m_id( id )	{}
	virtual ~Database()				{}

	const std::string& ID() const			{ return m_id; }
	virtual const char* type() const = 0;

	const _DatabaseChannel_* channel() const	{ return NULL; }
private:
	const std::string	m_id;
};


class DatabaseContainer
{
public:
	DatabaseContainer( const std::string id ) : m_id( id )	{}
	virtual ~DatabaseContainer()				{}

	const std::string& ID() const			{ return m_id; }
	virtual const char* type() const = 0;

private:
	const std::string	m_id;
};


/// database configuration
struct DBproviderConfig : public config::ConfigurationBase
{
public:
	std::list<DatabaseConfig*>	m_dbConfig;

	/// constructor & destructor
	DBproviderConfig() : ConfigurationBase( "Database(s)", NULL, "Database configuration" )	{}
	~DBproviderConfig();

	/// methods
	bool check() const;
	void print( std::ostream& os, size_t indent ) const;
	virtual void setCanonicalPathes( const std::string& referencePath );

//	Not implemented yet, inherited from base for the time being
//	bool test() const;
};

///
///
class DBprovider
{
public:
	DBprovider( const DBproviderConfig& conf );
	~DBprovider();

	const Database* database( std::string& ID ) const;
private:
	std::list<Database*>	m_db;
};

}} // namespace _Wolframe::db

#endif // _DATABASE_HPP_INCLUDED
