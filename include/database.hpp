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

#include "configurationBase.hpp"

namespace _Wolframe	{
namespace db	{

/// database type
enum DatabaseType	{
	DBTYPE_POSTGRESQL,
	DBTYPE_SQLITE,
	DBTYPE_UNKNOWN
};

/// database strategy
enum DatabaseStrategy	{
	DBSTRATEGY_ROUND_ROBIN,
	DBSTRATEGY_FAILOVER,
	DBSTRATEGY_UNKNOWN
};

struct	DatabaseConfigBase
{
	const DatabaseType	type;
public:
	DatabaseConfigBase( DatabaseType Type )	: type( Type )	{}
//	virtual ~DatabaseConfigBase();

	virtual bool parse( const std::string& module, const boost::property_tree::ptree& pt ) = 0;
	virtual bool check( const std::string& module ) const = 0;
	virtual void print( std::ostream& os ) const = 0;
	virtual void setCanonicalPathes( const std::string& /* referencePath */ )	{}
};


struct	PostgreSQLconfig : public DatabaseConfigBase
{
	std::string	host;
	unsigned short	port;
	std::string	name;
	std::string	user;
	std::string	password;
	unsigned short	connections;
	unsigned short	acquireTimeout;
public:
	PostgreSQLconfig();
	bool parse( const std::string& module, const boost::property_tree::ptree& pt );
	bool check( const std::string& module ) const;
	void print( std::ostream& os ) const;
};

struct	SQLiteConfig : public DatabaseConfigBase
{

	std::string	filename;
	bool		flag;
public:
	SQLiteConfig();
	bool parse( const std::string& module, const boost::property_tree::ptree& pt );
	bool check( const std::string& module ) const;
	void print( std::ostream& os ) const;
	virtual void setCanonicalPathes( const std::string& referencePath );
};


/// database configuration
struct Configuration : public _Wolframe::config::ConfigurationBase
{
public:
	DatabaseStrategy		strategy;
	std::list<DatabaseConfigBase*>	dbConfig_;

	/// constructor
	Configuration() : _Wolframe::config::ConfigurationBase( "Database Server" ),
				strategy( DBSTRATEGY_UNKNOWN )	{}

	/// methods
	bool parse( const boost::property_tree::ptree& pt, const std::string& node );
	bool check() const;
	void print( std::ostream& os ) const;
	virtual void setCanonicalPathes( const std::string& referencePath );

//	Not implemented yet, inherited from base for the time being
//	bool test() const;
};

/// database base class
class Database	{
public:
	Database( Configuration& config );
	static DatabaseType strToType( const char *str );
	static std::string& typeToStr( DatabaseType type );
	static DatabaseStrategy strToStrategy( const char *str );
	static std::string& strategyToStr( DatabaseStrategy strategy );
};

}} // namespace _Wolframe::db

#endif // _DATABASE_HPP_INCLUDED
