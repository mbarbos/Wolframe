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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
#include "comauto/typelib.hpp"
#include "comauto/utils.hpp"
#include "comauto/function.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <cstring>
#include <oaidl.h>
#include <comdef.h>
#include <atlcom.h>
#include <atlbase.h>

using namespace _Wolframe;

class comauto::AssignmentClosure::Impl
{
public:
	Impl();
	Impl( const TypeLib* typelib_, const langbind::TypedInputFilterR& input_, VARTYPE outtype, bool single_);
	Impl( const TypeLib* typelib_, const langbind::TypedInputFilterR& input_, const ITypeInfo* typeinfo_);
	virtual ~Impl(){}

	bool call( VARIANT& value);
	std::string variablepath() const;
	const IRecordInfo* recinfo() const		{return m_recinfo;}

private:
	struct StackElem
	{
		ITypeInfo* typeinfo;
		TYPEATTR* typeattr;
		const IRecordInfo* recinfo;
		VARTYPE vt;
		VARIANT value;
		std::string key;
		std::map<std::string,int> keymap;
		std::map<std::size_t,std::vector<VARIANT> > elemar;

		StackElem( ITypeInfo* typeinfo_, const IRecordInfo* recinfo_, VARTYPE vt_);
		StackElem( VARTYPE vt_);
		StackElem( const StackElem& o);
		~StackElem();
	};

private:
	const TypeLib* m_typelib;
	const ITypeInfo* m_typeinfo;
	const IRecordInfo* m_recinfo;
	std::vector<StackElem> m_stk;
	langbind::TypedInputFilterR m_input;
	VARTYPE m_outtype;
	bool m_single;
};

comauto::AssignmentClosure::AssignmentClosure()
	:m_impl(new Impl())
{}

comauto::AssignmentClosure::AssignmentClosure( const TypeLib* typelib_, const langbind::TypedInputFilterR& input_, unsigned short/*VARTYPE*/ outtype, bool single_)
	:m_impl( new Impl( typelib_, input_, outtype, single_))
{}

comauto::AssignmentClosure::AssignmentClosure( const TypeLib* typelib_, const langbind::TypedInputFilterR& input_, const ITypeInfo* typeinfo_)
	:m_impl( new Impl( typelib_, input_, typeinfo_))
{}

comauto::AssignmentClosure::~AssignmentClosure()
{
	delete m_impl;
}

bool comauto::AssignmentClosure::call( VARIANT& value)
{
	return m_impl->call( value);
}

std::string comauto::AssignmentClosure::variablepath() const
{
	return m_impl->variablepath();
}

const IRecordInfo* comauto::AssignmentClosure::recinfo() const
{
	return m_impl->recinfo();
}


class comauto::TypeLib::Impl
{
public:
	explicit Impl( const std::string& file, comauto::TypeLib* this_);
	virtual ~Impl();

	void print( std::ostream& out) const;
	void printvalue( std::ostream& out, const std::string& name, const VARIANT& val, const ITypeInfo* typeinfo, std::size_t indentcnt=0) const;

	const IRecordInfo* getRecordInfo( const ITypeInfo* typeinfo) const;

	const ITypeLib* typelib() const			{return m_typelib;}
	const TLIBATTR* libattr() const			{return m_libattr;}

private:
	comauto::TypeLib* m_this;
	ITypeLib* m_typelib;
	TLIBATTR* m_libattr;
};



comauto::TypeLib::Impl::Impl( const std::string& file, comauto::TypeLib* this_)
	:m_this(this_),m_typelib(0),m_libattr(0)
{
	try
	{
		std::wstring ws_type_lib_file = utf16string( file);
		WRAP( ::LoadTypeLibEx( ws_type_lib_file.c_str(), REGKIND_NONE, &m_typelib))
		WRAP( m_typelib->GetLibAttr( &m_libattr))
	}
	catch (const std::runtime_error& e)
	{
		if (m_typelib) m_typelib->Release();
		throw e;
	}

}

