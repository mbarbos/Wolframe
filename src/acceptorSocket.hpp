/************************************************************************

 Copyright (C) 2011 - 2013 Project Wolframe.
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
//\file acceptorSocket.hpp
//\brief Acceptor on socket
#ifndef _Wolframe_ACCEPTOR_SOCKET_HPP_INCLUDED
#define _Wolframe_ACCEPTOR_SOCKET_HPP_INCLUDED
#include "connectionTypeSocket.hpp"
#include "system/connectionHandler.hpp"
#include <boost/thread/mutex.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>

namespace _Wolframe {
namespace net {

//\brief Acceptor of new connections
class AcceptorSocket
	:private boost::noncopyable
{
public:
	//\brief Construct a connection with the given io_service.
	AcceptorSocket( boost::asio::io_service* IOservice,
			const std::string& host, unsigned short port, unsigned maxConnections,
			types::SyncCounter* globalCounter_,
			ServerHandler* srvHandler );
	~AcceptorSocket();

	//\brief Stop the acceptor. Outstanding asynchronous operations will be completed.
	void stop();

private:
	//\brief Handle completion of an asynchronous accept operation.
	void handleAccept( const boost::system::error_code& e );

	//\brief Handle a request to stop the server.
	void handleStop();

private:
	boost::asio::io_service*		m_IOservice;	// The io_service used to perform asynchronous operations.
	boost::asio::io_service::strand		m_strand;	// Strand to ensure the acceptor's handlers are not called concurrently.
	boost::asio::ip::tcp::acceptor		m_acceptor;	// Acceptor(s) used to listen for incoming connections.
	ConnectionTypeSocketR			m_newConnection;// The next connection to be accepted.
	types::SyncCounter			m_connectionCounter;
	types::SyncCounter*			m_globalCounter;

	std::string				m_identifier;
	_Wolframe::ServerHandler*		m_srvHandler;	// The handler for all incoming requests.
};

}}//namespace
#endif

