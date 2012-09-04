/************************************************************************
Copyright (C) 2011, 2012 Project Wolframe.
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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
///\file prnt/pdfPrinterDocument.hpp
///\brief Defines Methods of the simple PDF print function
#ifndef _Wolframe_PRNT_SIMPLE_PDF_PRINT_DOCUMENT_HPP_INCLUDED
#define _Wolframe_PRNT_SIMPLE_PDF_PRINT_DOCUMENT_HPP_INCLUDED
#include "prnt/pdfPrinterMethod.hpp"
#include "prnt/pdfPrinterVariable.hpp"

namespace _Wolframe {
namespace prnt {

struct Document
{
	Document(){}
	virtual ~Document(){}
	virtual void execute_enter( Method method, const VariableScope& vars)=0;
	virtual void execute_leave( Method method)=0;

	virtual std::string tostring() const=0;
};

Document* createLibHpdfDocument();
Document* createTestTraceDocument();

}}
#endif
