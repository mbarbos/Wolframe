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
#include "langbind/luaCommandEnvironment.hpp"
#include "langbind/luaDebug.hpp"
#include <boost/property_tree/ptree.hpp>
#include <boost/algorithm/string.hpp>
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>
#include <ostream>
#include "config/ConfigurationTree.hpp"
#include "config/valueParser.hpp"
#include "miscUtils.hpp"

extern "C" {
	#include <lualib.h>
	#include <lauxlib.h>
	#include <lua.h>
}

using namespace _Wolframe;
using namespace _Wolframe::langbind;

void LuaCommandEnvironment::Module::setType()
{
	if (m_type == LuaCommandEnvironment::Module::Undefined)
	{
		///TODO: Implement user defined modules
		m_type = Script;
	}
}

static int function_printlog( lua_State *ls)
{
	/* first parameter maps to a log level, rest gets printed depending on
	 * whether it's a string or a number
	 */
	int ii,nn = lua_gettop(ls);
	if (nn <= 0)
	{
		LOG_ERROR << "no arguments passed to 'printlog'";
		return 0;
	}
	const char *logLevel = luaL_checkstring( ls, 1);
	std::string logmsg;

	for (ii=2; ii<=nn; ii++)
	{
		if (!getDescription( ls, ii, logmsg))
		{
			LOG_ERROR << "failed to map 'printLog' arguments to a string";
		}
	}
	_Wolframe::log::LogLevel::Level lv = _Wolframe::log::LogLevel::strToLogLevel( logLevel);
	if (lv == _Wolframe::log::LogLevel::LOGLEVEL_UNDEFINED)
	{
		LOG_ERROR << "'printLog' called with undefined loglevel '" << logLevel << "' as first argument";
	}
	else
	{
		_Wolframe::log::Logger( _Wolframe::log::LogBackend::instance() ).Get( lv )
			<< _Wolframe::log::LogComponent::LogLua
			<< logmsg;
	}
	return 0;
}

bool LuaCommandEnvironment::Module::load( lua_State* ls) const
{
	if (m_type == UserLib)
	{
		lua_pushcfunction( ls, m_load);
		lua_pushstring( ls, m_name.c_str());
		if (lua_pcall( ls, 1, 1, 0) != 0)
		{
			LOG_ERROR << "Runtime error when loading base module " << m_name << ":" << lua_tostring( ls, -1);
			return false;
		}
	}
	else if (m_type == Script)
	{
		if (luaL_loadfile( ls, m_name.c_str()))
		{
			LOG_ERROR << "Failed to load script '" << m_name << "':" << lua_tostring( ls, -1);
			lua_pop( ls, 1);
			return false;
		}
		// register logging function already here because then it can be used in the script initilization part
		lua_pushcfunction( ls, &function_printlog);
		lua_setglobal( ls, "printlog");

		// call main, we may have to initialize LUA modules there
		if (lua_pcall( ls, 0, LUA_MULTRET, 0) != 0)
		{
			LOG_FATAL << "Unable to call main entry of script: " << lua_tostring( ls, -1 );
			lua_pop( ls, 1 );
			throw new std::runtime_error( "Can't initialize LUA processor" );
		}
	}
	else
	{
		LOG_WARNING << "Could not load module '" << m_name << "'";
	}
	return true;
}

bool LuaCommandEnvironment::Module::check() const
{
	switch (m_type)
	{
		case UserLib:
			if (!boost::filesystem::exists( m_name))
			{
				LOG_ERROR << "Library " << m_name << "does not exist";
				return false;
			}
			break;
		case Script:
			if (!boost::filesystem::exists( m_name))
			{
				LOG_ERROR << "Script " << m_name << "does not exist";
				return false;
			}
			break;
		case Undefined:
			break;
	}
	return true;
}

bool LuaCommandEnvironment::load( lua_State *ls) const
{
	luaL_openlibs( ls);

	for (std::vector<Module>::const_iterator it = m_modules.begin(); it != m_modules.end(); it++)
	{
		if (it->type() == Module::UserLib)
		{
			if (!it->load( ls)) return false;
		}
	}
	for (std::vector<Module>::const_iterator it = m_modules.begin(); it != m_modules.end(); it++)
	{
		if (it->type() == Module::Script)
		{
			if (!it->load( ls)) return false;
		}
	}
	if (!m_mainmodule.load( ls)) return false;
	return true;
}

bool LuaCommandEnvironment::check() const
{
	bool rt = true;
	for (std::vector<Module>::const_iterator it = m_modules.begin(); it != m_modules.end(); it++)
	{
		rt &= it->check();
	}
	return rt;
}

bool LuaCommandEnvironment::test() const
{
	lua_State *ls = luaL_newstate();
	if (!ls)
	{
		LOG_ERROR << "failed to create lua state in configuration test";
		return false;
	}
	bool rt = load( ls);
	lua_close( ls);
	return rt;
}

void LuaCommandEnvironment::print( std::ostream& os, size_t /*indent*/) const
{
	os << "Configuration of Lua Script Processor" << ":" << std::endl;
	os << "   Main Script: " << m_mainmodule.name() << " (" << m_mainmodule.name() << ")" << std::endl;
	os << "   Main Function: " << m_main << std::endl;

	if( !m_modules.empty())
	{
		os << "   Modules: ";
		for (std::vector<Module>::const_iterator it = m_modules.begin(); it != m_modules.end(); it++)
		{
			os << "      " << Module::typeName(it->type()) << " " << it->name() << std::endl;
		}
		os << std::endl;
	}
}

