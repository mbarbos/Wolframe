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
/// \file database/vm/instructionSet.hpp
/// \brief Defines the instruction set of the virtual machine defining database transactions
#ifndef _DATABASE_VIRTUAL_MACHINE_INSTRUCTION_SET_HPP_INCLUDED
#define _DATABASE_VIRTUAL_MACHINE_INSTRUCTION_SET_HPP_INCLUDED
#include <stdexcept>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <boost/cstdint.hpp>

namespace _Wolframe {
namespace db {
namespace vm {

/// \class InstructionSet
/// \brief Enumeration of instructions for the transaction VM with some static functions on them
struct InstructionSet
{
public:
	/// \enum OpCode
	/// \brief Implemented operation codes of the VM
	enum OpCode
	{
		// Control Flow Instructions:
		Op_EXIT,			//< exit (abort)
		Op_RETURN,			//< return from subroutine or termination of transaction
		Op_GOTO,			//< goto absolute address

		// Output Instructions:
		Op_OUTPUT_CONST,		//< print a value to output: constant
		Op_OUTPUT_PATH,			//< print a value to output: unique input path selection
		Op_OUTPUT_LOOPCNT,		//< print a value to output: FOREACH loop counter (first element is 0)
		Op_OUTPUT_SEL_IDX,		//< print a value to output: element in selected set (unique, one element set) adressed by column index
		Op_OUTPUT_SEL_NAM,		//< print a value to output: element in selected set (unique, one element set) adressed by column name
		Op_OUTPUT_ITR_IDX,		//< print a value to output: element in tuple set iterated adressed by column index
		Op_OUTPUT_ITR_NAM,		//< print a value to output: element in tuple set iterated adressed by column name
		Op_OUTPUT_ITR_COLUMN,		//< print the whole column: for each element print OPEN [column name], result value and CLOSE
		Op_OUTPUT_OPEN_ARRAY,		//< print an open tag of an array start to output (index in tagnametab)
		Op_OUTPUT_OPEN_ELEM,		//< print an open tag of an array element to output (index in tagnametab)
		Op_OUTPUT_OPEN,			//< print an open tag of a single element to output (index in tagnametab)
		Op_OUTPUT_CLOSE_ARRAY,		//< print a close tag of an array start to output (no argument)
		Op_OUTPUT_CLOSE_ELEM,		//< print a close tag of an array element to output (no argument)
		Op_OUTPUT_CLOSE,		//< print a close tag of a single element to output (no argument)
		Op_OUTPUT_ADD_SINK,		//< open a new output sink (e.g. for audit function inputs)

		// Assignment Instructions:
		Op_KEEP_RESULT,			//< keep result
		Op_SELECT_PARAMETER,		//< set value set selected as the parameter list of the subroutine executed
		Op_SELECT_LAST_RESULT,		//< set value set selected as the last result
		Op_SELECT_KEPT_RESULT,		//< set value set selected as a result named with KEEP

		// Iterator Instructions:
		Op_OPEN_ITER_LAST_RESULT,	//< open a value iterator (set cond flag on !empty): last result 
		Op_OPEN_ITER_KEPT_RESULT,	//< open a value iterator (set cond flag on !empty): index is referecing a kept result index
		Op_OPEN_ITER_PATH,		//< open a value iterator (set cond flag on !empty): index is referecing a path
		Op_OPEN_ITER_TUPLESET,		//< open a value iterator (set cond flag on !empty): index is referecing a tuple set
		Op_NEXT,			//< fetch the next value in iteration

		// Subroutine Call Instructions:
		Op_SUB_FRAME_OPEN,		//< start prepare a subroutine call or an own execution scope
		Op_SUB_ARG_CONST,		//< push argument for subroutine call: constant value
		Op_SUB_ARG_PATH,		//< push argument for subroutine call: path selection value
		Op_SUB_ARG_LOOPCNT,		//< push argument for subroutine call: FOREACH loop counter (first element is 0)
		Op_SUB_ARG_SEL_IDX,		//< push argument for subroutine call: element in selected set (unique, one element set) adressed by column index
		Op_SUB_ARG_SEL_NAM,		//< push argument for subroutine call: element in selected set (unique, one element set) adressed by column name
		Op_SUB_ARG_ITR_IDX,		//< push argument for subroutine call: element in tuple set iterated adressed by column index
		Op_SUB_ARG_ITR_NAM,		//< push argument for subroutine call: element in tuple set iterated adressed by column name
		Op_SUB_FRAME_CLOSE,		//< push state with parameters on stack for subroutine call. For the jump to the subroutine GOTO is used.

		// Scoping Instructions:
		Op_SCOPE_OPEN,			//< create a new scope context without calling a subroutine (inherits data from parent scope)
		Op_SCOPE_CLOSE,			//< restore the previous scope context

