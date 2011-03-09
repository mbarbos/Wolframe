//
// connectionHandler.cpp
//

#include "wolframeHandler.hpp"
#include "logger.hpp"

#ifdef WITH_SSL
#include "SSLcertificateInfo.hpp"
#endif

#include <string>
#include <cstring>

#include "boost/date_time/posix_time/posix_time.hpp"		// to print time_t structures

namespace _Wolframe	{

	wolframeConnection::wolframeConnection( const Network::LocalEndpoint& local )
	{
		_Wolframe::Network::ConnectionEndpoint::ConnectionType type = local.type();

		switch ( type )	{
		case _Wolframe::Network::ConnectionEndpoint::TCP_CONNECTION:	{
			const _Wolframe::Network::LocalTCPendpoint& lcl = static_cast<const _Wolframe::Network::LocalTCPendpoint&>( local );
			LOG_TRACE << "Created connection handler for " << lcl.toString();
			break;
		}
#ifdef WITH_SSL
		case _Wolframe::Network::ConnectionEndpoint::SSL_CONNECTION:	{
			const _Wolframe::Network::LocalSSLendpoint& lcl = static_cast<const _Wolframe::Network::LocalSSLendpoint&>( local );
			LOG_TRACE << "Created connection handler (SSL) for " << lcl.toString();
			break;
		}
#else
		case _Wolframe::Network::ConnectionEndpoint::SSL_CONNECTION:
#endif // WITH_SSL
		default:
			LOG_FATAL << "Impossible local connection type !";
			abort();
		}


		state_ = NEW;
		dataStart_ = NULL;
		dataSize_ = 0;
		idleTimeout_ = 30;
	}


	wolframeConnection::~wolframeConnection()
	{
		LOG_TRACE << "Connection handler destroyed";
	}

	void wolframeConnection::setPeer( const Network::RemoteEndpoint& remote )
	{
		_Wolframe::Network::ConnectionEndpoint::ConnectionType type = remote.type();

		switch ( type )	{
		case _Wolframe::Network::ConnectionEndpoint::TCP_CONNECTION:	{
			const _Wolframe::Network::RemoteTCPendpoint& rmt = static_cast<const _Wolframe::Network::RemoteTCPendpoint&>( remote );
			LOG_TRACE << "Peer set to " << rmt.toString() << ", connected at " << rmt.connectionTime();
			break;
		}
#ifdef WITH_SSL
		case _Wolframe::Network::ConnectionEndpoint::SSL_CONNECTION:	{
			const _Wolframe::Network::RemoteSSLendpoint& rmt = static_cast<const _Wolframe::Network::RemoteSSLendpoint&>( remote );
			LOG_TRACE << "Peer set to " << rmt.toString() << ", connected at " << boost::posix_time::from_time_t( rmt.connectionTime());
			if ( rmt.SSLcertInfo() )	{
				LOG_TRACE << "Peer SSL certificate serial number " << rmt.SSLcertInfo()->serialNumber()
					  << ", issued by: " << rmt.SSLcertInfo()->issuer();
				LOG_TRACE << "Peer SSL certificate valid from " << boost::posix_time::from_time_t( rmt.SSLcertInfo()->notBefore())
					  << " to " <<  boost::posix_time::from_time_t( rmt.SSLcertInfo()->notAfter());
				LOG_TRACE << "Peer SSL certificate subject: " << rmt.SSLcertInfo()->subject();
				LOG_TRACE << "Peer SSL certificate Common Name: " << rmt.SSLcertInfo()->commonName();
			}
			break;
		}
#else
		case _Wolframe::Network::ConnectionEndpoint::SSL_CONNECTION:
#endif // WITH_SSL
		default:
			LOG_FATAL << "Impossible remote connection type !";
			abort();
		}
	}