comauto::TypeLib::Impl::~Impl()
{
	if (m_typelib)
	{
		if (m_libattr) m_typelib->ReleaseTLibAttr( m_libattr);
		m_typelib->Release();
	}
}

// forward declaration for printAttributes(const char*,ITypeInfo*,TYPEATTR*,int)
static void printItem( std::ostream& out, const comauto::TypeLib* typelib, const ITypeInfo* typeinfo, int indentcnt=0);

static void printVar( std::ostream& out, const ITypeInfo* typeinfo, VARDESC* var, int indentcnt=0)
{
	std::string name = comauto::variablename( typeinfo, var);
	std::string type = comauto::variabletype( typeinfo, var);
	out << std::string(indentcnt,'\t') << "VAR " << name << " :" << type << std::endl;
}

static void printAttributes( std::ostream& out, const char* title, const comauto::TypeLib* typelib, const ITypeInfo* typeinfo, TYPEATTR* typeattr, int indentcnt=0)
{
	ITypeInfo* classtypeinfo = 0;
	VARDESC* var = 0;
	try
	{
		std::string namestr( comauto::typestr( typeinfo));
		if (namestr != "IUnknown")
		{
			out << std::string( indentcnt, '\t') << title << " " << namestr << std::endl;
			unsigned short ii;

			for (ii = 0; ii < typeattr->cImplTypes; ++ii)
			{
				HREFTYPE hreftype;
				WRAP( const_cast<ITypeInfo*>(typeinfo)->GetRefTypeOfImplType( ii, &hreftype))
				WRAP( const_cast<ITypeInfo*>(typeinfo)->GetRefTypeInfo( hreftype, &classtypeinfo))
				printItem( out, typelib, classtypeinfo, indentcnt+1);
				classtypeinfo->Release();
				classtypeinfo = 0;
			}
			for (ii = 0; ii < typeattr->cFuncs; ++ii)
			{
				comauto::DotnetFunction func( 0, typelib, typeinfo, "", "", ii);
				if (!comauto::isCOMInterfaceMethod( func.methodname()))
				{
					out << std::string( indentcnt+1, '\t');
					func.print( out);
				}
			}
			for (ii = 0; ii < typeattr->cVars; ++ii)
			{
				WRAP( const_cast<ITypeInfo*>(typeinfo)->GetVarDesc( ii, &var))
				printVar( out, typeinfo, var, indentcnt+1);
				const_cast<ITypeInfo*>(typeinfo)->ReleaseVarDesc( var);
				var = 0;
			}
		}
	}
	catch (const std::runtime_error& e)
	{
		if (classtypeinfo) classtypeinfo->Release();
		if (var) const_cast<ITypeInfo*>(typeinfo)->ReleaseVarDesc( var);
		throw e;
	}
}

