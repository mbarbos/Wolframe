#ifndef _Wolframe_COM_AUTOMATION_RECORDINFO_HPP_INCLUDED
#define _Wolframe_COM_AUTOMATION_RECORDINFO_HPP_INCLUDED
#include <oaidl.h>
#include <comdef.h>
#include <atlbase.h>
#include <atlcom.h>
#include <atlctl.h>
#include <windef.h>
#include <map>
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace comauto {

class RecordInfo
	:public CComObjectRoot
	,public IRecordInfo
{
public:
	RecordInfo( ITypeInfo* typeinfo_);
	virtual ~RecordInfo();

	// Implementation of the IUnknown interface:
	virtual ULONG STDMETHODCALLTYPE AddRef()		{return InternalAddRef();}
	virtual ULONG STDMETHODCALLTYPE Release()		{return InternalRelease();}
	virtual HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID* ppvObj);

	// Implementation of the IRecordInfo interface:
	virtual HRESULT STDMETHODCALLTYPE RecordInit( PVOID pvNew);
	virtual HRESULT STDMETHODCALLTYPE RecordClear( PVOID pvExisting);
	virtual HRESULT STDMETHODCALLTYPE RecordCopy( PVOID pvExisting, PVOID pvNew);
	virtual HRESULT STDMETHODCALLTYPE GetGuid( GUID* pguid);
	virtual HRESULT STDMETHODCALLTYPE GetName( BSTR* pbstrName);
	virtual HRESULT STDMETHODCALLTYPE GetSize( ULONG* pcbSize);
	virtual HRESULT STDMETHODCALLTYPE GetTypeInfo( ITypeInfo** ppTypeInfo);
	virtual HRESULT STDMETHODCALLTYPE GetField( PVOID pvData, LPCOLESTR szFieldName, VARIANT* pvarField);
	virtual HRESULT STDMETHODCALLTYPE GetFieldNoCopy( PVOID pvData, LPCOLESTR szFieldName, VARIANT* pvarField, PVOID* ppvDataCArray);
	virtual HRESULT STDMETHODCALLTYPE PutField( ULONG wFlags, PVOID pvData, LPCOLESTR szFieldName, VARIANT* pvarField);
	virtual HRESULT STDMETHODCALLTYPE PutFieldNoCopy( ULONG wFlags, PVOID pvData, LPCOLESTR szFieldName, VARIANT* pvarField);
	virtual HRESULT STDMETHODCALLTYPE GetFieldNames( ULONG* pcNames, BSTR* rgBstrNames);
	virtual BOOL STDMETHODCALLTYPE IsMatchingType( IRecordInfo *pRecordInfo);
	virtual PVOID STDMETHODCALLTYPE RecordCreate();
	virtual HRESULT STDMETHODCALLTYPE RecordCreateCopy( PVOID pvSource, PVOID* ppvDest);
	virtual HRESULT STDMETHODCALLTYPE RecordDestroy( PVOID pvRecord);

	const ITypeInfo* typeinfo() const	{return m_typeinfo;}
	const TYPEATTR* typeattr() const	{return m_typeattr;}

	struct VariableDescriptor
	{
		VARTYPE type;
		std::size_t ofs;
		int varnum;
	};
	bool getVariableDescriptor( const std::string& name, VariableDescriptor& descr) const;

private:
	enum InitType {ClearInit,CopyInit,DefaultConstructor,DefaultConstructorZero,CopyConstructor,Destructor};
	HRESULT RecordFill( PVOID pvNew, InitType initType, PVOID pvOld=0);
	void initDescr();

private:
	ITypeInfo* m_typeinfo;
	TYPEATTR* m_typeattr;
	typedef std::map<std::string,VariableDescriptor> VariableDescriptorMap;
	VariableDescriptorMap m_descrmap;
};

typedef boost::shared_ptr<RecordInfo> RecordInfoR;

std::map<std::size_t,RecordInfoR> getRecordInfoMap( ITypeInfo* typeinfo);

}}//namespace
#endif

