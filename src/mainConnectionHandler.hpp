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
/// \file mainConnectionHandler.hpp
///
#ifndef _Wolframe_MAIN_CONNECTION_HANDLER_HPP_INCLUDED
#define _Wolframe_MAIN_CONNECTION_HANDLER_HPP_INCLUDED
#include "mainCommandHandler.hpp"
#include "system/connectionHandler.hpp"
#include "processor/execContext.hpp"
#include "protocol/ioblocks.hpp"
#include "types/keymap.hpp"
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace proc {

/// \brief The wolframed connection handler
class MainConnectionHandler : public net::ConnectionHandler
{
public:
	/// \brief Constructor
	MainConnectionHandler( const net::LocalEndpoint& local);

	/// \brief Destructor
	virtual ~MainConnectionHandler();

	/// \brief Set the remote peer and indicate that the connection is up now.
	/// \param [in] remote remote peer
	virtual void setPeer( const net::RemoteEndpoint& remote);

	/// \brief Handle a request and produce a reply (statemachine of the processor)
	virtual const net::NetworkOperation nextOperation();

	/// \brief Passes the network input to the processor
	/// \param [in] begin start of the network input block.
	/// \param [in] bytesTransferred number of bytes passed in the input block
	/// \remark Begin is ignored because it points always to the same block as given by the read network message
	virtual void networkInput( const void *begin, std::size_t bytesTransferred);

	/// \brief Indicate that an unrecoverable error, a timeout or a terminate signal has occurred and the connection will be terminated
	virtual void signalOccured( NetworkSignal);

	/// \brief Set the reference to the execution context and initialize the protocol handler
	void setExecContext( proc::ExecContext* context_);

private:
	void initSessionExceptionBYE();

private:
	std::string m_protocol;				///< name of the protocol to use
	cmdbind::ProtocolHandlerR m_protocolHandler;	///< top level protocol handler
	protocol::InputBlock m_input;			///< buffer for network read messages
	protocol::OutputBlock m_output;			///< buffer for network write messages
	bool m_terminated;				///< true, if a termination signal came from the network
	std::string m_exceptionByeMessage;		///< message to terminate connection on exception
	const char* m_exceptionByeMessagePtr;		///< reference to message to terminate connection on exception
};
} // namespace proc
} // namespace _Wolframe
#endif

