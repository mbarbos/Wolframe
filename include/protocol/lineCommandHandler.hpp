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
///\file protocol/commandHandler.hpp
///\brief interface to a generic command handler for a networkHandler command with delegation of network I/O until the command context is left
#ifndef _Wolframe_PROTOCOL_LINE_COMMAND_HANDLER_HPP_INCLUDED
#define _Wolframe_PROTOCOL_LINE_COMMAND_HANDLER_HPP_INCLUDED
#include "protocol/commandHandler.hpp"
#include "protocol/ioblocks.hpp"
#include "protocol/parser.hpp"
#include "connectionHandler.hpp"
#include "countedReference.hpp"
#include "logger-v1.hpp"
#include <vector>
#include <string>
#include <iostream>

namespace _Wolframe {
namespace protocol {

///\brief State machine definition of a LineCommandHandler
class LineCommandHandlerSTM
{
public:
	///\return -1 for terminate or a valid state in the state machine definition
	typedef int (*RunCommand)( void* obj, int argc, const char** argv, std::ostream& out);
	struct State
	{
		protocol::CmdParser<std::string> m_parser;
		std::vector<RunCommand> m_cmds;

		State()
		{
			m_parser.add("");
		}
		void defineCommand( const char* name_, RunCommand run_)
		{
			m_parser.add( name_);
			m_cmds.push_back( run_);
		}
	};

	int runCommand( std::size_t stateidx, std::size_t cmdidx, void* obj, int argc, const char** argv, std::ostream& out) const
	{
		const State& st = get( stateidx);
		if (cmdidx >= st.m_cmds.size()) throw std::logic_error( "illegal command reference");
		return st.m_cmds.at(cmdidx)( obj, argc, argv, out);
	}

	void defineState( std::size_t se)
	{
		if (se != m_statear.size()) throw std::logic_error( "state index defined not matching to enum given as state index");
		State st;
		m_statear.push_back( st);
	}

	void defineCommand( const char* name_, RunCommand run_)
	{
		m_statear.back().defineCommand( name_, run_);
	}

	const State& get( std::size_t idx) const
	{
		if (idx >= m_statear.size()) throw std::logic_error( "illegal state");
		return m_statear.at( idx);
	}
private:
	std::vector<State> m_statear;
};


class LineCommandHandler :public CommandHandler
{
public:
	LineCommandHandler( const LineCommandHandlerSTM* stm_);

	///\brief See Parent::setInputBuffer(void*,std::size_t,std::size_t,std::size_t)
	virtual void setInputBuffer( void* buf, std::size_t allocsize, std::size_t size, std::size_t itrpos);

	///\brief See Parent::setOutputBuffer(void*,std::size_t,std::size_t)
	virtual void setOutputBuffer( void* buf, std::size_t size, std::size_t pos);

	///\brief See Parent::nextOperation()
	virtual Operation nextOperation();

	///\brief See Parent::putInput(const void*,std::size_t)
	virtual void putInput( const void *begin, std::size_t bytesTransferred);

	///\brief See Parent::getInputBlock(void*&,std::size_t&)
	virtual void getInputBlock( void*& begin, std::size_t& maxBlockSize);

	///\brief See Parent::getOutput(const void*&,std::size_t&)
	virtual void getOutput( const void*& begin, std::size_t& bytesToTransfer);

	///\brief See Parent::getDataLeft(const void*&,std::size_t&)
	virtual void getDataLeft( const void*& begin, std::size_t& nofBytes);

	///\brief Get the error code of command execution to be returned to the client
	int statusCode() const				{return m_statusCode;}

private:
	InputBlock m_input;					///< buffer for network read messages
	OutputBlock m_output;					///< buffer for network write messages

	InputBlock::iterator m_itr;				///< iterator to scan protocol input
	InputBlock::iterator m_end;				///< iterator pointing to end of message buffer

	///\enum State
	///\brief Enumeration of processor states
	enum CommandState
	{
		Init,						///< start state, called first time in this session
		EnterCommand,					///< parse command
		ParseArgs,					///< parse command arguments
		ParseArgsEOL,					///< parse end of line after command arguments
		ProtocolError,					///< a protocol error (bad command etc) appeared and the rest of the line has to be discarded
		Terminate					///< terminate application processor session (close for network)
	};
	///\brief Returns the state as string for logging etc.
	///\param [in] i state to get as string
	static const char* stateName( CommandState i)
	{
		static const char* ar[] = {"Init","EnterCommand","ParseArgs","ParseArgsEOL","Processing","ProtocolError","DiscardInput","FlushOutput","Terminate"};
		return ar[i];
	}

	const LineCommandHandlerSTM* m_stm;			///< command level protocol state machine
	protocol::CArgBuffer< std::string > m_argBuffer;	///< buffer type for the command arguments
	std::string m_buffer;					///< line buffer
	CommandState m_cmdstateidx;				///< current state of command execution
	std::size_t m_stateidx;					///< current state in the STM
	int m_cmdidx;						///< index of the command to execute starting with 0 (-1 = undefined command)
};


///\brief defines a static function calling a member function with fixed signature
///\warning do not declare virtual method calls like this. It is not portable (GCC only) !
///\TODO make a static assert here for refusing virtual methods here
template <class T, int (T::*Method)( int argc, const char** argv, std::ostream& out)>
struct LineCommandHandlerWrapper
{
	static int function( void* this_, int argc, const char** argv, std::ostream& out)
	{
		return (((T*)this_)->*Method)( argc, argv, out);
	}
};


template <class LineCommandHandlerImpl>
struct LineCommandHandlerSTMTemplate :public LineCommandHandlerSTM
{
	LineCommandHandlerSTMTemplate& operator[]( std::size_t se)
	{
		defineState( se);
		return *this;
	}

	template <int (LineCommandHandlerImpl::*Method)(int,const char**,std::ostream&)>
	LineCommandHandlerSTMTemplate& cmd( const char* cmdname_)
	{
		defineCommand( cmdname_, &LineCommandHandlerWrapper<LineCommandHandlerImpl,Method>::function);
		return *this;
	}
};

}}
#endif

