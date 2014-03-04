#include "PostgreSQLsubstitutingStatement.hpp"
#include "types/datetime.hpp"
#include "types/bignumber.hpp"
#include "types/customDataType.hpp"
#include <boost/shared_ptr.hpp>

using namespace _Wolframe;
using namespace _Wolframe::db;

PostgreSQLsubstitutingStatement::PostgreSQLsubstitutingStatement( )
	: SubstitutingStatement( ),
	m_conn( 0 )
{
}

PostgreSQLsubstitutingStatement::PostgreSQLsubstitutingStatement( const PostgreSQLsubstitutingStatement &o )
	: SubstitutingStatement( o ),
	m_conn( 0 )
{
}

void PostgreSQLsubstitutingStatement::setConnection( PGconn *conn )
{
	m_conn = conn;
}

const std::string PostgreSQLsubstitutingStatement::convert( const types::Variant &value ) const
{
	switch( value.type( ) ) {
		
		case types::Variant::Null:
			return "NULL";
			
		case types::Variant::Int:
		case types::Variant::UInt:
		case types::Variant::Double:
		case types::Variant::BigNumber:
			return value.tostring( );
			
		case types::Variant::Custom:
		{
			types::Variant baseval;
			try
			{
				if (value.customref()->getBaseTypeValue( baseval)
				&&  baseval.type() != types::Variant::Custom)
				{
					return convert( baseval);
				}
			}
			catch (const std::runtime_error& e)
			{
				throw std::runtime_error( std::string("cannot convert value to base type for binding: ") + e.what());
			}
			std::string strval = value.tostring();
			return convert( strval);
		}
		case types::Variant::Timestamp:
		{
			std::string strval = types::DateTime(value.totimestamp()).tostring( types::DateTime::sf_ExtendedISOdateTime);
			return convert( strval);
		}
		case types::Variant::String:
		{
			std::string strval = value.tostring();
			char *encvalue = (char *)std::malloc( strval.size() * 2 + 3);
			encvalue[0] = '\'';
			boost::shared_ptr<void> encvaluer( encvalue, std::free);
			int error = 0;
			size_t encvaluesize = PQescapeStringConn( m_conn, encvalue+1, strval.c_str( ), strval.size( ), &error);
			if( error != 0 ) {
				throw new std::runtime_error( "Encoding error in PQescapeStringConn for string '" + strval + "'");
			}
			encvalue[encvaluesize+1] = '\'';
			return std::string( encvalue, encvaluesize+2);
		}

		case types::Variant::Bool:
			if( value.tobool( ) ) return "'t'";
			else return "'f'";
	}
	throw new std::logic_error( "Unknown variant type '" + std::string( value.typeName( ) ) + "'" );
}

PGresult *PostgreSQLsubstitutingStatement::execute( ) const
{
	return PQexec( m_conn, nativeSQL( ).c_str( ) );
}



