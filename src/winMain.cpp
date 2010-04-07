//
// winMain.cpp
//

#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include "appInstance.hpp"
#include "commandLine.hpp"
#include "configFile.hpp"
#include "appConfig.hpp"
#include "server.hpp"
#include "ErrorCodes.hpp"
#include "logger.hpp"

#if !defined(_WIN32)
#error "This is the WIN32 main !"
#else

#include <WinSvc.h>

static const unsigned short MAJOR_VERSION = 0;
static const short unsigned MINOR_VERSION = 0;
static const short unsigned REVISION_NUMBER = 3;

static const int DEFAULT_DEBUG_LEVEL = 3;

static const char *DEFAULT_MAIN_CONFIG = "/etc/smerpd.conf";
static const char *DEFAULT_USER_CONFIG = "~/smerpd.conf";
static const char *DEFAULT_LOCAL_CONFIG = "./smerpd.conf";

static const char *DEFAULT_SERVICE_NAME = "smerpd";

boost::function0<void> consoleCtrlFunction;

BOOL WINAPI consoleCtrlHandler(DWORD ctrlType)
{
	switch (ctrlType)	{
		case CTRL_C_EVENT:
		case CTRL_BREAK_EVENT:
		case CTRL_CLOSE_EVENT:
		case CTRL_LOGOFF_EVENT:
		case CTRL_SHUTDOWN_EVENT:
			LOG_INFO << "Stopping server";
			consoleCtrlFunction();
			return TRUE;
		default:
			return FALSE;
	}
}

static void install_as_service( const _SMERP::ApplicationConfiguration& config )
{
// get service control manager
	SC_HANDLE scm = (SC_HANDLE)OpenSCManager( NULL, SERVICES_ACTIVE_DATABASE, SC_MANAGER_ALL_ACCESS );
	
// retrieve absolute path of binary
	TCHAR binary_path[MAX_PATH];
	DWORD res = GetModuleFileName( NULL, binary_path, MAX_PATH );
	
// create the service
	SC_HANDLE service = CreateService( scm,
		config.serviceName.c_str( ), config.serviceDisplayName.c_str( ),
		SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS,
		SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL,
		binary_path, NULL, NULL, NULL, NULL, NULL );

// set description of the service
	SERVICE_DESCRIPTION descr;
	descr.lpDescription = (LPTSTR)config.serviceDescription.c_str( );
	(void)ChangeServiceConfig2( service, SERVICE_CONFIG_DESCRIPTION, &descr );

// TODO: add location of the configuration file to the registry

// free handles
	(void)CloseServiceHandle( service );
	(void)CloseServiceHandle( scm );
}

static void remove_as_service( const _SMERP::ApplicationConfiguration& config )
{
// get service control manager
	SC_HANDLE scm = (SC_HANDLE)OpenSCManager( NULL, SERVICES_ACTIVE_DATABASE, SC_MANAGER_ALL_ACCESS );

// get service handle of the service to delete (identified by service name)
	SC_HANDLE service = OpenService( scm, config.serviceName.c_str( ), SERVICE_ALL_ACCESS );

// remove the service
	(void)DeleteService( service );

// TODO: remove location of the configuration file to the registry

// free handles
	(void)CloseServiceHandle( service );
	(void)CloseServiceHandle( scm );
}

static void WINAPI service_main( DWORD argc, LPTSTR *argv ) {
}

