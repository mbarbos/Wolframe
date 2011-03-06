//
// standard configuration structures
//

#ifndef _STANDARD_CONFIGS_HPP_INCLUDED
#define _STANDARD_CONFIGS_HPP_INCLUDED

#include "configurationBase.hpp"
#include "serverEndpoint.hpp"
#include "logLevel.hpp"

#if !defined( _WIN32 )
#include "logSyslogFacility.hpp"
#endif	// !defined( _WIN32 )

#include <string>
#include <list>


namespace _Wolframe {
	namespace Network	{

		/// network server configuration
		struct ServerConfiguration : public _Wolframe::Configuration::ConfigurationBase	{
		public:
			unsigned short		threads;
			unsigned short		maxConnections;

			// listen on
			std::list<Network::ServerTCPendpoint> address;
			std::list<Network::ServerSSLendpoint> SSLaddress;

			/// constructor
			ServerConfiguration();

			/// methods
			bool parse( const boost::property_tree::ptree& pt, const std::string& node, std::ostream& os );
			bool check( std::ostream& os ) const;
			void print( std::ostream& os ) const;

			void setCanonicalPathes( const std::string& referencePath );

//			Not implemented yet, inherited from base for the time being
//			bool test( std::ostream& os ) const;
		};

	} // namespace Network


	namespace Configuration	{

	/// daemon / service configuration
	struct ServiceConfiguration : public _Wolframe::Configuration::ConfigurationBase	{
	public:
#if !defined( _WIN32 )
		// daemon configuration
		std::string		user;
		std::string		group;
		std::string		pidFile;
#endif
#if defined( _WIN32 )
		// Windows service configuration
		std::string		serviceName;
		std::string		serviceDisplayName;
		std::string		serviceDescription;
#endif // !defined( _WIN32 )

		/// constructor
		ServiceConfiguration();

		/// methods
		bool parse( const boost::property_tree::ptree& pt, const std::string& node, std::ostream& os );
		bool check( std::ostream& os ) const;
		void print( std::ostream& os ) const;

//			Not implemented yet, inherited from base for the time being
//			bool test( std::ostream& os ) const;

#if !defined( _WIN32 )
		void setCanonicalPathes( const std::string& referencePath );
		void override( const std::string& user, const std::string& group );
#endif // !defined( _WIN32 )
	};

		/// logger configuration
		struct LoggerConfiguration : public _Wolframe::Configuration::ConfigurationBase
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
			LoggerConfiguration();

			/// methods
			bool parse( const boost::property_tree::ptree& pt, const std::string& node, std::ostream& os );
			bool check( std::ostream& os ) const;
			void print( std::ostream& os ) const;

			void setCanonicalPathes( const std::string& referencePath );

//			Not implemented yet, inherited from base for the time being
//			bool test( std::ostream& os ) const;

			void foreground( LogLevel::Level debugLevel, bool useConfig );
		};


		/// database configuration
		struct DatabaseConfiguration : public _Wolframe::Configuration::ConfigurationBase
		{
		public:
			std::string		host;
			unsigned short		port;
			std::string		name;
			std::string		user;
			std::string		password;
			unsigned short		connections;
			unsigned short		acquireTimeout;

			/// constructor
			DatabaseConfiguration();

			/// methods
			bool parse( const boost::property_tree::ptree& pt, const std::string& node, std::ostream& os );
			bool check( std::ostream& os ) const;
			void print( std::ostream& os ) const;

//			Not implemented yet, inherited from base for the time being
//			bool test( std::ostream& os ) const;
		};

	} // namespace Configuration
} // namespace _Wolframe


#endif // _STANDARD_CONFIGS_HPP_INCLUDED
