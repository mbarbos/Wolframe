//
// appConfig.hpp
//

#ifndef _APP_CONFIG_HPP_INCLUDED
#define _APP_CONFIG_HPP_INCLUDED

#include "logLevel.hpp"

#if !defined( _WIN32 )
#include "logSyslogFacility.hpp"
#endif	// !defined( _WIN32 )

#include "serverEndpoint.hpp"
#include "configurationBase.hpp"

#include <string>
#include <list>

namespace _SMERP {

	/// server configuration
	struct ServerConfiguration : public _SMERP::ConfigurationBase	{
	public:
#if !defined( _WIN32 )
		// daemon configuration
		std::string		user;
		std::string		group;
		std::string		pidFile;
#else
		// service configuration
		std::string		serviceName;
		std::string		serviceDisplayName;
		std::string		serviceDescription;
#endif // !defined( _WIN32 )
		// server configuration
		unsigned		threads;
		unsigned		maxConnections;

		// network configuration
		std::list<Network::ServerTCPendpoint> address;
		std::list<Network::ServerSSLendpoint> SSLaddress;

		/// constructor
		ServerConfiguration( std::string node, std::string header ) : ConfigurationBase( node, header )
									{}
		/// methods
		bool parse( boost::property_tree::ptree& pt, std::ostream& os );
		bool check( std::ostream& os ) const;
		bool test( std::ostream& os ) const;
		void print( std::ostream& os ) const;
#if !defined( _WIN32 )
		void override( std::string user, std::string group );
#endif // !defined( _WIN32 )
	};


	/// logger configuration
	struct LoggerConfiguration : public _SMERP::ConfigurationBase
	{
	public:
		bool			logToStderr;
		LogLevel::Level		stderrLogLevel;
		bool			logToFile;
		std::string		logFile;
		LogLevel::Level		logFileLogLevel;
		std::string		logFileIdent;
#if !defined( _WIN32 )
		bool			logToSyslog;
		SyslogFacility::Facility syslogFacility;
		LogLevel::Level		syslogLogLevel;
		std::string		syslogIdent;
#else
		bool			logToEventlog;
		std::string		eventlogLogName;
		std::string		eventlogSource;
		LogLevel::Level		eventlogLogLevel;
#endif // !defined( _WIN32 )

		/// constructor
		LoggerConfiguration( std::string node, std::string header ) : ConfigurationBase( node, header )
									{}
		/// methods
		bool parse( boost::property_tree::ptree& pt, std::ostream& os );
		bool check( std::ostream& os ) const;
		bool test( std::ostream& os ) const;
		void print( std::ostream& os ) const;

		void foreground( LogLevel::Level debugLevel, bool useConfig );
	};


	/// database configuration
	struct DatabaseConfiguration : public _SMERP::ConfigurationBase
	{
	public:
		std::string		host;
		unsigned short		port;
		std::string		name;
		std::string		user;
		std::string		password;

		/// constructor
		DatabaseConfiguration( std::string node, std::string header ) : ConfigurationBase( node, header )
									{ port = 0; }
		/// methods
		bool parse( boost::property_tree::ptree& pt, std::ostream& os );
		bool check( std::ostream& os ) const;
		bool test( std::ostream& os ) const;
		void print( std::ostream& os ) const;
	};


/// application configuration structure
	struct CmdLineConfig;		// forward declaration for configuration from the command line
	struct CfgFileConfig;		// forward declaration for configuration from the config file

	struct ApplicationConfiguration	{
		std::string		configFile;
// from command line
		bool			foreground;
// daemon configuration
		std::string		user;
		std::string		group;
		std::string		pidFile;
// service configuration
		std::string		serviceName;
		std::string		serviceDisplayName;
		std::string		serviceDescription;

// server configuration
		unsigned		threads;
		unsigned		maxConnections;

// network configuration
		std::list<Network::ServerTCPendpoint> address;
		std::list<Network::ServerSSLendpoint> SSLaddress;

		unsigned		idleTimeout;
		unsigned		requestTimeout;
		unsigned		answerTimeout;
		unsigned		processTimeout;

// database configuration
		DatabaseConfiguration	*dbConfig;
// logger configuration
		LoggerConfiguration	*logConfig;

	private:
		std::string		errMsg_;

	public:
		ApplicationConfiguration( const CmdLineConfig& cmdLine, const CfgFileConfig& cfgFile );
		std::string errMsg() const	{ return errMsg_; }
		bool check();
		bool test();
		void print( std::ostream& os ) const;
	};

} // namespace _SMERP

#endif // _APP_CONFIG_HPP_INCLUDED
