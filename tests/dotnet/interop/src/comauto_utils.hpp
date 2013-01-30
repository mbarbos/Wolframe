#ifndef _Wolframe_COM_AUTOMATION_UTILS_INCLUDED
#define _Wolframe_COM_AUTOMATION_UTILS_INCLUDED
#include <cstring>
#include <string>
#include <stdexcept>
#include <oaidl.h>
#include <comdef.h>
#include <atlbase.h>
#include <atlcom.h>

namespace _Wolframe {
namespace comauto {

std::string asciistr( BSTR str);
std::string asciistr( const TCHAR* str);
std::wstring utf16string( const std::string& utf8str);
std::wstring utf16string( const char* utf8ptr, std::size_t utf8size);
std::string utf8string( const std::wstring& utf16str);
std::string utf8string( const BSTR& str);
std::wstring tostring( VARIANT* v);
std::wstring tostring( const _com_error& err);
std::string typestr( VARTYPE vt);
std::string typestr( ITypeInfo* typeinfo, const TYPEDESC* ed);
std::string structstring( ITypeInfo* typeinfo);
std::string variablename( ITypeInfo* typeinfo, VARDESC* vardesc);
std::string variabletype( ITypeInfo* typeinfo, VARDESC* vardesc);

bool isCOMInterfaceMethod( const std::string& name);

VARIANT createVariantType( bool val);
VARIANT createVariantType( int val);
VARIANT createVariantType( unsigned int val);
VARIANT createVariantType( short val);
VARIANT createVariantType( unsigned short val);
VARIANT createVariantType( long val);
VARIANT createVariantType( unsigned long val);
VARIANT createVariantType( float val);
VARIANT createVariantType( double val);
VARIANT createVariantType( const std::string& val);
VARIANT createVariantType( const char* val, std::size_t valsize);
VARIANT createVariantType( const std::wstring& val);

unsigned char sizeofAtomicType( int vt);
bool isAtomicType( int vt);
const void* arithmeticTypeAddress( const VARIANT* val);
void* arithmeticTypeAddress( VARIANT* val);

#define WRAP(SYSCALL){\
	static const char* call = "" #SYSCALL;\
	HRESULT hr = SYSCALL;\
	if (hr != S_OK)\
	{\
		const char* endcallname = strchr( call, '(');\
		std::string callname( call, endcallname?(endcallname-call):strlen(call));\
		_com_error error(hr);\
		throw std::runtime_error( std::string("Error calling ") + callname + ": '" + comauto::utf8string(error.ErrorMessage()) + "'");\
	}}

}}
#endif