static void printItem( std::ostream& out, const comauto::TypeLib* typelib, const ITypeInfo* typeinfo, int indentcnt)
{
	TYPEATTR* typeattr = 0;
	ITypeInfo* reftypeinfo = 0;
	try
	{
		std::string indent( indentcnt, '\t');
		WRAP( const_cast<ITypeInfo*>(typeinfo)->GetTypeAttr( &typeattr))

		switch (typeattr->typekind)
		{
			case TKIND_ENUM:
			{
				out << indent << "ENUM " << std::endl;
				break;
			}
			case TKIND_RECORD:
			{
				printAttributes( out, "RECORD", typelib, typeinfo, typeattr, indentcnt);
				break;
			}
			case TKIND_MODULE:
			{
				out << indent << "MODULE " << std::endl;
				break;
			}
			case TKIND_INTERFACE:
			{
				printAttributes( out, "INTERFACE", typelib, typeinfo, typeattr, indentcnt);
				break;
			}
			case TKIND_DISPATCH:
			{
				printAttributes( out, "DISPATCH", typelib, typeinfo, typeattr, indentcnt);
				break;
			}
			case TKIND_ALIAS:
			{
				out << indent << "ALIAS" << std::endl;
				WRAP( const_cast<ITypeInfo*>(typeinfo)->GetRefTypeInfo( typeattr->tdescAlias.hreftype, &reftypeinfo))
				printItem( out, typelib, reftypeinfo, indentcnt+1);
				reftypeinfo->Release();
				reftypeinfo = 0;
				break;
			}
			case TKIND_UNION:
			{
				out << indent << "UNION " << std::endl;
				break;
			}
			case TKIND_COCLASS:
			{
				printAttributes( out, "COCLASS", typelib, typeinfo, typeattr, indentcnt);
				break;
			}
		}
		const_cast<ITypeInfo*>(typeinfo)->ReleaseTypeAttr( typeattr);
	}
	catch (const std::runtime_error& e)
	{
		if (typeattr) const_cast<ITypeInfo*>(typeinfo)->ReleaseTypeAttr( typeattr);
		if (reftypeinfo) const_cast<ITypeInfo*>(reftypeinfo)->Release();
		throw e;
	}
}

static void printItem( std::ostream& out, const comauto::TypeLib* typelib, int indentcnt=0)
{
	ITypeInfo* typeinfo = 0;
	try
	{
		UINT ii = 0, nn = const_cast<ITypeLib*>(typelib->typelib())->GetTypeInfoCount();

		for (ii = 0; ii < nn; ++ii)
		{
			WRAP( const_cast<ITypeLib*>(typelib->typelib())->GetTypeInfo( ii, &typeinfo))
			printItem( out, typelib, typeinfo, indentcnt);
			typeinfo->Release();
			typeinfo = 0;
		}
	}
	catch (const std::runtime_error& e)
	{
		if (typeinfo) typeinfo->Release();
		throw e;
	}
}

void comauto::TypeLib::Impl::print( std::ostream& out) const
{
	printItem( out, m_this);
}

void comauto::TypeLib::Impl::printvalue( std::ostream& out, const std::string& name, const VARIANT& val, const ITypeInfo* typeinfo, std::size_t indentcnt) const
{
	VARIANT elem;
	elem.vt = VT_EMPTY;
	VARDESC* vardesc = 0;
	TYPEATTR* typeattr = 0;
	ITypeInfo* rectypeinfo = 0;
	try
	{
		if (typeinfo && val.pRecInfo && val.pvRecord)
		{
			out << std::string(indentcnt,'\t') << name << " {" << std::endl;

			WRAP( const_cast<ITypeInfo*>(typeinfo)->GetTypeAttr( &typeattr))
			UINT ii=0, nn=typeattr->cVars;
			for (; ii < nn; ++ii)
			{
				WRAP( const_cast<ITypeInfo*>(typeinfo)->GetVarDesc( ii, &vardesc))
				std::wstring recname( comauto::variablename_utf16( typeinfo, vardesc));
				val.pRecInfo->GetField( val.pvRecord, recname.c_str(), &elem);

				if (vardesc->elemdescVar.tdesc.vt == VT_USERDEFINED)
				{
					WRAP( elem.pRecInfo->GetTypeInfo( &rectypeinfo))
					printvalue( out, comauto::utf8string(recname), elem, rectypeinfo, indentcnt+1);
					rectypeinfo->Release();
					rectypeinfo = 0;
				}
				else
				{
					printvalue( out, comauto::utf8string(recname), elem, 0, indentcnt+1);
				}
				if (elem.vt != VT_EMPTY)
				{
					comauto::wrapVariantClear( &elem);
					elem.vt = VT_EMPTY;
				}
				const_cast<ITypeInfo*>(typeinfo)->ReleaseVarDesc( vardesc);
				vardesc = 0;
			}
			out << std::string(indentcnt,'\t') << "}" << std::endl;
		}
		else if (comauto::isAtomicType(val.vt) || comauto::isStringType(val.vt))
		{
			std::string elembuf;
			out << std::string(indentcnt,'\t') << name << " : " << comauto::typestr(val.vt) << " '" << comauto::getAtomicElement( val, elembuf).tostring() << "'" << std::endl;
		}
		else
		{
			throw std::runtime_error( "print not implemented for this type value");
		}
	}
	catch (const std::runtime_error& e)
	{
		if (elem.vt != VT_EMPTY) comauto::wrapVariantClear( &elem);
		if (vardesc) const_cast<ITypeInfo*>(typeinfo)->ReleaseVarDesc( vardesc);
		if (rectypeinfo) rectypeinfo->Release();
		if (typeattr) const_cast<ITypeInfo*>(typeinfo)->ReleaseTypeAttr( typeattr);
		throw e;
	}
}