int _SMERP_winMain( int argc, char* argv[] )
{
	try	{
		_SMERP::AppInstance	app( MAJOR_VERSION, MINOR_VERSION, REVISION_NUMBER );
		_SMERP::CmdLineConfig	cmdLineCfg;
		const char		*configFile;

// TODO: service can't read command line options, read the only relevant one (the absolute
// path of the configuration file from the registry

		if ( !cmdLineCfg.parse( argc, argv ))	{	// there was an error parsing the command line
			std::cerr << cmdLineCfg.errMsg() << std::endl << std::endl;
			cmdLineCfg.usage( std::cerr );
			std::cerr << std::endl;
			return _SMERP::ErrorCodes::FAILURE;
		}
// command line has been parsed successfully
// if cmdLineCfg.errMsg() is not empty than we have a warning
		if ( !cmdLineCfg.errMsg().empty() )	// there was a warning parsing the command line
			std::cerr << "BOO:" << cmdLineCfg.errMsg() << std::endl << std::endl;

// if we have to print the version or the help do it and exit
		if ( cmdLineCfg.command == _SMERP::CmdLineConfig::PRINT_VERSION )	{
			std::cout << "BOBOBO version " << app.version().toString() << std::endl << std::endl;
			return _SMERP::ErrorCodes::OK;
		}
		if ( cmdLineCfg.command == _SMERP::CmdLineConfig::PRINT_HELP )	{
			cmdLineCfg.usage( std::cout );
			std::cerr << std::endl;
			return _SMERP::ErrorCodes::OK;
		}

// decide what configuration file to use
		if ( !cmdLineCfg.cfgFile.empty() )	// if it has been specified than that's The One ! (and only)
			configFile = cmdLineCfg.cfgFile.c_str();
		else
			configFile = _SMERP::CfgFileConfig::chooseFile( DEFAULT_MAIN_CONFIG,
								       DEFAULT_USER_CONFIG,
								       DEFAULT_LOCAL_CONFIG );
		if ( configFile == NULL )	{	// there is no configuration file
			std::cerr << "MOMOMO: no configuration file found !" << std::endl << std::endl;
			return _SMERP::ErrorCodes::FAILURE;
		}

		_SMERP::CfgFileConfig	cfgFileCfg;
		if ( !cfgFileCfg.parse( configFile ))	{	// there was an error parsing the configuration file
			std::cerr << cfgFileCfg.errMsg() << std::endl << std::endl;
			return _SMERP::ErrorCodes::FAILURE;
		}
// configuration file has been parsed successfully
// build the application configuration
		_SMERP::ApplicationConfiguration config( cmdLineCfg, cfgFileCfg);

// Check the configuration
		if ( cmdLineCfg.command == _SMERP::CmdLineConfig::CHECK_CONFIG )	{
			if ( config.check() )	{
				std::cout << "Configuration OK" << std::endl << std::endl;
				return _SMERP::ErrorCodes::OK;
			}
			else	{
				std::cout << config.errMsg() << std::endl << std::endl;
				return _SMERP::ErrorCodes::OK;
			}
		}

		if ( cmdLineCfg.command == _SMERP::CmdLineConfig::PRINT_CONFIG )	{
			config.print( std::cout );
			std::cout << std::endl;
			return _SMERP::ErrorCodes::OK;
		}

		if ( cmdLineCfg.command == _SMERP::CmdLineConfig::TEST_CONFIG )	{
			std::cout << "Not implemented yet" << std::endl << std::endl;
			return _SMERP::ErrorCodes::OK;
		}

		if ( cmdLineCfg.command == _SMERP::CmdLineConfig::INSTALL_SERVICE ) {
			install_as_service( config );
			std::cout << "Installed as Windows service" << std::endl << std::endl;
			return _SMERP::ErrorCodes::OK;
		}
		
		if ( cmdLineCfg.command == _SMERP::CmdLineConfig::REMOVE_SERVICE ) {
			remove_as_service( config );
			std::cout << "Removed as Windows service" << std::endl << std::endl;
			return _SMERP::ErrorCodes::OK;
		}
		
		// if started as service we dispatch the service thread now
		if ( !config.foreground ) {
			SERVICE_TABLE_ENTRY dispatch_table[2] =
				{ { const_cast<char *>( config.serviceName.c_str( ) ), service_main },
				{ NULL, NULL } };

			if( !StartServiceCtrlDispatcher( dispatch_table ) ) {
				if( GetLastError( ) == ERROR_FAILED_SERVICE_CONTROLLER_CONNECT ) {
					// not called as service, continue as console application
					goto CONSOLE;
				}
				return _SMERP::ErrorCodes::FAILURE;
			}
				
			// here we get if the service has been stopped
			return _SMERP::ErrorCodes::OK;
		}
CONSOLE:

		// Create the final logger based on the configuration
		_SMERP::Logger::initialize( config );
		LOG_NOTICE << "Starting server";

		_SMERP::server s( config );

		// Set console control handler to allow server to be stopped.
		consoleCtrlFunction = boost::bind(&_SMERP::server::stop, &s);
		SetConsoleCtrlHandler(consoleCtrlHandler, TRUE);

		// Run the server until stopped.
		s.run();
	}
	catch (std::exception& e)	{
		std::cerr << "exception: " << e.what() << "\n";
		return _SMERP::ErrorCodes::FAILURE;
	}
	LOG_NOTICE << "Server stopped";

	return _SMERP::ErrorCodes::OK;
}

#endif // defined(_WIN32)
