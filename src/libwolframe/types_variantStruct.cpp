#include "types/variantStruct.hpp"
#include "types/variantStructDescription.hpp"
#include "types/malloc.hpp"
#include <limits>
#include <stdexcept>
#include <cstdlib>
#include <sstream>
#include <stdint.h>	//... for uintptr_t

using namespace _Wolframe;
using namespace _Wolframe::types;

void VariantStruct::makeArray()
{
	VariantStruct* prototype_ = (VariantStruct*)wolframe_malloc( sizeof( *this));
	if (!prototype_) throw std::bad_alloc();
	std::memcpy( prototype_, this, sizeof( *this));
	init();
	setType( array_);
	m_data.dim.size = 0;
	m_data.value.ref_ = prototype_;
}

VariantIndirection::VariantIndirection( const VariantStructDescription* descr)
{
	m_type = (unsigned char)( VariantStruct::indirection_);
	m_data.dim.metadata = (const void*)descr;
	m_data.value.ref_ = 0;
}

void VariantStruct::expandIndirection()
{
	if (type() != indirection_) return;
	release();
	initStruct( description());
}

void VariantStruct::initStruct( const VariantStructDescription* descr)
{
	setType( struct_);
	m_data.dim.metadata = (const void*)descr;
	m_data.value.ref_ = (VariantStruct*)wolframe_calloc( descr->size(), sizeof(VariantStruct));
	if (!m_data.value.ref_) throw std::bad_alloc();
	VariantStructDescription::const_iterator si = descr->begin(), se = descr->end();
	std::size_t idx=0;
	try
	{
		for (; si!=se; ++si,++idx)
		{
			VariantStruct* elem = (VariantStruct*)m_data.value.ref_ + idx;
			elem->initCopy( *si->initvalue);
		}
	}
	catch (const std::bad_alloc& e)
	{
		for (; idx>0; --idx) ((VariantStruct*)m_data.value.ref_)[idx-1].release();
		wolframe_free( m_data.value.ref_);
		throw e;
	}
	catch (const std::runtime_error& e)
	{
		for (; idx>0; --idx) ((VariantStruct*)m_data.value.ref_)[idx-1].release();
		wolframe_free( m_data.value.ref_);
		throw e;
	}
}

void VariantStruct::initCopy( const VariantStruct& orig)
{
	if (orig.constant())
	{
		std::memcpy( this, &orig, sizeof(*this));
	}
	else
	{
		std::size_t ii, nn;
		bool init_ = orig.initialized();

		switch (orig.type())
		{
			case VariantStruct::bool_:
			case VariantStruct::double_:
			case VariantStruct::int_:
			case VariantStruct::uint_:
			case VariantStruct::string_:
				Variant::initCopy( orig);
				break;

			case VariantStruct::struct_:
			case VariantStruct::array_:
				init();
				if (orig.type() == VariantStruct::struct_)
				{
					nn = orig.description()->size();
					setType( struct_);
					m_data.dim.metadata = orig.m_data.dim.metadata;
				}
				else
				{
					nn = orig.m_data.dim.size+1;
					setType( array_);
					m_data.dim.size = orig.m_data.dim.size;
				}
				m_data.value.ref_ = wolframe_calloc( nn, sizeof( VariantStruct));
				if (!m_data.value.ref_) throw std::bad_alloc();
				ii = 0;
				try
				{
					for (; ii<nn; ++ii)
					{
						VariantStruct* elem = (VariantStruct*)m_data.value.ref_ + ii;
						elem->initCopy( ((VariantStruct*)orig.m_data.value.ref_)[ ii]);
					}
				}
				catch (const std::bad_alloc& e)
				{
					for (; ii>0; --ii) ((VariantStruct*)m_data.value.ref_)[ ii-1].release();
					wolframe_free( m_data.value.ref_);
					throw e;
				}
				catch (const std::runtime_error& e)
				{
					for (; ii>0; --ii) ((VariantStruct*)m_data.value.ref_)[ ii-1].release();
					wolframe_free( m_data.value.ref_);
					throw e;
				}
				setInitialized( init_);
				break;

			case VariantStruct::indirection_:
				init();
				setType( indirection_);
				m_data.dim.metadata = orig.m_data.dim.metadata;
				setInitialized( init_);
				break;
		}
	}
}

