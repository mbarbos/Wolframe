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
///\file prnt/pdfPrinter.hpp
///\brief Defines a prnt::PrintFunction implementation based on libhpdf with a simple document description
#ifndef _Wolframe_PRNT_SIMPLE_PDF_PRINT_FUNCTION_HPP_INCLUDED
#define _Wolframe_PRNT_SIMPLE_PDF_PRINT_FUNCTION_HPP_INCLUDED
#include "prnt/printFunction.hpp"
#include "types/countedReference.hpp"
#include <string>

namespace _Wolframe {
namespace prnt {

///\class SimplePdfPrintFunction
///\brief Implementaion of a PrintFunction for printing PDFs with libhpdf with a simple document description
class SimplePdfPrintFunction :public PrintFunction
{
public:
	///\brief Constructor
	///\param[in] description Source of the document print description
	SimplePdfPrintFunction( const std::string& description, bool test=false);
	///\brief Destructor
	virtual ~SimplePdfPrintFunction(){}

	virtual InputR getInput() const;
	virtual std::string execute( const Input* i) const;

	virtual std::string tostring() const;
public:
	struct SimplePdfPrintFunctionImpl;
private:
	SimplePdfPrintFunctionImpl* m_impl;		//< hidden implementation (PIMPL)
};

///\param[in] description print description source
PrintFunctionR createSimplePdfPrintFunction( const std::string& description);

///\param[in] description print description source
PrintFunctionR createTestTracePdfPrintFunction( const std::string& description);

}}//namespace
#endif