	/// Handle a request and produce a reply.
	const Network::NetworkOperation wolframeConnection::nextOperation()
	{
		switch( state_ )	{
		case NEW:	{
				state_ = HELLO_SENT;
				return Network::NetworkOperation( Network::SendString( "Welcome to Wolframe.\n" ));
			}

		case HELLO_SENT:	{
				state_ = READ_INPUT;
				return Network::NetworkOperation( Network::ReadData( readBuf_, ReadBufSize, idleTimeout_ ));
			}

		case READ_INPUT:
			dataStart_ = readBuf_;
			// Yes, it continues with OUTPUT_MSG, sneaky, sneaky, sneaky :P

		case OUTPUT_MSG:
			if ( !strncmp( "quit", dataStart_, 4 ))	{
				state_ = TERMINATE;
				return Network::NetworkOperation( Network::SendString( "Thanks for using Wolframe.\n" ));
			}
			else	{
				char *s = dataStart_;
				for ( std::size_t i = 0; i < dataSize_; i++ )	{
					if ( *s == '\n' )	{
						s++;
						outMsg_ = std::string( dataStart_, s - dataStart_ );
						dataSize_ -= s - dataStart_;
						dataStart_ = s;
						state_ = OUTPUT_MSG;
						return Network::NetworkOperation( Network::SendString( outMsg_ ));
					}
					s++;
				}
				// If we got here, no \n was found, we need to read more
				// or close the connection if the buffer is full
				if ( dataSize_ >= ReadBufSize )	{
					state_ = TERMINATE;
					return Network::NetworkOperation( Network::SendString( "Line too long. Bye.\n" ));
				}
				else {
					memmove( readBuf_, dataStart_, dataSize_ );
					state_ = READ_INPUT;
					return Network::NetworkOperation( Network::ReadData( readBuf_ + dataSize_,
											     ReadBufSize - dataSize_,
											     idleTimeout_ ));
				}
			}

		case TIMEOUT:	{
				state_ = TERMINATE;
				return Network::NetworkOperation( Network::SendString( "Timeout. :P\n" ));
			}

		case SIGNALLED:	{
				state_ = TERMINATE;
				return Network::NetworkOperation( Network::SendString( "Server is shutting down. :P\n" ));
			}

		case TERMINATE:	{
				state_ = FINISHED;
				return Network::NetworkOperation( Network::CloseConnection() );
			}

		case FINISHED:
			LOG_DEBUG << "Processor in FINISHED state";
			break;

		} /* switch( state_ ) */
		return Network::NetworkOperation( Network::CloseConnection() );
	}


	/// Parse incoming data. The return value indicates how much of the
	/// input has been consumed.
	void wolframeConnection::networkInput( const void*, std::size_t bytesTransferred )
	{
		LOG_DATA << "network Input: Read " << bytesTransferred << " bytes";
		dataSize_ += bytesTransferred;
	}

	void wolframeConnection::timeoutOccured()
	{
		state_ = TIMEOUT;
		LOG_TRACE << "Processor received timeout";
	}

	void wolframeConnection::signalOccured()
	{
		state_ = SIGNALLED;
		LOG_TRACE << "Processor received signal";
	}

	void wolframeConnection::errorOccured( NetworkSignal signal )
	{
		switch( signal )	{
		case END_OF_FILE:
			LOG_TRACE << "Processor received EOF (read on closed connection)";
			break;

		case BROKEN_PIPE:
			LOG_TRACE << "Processor received BROKEN PIPE (write on closed connection)";
			break;

		case OPERATION_CANCELLED:
			LOG_TRACE << "Processor received OPERATION_CANCELED (should have been requested by us)";
			break;

		case UNKNOWN_ERROR:
			LOG_TRACE << "Processor received an UNKNOWN error from the framework";
			break;
		}
		state_ = TERMINATE;
	}


	/// ServerHandler PIMPL
	Network::connectionHandler* ServerHandler::ServerHandlerImpl::newConnection( const Network::LocalEndpoint& local )
	{
		return new wolframeConnection( local );
	}

	/// Outside face of the PIMPL
	ServerHandler::ServerHandler( const HandlerConfiguration* ) : impl_( new ServerHandlerImpl )	{}

	ServerHandler::~ServerHandler()	{ delete impl_; }

	Network::connectionHandler* ServerHandler::newConnection( const Network::LocalEndpoint& local )
	{
		return impl_->newConnection( local );
	}

} // namespace _Wolframe
