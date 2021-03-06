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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
/// \file types/form.hpp
/// \brief Defines the unified form data structure as defined by a DDL (data definition language). A form data structure represents the document unit of a wolframe request or answer.

#ifndef _Wolframe_TYPES_FORM_HPP_INCLUDED
#define _Wolframe_TYPES_FORM_HPP_INCLUDED
#include "types/variantStruct.hpp"
#include "types/variantStructDescription.hpp"
#include "types/docmetadata.hpp"
#include <string>
#include <cstddef>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace utils {
	/// \brief Forward declaration
	struct PrintFormat;
}
namespace types {

/// \class FormDescription
/// \brief Description of a form structure
class FormDescription
	:public types::VariantStructDescription
{
public:
	/// \brief Default constructor
	FormDescription(){}
	/// \brief Destructor
	virtual ~FormDescription(){}

	/// \brief Constructor
	FormDescription( const std::string& ddlname_, const std::string& name_, const types::DocMetaData& metadata_)
		:m_name(name_)
		,m_ddlname(ddlname_)
		,m_metadata(metadata_){}
	explicit FormDescription( const std::string& ddlname_)
		:m_ddlname(ddlname_)
		{}
	/// \brief Copy constructor
	FormDescription( const FormDescription& o)
		:types::VariantStructDescription(o)
		,m_name(o.m_name)
		,m_ddlname(o.m_ddlname)
		,m_metadata(o.m_metadata){}

	/// \brief Assignement operator
	/// \param[in] o object to copy
	FormDescription& operator= ( const FormDescription& o)
	{
		types::VariantStructDescription::operator=( o);
		m_name = o.m_name;
		m_ddlname = o.m_ddlname;
		m_metadata = o.m_metadata;
		return *this;
	}

	/// \brief Get the metadata attribute of the form
	const char* attribute( const std::string& name_) const
	{
		return m_metadata.getAttribute( name_);
	}

	/// \brief Get the name of the form
	const std::string& name() const
	{
		return m_name;
	}

	/// \brief Get the data definition language name of the form
	const std::string& ddlname() const
	{
		return m_ddlname;
	}

	void print( std::ostream& out, const utils::PrintFormat* pformat, size_t level=0) const;

	/// \brief Gets the form as string
	std::string tostring( const utils::PrintFormat* pformat=0) const;

	const types::DocMetaData& metadata() const
	{
		return m_metadata;
	}

private:
	std::string m_name;		///< name of the form
	std::string m_ddlname;		///< data definition language name of the form
	types::DocMetaData m_metadata;	///< document meta data
};

typedef boost::shared_ptr<FormDescription> FormDescriptionR;


/// \class Form
/// \brief Form data structure
class Form
	:public types::VariantStruct
{
public:
	/// \brief Default constructor
	Form()
		:m_description(0){}
	/// \brief Constructor
	explicit Form( const FormDescription* description_)
		:types::VariantStruct(description_)
		,m_description(description_){}
	/// \brief Copy constructor
	Form( const Form& o)
		:types::VariantStruct(o)
		,m_description(o.m_description){}

	/// \brief Destructor
	virtual ~Form(){}

	/// \brief Assignement operator
	/// \param[in] o object to copy
	Form& operator= ( const Form& o)
	{
		types::VariantStruct::operator=( o);
		m_description = o.m_description;
		return *this;
	}

	const FormDescription* description() const
	{
		return m_description;
	}

	void print( std::ostream& out, const utils::PrintFormat* pformat, size_t level=0) const;

	/// \brief Gets the form as string
	std::string tostring( const utils::PrintFormat* pformat=0) const;

private:
	const FormDescription* m_description;
};

/// \brief shared ownership reference to form data structure
typedef boost::shared_ptr<Form> FormR;

}}//namespace
#endif

