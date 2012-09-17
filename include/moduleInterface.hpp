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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
///
/// \file moduleInterface.hpp
///

#ifndef _MODULE_INTERFACE_HPP_INCLUDED
#define _MODULE_INTERFACE_HPP_INCLUDED

#include <string>
#include <cstring>
#include <list>
#include "config/configurationBase.hpp"
#include "constructor.hpp"

namespace _Wolframe {
namespace module {

///
class SimpleBuilder
{
	friend class ModulesDirectory;
public:
	SimpleBuilder( const char* id )
		: m_identifier( id )			{}

	virtual ~SimpleBuilder()			{}

	const char* identifier() const			{ return m_identifier; }

	virtual ObjectConstructorBase::ObjectType objectType() const = 0;
	virtual ObjectConstructorBase* constructor() = 0;
protected:
	const char* m_identifier;
};

///
class ConfiguredBuilder
{
	friend class ModulesDirectory;
public:
	ConfiguredBuilder( const char* title, const char* section, const char* keyword,
			   const char* id )
		: m_title( title ), m_section( section), m_keyword( keyword ),
		  m_identifier( id )			{}

	virtual ~ConfiguredBuilder()			{}

	const char* identifier() const			{ return m_identifier; }

	virtual ObjectConstructorBase::ObjectType objectType() const = 0;
	virtual config::NamedConfiguration* configuration( const char* logPrefix ) = 0;
	virtual ObjectConstructorBase* constructor() = 0;
protected:
	const char* m_title;		///< used for printing (logging etc.)
	const char* m_section;		///< configuration section to which the
					/// configuration parser reacts
	const char* m_keyword;		///< configuration keyword (element)
	const char* m_identifier;	///< identifier of the builder
};


/// Template for constructing a configured builder.
template < class Tconstructor, class Tconf >
class ConfiguredBuilderDescription : public ConfiguredBuilder
{
public:
	ConfiguredBuilderDescription( const char* title, const char* section,
				      const char* keyword, const char* id )
		: ConfiguredBuilder( title, section, keyword, id )	{}

	virtual ~ConfiguredBuilderDescription()		{}

	virtual config::NamedConfiguration* configuration( const char* logPrefix )	{
		return new Tconf( m_title, logPrefix, m_keyword );
	}
	virtual ObjectConstructorBase::ObjectType objectType() const	{
		return m_constructor.objectType();
	}
	virtual ObjectConstructorBase* constructor()	{
		return &m_constructor;
	}
private:
	Tconstructor	m_constructor;
};


//*********** Module interface *********

/// Function that constructs a configured builder.
/// This function is specific for each of the configured builders in the module.
typedef ConfiguredBuilder* (*createCfgdBuilderFunc)();

/// Function that constructs a simple (non-configured) builder.
/// This function is specific for each of the simple builders in the module.
typedef SimpleBuilder* (*createBuilderFunc)();


/// The module entry point structure. Only one entry point per module.
struct ModuleEntryPoint
{
	enum	SignSize	{
		MODULE_SIGN_SIZE = 16
	};

	char signature[MODULE_SIGN_SIZE];		///< module entry point signature
	unsigned short ifaceVersion;			///< version of the module loader interface
	const char* name;				///< name of the module
	void (*setLogger)(void*);			///< pointer to function that sets the
							/// the logger for the module objects
	unsigned short		cfgdContainers;		///< number of configured builders
	createCfgdBuilderFunc	*createCfgdBuilder;	///< the array of functions that create the configured builders
	unsigned short		containers;		///< number of simple (unconfigured) builders
	createBuilderFunc	*createBuilder;		///< the array of functions that create the simple builders
public:
	ModuleEntryPoint( unsigned short iVer, const char* modName,
			  void (*setLoggerFunc)(void*),
			  unsigned short nrContainers, createCfgdBuilderFunc* containerFunc,
			  unsigned short nrObjects, createBuilderFunc* objectFunc
			  )
		: ifaceVersion( iVer ), name( modName ),
		  setLogger( setLoggerFunc ),
		  cfgdContainers( nrContainers ), createCfgdBuilder( containerFunc ),
		  containers( nrObjects ), createBuilder( objectFunc )
	{
		std::memcpy ( signature, "Wolframe Module", MODULE_SIGN_SIZE );
		if ( createCfgdBuilder == NULL ) cfgdContainers = 0;
		if ( createBuilder == NULL ) containers = 0;
	}
};

extern "C" ModuleEntryPoint entryPoint;

}} // namespace _Wolframe::module

#endif // _MODULE_INTERFACE_HPP_INCLUDED
