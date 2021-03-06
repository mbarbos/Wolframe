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
/// \file appdevel/runtimeEnvironmentModuleMacros.hpp
/// \brief Macros for a module for a configurable runtime environment for a binding language or a binding language universe
#include "appdevel/module/runtimeEnvironmentConstructor.hpp"
#include "module/moduleInterface.hpp"
#include "processor/procProviderInterface.hpp"
#include <boost/lexical_cast.hpp>

/// \brief Defines a program type with a runtime environment (e.g. program with shared resource context)
#define WF_RUNTIME_ENVIRONMENT(DESCRIPTION,CONFIG_SECTION,CONFIG_TITLE,CLASSDEF,CONFIGDEF,INITFUNCTION) \
{\
	class RuntimeEnvConstructor\
		:public _Wolframe::module::RuntimeEnvironmentConstructor\
	{\
	public:\
		RuntimeEnvConstructor(){}\
		virtual ~RuntimeEnvConstructor(){}\
		virtual CLASSDEF* object( const _Wolframe::config::NamedConfiguration& cfgi)\
		{\
			const CONFIGDEF* cfg = dynamic_cast<const CONFIGDEF*>(&cfgi);\
			if (!cfg) throw std::logic_error( "internal: wrong configuration interface passed to runtime environment constructor " CONFIG_TITLE);\
			CLASSDEF* rt = new CLASSDEF( cfg);\
			return rt;\
		}\
		virtual const char* objectClassName() const\
		{\
			return CONFIG_TITLE "RuntimeEnvironment";\
		}\
		virtual bool checkReferences( const _Wolframe::config::NamedConfiguration&, const _Wolframe::proc::ProcessorProviderInterface*) const\
		{\
			return true;\
		}\
	};\
	class RuntimeEnvBuilder\
		:public _Wolframe::module::ConfiguredBuilder\
	{\
	public:\
		RuntimeEnvBuilder()\
			:_Wolframe::module::ConfiguredBuilder( DESCRIPTION, CONFIG_SECTION, CONFIG_TITLE, CONFIG_TITLE "RuntimeEnvironment")\
		{\
			int err;\
			if (0!=(err=INITFUNCTION())) throw std::runtime_error(std::string("failed to initialize runtime environment (error code ") + boost::lexical_cast<std::string>(err) + ")");\
		}\
		virtual ~RuntimeEnvBuilder(){}\
		virtual _Wolframe::ObjectConstructorBase::ObjectType objectType() const\
		{\
			return _Wolframe::ObjectConstructorBase::RUNTIME_ENVIRONMENT_OBJECT;\
		}\
		virtual _Wolframe::config::NamedConfiguration* configuration( const char* logPrefix)\
		{\
			return new CONFIGDEF( CONFIG_TITLE "RuntimeEnvironment", m_title, logPrefix, m_keyword);\
		}\
		virtual _Wolframe::ObjectConstructorBase* constructor()\
		{\
			return new RuntimeEnvConstructor();\
		}\
	};\
	struct Constructor\
	{\
		static _Wolframe::module::BuilderBase* impl()\
		{\
			return new RuntimeEnvBuilder();\
		}\
	};\
	(*this)(&Constructor ::impl);\
}