const IRecordInfo* comauto::TypeLib::Impl::getRecordInfo( const ITypeInfo* typeinfo) const
{
	LCID lcid_US = 0x0409;
	IRecordInfo* rt = NULL;
	TYPEATTR* typeattr = NULL;
	if (!typeinfo) return 0;
	try
	{
		WRAP( const_cast<ITypeInfo*>(typeinfo)->GetTypeAttr( &typeattr))
		ULONG uVerMajor = m_libattr->wMajorVerNum;
		ULONG uVerMinor = m_libattr->wMinorVerNum;
		if (typeattr->typekind == TKIND_RECORD)
		{
			WRAP( ::GetRecordInfoFromGuids( m_libattr->guid, uVerMajor, uVerMinor, lcid_US, typeattr->guid, &rt))
			const_cast<ITypeInfo*>(typeinfo)->ReleaseTypeAttr( typeattr);
			if (!rt) throw std::runtime_error( std::string("record info not defined for type '") + comauto::typestr( typeinfo) + "'");
		}
		else
		{
			const_cast<ITypeInfo*>(typeinfo)->ReleaseTypeAttr( typeattr);
		}
	}
	catch (...)
	{
		if (typeattr) const_cast<ITypeInfo*>(typeinfo)->ReleaseTypeAttr( typeattr);
		return 0;
	}
	return rt;
}

comauto::AssignmentClosure::Impl::StackElem::StackElem( ITypeInfo* typeinfo_, const IRecordInfo* recinfo_, VARTYPE vt_)
	:typeinfo(typeinfo_),typeattr(0),recinfo(recinfo_),vt(vt_)
{
	if (!typeinfo) throw std::logic_error( "illegal state in AssignmentClosure::StackElem");
	VARDESC* vardesc = NULL;
	value.vt = VT_EMPTY;
	try
	{
		WRAP( typeinfo->GetTypeAttr( &typeattr))
		if (typeattr->typekind == TKIND_RECORD && !recinfo)
		{
			throw std::logic_error( "using typeinfo of structure without recordinfo");
		}
		for (UINT ii=0; ii<typeattr->cVars; ++ii)
		{
			WRAP( typeinfo->GetVarDesc( ii, &vardesc))
			keymap[ comauto::variablename( typeinfo, vardesc)] = ii; 
			typeinfo->ReleaseVarDesc( vardesc);
			vardesc = 0;
		}
		value.pRecInfo = const_cast<IRecordInfo*>(recinfo);
		value.pvRecord = value.pRecInfo->RecordCreate();
		value.vt = vt;
		typeinfo->AddRef();
	}
	catch (const std::runtime_error& e)
	{
		if (typeattr) typeinfo->ReleaseTypeAttr( typeattr);
		if (vardesc) typeinfo->ReleaseVarDesc( vardesc);
		throw e;
	}
}

comauto::AssignmentClosure::Impl::StackElem::StackElem( VARTYPE vt_)
	:typeinfo(0),typeattr(0),recinfo(0),vt(vt_)
{
	value.vt = VT_EMPTY;
}

