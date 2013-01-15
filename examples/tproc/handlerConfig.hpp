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
//
// handlerConfig.hpp
//

#ifndef _HANDLERCONFIG_HPP_INCLUDED
#define _HANDLERCONFIG_HPP_INCLUDED
#include "standardConfigs.hpp"
#include "processor/procProvider.hpp"
#include "tprocHandlerConfig.hpp"

namespace _Wolframe {

struct HandlerConfiguration
{
public:
	tproc::Configuration* m_appConfig;
	proc::ProcProviderConfig* m_procCfg;

	HandlerConfiguration()
		:m_appConfig(0)
		,m_procCfg(0)
	{
		m_appConfig = new tproc::Configuration();
		m_procCfg = new proc::ProcProviderConfig();
	}

	virtual ~HandlerConfiguration( )
	{
		if (m_appConfig) delete m_appConfig;
		if (m_procCfg) delete m_procCfg;
	}
};

}//namespace _Wolframe
#endif
