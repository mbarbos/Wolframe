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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
///\file database/vm/program.hpp
///\brief Interface for constructing and inspecting programs executing database transactions
#ifndef _DATABASE_VM_PROGRAM_HPP_INCLUDED
#define _DATABASE_VM_PROGRAM_HPP_INCLUDED
#include "database/vm/instructionSet.hpp"
#include "database/vm/programCode.hpp"
#include "database/vm/valueTupleSet.hpp"
#include "vm/nameTable.hpp"
#include "vm/errorHintTable.hpp"
#include "vm/selectorPathSet.hpp"
#include "database/vm/programImage.hpp"
#include "utils/fileLineInfo.hpp"
#include "types/keymap.hpp"
#include "types/variant.hpp"
#include <string>
#include <vector>
#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace db {
namespace vm {

class Program
	:public InstructionSet
{
public:
	typedef std::vector<std::string> SubroutineSignature;
	struct InstructionPos
	{
		std::size_t ip;
		utils::FileLineInfo pos;

		InstructionPos()
			:ip(0){}
		InstructionPos( std::size_t ip_, const utils::FileLineInfo& pos_)
			:ip(ip_),pos(pos_){}
		InstructionPos( const InstructionPos& o)
			:ip(o.ip),pos(o.pos){}
	};

public:
	ProgramCode code;				//< program code
	SelectorPathSet pathset;			//< input selector path expressions
	std::vector<types::Variant> constants;		//< constants
	NameTable colnametab;				//< column name table
	NameTable tagnametab;				//< tag table
	NameTable resultnametab;			//< result name table
	std::vector<std::string> statements;		//< database statements
	ErrorHintTable hinttab;				//< hints for error classes of failing database statements
	std::vector<SubroutineSignature> signatures;	//< subroutine signatures
	std::vector<ValueTupleSetR> tuplesets;		//< values from path expressions
	std::vector<InstructionPos> tdlpositions;	//< map of positions in tdl

public:
	Program(){}
	Program( const Program& o)
		:code(o.code)
		,pathset(o.pathset)
		,constants(o.constants)
		,colnametab(o.colnametab)
		,tagnametab(o.tagnametab)
		,resultnametab(o.resultnametab)
		,statements(o.statements)
		,hinttab(o.hinttab)
		,signatures(o.signatures)
		,tuplesets(o.tuplesets)
		,tdlpositions(o.tdlpositions)
	{}

	///\brief Return an image of this program reduced to its basic structures
	ProgramImage image() const;

	///\brief Add the program to this, joining all tables and doing necessary instruction patches
	void add( const Program& oth, bool doPatchGOTOs=true);

	void setCurrentSourceReference( const utils::FileLineInfo& posinfo);
	bool getSourceReference( std::size_t ip, utils::FileLineInfo& posinfo) const;

	void print( std::ostream& out) const;
	void printInstructionAt( std::ostream& out, const std::size_t& ip) const;
	void printInstruction( std::ostream& out, const Instruction& instr) const;

	std::string instructionString( const Instruction& instr) const;
	std::string instructionStringAt( const std::size_t& ip) const;
};

typedef boost::shared_ptr<Program> ProgramR;

}}}//namespace
#endif

