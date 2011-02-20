#ifndef _CONFIG_HELPERS_HPP_INCLUDED
#define _CONFIG_HELPERS_HPP_INCLUDED

#include <string>
#include <ostream>

#include <boost/property_tree/ptree.hpp>

namespace _SMERP {
	namespace Configuration	{

		bool getBoolValue( boost::property_tree::ptree::const_iterator it, const std::string& module,
				   const std::string& name, bool& value, std::ostream& os );

		bool getStringValue( boost::property_tree::ptree::const_iterator it, const std::string& module,
				     const std::string& name, std::string& value, std::ostream& os );

		bool getUnsignedShortValue( boost::property_tree::ptree::const_iterator it, const std::string& module,
					    const std::string& name, unsigned short& value, std::ostream& os );

	} // namespace Configuration
} // namespace _SMERP


#endif // _CONFIG_HELPERS_HPP_INCLUDED