		// Database Instructions:
		Op_DBSTM_START,			//< open a statement to execute
		Op_DBSTM_BIND_CONST,		//< bind a statement parameter: constant value
		Op_DBSTM_BIND_PATH,		//< bind a statement parameter: path selection value
		Op_DBSTM_BIND_LOOPCNT,		//< bind a statement parameter: FOREACH loop counter (first element is 0)
		Op_DBSTM_BIND_SEL_IDX,		//< bind a statement parameter
		Op_DBSTM_BIND_SEL_NAM,		//< bind a statement parameter
		Op_DBSTM_BIND_ITR_IDX,		//< bind a statement parameter
		Op_DBSTM_BIND_ITR_NAM,		//< bind a statement parameter
		Op_DBSTM_HINT,			//< define a list of hints for open statement
		Op_DBSTM_EXEC,			//< execute open statement

		// Collect Results and Constraints:
		Op_RESULT_SET_INIT,
		Op_RESULT_CONSTRAINT_UNIQUE,
		Op_RESULT_CONSTRAINT_NONEMPTY,

		// Others:
		Op_NOP				//< no operation
	};
	static const char* opCodeName( OpCode i)
	{
		static const char* ar[] = {
			"EXIT",
			"RETURN",
			"GOTO",
	
			"OUTPUT_CONST",
			"OUTPUT_PATH",
			"OUTPUT_LOOPCNT",
			"OUTPUT_SEL_IDX",
			"OUTPUT_SEL_NAM",
			"OUTPUT_ITR_IDX",
			"OUTPUT_ITR_NAM",
			"OUTPUT_ITR_COLUMN",
			"OUTPUT_OPEN_ARRAY",
			"OUTPUT_OPEN_ELEM",
			"OUTPUT_OPEN",
			"OUTPUT_CLOSE_ARRAY",
			"OUTPUT_CLOSE_ELEM",
			"OUTPUT_CLOSE",
			"OUTPUT_ADD_SINK",

			"KEEP_RESULT",
			"SELECT_PARAMETER",
			"SELECT_LAST_RESULT",
			"SELECT_KEPT_RESULT",

			"OPEN_ITER_LAST_RESULT",
			"OPEN_ITER_KEPT_RESULT",
			"OPEN_ITER_PATH",
			"OPEN_ITER_TUPLESET",
			"NEXT",

			"SUB_FRAME_OPEN",
			"SUB_ARG_CONST",
			"SUB_ARG_PATH",
			"SUB_ARG_LOOPCNT",
			"SUB_ARG_SEL_IDX",
			"SUB_ARG_SEL_NAM",
			"SUB_ARG_ITR_IDX",
			"SUB_ARG_ITR_NAM",
			"SUB_FRAME_CLOSE",

			"SCOPE_OPEN",
			"SCOPE_CLOSE",
	
			"DBSTM_START",
			"DBSTM_BIND_CONST",
			"DBSTM_BIND_PATH",
			"DBSTM_BIND_LOOPCNT",
			"DBSTM_BIND_SEL_IDX",
			"DBSTM_BIND_SEL_NAM",
			"DBSTM_BIND_ITR_IDX",
			"DBSTM_BIND_ITR_NAM",
			"DBSTM_HINT",
			"DBSTM_EXEC",
	
			"RESULT_SET_INIT",
			"RESULT_CONSTRAINT_UNIQUE",
			"RESULT_CONSTRAINT_NONEMPTY",
	
			"NOP"
		};
		return ar[i];
	}

	/// \enum ArgumentType
	/// \brief Enumeration of argument types
	enum ArgumentType
	{
		At_None,			//< no argument
		At_Address,			//< index in ProgramCode
		At_Path,			//< index in Program::pathset
		At_Constant,			//< index in Program::constants
		At_ColumnName,			//< index in Program::colnametab
		At_TagName,			//< index in Program::tagnametab
		At_ResultName,			//< index in Program::resultnametab
		At_Statement,			//< index in Program::statements
		At_Hint,			//< index in Program::hinttab
		At_SubroutineSignature,		//< index in Program::signatures
		At_TupleSet,			//< index of tuple set
		At_SelectedColumnIdx,		//< Index of Column in selected set
		At_IteratorColumnIdx		//< Index of Column in tuple set iterator element
	};

	/// \brief Get the name of an argument type
	static const char* argumentTypeName( ArgumentType i)
	{
		static const char* ar[] =
		{
			"",
			"Address",
			"Path",
			"Constant",
			"ColumnName",
			"TagName",
			"ResultName",
			"Statement",
			"SubroutineSignature",
			"TupleSet",
			"SelectedColumnIdx",
			"IteratorColumnIdx"
		};
		return ar[i];
	}

