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
//
// Authentication factory implementation
//

#include <stdexcept>

#include "AAAAproviderImpl.hpp"
#include "logger-v1.hpp"
#include "boost/algorithm/string.hpp"
#include "module/moduleDirectory.hpp"

namespace _Wolframe {
namespace AAAA {

AuthenticationFactory::AuthenticationFactory( const std::list< config::NamedConfiguration* >& confs,
					  const module::ModulesDirectory* modules )
{
	for ( std::list<config::NamedConfiguration*>::const_iterator it = confs.begin();
							it != confs.end(); it++ )	{
		module::ConfiguredBuilder* builder = modules->getBuilder((*it)->className());
		if ( builder )	{
			ConfiguredObjectConstructor< AuthenticationUnit >* auth =
					dynamic_cast< ConfiguredObjectConstructor< AuthenticationUnit >* >( builder->constructor());
			if ( auth == NULL )	{
				LOG_ALERT << "AuthenticationFactory: '" << builder->objectClassName()
					  << "' is not an Authentication Unit builder";
				throw std::logic_error( "object is not an AuthenticationUnit builder" );
			}
			m_authUnits.push_back( auth->object( **it ));
			LOG_TRACE << "'" << auth->objectClassName() << "' authentication unit registered";
		}
		else	{
			LOG_ALERT << "AuthenticationFactory: unknown authentication type '" << (*it)->className() << "'";
			throw std::domain_error( "Unknown authentication mechanism type in AAAAprovider constructor. See log" );
		}
	}

	// Iterate through the list of authenticators (built at this point)
	// and build the vector of available mechs
	for ( std::list< AuthenticationUnit* >::const_iterator ait = m_authUnits.begin();
						ait != m_authUnits.end(); ait ++ )	{
		// add unit mechs to the list
		const std::string* p_mech = (*ait)->mechs();
		if ( p_mech->empty() )	{
			LOG_WARNING << "'" << (*ait)->className() << "' has no authentication mechanisms";
		}
		while ( ! p_mech->empty() )	{
			std::string mech = boost::to_upper_copy( *p_mech );
			bool exists = false;
			for( std::vector< std::string >::const_iterator mit = m_mechs.begin();
						mit != m_mechs.end(); mit++ )	{
				if ( *mit == mech )
					exists = true;
			}
			if ( ! exists )	{
				m_mechs.push_back( mech );
				LOG_TRACE << "'" << mech << "' authentication mechanism registered";
			}
		}
	}
}

AuthenticationFactory::~AuthenticationFactory()
{
	for ( std::list< AuthenticationUnit* >::const_iterator it = m_authUnits.begin();
								it != m_authUnits.end(); it++ )
		delete *it;
}

bool AuthenticationFactory::resolveDB( const db::DatabaseProvider& db )
{
	for ( std::list< AuthenticationUnit* >::const_iterator it = m_authUnits.begin();
								it != m_authUnits.end(); it++ )
		if ( ! (*it)->resolveDB( db ) )
			return false;
	return true;
}


Authenticator* AuthenticationFactory::authenticator()
{
	return new StandardAuthenticator( m_mechs, m_authUnits );
}


//*********************************************************************************
// Standard authenticator
StandardAuthenticator::StandardAuthenticator( const std::vector<std::string>& mechs_,
					      const std::list<AuthenticationUnit *> &units_ )
	: m_mechs( mechs_ ), m_authUnits( units_ ),
	  m_status( INITIALIZED ), m_selectedSlice( NULL ), m_user( NULL )
{
}

StandardAuthenticator::~StandardAuthenticator()
{
	for ( std::vector< AuthenticatorSlice* >::iterator it = m_slices.begin();
							it != m_slices.end(); it++ )
		delete *it;
	if ( m_user )
		delete m_user, m_user = NULL;
}

void StandardAuthenticator::destroy()
{
	delete this;
}

// Get the list of available mechs
const std::vector< std::string >& StandardAuthenticator::mechs() const
{
	return m_mechs;
}

// Set the authentication mech
bool StandardAuthenticator::setMech( const std::string& mech,
				     const net::RemoteEndpoint& client  )
{
	// Reset all data
	if ( m_user )
		delete m_user, m_user = NULL;
	for ( std::vector< AuthenticatorSlice* >::iterator it = m_slices.begin();
							it != m_slices.end(); it++ )
		delete *it;
	m_selectedSlice = NULL;

	bool	mechAvailable = false;

	LOG_TRACE << "StandardAuthenticator: set the authentication mechanism to '"
		  << mech << "'";
	if ( mech.empty() )	{
		LOG_WARNING << "StandardAuthenticator: request for an empty authentication mechanism";
		m_status = MECH_UNAVAILABLE;
		return false;
	}
	// First check if the requested mech is in the list
	for ( std::vector< std::string >::const_iterator it = m_mechs.begin();
						it != m_mechs.end(); it ++ )	{
		if ( boost::iequals( mech, *it ) )	{
			LOG_TRACE << "StandardAuthenticator: authentication mechanism '"
				  << mech << "' found";
			mechAvailable = true;
			break;
		}
	}
	if ( ! mechAvailable )	{
		m_status = MECH_UNAVAILABLE;
		LOG_DEBUG << "StandardAuthenticator: authentication mechanism '"
			  << mech << "' not found";
		return false;
	}

	for ( std::list< AuthenticationUnit* >::const_iterator it = m_authUnits.begin();
							it != m_authUnits.end(); it++ )	{
		AuthenticatorSlice* slice = (*it)->slice( mech, client );
		if ( slice != NULL )	{
			LOG_TRACE << "StandardAuthenticator: authentication mechanism '"
				  << mech << "' provided by '" << (*it)->className();
			m_slices.push_back( slice );
		}
	}
	// Note the the following logic is badly flawed. A lot more knowledge is needed
	// in order to do this reasonably correct
	if ( ! m_slices.empty() )	{
		m_selectedSlice = m_slices[ 0 ];
		switch ( m_selectedSlice->status() )	{
			case AuthenticatorSlice::MESSAGE_AVAILABLE:
				m_status = MESSAGE_AVAILABLE;
				break;
			case AuthenticatorSlice::AWAITING_MESSAGE:
				m_status = AWAITING_MESSAGE;
				break;
			case AuthenticatorSlice::AUTHENTICATED:
				throw std::logic_error( "StandardAuthenticator (setMech): authentication slice status is AUTHENTICATED" );
				m_status = SYSTEM_FAILURE;
				break;
			case AuthenticatorSlice::INVALID_CREDENTIALS:
				throw std::logic_error( "StandardAuthenticator (setMech): authentication slice status is INVALID_CREDENTIALS" );
				m_status = SYSTEM_FAILURE;
				break;
			case AuthenticatorSlice::SYSTEM_FAILURE:
				LOG_DEBUG << "StandardAuthenticator: authentication slice '"
					  << m_selectedSlice->typeName() << "' status is SYSTEM_FAILURE";
				m_status = SYSTEM_FAILURE;
				break;
		}
		return true;
	}
	else	{
		m_status = MECH_UNAVAILABLE;
		LOG_DEBUG << "StandardAuthenticator: authentication mechanism '"
			  << mech << "' is not available for this client";
		return false;
	}
}

// The input message
void StandardAuthenticator::messageIn( const std::string& message )
{
	// Missing a lot here ....

	if ( m_status != AWAITING_MESSAGE )
		throw std::logic_error( "StandardAuthenticator: unexpected message received" );
	if ( m_selectedSlice != NULL )
		m_selectedSlice->messageIn( message );
	else
		throw std::logic_error( "StandardAuthenticator: message received but no authentication slice selected" );
}

// The output message
const std::string& StandardAuthenticator::messageOut()
{
	// Missing a lot here ....

	if ( m_status != MESSAGE_AVAILABLE )
		throw std::logic_error( "StandardAuthenticator: unexpected request for output message" );
	if ( m_selectedSlice == NULL )
		throw std::logic_error( "StandardAuthenticator: message requested but no authentication slice selected" );
	return m_selectedSlice->messageOut();
}

// The authenticated user or NULL if not authenticated
User* StandardAuthenticator::user()
{
	AAAA::User* retUser = m_user;
	if ( m_user )
		m_user = NULL;
	return retUser;
}

}} // namespace _Wolframe::AAAA
