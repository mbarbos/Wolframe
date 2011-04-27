#ifndef _Wolframe_FILTER_CHAR_ISOLATIN1_HPP_INCLUDED
#define _Wolframe_FILTER_CHAR_ISOLATIN1_HPP_INCLUDED
#include "protocol/inputfilter.hpp"
#include "protocol/formatoutput.hpp"
#include <cstring>
#include "textwolf.hpp"

namespace _Wolframe {
namespace filter {

template <struct Charset_>
struct XmlFilterT
{
	struct Generator //[+]:public protocol::Generator
	{
		enum ErrorCodes {Ok=0,ErrBufferTooSmall=1,ErrBrokenInputStream=2};

		struct InputIterator
		{
			struct EOM{};
			InputFilter* m_gen;

			InputIterator() :m_gen(0) {}
			InputIterator( InputFilter* gen) :m_gem(gen){}
			InputIterator( const InputIterator& o) :m_gem(o.m_gen){}

			char operator*()
			{
				if (m_gen->m_pos >= m_gen->m_size) throw EOM();
				return ((char*)m_gen->m_ptr)[ m_gen->m_pos];
			}
			InputIterator& operator++()
			{
				m_gen->m_pos++;
			}
		};
		typedef textwolf::XMLScanner<InputIterator,Charset_,charset::UTF8> XMLScanner;
		char m_outputbuf;
		InputIterator itr;
		XMLScanner* scanner;

		InputFilter() :scanner(0)
		{
			itr = InputIterator(this);
			scanner = new XMLScanner( itr, &m_outputbuf, 1);
		}

		~InputFilter()
		{
			delete scanner;
		}

		virtual bool getNext( ElementType* type, void* buffer, size_type buffersize, size_type* bufferpos)
		{
			scanner->setOutputBuffer( (char*)buffer + *bufferpos, buffersize - *bufferpos);
			try
			{
				itr++;
				*bufferpos += itr->size();
				switch (itr->type())
				{
					case ... *type = ... break;
					case ... *type = ... break;
					case EOF
                                        case Error: setState( Error, errorcode);
				}
			}
			catch (InputIterator::EOM)
			{
				if (gotEoD())
				{
					setState( Error, ErrBrokenInputStream);
					return false;
				}
				else
				{
					setState( EndOfMessage);
					return false;
				}
			};
			if (EOF)
			{
				if (gotEoD())
				{
					setState( Open);
					return false;
				}
				else
				{
					setState( EndOfMessage);
					return false;
				}
			}
			return true;
		}
	};
};

}}//namespace
#endif


