#include "ddl_form.hpp"
#include <stdexcept>

using namespace _Wolframe;
using namespace _Wolframe::langbind;

Form& Form::operator()( const std::string& name, const Form& value)
{
	if (!isStruct()) throw std::runtime_error( "set named member in form that is not a structure");
	if (m_map.find(name) != m_map.end()) throw std::runtime_error( "set duplicate element in structure");
	m_map[ name] = value;
	return *this;
}

Form& Form::operator()( const Form& value)
{
	if (!isAtomic()) throw std::runtime_error( "append array element to form that is not an array");
	m_ar.push_back( value);
	return *this;
}

const std::string& Form::value() const
{
	return m_value;
}

const wchar_t* Form::wcvalue() const
{
	return std::wstring( m_value.begin(), m_value.end()).c_str();
}

const Form& Form::operator[]( const std::string& name) const
{
	if (!isStruct()) throw std::runtime_error( "select named member in form that is not a structure");
	Map::const_iterator fi = m_map.find( name);
	if (fi == m_map.end()) throw std::runtime_error( std::string("element '") + name + "' not found");
	return fi->second;
}
Form& Form::operator[]( const std::string& name)
{
	if (!isStruct()) throw std::runtime_error( "select named member in form that is not a structure");
	Map::iterator fi = m_map.find( name);
	if (fi == m_map.end()) throw std::runtime_error( std::string("element '") + name + "' not found");
	return fi->second;
}

const Form& Form::operator[]( std::size_t idx) const
{
	if (!isArray()) throw std::runtime_error( "select member by index in form that is not aa array");
	if (m_ar.size() >= idx) throw std::runtime_error( "array index out of range");
	return m_ar.at(idx);
}

Form& Form::operator[]( std::size_t idx)
{
	if (!isArray()) throw std::runtime_error( "select member by index in form that is not aa array");
	if (m_ar.size() >= idx) throw std::runtime_error( "array index out of range");
	return m_ar.at(idx);
}

langbind::TypedInputFilterR Form::get() const
{
	return langbind::TypedInputFilterR( new FormInputFilter( *this));
}

FormInputFilter::FormInputFilter( const FormInputFilter& o)
	:types::TypeSignature("langbind::FormInputFilter", __LINE__)
	,langbind::TypedInputFilter(o)
	,m_stk(o.m_stk){}

FormInputFilter::FormInputFilter( const Form& form)
	:types::TypeSignature("langbind::FormInputFilter", __LINE__)
{
	if (form.isArray())
	{
		throw std::runtime_error("form input filter can only be created from structure or atomic type");
	}
	else if (form.isStruct())
	{
		m_stk.push_back( StackElem( form.structbegin(), form.structend()));
	}
	else
	{
		m_stk.push_back( StackElem( &form.value()));
	}
}

static TypedFilterBase::Element::Type getType( const std::string& val)
{
	std::string::const_iterator vi = val.begin(), ve = val.end();
	TypedFilterBase::Element::Type rt = TypedFilterBase::Element::string_;
	if (vi != ve && *vi == '-')
	{
		++vi;
		if (vi != ve && *vi >= '0' && *vi <= '9') rt = TypedFilterBase::Element::int_;
	}
	else if (vi != ve && *vi >= '0' && *vi <= '9')
	{
		rt = TypedFilterBase::Element::uint_;
	}
	else if (vi != ve && (*vi == 'f' || *vi == 't'))
	{
		if (val == "true") return TypedFilterBase::Element::bool_;
		if (val == "false") return TypedFilterBase::Element::bool_;
	}
	for (++vi; vi != ve; ++vi)
	{
		switch (rt)
		{
			case TypedFilterBase::Element::string_:
			case TypedFilterBase::Element::bool_: return rt;

			case TypedFilterBase::Element::int_:
			case TypedFilterBase::Element::uint_:
				if (*vi >= '0' && *vi <= '9') continue; 
				if (*vi == '.') rt = TypedFilterBase::Element::double_;
				rt = TypedFilterBase::Element::string_;
				break;
			case TypedFilterBase::Element::double_:
				if (*vi >= '0' && *vi <= '9') continue; 
				rt = TypedFilterBase::Element::string_;
				break;
		}
	}
	return rt;
}

