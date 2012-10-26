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
///
/// \file UserInterface.hpp
///

#ifndef _USER_INTERFACE_HPP_INCLUDED
#define _USER_INTERFACE_HPP_INCLUDED

#include <string>
#include "version.hpp"

namespace _Wolframe {
namespace UI {

/// User Interface form header.
class UIformHeader
{
public:
	enum FormType	{
		FORM,
		MENU
	};

	UIformHeader()				{}

	FormType type() const			{ return m_type; }
	const std::string& name() const		{ return m_name; }
	const Version& version() const		{ return m_version; }
	const std::string& description() const	{ return m_description; }
private:
	FormType	m_type;			///< the type of the form
	std::string	m_name;			///< the name of the form
	Version		m_version;		///< the version of the form
	std::string	m_description;		///< the description of the form
};

/// User Interface form body.
class UIformBody
{
public:
	UIformBody()				{}

	const std::string& body() const		{ return m_body; }
private:
	std::string	m_body;			///< the body of the form
};


/// User Interface form.
class UIform
{
public:
	UIform()				{}

	const UIformHeader& header() const	{ return m_header; }
	const UIformBody& body() const		{ return m_body; }
private:
	UIformHeader	m_header;		///< the header of the form
	UIformBody	m_body;			///< the body of the form
};

/// Supplier of user interface elements.
/// Work in progress...
class UserInterfaceLibrary
{
public:
	UserInterfaceLibrary()			{}

	virtual ~UserInterfaceLibrary()		{}
};


}} // namespace _Wolframe::UI

#endif // _USER_INTERFACE_HPP_INCLUDED
