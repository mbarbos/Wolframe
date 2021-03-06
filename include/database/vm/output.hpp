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
/// \file database/vm/output.hpp
/// \brief Defines the output of a virtual machine for database transactions
#ifndef _DATABASE_VIRTUAL_MACHINE_OUTPUT_HPP_INCLUDED
#define _DATABASE_VIRTUAL_MACHINE_OUTPUT_HPP_INCLUDED
#include "types/variant.hpp"
#include <vector>
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace db {
namespace vm {

/// \class Output
/// \brief Output structure of the VM for transactions
class Output
{
public:
	/// \brief Default constructor
	Output()
	{
		addOutputSink();
	}
	/// \brief Copy constructor
	Output( const Output& o)
		:m_ar(o.m_ar){}

	/// \class Element
	/// \brief Element of output
	class Element
	{
	public:
		/// \brief Operation of output
		enum Operation
		{
			Open,			//< open of a single element
			OpenArray,		//< open of an array 
			OpenArrayElement,	//< open of an array element
			Close,			//< close of a single element
			CloseArray,		//< close of an array
			CloseArrayElement,	//< close of an array element
			Value			//< content value
		};
		/// \brief Default constructor
		Element()
			:m_op(Value){}
		/// \brief Copy constructor
		Element( const Element& o)
			:m_op(o.m_op),m_arg(o.m_arg){}
		/// \brief Constructor
		Element( const Operation& op_, const types::Variant& arg_)
			:m_op(op_),m_arg(arg_){}
		/// \brief Constructor
		explicit Element( const Operation& op_)
			:m_op(op_){}

		/// \brief Get the operation of output
		Operation op() const			{return m_op;}
		/// \brief Get the argument of output
		const types::Variant& arg() const	{return m_arg;}

	private:
		Operation m_op;				///< operation of output
		types::Variant m_arg;			///< argument of output
	};

	/// \brief Add element to output
	/// \param[in] elem element to add
	void add( const Element& elem)
	{
		m_ar.back().push_back( elem);
	}
	/// \brief Add a value element to output
	/// \param[in] value value of element to add
	void addValue( const types::Variant& value)
	{
		m_ar.back().push_back( Element( Element::Value, value));
	}

	/// \brief Add a new output sink and declare it as the current output
	void addOutputSink()
	{
		m_ar.push_back( ElementArray());
	}

	typedef std::vector<Element>::const_iterator const_iterator;
	/// \brief Get the start iterator on this output
	const_iterator begin( std::size_t i=0) const			{if (i>=m_ar.size()) throw std::runtime_error("array bound access"); return m_ar[i].begin();}
	/// \brief Get the end iterator on this output
	const_iterator end( std::size_t i=0) const			{if (i>=m_ar.size()) throw std::runtime_error("array bound access"); return m_ar[i].end();}

	/// \brief Get the array of output elements
	const std::vector<Element>& elements( std::size_t i=0) const	{if (i>=m_ar.size()) throw std::runtime_error("array bound access"); return m_ar[i];}

private:
	typedef std::vector<Element> ElementArray;
	std::vector<ElementArray> m_ar;			//< array of output element vectors (first element is the transaction output and the following are the auditing function inputs)
};

typedef boost::shared_ptr<Output> OutputR;

}}}//namespace
#endif

