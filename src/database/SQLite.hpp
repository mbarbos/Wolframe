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
// Wolframe SQLite client
//

#ifndef _SQLITE_HPP_INCLUDED
#define _SQLITE_HPP_INCLUDED

#include "database/database.hpp"
#include "database/databaseContainer.hpp"

#include <list>
#include "objectPool.hpp"
#include "sqlite3.h"

namespace _Wolframe {
namespace db {

/// SQLite database configuration
class SQLiteConfig : public module::ModuleConfiguration< SQLiteConfig, DatabaseConfig >
{
	friend class config::ConfigurationParser;
	friend class SQLiteContainer;
public:
	const char* typeName() const			{ return "SQLite"; }

	SQLiteConfig( const char* name, const char* logParent, const char* logName );
	~SQLiteConfig()					{}

	bool check() const;
	void print( std::ostream& os, size_t indent ) const;
	virtual void setCanonicalPathes( const std::string& referencePath );
private:
	std::string	filename;
	bool		flag;
};


class SQLiteDatabase : public Database
{
public:
	SQLiteDatabase( const std::string& id,
			const std::string& filename, unsigned short connections, bool flag );
	virtual ~SQLiteDatabase();

	virtual const std::string& ID() const		{ return m_ID; }
	virtual const char* typeName() const		{ return "SQLite"; }
	virtual bool doTransaction( DatabaseRequest&, DatabaseAnswer&,
			    unsigned short, unsigned short );
private:
	const std::string	m_ID;
	const std::string	m_filename;
	bool			m_flag;
	std::list< sqlite3* >	m_connections;		//< list of DB connections
	ObjectPool< sqlite3* >	m_connPool;		//< pool of connections
};


class SQLiteContainer : public module::ModuleContainer< SQLiteContainer, SQLiteConfig,
		DatabaseContainer, DatabaseConfig >
{
public:
	SQLiteContainer( const SQLiteConfig& conf );
	~SQLiteContainer();

	const std::string& ID() const			{ return m_db.ID(); }
	const char* typeName() const			{ return m_db.typeName(); }
	virtual const Database& database()		{ return m_db; }
private:
	SQLiteDatabase	m_db;
};

}} // _Wolframe::db

#endif // _SQLITE_HPP_INCLUDED
