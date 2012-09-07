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
///\file filter_blob_filter.cpp
///\brief Filter implementation reading/writing of uninterpreted binary data

#include "filter/blob_filter.hpp"
#include <cstring>
#include <cstddef>
#include <algorithm>

using namespace _Wolframe;
using namespace langbind;

namespace {

///\class InputFilterImpl
///\brief input filter for data as single binary blob
struct InputFilterImpl :public InputFilter
{
	///\brief Constructor
	InputFilterImpl()
		:m_end(false)
		,m_done(false){}

	///\brief Copy constructor
	///\param [in] o output filter to copy
	InputFilterImpl( const InputFilterImpl& o)
		:InputFilter( o)
		,m_elembuf( m_elembuf)
		,m_end(o.m_end)
		,m_done(o.m_done){}

	///\brief self copy
	///\return copy of this
	virtual InputFilter* copy() const
	{
		return new InputFilterImpl( *this);
	}

	///\brief implement interface member InputFilter::putInput(const void*,std::size_t,bool)
	virtual void putInput( const void* ptr, std::size_t size, bool end)
	{
		m_elembuf.append( (const char*)ptr, size);
		m_end = end;
	}

	virtual void getRest( const void*& ptr, std::size_t& size, bool& end)
	{
		ptr = 0;
		size = 0;
		end = m_end;
	}

	///\brief implement interface member InputFilter::getNext( typename InputFilter::ElementType&,const void*&,std::size_t&)
	virtual bool getNext( InputFilter::ElementType& type, const void*& element, std::size_t& elementsize)
	{
		if (m_done)
		{
			type = InputFilter::CloseTag;
			element = 0;
			elementsize = 0;
			return true;
		}
		if (m_end)
		{
			type = InputFilter::Value;
			element = m_elembuf.c_str();
			elementsize = m_elembuf.size();
			m_done = true;
			return true;
		}
		return false;
	}
private:
	std::string m_elembuf;
	bool m_end;				//< true if we got EoD
	bool m_done;				//< true if we have finished
};

///\class OutputFilterImpl
///\brief output filter filter for data as binary blob
struct OutputFilterImpl :public OutputFilter
{
	///\brief Constructor
	OutputFilterImpl()
		:m_elemitr(0){}

	///\brief Copy constructor
	///\param [in] o output filter to copy
	OutputFilterImpl( const OutputFilterImpl& o)
		:OutputFilter(o)
		,m_elembuf(o.m_elembuf)
		,m_elemitr(o.m_elemitr){}

	///\brief self copy
	///\return copy of this
	virtual OutputFilter* copy() const
	{
		return new OutputFilterImpl( *this);
	}

	bool emptybuf()
	{
		std::size_t nn = m_elembuf.size() - m_elemitr;
		m_elemitr += write( m_elembuf.c_str() + m_elemitr, nn);
		if (m_elemitr == m_elembuf.size())
		{
			m_elembuf.clear();
			m_elemitr = 0;
			return true;
		}
		return false;
	}

	///\brief Implementation of OutputFilter::print(typename OutputFilter::ElementType,const void*,std::size_t)
	///\param [in] type type of the element to print
	///\param [in] element pointer to the element to print
	///\param [in] elementsize size of the element to print in bytes
	///\return true, if success, false else
	virtual bool print( OutputFilter::ElementType type, const void* element, std::size_t elementsize)
	{
		setState( Open);
		if (m_elemitr < m_elembuf.size())
		{
			// there is something to print left from last time
			if (!emptybuf())
			{
				setState( EndOfBuffer);
				return false;
			}
			//... we've done the emptying of the buffer left
			return true;
		}
		if (type == Value)
		{
			m_elembuf.append( (const char*)element, elementsize);
			if (!emptybuf())
			{
				setState( EndOfBuffer);
				return false;
			}
		}
		return true;
	}
private:
	std::string m_elembuf;				//< buffer for the currently printed element
	std::size_t m_elemitr;				//< iterator to pass it to output
};
}//end anonymous namespace


struct BlobFilter :public Filter
{
	BlobFilter()
	{
		m_inputfilter.reset( new InputFilterImpl());
		m_outputfilter.reset( new OutputFilterImpl());
	}
};

Filter _Wolframe::langbind::createBlobFilter( const std::string& name)
{
	enum {namelen=4};
	std::string nam( name);
	std::transform( nam.begin(), nam.end(), nam.begin(), ::tolower);
	if (nam != "blob") throw std::runtime_error( "filter name does not match");
	return BlobFilter();
}

Filter* _Wolframe::langbind::createBlobFilterPtr( const std::string& name)
{
	return new Filter( createBlobFilter( name));
}

