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
// a test module
//

#include "moduleInterface.hpp"
#include "logger-v1.hpp"
#include "mod_test.hpp"

_Wolframe::log::LogBackend*	logBackendPtr;

namespace _Wolframe {
namespace module {
namespace test {

TestModuleConfig::TestModuleConfig( const char* cfgName, const char* logParent, const char* logName )
	: config::ObjectConfiguration( cfgName, logParent, logName )
{			
	MOD_LOG_NOTICE << "Test module config created";
}

bool TestModuleConfig::parse( const config::ConfigurationTree& pt, const std::string& node,
    const module::ModulesDirectory* modules )
{
	return true;
}

bool TestModuleConfig::check( ) const
{
	return true;
}

void TestModuleConfig::print( std::ostream& os, size_t indent ) const
{
	std::string indStr( indent, ' ' );
	os << indStr << sectionName() << ": no config " << std::endl;
}

void TestModuleConfig::setCanonicalPathes( const std::string& refPath )
{
}

TestModuleContainer::TestModuleContainer( const TestModuleConfig& conf )
{
	MOD_LOG_NOTICE << "Test module container created";
}

TestUnitBase::~TestUnitBase( )
{
}

const std::string TestUnit::hello( )
{
	return "hello";
}

bool TestUnit::resolveDB( const db::DatabaseProvider& /* db */ )
{
	return true;
}

static ModuleContainer* createModule( void )
{
	static module::ContainerDescription< test::TestModuleContainer,
		test::TestModuleConfig > mod( "Test Module", "Test", "test", "TestObject" );
	return &mod;
}

static void setModuleLogger( void* logger )
{
	logBackendPtr = reinterpret_cast< _Wolframe::log::LogBackend* >( logger );
}

ModuleEntryPoint entryPoint( 0, CONTAINER_MODULE, "Test Module",
			     createModule, setModuleLogger );

}}} // namespace _Wolframe::module:test
