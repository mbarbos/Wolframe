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
// database.hpp - Wolframe base database class
//

#ifndef _DATABASE_HPP_INCLUDED
#define _DATABASE_HPP_INCLUDED

#include "database/transaction.hpp"
#include "database/transactionFunction.hpp"
#include <string>

namespace _Wolframe {
namespace db {

///\class Database
///\brief Base class for database interface. All databases should provide this interface
class Database
{
public:
	virtual ~Database()			{}

	/// Database identification.
	/// All databases must have an identifier as they are referenced using this identifier.
	/// The identifier must be unique (of course).
	virtual const std::string& ID() const = 0;

	///\ Just and interface at the moment
	virtual Transaction* transaction( const std::string& name ) = 0;

	/// DEPRECATED:
	virtual const TransactionFunction* transactionFunction( const std::string&)
	{
		return 0;
	}

	/// Close the database connetion
	/// This exists for no good reason (mostly to make the code look uniform)
	virtual void close()			{}
};


///\class DatabaseUnit
///\brief This is the base class for database unit implementations
class DatabaseUnit
{
public:
	virtual ~DatabaseUnit()			{}

	///\brief Database class (module type).
	///\remark All database implementations need a class name.
	///\remark Class names must be unique.
	virtual const char* className() const = 0;

	///\brief The database identifier. This is the configured name.
	virtual const std::string& ID() const = 0;

	///\brief The actual database object.
	virtual Database* database() = 0;

	/// DEPRECATED:
	virtual bool loadProgram() = 0;

	///\brief Load the database specific part of the transaction program for this database
	///\remark throws std::runtime_error with position info in case of error
	virtual void loadProgram( const std::string& ) = 0;
};

}} // namespace _Wolframe::db

#endif // _DATABASE_HPP_INCLUDED
