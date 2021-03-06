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
///\file tprocHandlerTest.cpp
///\brief Class unit tests using google test framework (gTest)

#include "tprocHandler.hpp"
#include "handlerConfig.hpp"
#include "system/connectionHandler.hpp"
#include "appConfig.hpp"
#include "handlerConfig.hpp"
#include "wtest/testReport.hpp"
#include "wtest/testDescription.hpp"
#include "module/moduleDirectory.hpp"
#include "prgbind/programLibrary.hpp"
#include "config/configurationTree.hpp"
#include "processor/procProvider.hpp"
#include "wtest/testHandlerTemplates.hpp"
#include "utils/fileUtils.hpp"
#include "logger-v1.hpp"
#include "wtest/testModules.hpp"
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include "gtest/gtest.h"
#include <boost/thread/thread.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <ctime>

using namespace _Wolframe;
using namespace _Wolframe::tproc;

static int g_gtest_ARGC = 0;
static char* g_gtest_ARGV[2] = {0, 0};
static boost::filesystem::path g_testdir;

static module::ModulesDirectory* g_modulesDirectory = 0;
static boost::filesystem::path g_referencePath;

static boost::shared_ptr<proc::ProcessorProvider> getProcProvider( const proc::ProcProviderConfig* cfg, prgbind::ProgramLibrary* prglib)
{
	boost::shared_ptr<proc::ProcessorProvider>  rt( new proc::ProcessorProvider( cfg, g_modulesDirectory, prglib));
	rt->loadPrograms();
	return rt;
}

class TestConfiguration :public tproc::Configuration
{
public:
	TestConfiguration( const TestConfiguration& o)
		:Configuration(o)
		,m_appConfig(o.m_appConfig)
	{}

	TestConfiguration()
	{
		m_appConfig.addModules( g_modulesDirectory);
		m_appConfig.addConfig( "proc", this);
		m_appConfig.addConfig( "provider", &m_procConfig);

		boost::filesystem::path configFile( g_testdir / "temp" / "test.cfg");
		if (utils::fileExists( configFile.string()))
		{
			if (!m_appConfig.parse( configFile.string().c_str(), config::ApplicationConfiguration::CONFIG_INFO))
			{
				throw std::runtime_error( "Error in configuration");
			}
		}
		m_appConfig.finalize();
	}

	const proc::ProcProviderConfig* providerConfig() const
	{
		return &m_procConfig;
	}

private:
	config::ApplicationConfiguration m_appConfig;
	proc::ProcProviderConfig m_procConfig;
};

class TProcHandlerTest : public ::testing::TestWithParam<std::string>
{
protected:
	TProcHandlerTest() {}
	virtual ~TProcHandlerTest() {}
	virtual void SetUp() {}
	virtual void TearDown() {}
};

class TProcHandlerTestInstance
{
public:
	TProcHandlerTestInstance( const wtest::TestDescription& descr, TestConfiguration* config, std::size_t ib, std::size_t ob)
		:ep( new net::LocalTCPendpoint( "127.0.0.1", 12345))
		,m_connection(0)
		,m_config( config)
		,m_input( descr.input)
		,m_expected( descr.expected)
	{
		m_config->setBuffers( ib + EoDBufferSize, ob + MinOutBufferSize);
		m_connection = new tproc::Connection( ep, m_config);
		m_provider = getProcProvider( m_config->providerConfig(), &m_prglib);
		m_execContext.reset( new proc::ExecContext( m_provider.get(), 0));
		m_connection->setExecContext( m_execContext.get());
	}

	~TProcHandlerTestInstance()
	{
		delete m_connection;
	}

	const std::string& input() const {return m_input;}
	const std::string& output() const {return m_output;}
	const std::string& expected() const {return m_expected;}

	int run()
	{
		m_output.clear();
		return test::runTestIO( m_input, m_output, *m_connection);
	}

private:
	net::LocalEndpointR ep;
	tproc::Connection* m_connection;
	boost::shared_ptr<proc::ProcessorProvider> m_provider;
	prgbind::ProgramLibrary m_prglib;
	TestConfiguration* m_config;
	std::string m_input;
	std::string m_output;
	std::string m_expected;
	boost::shared_ptr<proc::ExecContext> m_execContext;