	/// \brief get the argument type of an operation
	static ArgumentType argumentType( OpCode i)
	{
		static ArgumentType ar[] =
		{
			/*Op_EXIT*/			At_None,
			/*Op_RETURN*/			At_None,
			/*Op_GOTO*/			At_Address,

			/*Op_OUTPUT_CONST*/		At_Constant,
			/*Op_OUTPUT_PATH*/		At_Path,
			/*Op_OUTPUT_LOOPCNT*/		At_None,
			/*Op_OUTPUT_SEL_IDX*/		At_SelectedColumnIdx,
			/*Op_OUTPUT_SEL_NAM*/		At_ColumnName,
			/*Op_OUTPUT_ITR_IDX*/		At_IteratorColumnIdx,
			/*Op_OUTPUT_ITR_NAM*/		At_ColumnName,
			/*Op_OUTPUT_ITR_COLUMN*/	At_None,
			/*Op_OUTPUT_OPEN_ARRAY*/	At_TagName,
			/*Op_OUTPUT_OPEN_ELEM*/		At_None,
			/*Op_OUTPUT_OPEN*/		At_TagName,
			/*Op_OUTPUT_CLOSE_ARRAY*/	At_None,
			/*Op_OUTPUT_CLOSE_ELEM*/	At_None,
			/*Op_OUTPUT_CLOSE*/		At_None,
			/*Op_OUTPUT_ADD_SINK*/		At_None,

			/*Op_KEEP_RESULT*/		At_ResultName,
			/*Op_SELECT_PARAMETER*/		At_None,
			/*Op_SELECT_LAST_RESULT*/	At_None,
			/*Op_SELECT_KEPT_RESULT*/	At_ResultName,
	
			/*Op_OPEN_ITER_LAST_RESULT*/	At_None,
			/*Op_OPEN_ITER_KEPT_RESULT*/	At_ResultName,
			/*Op_OPEN_ITER_PATH*/		At_Path,
			/*Op_OPEN_ITER_TUPLESET*/	At_TupleSet,
			/*Op_NEXT*/			At_None,

			/*Op_SUB_FRAME_OPEN*/		At_SubroutineSignature,
			/*Op_SUB_ARG_CONST*/		At_Constant,
			/*Op_SUB_ARG_PATH*/		At_Path,
			/*Op_SUB_ARG_LOOPCNT*/		At_None,
			/*Op_SUB_ARG_SEL_IDX*/		At_SelectedColumnIdx,	
			/*Op_SUB_ARG_SEL_NAM*/		At_ColumnName,
			/*Op_SUB_ARG_ITR_IDX*/		At_IteratorColumnIdx,
			/*Op_SUB_ARG_ITR_NAM*/		At_ColumnName,
			/*Op_SUB_FRAME_CLOSE*/		At_None,

			/*Op_SCOPE_OPEN*/		At_None,
			/*Op_SCOPE_RESTORE*/		At_None,

			/*Op_DBSTM_START*/		At_Statement,
			/*Op_DBSTM_BIND_CONST*/		At_Constant,
			/*Op_DBSTM_BIND_PATH*/		At_Path,
			/*Op_DBSTM_BIND_LOOPCNT*/	At_None,
			/*Op_DBSTM_BIND_SEL_IDX*/	At_SelectedColumnIdx,
			/*Op_DBSTM_BIND_SEL_NAM*/	At_ColumnName,
			/*Op_DBSTM_BIND_ITR_IDX*/	At_IteratorColumnIdx,
			/*Op_DBSTM_BIND_ITR_NAM*/	At_ColumnName,
			/*Op_DBSTM_HINT*/		At_Hint,
			/*Op_DBSTM_EXEC*/		At_None,

			/*Op_RESULT_SET_INIT*/		At_None,
			/*Op_RESULT_CONSTRAINT_UNIQUE*/	At_None,
			/*Op_RESULT_CONSTRAINT_NONEMPTY*/At_None,
	
			/*Op_NOP*/			At_None
		};
		return ar[i];
	}

	/// \enum CondCode
	/// \brief Enumeration of conditional codes
	enum CondCode
	{
		Co_ALWAYS,			//< execute always
		Co_IF_COND,			//< execute, if cond flag set to true
		Co_NOT_IF_COND			//< execute, if cond flag set to false
	};

	/// \brief Get the name of a conditional code
	static const char* condCodeName( CondCode i)
	{
		static const char* ar[] = 
		{
			"",
			"IF_COND",
			"NOT_IF_COND"
		};
		return ar[i];
	}
	/// \brief Index of an argument
	typedef boost::uint32_t ArgumentIndex;
	/// \brief Address in program code
	typedef boost::uint32_t Address;
	/// \brief Instruction
	typedef boost::uint32_t Instruction;