void VariantStruct::push()
{
	if (type() != array_) throw std::logic_error("illegal operation push on non array");
	void* ref_ = wolframe_realloc( m_data.value.ref_, (m_data.dim.size+2) * sizeof( VariantStruct));
	if (!ref_) throw std::bad_alloc();
	m_data.value.ref_ = ref_;
	std::size_t idx = m_data.dim.size;
	VariantStruct* elem = (VariantStruct*)m_data.value.ref_ + idx+1;
	elem->init();
	elem->initCopy( ((VariantStruct*)m_data.value.ref_)[ 0]); //... copy prototype as value of the new element pushed
	++m_data.dim.size; //... increment size
}

const VariantStruct& VariantStruct::back() const
{
	if (type() != array_) throw std::logic_error("illegal operation push on non array");
	if (!m_data.dim.size) throw std::logic_error( "array bound read");
	return ((VariantStruct*)m_data.value.ref_)[ m_data.dim.size];
}

VariantStruct& VariantStruct::back()
{
	if (type() != array_) throw std::logic_error("illegal operation push on non array");
	if (!m_data.dim.size) throw std::logic_error( "array bound write");
	return ((VariantStruct*)m_data.value.ref_)[ m_data.dim.size];
}

void VariantStruct::release()
{
	if (constant()) return;
	std::size_t ii, nn;

	switch (type())
	{
		case VariantStruct::bool_:
		case VariantStruct::double_:
		case VariantStruct::int_:
		case VariantStruct::uint_:
		case VariantStruct::string_:
			Variant::release();
			break;

		case VariantStruct::struct_:
			ii=0; nn = description()->size();
			for (; ii<nn; ++ii)
			{
				((VariantStruct*)m_data.value.ref_)[ ii].release();
			}
			wolframe_free( m_data.value.ref_);
			std::memset( this, 0, sizeof( *this));
			break;

		case VariantStruct::array_:
			ii=0; nn=m_data.dim.size+1;
			for (; ii<nn; ++ii)
			{
				((VariantStruct*)m_data.value.ref_)[ ii].release();
			}
			wolframe_free( m_data.value.ref_);
			std::memset( this, 0, sizeof( *this));
			break;

		case VariantStruct::indirection_:
			std::memset( this, 0, sizeof( *this));
			break;
	}
}

int VariantStruct::compareArray( std::size_t size, const VariantStruct* a1, const VariantStruct* a2)
{
	std::size_t ii=0;
	for (; ii<size; ++ii)
	{
		int cmp = a1[ii].compare( a2[ii]);
		if (!cmp) return cmp;
	}
	return 0;
}

int VariantStruct::compare( const VariantStruct& o) const
{
	Type tt = type();
	Type ot = o.type();
	if (tt == array_)
	{
		if (o.type() != array_) return -2;
		if (m_data.dim.size != o.m_data.dim.size)
		{
			return (int)(m_data.dim.size >= o.m_data.dim.size) + (int)(m_data.dim.size > o.m_data.dim.size) -1;
		}
		return compareArray( m_data.dim.size, (const VariantStruct*)m_data.value.ref_ + 1, (const VariantStruct*)o.m_data.value.ref_ + 1);
		//... ref +1, because prototype is not compared
	}
	else if (tt == struct_)
	{
		if (o.type() != struct_) return -2;
		int size1 = (int)(description()->size());
		int size2 = (int)(o.description()->size());
		if (size1 != size2) return size1 - size2;
		return compareArray( size1, (const VariantStruct*)m_data.value.ref_, (const VariantStruct*)o.m_data.value.ref_);
	}
	else if (tt == indirection_)
	{
		if (o.type() != indirection_) return -2;
		const VariantStructDescription* dd = description();
		const VariantStructDescription* od = o.description();
		if (!dd) return 1;
		if (!od) return -2;
		return dd->compare( *od);
	}
	else if (ot == array_ || ot == struct_ || ot == indirection_)
	{
		return +2;
	}
	return Variant::compare( o);
}

