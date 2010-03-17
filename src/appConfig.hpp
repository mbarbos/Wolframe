//
// appConfig.hpp
//

#ifndef _APP_CONFIG_HPP_INCLUDED
#define _APP_CONFIG_HPP_INCLUDED

#include <string>
#include <vector>


namespace _SMERP {

// application configuration structure

	struct CmdLineConfig;		// for configuration from the command line
	struct CfgFileConfig;		// for configuration from the config file

	struct ApplicationConfiguration	{
		std::string	configFile;
// from command line
		bool		foreground;
		unsigned short	debug;
// daemon configuration
		std::string	user;
		std::string	group;
		unsigned	threads;
		unsigned	maxClients;

// network configuration
		std::vector<std::string> address;
		unsigned short	port;
		unsigned short	SSLport;
		unsigned	idleTimeout;
		unsigned	requestTimeout;
		unsigned	answerTimeout;

// database configuration
		std::string	dbHost;
		unsigned short	dbPort;
		std::string	dbName;
		std::string	dbUser;
		std::string	dbPassword;
// logger configuration
		std::string	logFile;
	public:
		ApplicationConfiguration( const CmdLineConfig& cmdLine, const CfgFileConfig& cfgFile );
		void print( std::ostream& os ) const;
	};
} // namespace _SMERP

#endif // _APP_CONFIG_HPP_INCLUDED