	enum
	{
		EoDBufferSize=4,
		MinOutBufferSize=16
	};
};

static std::string selectedTestName;

static std::vector<std::string> tests;

INSTANTIATE_TEST_CASE_P(AllTProcHandlerTests,
                        TProcHandlerTest,
                        ::testing::ValuesIn(tests));

static unsigned int testSeed()
{

	std::time_t now = std::time(0);
	std::tm *ltm = localtime(&now);
	return (int)(ltm->tm_mday + 100 * ltm->tm_mon);
}

TEST_P( TProcHandlerTest, tests)
{
	std::string filename = GetParam();
	
	enum {NOF_IB=11,NOF_OB=7,TEST_MOD=13,NOF_TESTS=5};
	std::size_t ib[] = {16000,127,1,2,3,5,7,8,11,13,17};
	std::size_t ob[] = {16000,127,1,2,5,7,8};

	std::string testname = boost::filesystem::basename(filename);
	// Read test description:
	wtest::TestDescription td( filename, g_gtest_ARGV[0]);
	if (td.requires.size())
	{
		// Skip tests when disabled:
		std::cerr << "skipping test '" << testname << "' ( is " << td.requires << ")" << std::endl;
		return;
	}
	// Run test:
	std::cerr << "processing test '" << testname << "'" << std::endl;
	TestConfiguration testConfiguration;

	unsigned int rr = testSeed();
	for (int tt=0; tt<NOF_TESTS; tt++,rr+=TEST_MOD)
	{
		unsigned int ii = rr % NOF_IB;
		unsigned int oo = rr % NOF_OB;

		TProcHandlerTestInstance test( td, &testConfiguration, ib[ii], ob[oo]);
		int trt = test.run();
		if (trt != 0) boost::this_thread::sleep( boost::posix_time::seconds( 3 ) );
		EXPECT_EQ( 0, trt);

		std::string expected = boost::erase_all_copy( test.expected(), "\r");
		std::string output = boost::erase_all_copy( test.output(), "\r");
		if (expected != output)
		{
			boost::filesystem::path OUTPUT( g_testdir / "temp" / "OUTPUT");
			std::fstream outputf( OUTPUT.string().c_str(), std::ios::out | std::ios::binary);
			outputf.write( output.c_str(), output.size());
			if (outputf.bad()) std::cerr << "error writing file '" << OUTPUT.string() << "'" << std::endl;
			outputf.close();

			boost::filesystem::path EXPECT( g_testdir / "temp" / "EXPECT");
			std::fstream expectedf( EXPECT.string().c_str(), std::ios::out | std::ios::binary);
			expectedf.write( expected.c_str(), expected.size());
			if (expectedf.bad()) std::cerr << "error writing file '" << EXPECT.string() << "'" << std::endl;
			expectedf.close();

			boost::filesystem::path INPUT( g_testdir / "temp" / "INPUT");
			std::fstream inputf( INPUT.string().c_str(), std::ios::out | std::ios::binary);
			inputf.write( test.input().c_str(), test.input().size());
			if (inputf.bad()) std::cerr << "error writing file '" << INPUT.string() << "'" << std::endl;
			inputf.close();

			std::cerr << "test output [" << ib[ii] << "/" << ob[oo] << "] does not match for '" << filename << "'" << std::endl;
			std::cerr << "INPUT  written to file '"  << INPUT.string() << "'" << std::endl;
			std::cerr << "OUTPUT written to file '" << OUTPUT.string() << "'" << std::endl;
			std::cerr << "EXPECT written to file '" << EXPECT.string() << "'" << std::endl;
			boost::this_thread::sleep( boost::posix_time::seconds( 3 ) );
		}
		EXPECT_EQ( expected, output);
	}
}

static void printUsage( const char *prgname)
{
	std::cout << "Usage " << prgname << " [OPTION] [<test name substring>]" << std::endl;
	std::cout << "Description:" << std::endl;
	std::cout << "\tRun tproc hander tests" << std::endl;
	std::cout << "\t-h:" << " Print usage" << std::endl;
	std::cout << "\t-t:" << " Raise verbosity level (-t,-tt,-ttt,..)" << std::endl;
}

