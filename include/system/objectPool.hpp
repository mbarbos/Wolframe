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
/// \brief Thread capable pool of objects
/// \file system/objectPool.hpp

#ifndef _OBJECT_POOL_HPP_INCLUDED
#define _OBJECT_POOL_HPP_INCLUDED

#include <vector>
#include <stdexcept>

#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>

namespace _Wolframe {

/// Timeout exception for object pool
class ObjectPoolTimeout : public std::runtime_error
{
public:
	ObjectPoolTimeout()
		:std::runtime_error( "object pool exception"){}
};

/// \class ObjectPool
/// \tparam objectType type of object stored in the pool
/// \brief Class represening a pool of objects
template < typename objectType >
class ObjectPool	{
public:
	ObjectPool( const unsigned to )		{ m_timeout = to; }
	ObjectPool()				{ m_timeout = 0; }
	~ObjectPool()	{
		boost::lock_guard<boost::mutex> lock( m_mutex );
		if ( !m_availList.empty() )
			throw std::runtime_error( "ObjectPool not empty at destruction" );
	}

	std::size_t available() const		{ return m_availList.size(); }

	objectType get()	{
		while( true )	{
			boost::unique_lock<boost::mutex> lock( m_mutex );
			if ( !m_availList.empty())	{
				objectType obj = m_availList.back();
				m_availList.pop_back();
				lock.unlock();
				return obj;
			}
			else	{
				if ( m_timeout == 0 )	{
					while( m_availList.empty() )
						m_cond.wait( lock );
				}
				else {
					boost::system_time absTime = boost::get_system_time()
							+ boost::posix_time::seconds( m_timeout );
					while( m_availList.empty() )
						if ( ! m_cond.timed_wait( lock, absTime ))
							throw ObjectPoolTimeout();
				}
			}
		}
		throw std::logic_error( "Logic error #1 in ObjectPool" );
	}

	void add( objectType obj )	{
		boost::lock_guard<boost::mutex> lock( m_mutex );
		m_availList.push_back( obj );
		m_cond.notify_one();
	}

	static void static_add( ObjectPool* pool, objectType obj)
	{
		pool->add( obj);
	}

	unsigned timeout() const		{ return m_timeout; }
	void timeout( unsigned to )		{ m_timeout = to; }
private:
	std::vector< objectType >	m_availList;	///< list (vector really) of available objects
	boost::mutex			m_mutex;	///< condition variable associated mutex
	boost::condition_variable	m_cond;		///< the condition variable
	unsigned			m_timeout;	///< acquire timeout
};

} // namespace _Wolframe

#endif // _OBJECT_POOL_HPP_INCLUDED
