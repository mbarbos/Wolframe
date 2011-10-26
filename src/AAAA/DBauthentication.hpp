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
// database authenticator
//

#ifndef _DB_AUTHENTICATION_HPP_INCLUDED
#define _DB_AUTHENTICATION_HPP_INCLUDED

#include "AAAA/authentication.hpp"
#include "config/reference.hpp"
#include "moduleInterface.hpp"

namespace _Wolframe {
namespace AAAA {

class DBauthenticator : public AuthenticationUnit
{
};

class DatabaseAuthConfig : public config::ObjectConfiguration
{
	friend class DBauthContainer;
	friend class config::ConfigurationParser;
public:
	DatabaseAuthConfig( const char* cfgName, const char* logParent, const char* logName )
		: config::ObjectConfiguration( cfgName, logParent, logName ),
		  m_dbConfig( "", logParent, "Database" )	{}

	virtual const char* objectName() const			{ return "DatabaseAuth"; }

	/// methods
	bool check() const					{ return m_dbConfig.check(); }

	void print( std::ostream& os, size_t indent ) const	{
		std::string indStr( indent, ' ' );
		os << indStr << sectionName();
		m_dbConfig.print( os, 0 );
	}

	void setCanonicalPathes( const std::string& refPath )	{ m_dbConfig.setCanonicalPathes( refPath ); }
private:
	config::ReferenceConfig	m_dbConfig;
};


class DBauthContainer : public ObjectContainer< AuthenticationUnit >
{
public:
	DBauthContainer( const DatabaseAuthConfig& conf );
	~DBauthContainer();

	virtual const char* objectName() const			{ return "DatabaseAuth"; }
	virtual const AuthenticationUnit& object() const	{ return m_auth; }

	bool resolveDB( const db::DatabaseProvider& db );
private:
	std::string		m_dbLabel;
	const db::Database*	m_db;
	DBauthenticator		m_auth;
};

}} // namespace _Wolframe::AAAA

#endif // _DB_AUTHENTICATION_HPP_INCLUDED