static TypedFilterBase::Element getElement( const std::string& val)
{
	switch (getType( val))
	{
		case TypedFilterBase::Element::string_: return TypedFilterBase::Element( val);
		case TypedFilterBase::Element::blob_: return TypedFilterBase::Element( val);
		case TypedFilterBase::Element::bool_: return TypedFilterBase::Element( (bool)(val == "true"));
		case TypedFilterBase::Element::int_: return TypedFilterBase::Element( boost::lexical_cast<int>( val));
		case TypedFilterBase::Element::uint_: return TypedFilterBase::Element( boost::lexical_cast<unsigned int>( val));
		case TypedFilterBase::Element::double_: return TypedFilterBase::Element( boost::lexical_cast<double>( val));
	}
	return TypedFilterBase::Element( val);
}

bool FormInputFilter::StackElem::end() const
{
	switch (type)
	{
		case Form::Atomic: return ptr==0;
		case Form::Struct: return sitr==send;
		case Form::Array: return aitr==aend;
	}
	return true;
}

bool FormInputFilter::getNext( ElementType& type, Element& element)
{
AGAIN:
	if (m_stk.empty()) return false;
	if (m_stk.back().end())
	{
		m_stk.pop_back();
		goto AGAIN;
	}
	switch (m_stk.back().state)
	{
		case OpenTagState:
			type = FilterBase::OpenTag;
			switch (m_stk.back().type)
			{
				case Form::Struct: element = getElement( m_stk.back().sitr->first); break;
				case Form::Array: element = getElement( m_stk.back().atag); break;
				case Form::Atomic: throw std::logic_error( "illegal state in OpenTagState of FormInputFilter::getNext(..)");
			}
			m_stk.back().state = ValueState;
			return true;

		case CloseTagState:
			type = FilterBase::CloseTag;
			element = Element();
			switch (m_stk.back().type)
			{
				case Form::Struct: ++m_stk.back().sitr; break;
				case Form::Array: ++m_stk.back().aitr; break;
				case Form::Atomic: throw std::logic_error( "illegal state in CloseTagState of FormInputFilter::getNext(..)");
			}
			m_stk.back().state = OpenTagState;
			return true;

		case ValueState:
			type = FilterBase::Value;
			switch (m_stk.back().type)
			{
				case Form::Atomic:
					element = getElement( *m_stk.back().ptr);
					return true;

				case Form::Struct:
					if (m_stk.back().sitr->second.isAtomic())
					{
						element = getElement( m_stk.back().sitr->second.value());
						m_stk.back().state = CloseTagState;
						return true;
					}
					else if (m_stk.back().sitr->second.isArray())
					{
						m_stk.back().state = CloseTagState;
						m_stk.push_back( StackElem( m_stk.back().sitr->first, m_stk.back().sitr->second.arraybegin(), m_stk.back().sitr->second.arrayend()));
						goto AGAIN;
					}
					else
					{
						m_stk.back().state = CloseTagState;
						m_stk.push_back( StackElem( m_stk.back().sitr->second.structbegin(), m_stk.back().sitr->second.structend()));
						goto AGAIN;
					}
				case Form::Array:
					if (m_stk.back().aitr->isAtomic())
					{
						element = getElement( m_stk.back().aitr->value());
						m_stk.back().state = CloseTagState;
						return true;
					}
					else if (m_stk.back().aitr->isArray())
					{
						throw std::runtime_error( "array of array not allowed in strucure");
					}
					else
					{
						m_stk.back().state = CloseTagState;
						m_stk.push_back( StackElem( m_stk.back().aitr->structbegin(), m_stk.back().aitr->structend()));
						goto AGAIN;
					}
			}
	}
	throw std::runtime_error( "illegal state in 'FormInputFilter::getNext'");
}

