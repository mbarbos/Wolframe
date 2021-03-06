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
/// \file password.cpp
/// Wolframe password hashes
//

#include <stdexcept>
#include <cstring>
#include <cassert>

#include "AAAA/passwordHash.hpp"
#include "crypto/PBKDF2.hpp"
#include "types/byte2hex.h"
#include "types/base64.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

using namespace _Wolframe::AAAA;
using namespace _Wolframe;

static const unsigned int PBKDF2_ROUNDS = 10589;

/****  Password Salt  ************************************************/
static const size_t PASSWORD_SALT_BCD_SIZE = PASSWORD_SALT_SIZE * 2 + 1;
static const size_t PASSWORD_SALT_BASE64_SIZE = (( PASSWORD_SALT_SIZE - 1 ) / 3 ) * 4 + 5;

PasswordHash::Salt::Salt()
{
	m_size = 0;
	memset( m_salt, 0, PASSWORD_SALT_SIZE );
}

PasswordHash::Salt::~Salt()
{
	m_size = 0;
	memset( m_salt, 0, PASSWORD_SALT_SIZE );
}

PasswordHash::Salt::Salt( const crypto::RandomGenerator& rndGen )
{
	m_size = PASSWORD_SALT_SIZE;
	rndGen.generate( m_salt, PASSWORD_SALT_SIZE );
}

/// \note The byte array is considered to be of size PASSWORD_SALT_SIZE
/// and it can not be changed.
PasswordHash::Salt::Salt( const unsigned char* data , size_t bytes )
{
	memset( m_salt, 0, PASSWORD_SALT_SIZE );
	m_size = bytes > PASSWORD_SALT_SIZE ? PASSWORD_SALT_SIZE : bytes;
	memcpy( m_salt, data, m_size );
}

PasswordHash::Salt::Salt( const std::string& str )
{
	int ret;
	memset( m_salt, 0, PASSWORD_SALT_SIZE );
	if (( ret = base64::decode( str, m_salt, PASSWORD_SALT_SIZE )) < 0 )	{
		std::string errMsg = "Cannot convert '" + str + "' to a password salt";
		throw std::runtime_error( errMsg );
	}
	m_size = ret;
}


bool PasswordHash::Salt::operator == ( const Salt& rhs )
{
	if ( m_size != rhs.m_size || memcmp( m_salt, rhs.m_salt, m_size ))
		return false;
	return true;
}


std::string PasswordHash::Salt::toBCD() const
{
	char	buffer[ PASSWORD_SALT_BCD_SIZE ];

	int len = byte2hex( m_salt, m_size, buffer, PASSWORD_SALT_BCD_SIZE );
	assert( len == (int)m_size * 2 );

	return std::string( buffer );
}

std::string PasswordHash::Salt::toString() const
{
	char	buffer[ PASSWORD_SALT_BASE64_SIZE ];

	int len = base64::encode( m_salt, m_size, buffer, PASSWORD_SALT_BASE64_SIZE, 0 );
	assert( len >= 0 && len < (int)PASSWORD_SALT_BASE64_SIZE );
	while ( len > 0 && buffer[ len - 1 ] == '=' )
		len--;
	buffer[ len ] = 0;
	return std::string( buffer );
}


/****  Password Hash  ************************************************/
static const size_t PASSWORD_HASH_BCD_SIZE = PASSWORD_HASH_SIZE * 2 + 1;
static const size_t PASSWORD_HASH_BASE64_SIZE = (( PASSWORD_HASH_SIZE - 1 ) / 3 ) * 4 + 5;

PasswordHash::Hash::Hash()
{
	memset( m_hash, 0, PASSWORD_HASH_SIZE );
}

PasswordHash::Hash::~Hash()
{
	memset( m_hash, 0, PASSWORD_HASH_SIZE );
}

/// \note The byte array is considered to be of size PASSWORD_SALT_SIZE
/// and it can not be changed.
PasswordHash::Hash::Hash( const unsigned char* data, size_t bytes )
{
	memset( m_hash, 0, PASSWORD_HASH_SIZE );
	memcpy( m_hash, data, (bytes > PASSWORD_HASH_SIZE ? PASSWORD_HASH_SIZE : bytes) );
}

