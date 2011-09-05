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
// AAAA provider
//

#ifndef _AAAA_PROVIDER_HPP_INCLUDED
#define _AAAA_PROVIDER_HPP_INCLUDED

#include "config/configurationBase.hpp"
#include "AAAA/authentication.hpp"
#include "AAAA/audit.hpp"
#include "container.hpp"
#include "moduleInterface.hpp"

#include "database/database.hpp"
#include "../database/DBprovider.hpp"

#include <string>
#include <list>

namespace _Wolframe {
namespace AAAA {

class AAAAconfiguration : public config::ConfigurationBase
{
	friend class AAAAprovider;
	friend class config::ConfigurationParser;
public:
	/// x-structor
	AAAAconfiguration();
	~AAAAconfiguration();

	/// methods
	bool check() const;
	void print( std::ostream& os, size_t indent ) const;
	void setCanonicalPathes( const std::string& referencePath );
private:
	bool						m_allowAnonymous;
	std::list< config::ContainerConfiguration* >	m_authConfig;
	std::list< config::ContainerConfiguration* >	m_auditConfig;
};


class AuthenticationGroup
{
public:
	AuthenticationGroup( const std::list< config::ContainerConfiguration* >& confs,
			     module::ModuleContainerDescription<Container < Authenticator >, config::ContainerConfiguration>* description,
			     size_t descrSize );
	~AuthenticationGroup();
	bool resolveDB( db::DatabaseProvider& db );

	Authenticator* authenticator()		{ return NULL; }
private:
	std::list< Container < Authenticator >* > m_authenticators;
};


class AuditGroup
{
public:
	AuditGroup( const std::list< config::ContainerConfiguration* >& confs,
		    module::ModuleContainerDescription< Container< Auditor >, config::ContainerConfiguration >* description,
		    size_t descrSize );
	~AuditGroup();
	bool resolveDB( db::DatabaseProvider& db );

	Auditor* auditor()			{ return NULL; }
private:
	std::list< Container< Auditor >* >	m_auditors;
};


class AAAAprovider
{
public:
	AAAAprovider( const AAAAconfiguration& conf );
	~AAAAprovider()				{}
	bool resolveDB( db::DatabaseProvider& db );

	Authenticator* authenticator()		{ return m_authenticator.authenticator(); }
	Auditor* auditor()			{ return m_auditor.auditor(); }
private:
	AuthenticationGroup	m_authenticator;
	AuditGroup		m_auditor;
};

}} // namespace _Wolframe::AAAA

#endif // _AAAA_PROVIDER_HPP_INCLUDED
