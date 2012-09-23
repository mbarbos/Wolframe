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
///\file langbind/appConfig_option.hpp
///\brief Data structures on command line for the configuration of the processor environment
#ifndef _Wolframe_APPLICATION_ENVIRONMENT_CMDLINE_OPTION_STRUCT_HPP_INCLUDED
#define _Wolframe_APPLICATION_ENVIRONMENT_CMDLINE_OPTION_STRUCT_HPP_INCLUDED
#include "langbind/appConfig_struct.hpp"
#include <boost/property_tree/ptree.hpp>

namespace _Wolframe {
namespace langbind {

struct DDLFormOption :public langbind::DDLFormConfigStruct
{
	DDLFormOption( const std::string& src);
};

struct PrintLayoutOption :public langbind::PrintLayoutConfigStruct
{
	PrintLayoutOption( const std::string& opt);
};

struct TransactionFunctionOption :public langbind::TransactionFunctionConfigStruct
{
	TransactionFunctionOption( const std::string& src);
};

struct NormalizeFunctionOption :public langbind::NormalizeFunctionConfigStruct
{
	NormalizeFunctionOption( const std::string& src);
};


class DatabaseConfigOption
{
public:
	DatabaseConfigOption( const std::string& opt);

	boost::property_tree::ptree tree() const
	{
		return m_tree;
	}

private:
	boost::property_tree::ptree m_tree;
};

}}//namespace
#endif


