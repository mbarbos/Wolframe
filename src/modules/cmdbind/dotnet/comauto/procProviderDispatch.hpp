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
//\file comauto/procProviderDispatch.hpp
//\brief Dispatch Interface for calls of processor provider function from .NET via interop callback
#ifndef _Wolframe_COM_AUTOMATION_PROCESSOR_PROVIDER_DISPATCH_HPP_INCLUDED
#define _Wolframe_COM_AUTOMATION_PROCESSOR_PROVIDER_DISPATCH_HPP_INCLUDED
#include "filter/typedfilter.hpp"
#include <objbase.h>

namespace _Wolframe {
namespace proc
{
	class ProcessorProvider;
}
namespace comauto {

class ProcessorProviderDispatch :public IDispatch
{
public:
	ProcessorProviderDispatch( const proc::ProcessorProvider* provider_)
		:m_provider(provider_){}

	~ProcessorProviderDispatch(){}

    // Interface IDispatch:
    HRESULT STDMETHODCALLTYPE GetTypeInfoCount( UINT* pCountTypeInfo);
	HRESULT STDMETHODCALLTYPE GetTypeInfo( UINT, LCID, ITypeInfo**);
    HRESULT STDMETHODCALLTYPE GetIDsOfNames( REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid,  DISPID* rgDispId);
    HRESULT STDMETHODCALLTYPE Invoke( DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr);

	// Interface IUnknown:
	HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, void** ppvObj);
	ULONG STDMETHODCALLTYPE AddRef();
	ULONG STDMETHODCALLTYPE Release();

private:
	const proc::ProcessorProvider* m_provider;	//< processor provider reference
	volatile LONG m_refcount;					//< atomic counter
};

}}//namespace
#endif