void VariantStruct::setDescription( const VariantStructDescription* descr)
{
	Type tp = (Type)type();
	if (tp == indirection_)
	{
		if ((!m_data.dim.metadata && descr) || (m_data.dim.metadata && !descr))
		{
			throw std::logic_error("illegal operation (set description does not match to indirection type)");
		}
		m_data.dim.metadata = descr;
	}
	else if (tp == array_)
	{
		if (descr)
		{
			prototype()->setDescription( descr);
			VariantStruct::iterator vi = begin(), ve = end();
			for (; vi != ve; vi++)
			{
				vi->setDescription( descr);
			}
		}
		else
		{
			bool ok = true;
			if (prototype()->description()) ok = false;
			VariantStruct::iterator vi = begin(), ve = end();
			for (; vi != ve; vi++)
			{
				if (vi->description()) ok = false;
			}
			if (!ok)
			{
				throw std::logic_error("illegal operation (set description does not match to atomic type)");
			}
		}
	}
	else if (tp == struct_)
	{
		VariantStruct::iterator vi = begin(), ve = end();
		VariantStructDescription::const_iterator di = descr->begin(), de = descr->end();
		for (; vi != ve && di != de; vi++,di++)
		{
			if (di->substruct)
			{
				vi->setDescription( di->substruct);
			}
			else
			{
				if (vi->type() != di->initvalue->type()) break;
			}
		}
		if (vi != ve || di != de)
		{
			throw std::logic_error("illegal operation (set description does not match to structure type)");
		}
		m_data.dim.metadata = descr;
	}
	else if (descr)
	{
		throw std::logic_error("illegal operation (try to set description of an atomic type)");
	}
}

const VariantStruct* VariantStruct::at( std::size_t idx) const
{
	Type tt = type();
	if (tt == array_)
	{
		if (idx >= m_data.dim.size) throw std::logic_error( "array bound read");
		return ((const VariantStruct*)m_data.value.ref_ + 1 + idx);
	}
	else if (tt == struct_)
	{
		if (idx >= description()->size()) throw std::logic_error( "array bound read");
		return ((const VariantStruct*)m_data.value.ref_ + idx);
	}
	else
	{
		return 0;
	}
}

VariantStruct* VariantStruct::at( std::size_t idx)
{
	Type tt = type();
	if (tt == array_)
	{
		if (idx >= m_data.dim.size) throw std::logic_error( "array bound read");
		return ((VariantStruct*)m_data.value.ref_ + 1 + idx);
	}
	else if (tt == struct_)
	{
		if (idx >= description()->size()) throw std::logic_error( "array bound read");
		return ((VariantStruct*)m_data.value.ref_ + idx);
	}
	else
	{
		return 0;
	}
}

const VariantStruct* VariantStruct::elementptr( std::size_t idx) const
{
	Type tt = type();
	if (tt == array_)
	{
		return ((const VariantStruct*)m_data.value.ref_ + 1 + idx);
	}
	else if (tt == struct_)
	{
		return ((const VariantStruct*)m_data.value.ref_ + idx);
	}
	else
	{
		return 0;
	}
}

VariantStruct* VariantStruct::elementptr( std::size_t idx)
{
	Type tt = type();
	if (tt == array_)
	{
		return ((VariantStruct*)m_data.value.ref_ + 1 + idx);
	}
	else if (tt == struct_)
	{
		return ((VariantStruct*)m_data.value.ref_ + idx);
	}
	else
	{
		return 0;
	}
}

