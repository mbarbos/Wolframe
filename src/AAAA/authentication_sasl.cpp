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
//
// authentication_sasl.cpp
//

#include "AAAA/authentication_sasl.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/thread/thread.hpp>

#include <stdexcept>

#include "sasl/sasl.h"

#include "logger.hpp"

namespace _Wolframe {
namespace AAAA {

Authenticator *CreateSaslAuthenticator( AuthenticatorFactory::properties props )
{
	return new SaslAuthenticator(
		findprop<std::string>( props, "appname" ),
		findprop<std::string>( props, "service" )
	);
}

static int sasl_my_log( void *context, int priority, const char *message )
{
	if( message == NULL ) return SASL_BADPARAM;
	
	_Wolframe::log::LogLevel::Level level;
	
	switch( priority ) {
		case SASL_LOG_ERR:	level = _Wolframe::log::LogLevel::LOGLEVEL_ERROR; break;
		case SASL_LOG_NOTE:	level = _Wolframe::log::LogLevel::LOGLEVEL_NOTICE; break;
		default:		level = _Wolframe::log::LogLevel::LOGLEVEL_INFO; break;
	}

	_Wolframe::log::Logger( _Wolframe::log::LogBackend::instance() ).Get( level )
		<< _Wolframe::log::LogComponent::LogAuth
		<< "SASL " << message;

	return SASL_OK;
}

SaslAuthenticator::SaslAuthenticator( const std::string appName, const std::string service )
	: m_appName( appName ),
	  m_service( service )
{
// register callbacks
	callbacks[0].id = SASL_CB_LOG;
	callbacks[0].proc = (int (*)( ))&sasl_my_log;
	callbacks[0].context = this;
	callbacks[1].id = SASL_CB_LIST_END;
	callbacks[1].proc = NULL;
	callbacks[1].context = NULL;

// initialize the SASL library
	int result = sasl_server_init( callbacks, "test" );
	if( result != SASL_OK ) {
		throw new std::runtime_error( "Failed to initialize libsasl" );
	}

// create authentication session (TODO: must be outside constructor and callable from outside!)
/*
	result = sasl_server_new( m_service,
	
			   localdomain,
			   userdomain,
			   iplocal,
			   ipremote,
			   NULL,
			   serverlast,
			   &conn);
  if (result != SASL_OK)
    saslfail(result, "Allocating sasl connection state", NULL);
  
*/	
	m_state = _Wolframe_SASL_STATE_NEED_LOGIN;
}

SaslAuthenticator::~SaslAuthenticator( )
{
	sasl_done( );
}

Step::AuthStep SaslAuthenticator::nextStep( )
{
	switch( m_state ) {
		case _Wolframe_SASL_STATE_NEED_LOGIN:
			m_token = "login";
			return Step::_Wolframe_AUTH_STEP_RECV_DATA;

		case _Wolframe_SASL_STATE_NEED_PASS:
			m_token = "password";
			return Step::_Wolframe_AUTH_STEP_RECV_DATA;

		case _Wolframe_SASL_STATE_COMPUTE:
// user not found
			if( false ) {
				m_state = _Wolframe_SASL_STATE_NEED_LOGIN;
				goto FAIL;
			}
// user found, but password doesn't match
			if( false ) {
				m_state = _Wolframe_SASL_STATE_NEED_LOGIN;
				goto FAIL;
			}

// everythink is peachy
			m_state = _Wolframe_SASL_STATE_NEED_LOGIN;
			return Step::_Wolframe_AUTH_STEP_SUCCESS;
	}

FAIL:
	boost::this_thread::sleep( boost::posix_time::seconds( 1 ) );

	return Step::_Wolframe_AUTH_STEP_FAIL;
}

// never used
std::string SaslAuthenticator::sendData( )
{
	return 0;
}

std::string SaslAuthenticator::token( )
{
	return m_token;
}

void SaslAuthenticator::receiveData( const std::string data )
{
	switch( m_state ) {
		case _Wolframe_SASL_STATE_NEED_LOGIN:
			m_login = data;
			m_state = _Wolframe_SASL_STATE_NEED_PASS;
			break;

		case _Wolframe_SASL_STATE_NEED_PASS:
			m_pass = data;
			m_state = _Wolframe_SASL_STATE_COMPUTE;
			break;

// TODO: application exception
		case _Wolframe_SASL_STATE_COMPUTE:
			throw new std::runtime_error( "Illegal state in auhenticator" );
			break;
	}
}

std::string SaslAuthenticator::getError( )
{
	return "";
}

}} // namespace _Wolframe::AAAA