comauto::AssignmentClosure::Impl::StackElem::StackElem( const StackElem& o)
	:typeinfo(o.typeinfo),typeattr(0),recinfo(o.recinfo),vt(o.vt),key(o.key),keymap(o.keymap),elemar(o.elemar)
{
	value.vt = VT_EMPTY;
	comauto::wrapVariantCopy( &value, &o.value);
	if (typeinfo)
	{
		WRAP( typeinfo->GetTypeAttr( &typeattr));
		typeinfo->AddRef();
	}
}

comauto::AssignmentClosure::Impl::StackElem::~StackElem()
{
	if (typeinfo)
	{
		if (typeattr) typeinfo->ReleaseTypeAttr( typeattr);
		typeinfo->Release();
		comauto::wrapVariantClear( &value);
	}
}

std::string comauto::AssignmentClosure::Impl::variablepath() const
{
	std::string rt;
	std::vector<StackElem>::const_iterator si = m_stk.begin(), se = m_stk.end();
	for (; si != se; ++si)
	{
		if (!si->key.empty())
		{
			if (!rt.empty()) rt.push_back('.');
			rt.append( si->key);
		}
	}
	return rt;
}

bool comauto::AssignmentClosure::Impl::call( VARIANT& output)
{
	VARIANT value;
	value.vt = VT_EMPTY;
	ITypeInfo* rectypeinfo = 0;
	VARDESC* vardesc = 0;
	if (m_stk.empty()) return true;
	langbind::FilterBase::ElementType elemtype;
	types::VariantConst elemvalue;
	StackElem* cur = 0;

	try
	{
		while (m_input->getNext( elemtype, elemvalue))
		{
AGAIN:
			cur = &m_stk.back();
			switch (elemtype)
			{
				case langbind::FilterBase::OpenTag:
				{
					if (!cur->key.empty()) throw std::runtime_error("illegal filter input sequence (value instead of open tag expected after attribute)");
					if (elemvalue.type() != types::Variant::String) throw std::runtime_error( "string expected for tag name");
					if (cur->value.vt != VT_RECORD || cur->value.pvRecord == 0 || cur->value.pRecInfo == 0) throw std::runtime_error( "structure assigned to atomic value or array"); 
					cur->key = elemvalue.tostring();
					std::map<std::string,int>::const_iterator ki = cur->keymap.find( cur->key);
					if (ki == cur->keymap.end())
					{
						cur->key.clear();
						throw std::runtime_error( std::string( "undefined element '") + elemvalue.tostring() + "'");
					}
					WRAP( cur->typeinfo->GetVarDesc( ki->second, &vardesc))
					VARTYPE elemvartype = vardesc->elemdescVar.tdesc.vt;
					HREFTYPE elemhreftype = vardesc->elemdescVar.tdesc.hreftype;

					if (elemvartype == VT_SAFEARRAY)
					{
						VARTYPE arelemvartype = vardesc->elemdescVar.tdesc.lptdesc->vt;
						HREFTYPE arelemhreftype = vardesc->elemdescVar.tdesc.lptdesc->hreftype;
						if (arelemvartype == VT_USERDEFINED)
						{
							WRAP( cur->typeinfo->GetRefTypeInfo( arelemhreftype, &rectypeinfo))
							const IRecordInfo* recinfo = m_typelib->getRecordInfo( rectypeinfo);
							m_stk.push_back( StackElem( rectypeinfo, recinfo, VT_RECORD));
							rectypeinfo->Release();
							rectypeinfo = 0;
						}
						else
						{
							m_stk.push_back( StackElem( arelemvartype));
						}
						cur->typeinfo->ReleaseVarDesc( vardesc);
						vardesc = 0;
					}
					else
					{
						cur->typeinfo->ReleaseVarDesc( vardesc);
						vardesc = 0;

						if (elemvartype == VT_USERDEFINED)
						{
							WRAP( cur->typeinfo->GetRefTypeInfo( elemhreftype, &rectypeinfo))
							const IRecordInfo* recinfo = m_typelib->getRecordInfo( rectypeinfo);
							m_stk.push_back( StackElem( rectypeinfo, recinfo, VT_RECORD));
							rectypeinfo->Release();
							rectypeinfo = 0;
						}
						else if (comauto::isAtomicType(elemvartype) || comauto::isStringType(elemvartype))
						{
							m_stk.push_back( StackElem( elemvartype));
						}
						else
						{
							throw std::runtime_error( std::string("cannot pass this type as parameter: '") + comauto::typestr(elemvartype) + "'");
						}
					}
					break;
				}
				case langbind::FilterBase::CloseTag:
				{
					std::map<std::size_t,std::vector<VARIANT> >::iterator ei = cur->elemar.begin(), ee = cur->elemar.end();
					for (; ei != ee; ++ei)
					{
						WRAP( cur->typeinfo->GetVarDesc( ei->first, &vardesc))
						std::wstring key( comauto::variablename_utf16( m_stk.back().typeinfo, vardesc));

						if (vardesc->elemdescVar.tdesc.vt == VT_SAFEARRAY)
						{
							VARTYPE avtype = vardesc->elemdescVar.tdesc.lptdesc->vt;
							HREFTYPE avhreftype = vardesc->elemdescVar.tdesc.lptdesc->hreftype;
							if (avtype == VT_USERDEFINED)
							{
								WRAP( cur->typeinfo->GetRefTypeInfo( avhreftype, &rectypeinfo))
								const IRecordInfo* avrecinfo = m_typelib->getRecordInfo( rectypeinfo);
								value = createVariantArray( VT_RECORD, avrecinfo, ei->second);
								rectypeinfo->Release();
								rectypeinfo = 0;
								WRAP( cur->value.pRecInfo->PutField( INVOKE_PROPERTYPUT, cur->value.pvRecord, key.c_str(), &value))
							}
							else
							{
								value = createVariantArray( avtype, 0, ei->second);
								WRAP( cur->value.pRecInfo->PutField( INVOKE_PROPERTYPUT, cur->value.pvRecord, key.c_str(), &value))
							}
						}
						else if (ei->second.size() > 1)
						{
							throw std::runtime_error( "duplicate assignment to non array variable");
						}
						else
						{
							WRAP( cur->value.pRecInfo->PutField( INVOKE_PROPERTYPUT, cur->value.pvRecord, key.c_str(), &ei->second[0]))
						}
						cur->typeinfo->ReleaseVarDesc( vardesc);
						vardesc = 0;
						comauto::wrapVariantClear( &value);
						value.vt = VT_EMPTY;
					}
					if (!m_stk.back().key.empty()) throw std::runtime_error( "illegal filter input sequence (value instead of close tag expected after attribute)");
					value = m_stk.back().value;
					m_stk.back().value.vt = VT_EMPTY;
					m_stk.pop_back();
					if (m_stk.empty())
					{
						output = value;
						if (m_outtype != VT_USERDEFINED && m_outtype != output.vt)
						{
							comauto::wrapVariantChangeType( &output, &output, 0, m_outtype);
						}
						return true;
					}
					cur = &m_stk.back();
					cur->elemar[ cur->keymap[ cur->key]].push_back( value);
					value.vt = VT_EMPTY;
					cur->key.clear();
					break;
				}
				case langbind::FilterBase::Attribute:
				{
					if (!cur->key.empty()) throw std::runtime_error("illegal filter input sequence (value instead of attribute expected after attribute)");
					if (elemvalue.type() != types::Variant::String) throw std::runtime_error( "string expected for attribute name");
					cur->key = elemvalue.tostring();
					break;
				}
				case langbind::FilterBase::Value:
				{
					if (!cur->key.empty())
					{
						value = comauto::createVariantType( elemvalue);
						if (cur->value.vt != VT_RECORD || cur->value.pvRecord == 0 || cur->value.pRecInfo == 0) throw std::runtime_error( "illegal state (structure element context expected)"); 
						cur->elemar[ cur->keymap[ cur->key]].push_back( value);
						value.vt = VT_EMPTY;
						cur->key.clear();
					}
					else if (cur->value.vt == VT_RECORD)
					{
						cur->key = "_";
						goto AGAIN;
					}
					else
					{
						if (cur->value.vt != VT_EMPTY) throw std::runtime_error( "duplicate value assignment");
						cur->value = comauto::createVariantType( elemvalue, cur->vt);
					}
					break;
				}
			}
			if (m_single)
			{
				if (elemtype != langbind::FilterBase::Value) throw std::runtime_error( "atomic value expected");
				break;
			}
		}
		if (m_input->state() == langbind::InputFilter::Open)
		{
			if (m_stk.size() != 1) throw std::runtime_error( "tags not balanced in input");
			output = m_stk.back().value;
			m_stk.back().value.vt = VT_EMPTY;
			m_stk.pop_back();
			if (m_outtype != VT_USERDEFINED && m_outtype != output.vt)
			{
				comauto::wrapVariantChangeType( &output, &output, 0, m_outtype);
			}
			return true;
		}
		return false;
	}
	catch (const std::runtime_error& e)
	{
		if (vardesc) cur->typeinfo->ReleaseVarDesc( vardesc);
		comauto::wrapVariantClear( &value);
		if (rectypeinfo) rectypeinfo->Release();
		throw e;
	}
}

