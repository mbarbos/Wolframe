/************************************************************************
Copyright (C) 2011 - 2014 Project Wolframe.
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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
/// \file directmapCommandHandlerUnit.hpp
/// \brief Interface to the directmap command handler unit
#ifndef _Wolframe_cmdbind_DIRECTMAP_COMMAND_HANDLER_UNIT_HPP_INCLUDED
#define _Wolframe_cmdbind_DIRECTMAP_COMMAND_HANDLER_UNIT_HPP_INCLUDED
#include "directmapCommandHandlerConfig.hpp"
#include "directmapCommandHandler.hpp"
#include "directmapProgram.hpp"
#include "filter/filterdef.hpp"
#include <vector>
#include <string>

namespace _Wolframe {
namespace cmdbind {

/// \class DirectmapCommandHandlerUnit
class DirectmapCommandHandlerUnit
	:public CommandHandlerUnit
{
public:
	DirectmapCommandHandlerUnit( const DirectmapCommandHandlerConfig* config_)
		:m_config(config_){}
	~DirectmapCommandHandlerUnit(){}

	/// \brief Create a new command handler
	virtual CommandHandler* createCommandHandler( const std::string& cmdname, const std::string& docformat);

	/// \brief Get the list of commands
	virtual std::vector<std::string> commands() const
	{
		return m_program.getkeys< std::vector<std::string> >();
	}

	/// \brief Load all configured programs
	virtual bool loadPrograms( const proc::ProcessorProviderInterface* provider);

private:
	const DirectmapCommandHandlerConfig* m_config;
	langbind::DirectmapProgram m_program;
	types::keymap<langbind::FilterDef> m_filtermap;
};

}}//namespace
#endif