	/// \brief Build an instruction from its parts
	/// \brief param[in] cond conditional code (condition for execution)
	/// \brief param[in] opcode instruction operation code
	/// \brief param[in] arg operation argument
	/// \return the instruction
	static Instruction instruction( CondCode cond, OpCode opcode, unsigned int arg=0)
	{
		Instruction rt = 0;
		if ((unsigned int)cond > Max_CondCode) throw std::runtime_error( "VM instruction condition code out of range");
		rt |= (Instruction)cond << Shift_CondCode;
		if ((unsigned int)opcode > Max_OpCode) throw std::runtime_error( "VM instruction opcode out of range");
		rt |= (Instruction)opcode << Shift_OpCode;
		if (arg > Max_ArgumentIndex) throw std::runtime_error( "VM instruction parameter reference out of range");
		rt |= (arg << Shift_ArgumentIndex);
		return rt;
	}
	/// \brief Build an instruction from its parts (no condition -> always executed)
	/// \brief param[in] opcode instruction operation code
	/// \brief param[in] arg operation argument
	/// \return the instruction
	static Instruction instruction( OpCode opcode, unsigned int arg=0)
	{
		return instruction( Co_ALWAYS, opcode, arg);
	}

	/// \brief Get the condition code of an instruction (defining on which condition the instruction is executed)
	/// \brief param[in] instr the instruction to extract the condition code from
	static CondCode condCode( const Instruction& instr)		{return static_cast<CondCode>((unsigned int)(instr&Mask_CondCode) >> Shift_CondCode);}
	/// \brief Get the operation code of an instruction (defining what is done on execution)
	/// \brief param[in] instr the instruction to extract the operation code from
	static OpCode opCode( const Instruction& instr)			{return static_cast<OpCode>((unsigned int)(instr&Mask_OpCode) >> Shift_OpCode);}
	/// \brief Get the argument index of the instruction (addressing the argument depending on the argument type of the instruction)
	/// \brief param[in] instr the instruction to extract the argument index from
	static ArgumentIndex argumentIndex( const Instruction& instr)	{return static_cast<ArgumentIndex>((unsigned int)(instr&Mask_ArgumentIndex) >> Shift_ArgumentIndex);}

	/// \brief Print the program code without any symbolic information that is not available here
	/// \brief param[out] out output stream to print to
	/// \brief param[out] prg vector of instructions to print
	static void printProgramRaw( std::ostream& out, const std::vector<Instruction>& prg)
	{
		std::vector<Instruction>::const_iterator pi = prg.begin(), pe = prg.end();
		unsigned int adr = 0;
		for (; pi != pe; ++pi,++adr)
		{
			CondCode cc = condCode( *pi);
			OpCode oc = opCode( *pi);
			ArgumentType at = argumentType( oc);
			ArgumentIndex ai = argumentIndex( *pi);
			out << "[" << adr << "] "<< condCodeName( cc) << " " << opCodeName( oc) << " ";
			if (at != At_None)
			{
				out << argumentTypeName(at) << " " << ai;
			}
			out << std::endl;
		}
	}

	/// \brief Get the instruction as string
	/// \return the instruction as string
	static std::string instructionstr( const Instruction& instr)
	{
		std::ostringstream out;
		CondCode cc = condCode( instr);
		OpCode oc = opCode( instr);
		ArgumentType at = argumentType( oc);
		ArgumentIndex ai = argumentIndex( instr);

		out << condCodeName( cc) << " " << opCodeName( oc) << " ";
		if (at != At_None)
		{
			out << argumentTypeName(at) << " " << ai;
		}
		return out.str();
	}

private:
	enum {
		BitCnt_OpCode=6,	Shift_OpCode=26,			//< location and size of the operation code in the instruction
		BitCnt_CondCode=2,	Shift_CondCode=24,			//< location and size of the condition code in the instruction
		BitCnt_ArgumentIndex=20,Shift_ArgumentIndex=0			//< location and size of the argument index in the instruction
	};
public:
	enum {
		Max_OpCode=((1<<BitCnt_OpCode)-1),				//< maximum value of the operation code in the instruction
		Max_CondCode=((1<<BitCnt_CondCode)-1),				//< maximum value of the condition code in the instruction
		Max_ArgumentIndex=((1<<BitCnt_ArgumentIndex)-1)			//< maximum value of the argument index in the instruction
	};
private:
	enum {
		Mask_OpCode=(Max_OpCode << Shift_OpCode),			//< separation bit mask of the operation code in the instruction
		Mask_CondCode=(Max_CondCode << Shift_CondCode),			//< separation bit mask of the condition code in the instruction
		Mask_ArgumentIndex=(Max_ArgumentIndex << Shift_ArgumentIndex)	//< separation bit mask of the argument index in the instruction
	};
};

}}}//namespace
#endif

