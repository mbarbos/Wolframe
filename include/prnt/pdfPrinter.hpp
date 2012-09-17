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
///\brief Defines a prnt::PrintFunction implementation based on libhpdf with a simple document layout description
#ifndef _Wolframe_PRNT_HARU_PDF_PRINT_FUNCTION_HPP_INCLUDED
#define _Wolframe_PRNT_HARU_PDF_PRINT_FUNCTION_HPP_INCLUDED
#include "prnt/printFunction.hpp"
#include "prnt/pdfPrinterDocument.hpp"
#include "types/countedReference.hpp"
#include <string>

namespace _Wolframe {
namespace prnt {

///\class HaruPdfPrintFunction
///\brief Implementation of a PrintFunction for printing PDFs with libhpdf with a simple document layout description
class HaruPdfPrintFunction :public PrintFunction
{
public:
	///\brief Constructor
	///\param[in] description Source of the document print description
	HaruPdfPrintFunction( const std::string& description, CreateDocumentFunc createDocument);
	///\brief Destructor
	virtual ~HaruPdfPrintFunction();

	virtual InputR getInput() const;
	virtual std::string execute( const Input* i) const;

	virtual std::string tostring() const;
public:
	struct Impl;
private:
	Impl* m_impl;		//< hidden implementation (PIMPL)
};

///\param[in] description print description source
PrintFunction* createHaruPdfPrintFunction( const std::string& description, CreateDocumentFunc createDocument);

}}//namespace
#endif

