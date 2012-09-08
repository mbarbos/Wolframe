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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
///\file filter/filter.hpp
///\brief Interface for filter class

#ifndef _Wolframe_FILTER_FILTER_INTERFACE_HPP_INCLUDED
#define _Wolframe_FILTER_FILTER_INTERFACE_HPP_INCLUDED
#include "types/countedReference.hpp"
#include "filter/inputfilter.hpp"
#include "filter/outputfilter.hpp"
#include <string>

namespace _Wolframe {
namespace langbind {

struct Filter
{
public:
	Filter( const InputFilterR& i_, const OutputFilterR& o_)
		:m_inputfilter(i_),m_outputfilter(o_){}
	Filter( const Filter& o)
		:m_inputfilter(o.m_inputfilter),m_outputfilter(o.m_outputfilter){}
	Filter(){}

	const InputFilterR& inputfilter() const		{return m_inputfilter;}
	const OutputFilterR& outputfilter() const	{return m_outputfilter;}
	InputFilterR& inputfilter()			{return m_inputfilter;}
	OutputFilterR& outputfilter()			{return m_outputfilter;}

	///\brief Get a member value of the filter
	///\param [in] name case sensitive name of the variable
	///\param [in] val the value returned
	///\return true on success, false, if the variable does not exist or the operation failed
	bool getValue( const char* name, std::string& val) const;

	///\brief Splits a name of the filter into its identifying parts (name + argument)
	///\param [in] id identifier of the filter (name + ":" + arguments)
	///\return pair describing the filter (name -> first, arguments -> second)
	static std::pair<std::string,std::string> identifier( const std::string& id);

	///\brief Set a member value of the filter
	///\param [in] name case sensitive name of the variable
	///\param [in] value new value of the variable to set
	///\return true on success, false, if the variable does not exist or the operation failed
	bool setValue( const char* name, const std::string& value);

protected:
	InputFilterR m_inputfilter;
	OutputFilterR m_outputfilter;
};

typedef Filter (*CreateFilterFunc)( const std::string& name, const std::string& arg);
typedef Filter* (*CreateFilterPtrFunc)( const std::string& name, const std::string& arg);


}}//namespace
#endif


