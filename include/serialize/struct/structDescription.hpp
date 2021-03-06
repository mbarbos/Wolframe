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
/// \file serialize/struct/structDescription.hpp
/// \brief Defines the bricks for the SDK to describe serialization/deserialization of objects in Wolframe.
/// \remark This module uses intrusive building blocks to build the serialization/deserialization of the objects interfaced as TypedInputFilter/TypedOutputFilter.
#ifndef _Wolframe_SERIALIZE_STRUCT_DESCRIPTION_HPP_INCLUDED
#define _Wolframe_SERIALIZE_STRUCT_DESCRIPTION_HPP_INCLUDED
#include "serialize/struct/structDescriptionBase.hpp"
#include "serialize/struct/traits_getCategory.hpp"
#include "serialize/struct/intrusiveParser.hpp"
#include "serialize/struct/intrusiveSerializer.hpp"
#include "serialize/struct/intrusiveProperty.hpp"
#include <exception>
#include <stdexcept>
#include <sstream>

namespace _Wolframe {
namespace serialize {

/// \class StructDescription
/// \brief Intrusive description of a filter/form map
/// \tparam Structure structure that is represented by this description
template <class Structure>
struct StructDescription :public StructDescriptionBase
{
	/// \brief Constructor
	StructDescription()
		:StructDescriptionBase( &constructor, &destructor, getTypename<Structure>(), 0, sizeof(Structure), IntrusiveProperty<Structure>::type(), &IntrusiveParser<Structure>::parse, &IntrusiveSerializer<Structure>::fetch, NoRequirement){}

	/// \brief Operator to build the structure description element by element
	/// \tparam Element element type
	/// \param[in] tag name of the element
	/// \param[in] eptr pointer to member of the element
	template <typename Element>
	StructDescription& operator()( const char* tag, Element Structure::*eptr)
	{
		StructDescriptionBase::Parse parse_ = &IntrusiveParser<Element>::parse;
		StructDescriptionBase::Fetch fetch_ = &IntrusiveSerializer<Element>::fetch;
		StructDescriptionBase::ElementType type_ = IntrusiveProperty<Element>::type();
		std::size_t pp = (std::size_t)&(((Structure*)0)->*eptr);

		StructDescriptionBase e( getTypename<Element>(), pp, sizeof(Element), type_, parse_, fetch_, NoRequirement);
		if (find( tag) != end())
		{
			std::ostringstream err;
			err << "duplicate definition of " << tag << " in structure";
			throw std::runtime_error( err.str().c_str());
		}
		define( tag, e);
		return *this;
	}

	/// \brief Define the last structure element defined to be mandatory always (independent of validation mode)
	StructDescription& mandatory()
	{
		if (last().optional()) throw std::logic_error( "ambiguous mandatory/optional declaration");
		last().requirement( Mandatory);
		return *this;
	}

	/// \brief Define the last structure element defined to be optional always (independent of validation mode)
	StructDescription& optional()
	{
		if (last().mandatory()) throw std::logic_error( "ambiguous mandatory/optional declaration");
		last().requirement( Optional);
		return *this;
	}

	/// \brief Define the elements defined until now to be attributes and the subsequent elements defined to be content elements (e.g. for XML validation). By default all elements are content elements
	StructDescription& operator--(int)
	{
		defineEndOfAttributes();
		return *this;
	}

	/// \brief Construct an object of this kind at an allocated location (with base pointer 'obj')
	static bool constructor( void* obj)
	{
		/// \brief create operator new for Structure on an already allocated chunk to call it's constructor
		struct StructureShell :public Structure
		{
			void* operator new( std::size_t num_bytes, void* obj) throw (std::bad_alloc)
			{
				if (sizeof(StructureShell) != num_bytes) throw std::bad_alloc();
				return obj;
			}
		};
		try
		{
			new (obj) StructureShell();
			return true;
		}
		catch (const std::exception&)
		{
			return false;
		}
	}

	/// \brief Destroy an object of this kind allocated with 'constructor(void*)' at the location 'obj'
	static void destructor( void* obj)
	{
		((Structure*)obj)->~Structure();
	}

	/// \brief Get the name of the type 'Element' if available
	template <class Element>
	static const char* getTypename()
	{
		const char* typ = 0;
		try
		{
			typ = typeid(Element).name();
		}
		catch (std::bad_typeid)
		{}
		return typ;
	}
};

}}// end namespace
#endif

