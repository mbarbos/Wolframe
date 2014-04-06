#include "database/vmTransactionOutput.hpp"
#include "utils/printFormats.hpp"
#include "utils/typeSignature.hpp"
#include <iostream>
#include <sstream>

using namespace _Wolframe;
using namespace _Wolframe::db;

void VmTransactionOutput::print( std::ostream& out, const utils::PrintFormat* pformat) const
{
	if (!pformat) print( out, utils::logPrintFormat());
	std::size_t indent = 0;
	std::size_t indentsize = (pformat->indent?std::strlen(pformat->indent):0);
	std::string indentstr;

	vm::Output::const_iterator oi = m_impl->begin(), oe = m_impl->end();
	for (; oi != oe; ++oi)
	{
		switch (oi->op())
		{
			case vm::Output::Element::Open:
				out << pformat->newitem << indentstr << oi->arg().tostring() << pformat->openstruct;
				indent += 1;
				indentstr.append( pformat->indent);
				break;

			case vm::Output::Element::Close:
				if (indent == 0)
				{
					++oi;
					if (oi != oe) throw std::runtime_error("transaction function output tags not balanced");
					return;
				}
				out << pformat->newitem << indentstr << pformat->closestruct;
				indent -= 1;
				indentstr.resize( indentstr.size() - indentsize);
				break;

			case vm::Output::Element::Value:
				out << pformat->newitem << indentstr << pformat->startvalue << oi->arg().tostring() << pformat->endvalue; 
				break;
		}
	}
	if (indent != 0)
	{
		throw std::runtime_error("transaction function output tags not balanced");
	}
}

std::string VmTransactionOutput::tostring( const utils::PrintFormat* pformat) const
{
	std::ostringstream out;
	print( out, pformat);
	return out.str();
}

class VmTransactionInputFilter
	:public langbind::TypedInputFilter
{
public:
	explicit VmTransactionInputFilter( const vm::OutputR& output_)
		:utils::TypeSignature("db::VmTransactionInputFilter", __LINE__)
		,langbind::TypedInputFilter("vmTransactionOutput")
		,m_done(false),m_output(output_),m_itr(output_->begin()),m_end(output_->end())
	{}
	VmTransactionInputFilter( const VmTransactionInputFilter& o)
		:utils::TypeSignature("db::VmTransactionInputFilter", __LINE__)
		,langbind::TypedInputFilter(o)
		,m_done(o.m_done),m_output(o.m_output),m_itr(o.m_itr),m_end(o.m_end)
	{}
	virtual ~VmTransactionInputFilter(){}

	virtual TypedInputFilter* copy() const
	{
		return new VmTransactionInputFilter( *this);
	}

	virtual bool getNext( ElementType& type, types::VariantConst& element)
	{
		if (m_itr == m_end)
		{
			if (m_done) return false;
			type = langbind::FilterBase::CloseTag;
			element = types::VariantConst();
			m_done = true;
			return true;
		}
		switch (m_itr->op())
		{
			case vm::Output::Element::Open:
				type = langbind::FilterBase::Value;
				element = m_itr->arg();
				break;

			case vm::Output::Element::Close:
				type = langbind::FilterBase::Value;
				element = m_itr->arg();
				break;

			case vm::Output::Element::Value:
				type = langbind::FilterBase::Value;
				element = m_itr->arg();
				break;
		}
		++m_itr;
		return true;
	}

private:
	bool m_done;
	vm::OutputR m_output;
	vm::Output::const_iterator m_itr;
	vm::Output::const_iterator m_end;
};

langbind::TypedInputFilterR VmTransactionOutput::get() const
{
	return langbind::TypedInputFilterR( new VmTransactionInputFilter( m_impl));
}


