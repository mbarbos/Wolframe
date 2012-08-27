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
///\brief Interface to process prepared statements with sqlite3
///\file modules/database/sqlite3/SQLite3PreparedStatement.hpp
#ifndef _DATABASE_PREPARED_STATEMENT_SQLITE3_HPP_INCLUDED
#define _DATABASE_PREPARED_STATEMENT_SQLITE3_HPP_INCLUDED
#include "database/preparedStatement.hpp"
#include <string>
#include <map>
#include <cstdlib>
#include "sqlite3.h"

namespace _Wolframe {
namespace db {

///\class PreparedStatementHandler_sqlite3
///\brief Handler to process prepared statements with sqlite3
///\remark The sqlite3 connection is opened, closed, created and disposed by the caller
struct PreparedStatementHandler_sqlite3 :public PreparedStatementHandler
{
	///\brief Constructor
	PreparedStatementHandler_sqlite3( sqlite3* conn, const std::map<std::string,std::string>* stmmap);

	///\brief Destructor
	virtual ~PreparedStatementHandler_sqlite3();

	///\brief Begin transaction
	virtual bool begin();
	///\brief Commit current transaction
	virtual bool commit();
	///\brief Rollback current transaction
	virtual bool rollback();
	///\brief Clear object and set initial state
	virtual void clear();
	///\brief Start new command statement
	virtual bool start( const std::string& stmname);
	///\brief Bind parameter value on current command statement
	virtual bool bind( std::size_t idx, const char* value);
	///\brief Execute instance of current statement
	virtual bool execute();
	///\brief Get the number of columns of the last result
	virtual std::size_t nofColumns();
	///\brief Get a column title of the last result
	virtual const char* columnName( std::size_t idx);
	///\brief Get the last database error as string
	virtual const char* getLastError();
	///\brief Get a column of the last result
	virtual const char* get( std::size_t idx);
	///\brief Skip to the next row of the last result
	virtual bool next();

private:
	enum State
	{
		Init,
		Transaction,
		Prepared,
		Executed,
		Error
	};
	static const char* stateName( State i)
	{
		const char* ar[] = {"Init","Transaction","Prepared","Executed","Error"};
		return ar[ (int)i];
	}

	void setDatabaseErrorMessage();
	bool status( int rc, State newstate);
	bool errorStatus( const std::string& message);
	bool executeInstruction( const char* stmstr, State newstate);

private:
	State m_state;
	sqlite3* m_conn;
	const std::map<std::string,std::string>* m_stmmap;
	std::string m_lasterror;
	sqlite3_stmt* m_stm;
};

}}//namespace
#endif