const VariantStruct* VariantStruct::select( const std::string& name) const
{
	const VariantStructDescription* descr = description();
	if (!descr) return 0;
	int fi = descr->findidx( name);
	return (fi < 0)?0:at(fi);
}

VariantStruct* VariantStruct::select( const std::string& name)
{
	const VariantStructDescription* descr = description();
	if (!descr) return 0;
	int fi = descr->findidx( name);
	return (fi < 0)?0:at(fi);
}

const VariantStruct& VariantStruct::operator[]( std::size_t idx) const
{
	const VariantStruct* rt = at( idx);
	if (!rt)
	{
		throw std::logic_error( std::string("random access not supported for type ") + typeName( rt->type()));
	}
	return *rt;
}

VariantStruct& VariantStruct::operator[]( std::size_t idx)
{
	VariantStruct* rt = at( idx);
	if (!rt)
	{
		throw std::logic_error( std::string("random access not supported for type ") + typeName( rt->type()));
	}
	return *rt;
}

VariantStruct::const_iterator VariantStruct::find( const std::string& name_) const
{
	const VariantStructDescription* descr = description();
	if (!descr) return end();
	int findidx_ = descr->findidx( name_);
	if (findidx_ < 0) return end();
	return VariantStruct::const_iterator( at( findidx_));
}

VariantStruct::iterator VariantStruct::find( const std::string& name_)
{
	const VariantStructDescription* descr = description();
	if (!descr) return end();
	int findidx_ = descr->findidx( name_);
	if (findidx_ < 0) return end();
	return VariantStruct::iterator( at( findidx_));
}

std::size_t VariantStruct::nof_elements() const
{
	if ((Type)type()==array_) return m_data.dim.size;
	if ((Type)type()==struct_) return description()->size();
	return 0;
}


int VariantStruct::const_iterator::compare( const const_iterator& o) const
{
	if (m_itr > o.m_itr) return +1;
	if (m_itr < o.m_itr) return -1;
	return 0;
}

int VariantStruct::iterator::compare( const iterator& o) const
{
	if (m_itr > o.m_itr) return +1;
	if (m_itr < o.m_itr) return -1;
	return 0;
}

void VariantStruct::print( std::ostream& out, const std::string& indent, const std::string& newitem, std::size_t level) const
{
	Type tt = type();
	if (tt == array_)
	{
		std::size_t ii=0, nn=m_data.dim.size;
		out << '{';
		for (; ii<nn; ++ii)
		{
			if (ii>0) out << ',';
			out << newitem;
			for (std::size_t ll=0; ll<level; ++ll) out << indent;
			((VariantStruct*)m_data.value.ref_ + 1 + ii)->print( out, indent, newitem, level+1);
		}
		if (nn>0)
		{
			out << newitem;
			for (std::size_t ll=0; ll<level; ++ll) out << indent;
		}
		out << '}';
	}
	else if (tt == struct_)
	{
		const VariantStructDescription* descr = description();
		VariantStructDescription::const_iterator di = descr->begin();
		const_iterator ii = begin(), ee = end();
		for (; ii!=ee; ++ii,++di)
		{
			out << newitem;
			for (std::size_t ll=0; ll<level; ++ll) out << indent;

			out << di->name << '=';
			ii->print( out, indent, newitem, level+1);
			out << ';';
		}
	}
	else if (tt == indirection_)
	{
	}
	else
	{
		out << '\'';
		out << Variant::tostring();
		out << '\'';
	}
}

std::string VariantStruct::tostring() const
{
	std::ostringstream buf;
	print( buf, "", " ", 0);
	return buf.str();
}

ConstVariantStruct::ConstVariantStruct( const VariantStruct& o)
{
	std::memcpy( this, &o, sizeof( *this));
	setConstant();
}