PasswordHash::Hash::Hash( const std::string& str )
{
	int	ret;
	memset( m_hash, 0, PASSWORD_HASH_SIZE );
	if (( ret = base64::decode( str, m_hash, PASSWORD_HASH_SIZE )) < 0 )	{
		std::string errMsg = "Cannot convert '" + str + "' to a password hash";
		throw std::runtime_error( errMsg );
	}
	if ( (size_t)ret != PASSWORD_HASH_SIZE )	{
		std::string errMsg = "Password hash has wrong size, expected "
				     + boost::lexical_cast<std::string>( PASSWORD_HASH_SIZE ) + ", got "
				     + boost::lexical_cast<std::string>( ret );
		throw std::runtime_error( errMsg );
	}
}


bool PasswordHash::Hash::operator == ( const Hash& rhs )
{
	if ( memcmp( m_hash, rhs.m_hash, PASSWORD_HASH_SIZE ))
		return false;
	return true;
}


std::string PasswordHash::Hash::toBCD() const
{
	char	buffer[ PASSWORD_HASH_BCD_SIZE ];

	int len = byte2hex( m_hash, PASSWORD_HASH_SIZE, buffer, PASSWORD_HASH_BCD_SIZE );
	assert( len == (int)PASSWORD_HASH_SIZE * 2 );

	return std::string( buffer );
}

std::string PasswordHash::Hash::toString() const
{
	char	buffer[ PASSWORD_HASH_BASE64_SIZE ];

	int len = base64::encode( m_hash, PASSWORD_HASH_SIZE, buffer, PASSWORD_HASH_BASE64_SIZE, 0 );
	assert( len >= 0 && len < (int)PASSWORD_HASH_BASE64_SIZE );
	while ( len > 0 && buffer[ len - 1 ] == '=' )
		len--;
	buffer[ len ] = 0;
	return std::string( buffer );
}

/****  PasswordHash  *************************************************/
static void hashPassword( const unsigned char* pwdSalt, size_t saltSize,
			  const std::string& password, unsigned char* hash )
{
	crypto::PBKDF2_HMAC_SHA1 key( password, pwdSalt, saltSize, PASSWORD_HASH_SIZE, PBKDF2_ROUNDS );
	assert( key.size() == PASSWORD_HASH_SIZE );
	memcpy( hash, key.hash(), key.size() );
}

PasswordHash::PasswordHash( const crypto::RandomGenerator& rndGen, const std::string& password )
	: m_salt( rndGen )
{
	hashPassword( m_salt.salt(), m_salt.size(), password, m_hash.m_hash );
}

PasswordHash::PasswordHash( const unsigned char* pwdSalt, size_t bytes, const std::string& password )
	: m_salt( pwdSalt, bytes )
{
	hashPassword( m_salt.salt(), m_salt.size(), password, m_hash.m_hash );
}

PasswordHash::PasswordHash( const Salt& pwdSalt, const std::string& password )
	: m_salt( pwdSalt )
{
	hashPassword( m_salt.salt(), m_salt.size(), password, m_hash.m_hash );
}

PasswordHash::PasswordHash( const std::string& pwdSalt, const std::string& password )
	: m_salt( pwdSalt )
{
	hashPassword( m_salt.salt(), m_salt.size(), password, m_hash.m_hash );
}

PasswordHash::PasswordHash( const std::string& str )
{
	std::string s = boost::algorithm::trim_copy( str );
	if ( s[ 0 ] == '$' )	{
		size_t hashStart = s.find( "$", 1 );
		if ( hashStart == s.npos )	{
			std::string errMsg = "'" + s + "' is not a valid password hash";
			throw std::runtime_error( errMsg );
		}
		m_salt = Salt( s.substr( 1, hashStart - 1 ));
		m_hash = Hash( s.substr( hashStart + 1 ));
	}
	else	{
		m_hash = Hash( str );
	}
}

std::string PasswordHash::toBCD() const
{
	return std::string( "$" ) + m_salt.toBCD() + "$" + m_hash.toBCD();
}

std::string PasswordHash::toString() const
{
	return std::string( "$" ) + m_salt.toString() + "$" + m_hash.toString();
}