comauto::AssignmentClosure::Impl::Impl()
	:m_typelib(0),m_typeinfo(0),m_recinfo(0),m_outtype(VT_EMPTY),m_single(false)
{}

comauto::AssignmentClosure::Impl::Impl( const TypeLib* typelib_, const langbind::TypedInputFilterR& input_, VARTYPE outtype_, bool single_)
	:m_typelib(const_cast<TypeLib*>(typelib_)),m_typeinfo(0),m_recinfo(0),m_input(input_),m_outtype(outtype_),m_single(single_)
{
	m_stk.push_back( StackElem( outtype_));
}

comauto::AssignmentClosure::Impl::Impl( const TypeLib* typelib_, const langbind::TypedInputFilterR& input_, const ITypeInfo* typeinfo_)
	:m_typelib(const_cast<TypeLib*>(typelib_)),m_typeinfo( const_cast<ITypeInfo*>(typeinfo_)),m_recinfo(0),m_input(input_),m_outtype(VT_USERDEFINED),m_single(false)
{
	m_recinfo = m_typelib->getRecordInfo( m_typeinfo);
	m_stk.push_back( StackElem( const_cast<ITypeInfo*>(m_typeinfo), m_recinfo, VT_RECORD));
}


comauto::TypeLib::TypeLib( const std::string& file)
	:m_impl( new Impl(file,this))
{}

comauto::TypeLib::~TypeLib()
{
	delete m_impl;
}

void comauto::TypeLib::print( std::ostream& out) const
{
	m_impl->print( out);
}

void comauto::TypeLib::printvalue( std::ostream& out, const std::string& name, const tagVARIANT& val, const ITypeInfo* typeinfo, std::size_t indentcnt) const
{
	m_impl->printvalue( out, name, val, typeinfo, indentcnt);
}

const IRecordInfo* comauto::TypeLib::getRecordInfo( const ITypeInfo* typeinfo) const
{
	return m_impl->getRecordInfo( typeinfo);
}

const ITypeLib* comauto::TypeLib::typelib() const
{
	return m_impl->typelib();
}

const tagTLIBATTR* comauto::TypeLib::libattr() const
{
	return m_impl->libattr();
}