int main( int argc, char **argv )
{
	g_gtest_ARGC = 1;
	g_gtest_ARGV[0] = argv[0];
	g_testdir = boost::filesystem::system_complete( argv[0]).parent_path();
	std::string topdir = g_testdir.parent_path().parent_path().parent_path().string();
	g_modulesDirectory = new module::ModulesDirectory( g_testdir.string());
	int argstart = 1;
	int tracelevel = 0;

	while (argc >= argstart+1 && argv[argstart][0] == '-')
	{
		char optionname = argv[argstart][1];
		if (optionname == 't')
		{
			tracelevel = 1;
			while (argv[argstart][tracelevel+1] == 't') ++tracelevel;
			if (argv[argstart][tracelevel+1])
			{
				std::cerr << "unknown option " << argv[argstart][tracelevel+1] << std::endl;
				printUsage( argv[0]);
				return 1;
			}
			argstart += 1;
		}
		else if (optionname == 'h')
		{
			printUsage( argv[0]);
			return 0;
		}
		else if (optionname == '-')
		{
			if (0==std::strcmp( argv[argstart], "--help"))
			{
				printUsage( argv[0]);
				return 0;
			}
			else
			{
				std::cerr << "unknown option -" << argv[argstart] << std::endl;
				printUsage( argv[0]);
				return 5;
			}
		}
		else
		{
			std::cerr << "unknown option -" << optionname << std::endl;
			printUsage( argv[0]);
			return 6;
		}
	}
	if (!g_modulesDirectory->loadModules( wtest::getTestModuleList( topdir)))
	{
		std::cerr << "failed to load modules" << std::endl;
		return 2;
	}
	if (argc > argstart)
	{
		if (argc - argstart > 1)
		{
			std::cerr << "too many arguments passed to " << argv[0] << std::endl;
			return 1;
		}
		selectedTestName = argv[argstart];
	}
	if (tracelevel != 0)
	{
		_Wolframe::log::LogLevel::Level loglevel;
		if (tracelevel == 1) loglevel = _Wolframe::log::LogLevel::LOGLEVEL_INFO;
		if (tracelevel == 2) loglevel = _Wolframe::log::LogLevel::LOGLEVEL_DEBUG;
		if (tracelevel == 3) loglevel = _Wolframe::log::LogLevel::LOGLEVEL_TRACE;
		if (tracelevel == 4) loglevel = _Wolframe::log::LogLevel::LOGLEVEL_DATA;
		if (tracelevel >= 5) loglevel = _Wolframe::log::LogLevel::LOGLEVEL_DATA2;

		_Wolframe::log::LogBackend::instance().setConsoleLevel( loglevel);
	}
	// [1] Selecting tests to execute:
	boost::filesystem::recursive_directory_iterator ditr( g_testdir / "data"), dend;
	if (selectedTestName.size())
	{
		std::cerr << "executing tests matching '" << selectedTestName << "'" << std::endl;
	}
	for (; ditr != dend; ++ditr)
	{
		if (boost::iequals( boost::filesystem::extension( *ditr), ".tst"))
		{
			std::string testname = boost::filesystem::basename(*ditr);
			if (selectedTestName.size())
			{
				if (std::search( ditr->path().string().begin(), ditr->path().string().end(), selectedTestName.begin(), selectedTestName.end()) != ditr->path().string().end())
				{
					std::cerr << "selected test '" << testname << "'" << std::endl;
					tests.push_back( ditr->path().string());
				}
			}
			else
			{
				tests.push_back( ditr->path().string());
			}
		}
		else if (!boost::filesystem::is_directory( *ditr))
		{
			std::cerr << "ignoring file '" << *ditr << "'" << std::endl;
		}
	}
	std::sort( tests.begin(), tests.end());

	// [2] Instantiate test cases with INSTANTIATE_TEST_CASE_P (see above)

	// [3] Execute tests:
	WOLFRAME_GTEST_REPORT( argv[0], refpath.string());
	::testing::InitGoogleTest( &g_gtest_ARGC, g_gtest_ARGV);
	return RUN_ALL_TESTS();
	delete g_modulesDirectory;
}

