/************************************************************************

 Copyright (C) 2011 - 2013 Project Wolframe.
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
///\file appdevel/normalizeModuleMacros.hpp
///\brief Macros for defining normalization and validaton function module
#include "module/normalizeFunctionBuilder.hpp"
#include "logger-v1.hpp"

//\brief Marks the start if the Wolframe C++ form function module after the includes section.
#define NORMALIZER_MODULE(DOMAIN,DESCRIPTION)\
	_Wolframe::log::LogBackend* logBackendPtr;\
	\
	static void _Wolframe__setModuleLogger( void* logger )\
	{\
		logBackendPtr = reinterpret_cast< _Wolframe::log::LogBackend*>( logger);\
	}\
	\
	static const char* _Wolframe__moduleName()\
	{\
		return "" #DOMAIN "Normalizer";\
	}\
	static const char* _Wolframe__moduleDomain()\
	{\
		return DOMAIN;\
	}\
	static const char* _Wolframe__moduleDescription()\
	{\
		return DESCRIPTION;\
	}\
	static _Wolframe::langbind::ResourceHandle* _Wolframe__createResourceHandle()\
	{\
		return 0;\
	}\
	static _Wolframe::module::NormalizeFunctionDef _Wolframe__normalizeFunctions[] =\
	{

#define NORMALIZER_MODULE_WITH_RESOURCE(DOMAIN,DESCRIPTION,RESOURCECLASS)\
	_Wolframe::log::LogBackend* logBackendPtr;\
	\
	static void setModuleLogger( void* logger )\
	{\
		logBackendPtr = reinterpret_cast< _Wolframe::log::LogBackend*>( logger);\
	}\
	\
	static const char* _Wolframe__moduleName()\
	{\
		return "" #DOMAIN "Normalizer";\
	}\
	static const char* _Wolframe__moduleDomain()\
	{\
		return DOMAIN;\
	}\
	static const char* _Wolframe__moduleDescription()\
	{\
		return DESCRIPTION;\
	}\
	static _Wolframe::langbind::ResourceHandle* _Wolframe__createResourceHandle()\
	{\
		return new RESOURCECLASS();\
	}\
	static _Wolframe::module::NormalizeFunctionDef _Wolframe__normalizeFunctions[] =\
	{



#define NORMALIZER_FUNCTION(NAME,CONSTRUCTOR)\
		{NAME,&CONSTRUCTOR},\

#define NORMALIZER_MODULE_END\
		{0,0}\
	};\
	namespace {\
	struct ModuleImpl\
	{\
		static _Wolframe::module::SimpleBuilder* constructor()\
		{\
			return new _Wolframe::module::NormalizeFunctionBuilder( _Wolframe__moduleName(), _Wolframe__moduleDomain(), _Wolframe__normalizeFunctions, &_Wolframe__createResourceHandle);\
		}\
	};\
	}\
	enum {NofObjects=1};\
	static _Wolframe::module::createBuilderFunc _Wolframe__objdef[ NofObjects] =\
	{\
		ModuleImpl::constructor\
	};\
	_Wolframe::module::ModuleEntryPoint entryPoint( 0, _Wolframe__moduleDescription(), _Wolframe__setModuleLogger, 0, 0, NofObjects, _Wolframe__objdef);
