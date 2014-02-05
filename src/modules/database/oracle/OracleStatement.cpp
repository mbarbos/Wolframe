#include "OracleStatement.hpp"

#include <boost/lexical_cast.hpp>

using namespace _Wolframe;
using namespace _Wolframe::db;

OracleStatement::OracleStatement( )
	: BaseStatement( ),
	m_env( 0 ),
	m_conn( 0 ),
	m_stmt( 0 ),
	m_status( 0 )
{
}

OracleStatement::OracleStatement( const OracleStatement &o )
	: BaseStatement( o ),
	m_env( o.m_env ),
	m_conn( o.m_conn ),
	m_stmt( o.m_stmt ),
	m_status( o.m_status )
{
}

OracleStatement::OracleStatement( OracleEnvirenment *env )
	: m_env( env ),
	m_conn( 0 ),
	m_stmt( 0 ),
	m_status( 0 )
{
}

void OracleStatement::setConnection( OracleConnection *conn )
{
	m_conn = conn;
}

void OracleStatement::setStatement( OCIStmt *stmt )
{
	m_stmt = stmt;
}

sword OracleStatement::getLastStatus( )
{
	return m_status;
}

void OracleStatement::bindUInt32( const unsigned int idx, const boost::uint32_t &value )
{
	OCIBind *bindhp = (OCIBind *)0;

	m_status = OCIBindByPos( m_stmt, &bindhp, m_conn->errhp,
		(ub4)idx, (dvoid *)&value, (sb4)sizeof( boost::uint32_t ),
		SQLT_INT, (dvoid *)0, (ub2 *)0, (ub2 *)0,
		(ub4)0, (ub4 *)0, OCI_DEFAULT );
}

void OracleStatement::bindInt32( const unsigned int idx, const boost::int32_t &value )
{
	OCIBind *bindhp = (OCIBind *)0;

	m_status = OCIBindByPos( m_stmt, &bindhp, m_conn->errhp,
		(ub4)idx, (dvoid *)&value, (sb4)sizeof( boost::int32_t ),
		SQLT_INT, (dvoid *)0, (ub2 *)0, (ub2 *)0,
		(ub4)0, (ub4 *)0, OCI_DEFAULT );
}

// TODO: hard-wired to a NUMBER(1) with 0 and 1 for now	
void OracleStatement::bindBool( const unsigned int idx, const bool &value )
{
	OCIBind *bindhp = (OCIBind *)0;

	m_status = OCIBindByPos( m_stmt, &bindhp, m_conn->errhp,
		(ub4)idx, (dvoid *)&value, (sb4)sizeof( bool ),
		SQLT_INT, (dvoid *)0, (ub2 *)0, (ub2 *)0,
		(ub4)0, (ub4 *)0, OCI_DEFAULT );
}

void OracleStatement::bindDouble( const unsigned int idx, const double &value )
{
	OCIBind *bindhp = (OCIBind *)0;
	
	m_status = OCIBindByPos( m_stmt, &bindhp, m_conn->errhp,
		(ub4)idx, (dvoid *)&value, (sb4)sizeof( double ),
		SQLT_FLT, (dvoid *)0, (ub2 *)0, (ub2 *)0,
		(ub4)0, (ub4 *)0, OCI_DEFAULT );
}

void OracleStatement::bindNumber( const unsigned int idx, const _WOLFRAME_INTEGER &value )
{
}

void OracleStatement::bindNumber( const unsigned int idx, const _WOLFRAME_UINTEGER &value )
{
}

void OracleStatement::bindString( const unsigned int idx, const char* value, const std::size_t size )
{
	OCIBind *bindhp = (OCIBind *)0;

	m_status = OCIBindByPos( m_stmt, &bindhp, m_conn->errhp,
		(ub4)idx, (dvoid *)value, (sb4)size,
		SQLT_CHR, (dvoid *)0, (ub2 *)0, (ub2 *)0,
		(ub4)0, (ub4 *)0, OCI_DEFAULT );
}

void OracleStatement::bindNull( const unsigned int idx )
{
	OCIBind *bindhp = (OCIBind *)0;

	m_status = OCIBindByPos( m_stmt, &bindhp, m_conn->errhp,
		(ub4)idx, (dvoid *)0, (sb4)0,
		SQLT_CHR, (dvoid *)0, (ub2 *)0, (ub2 *)0,
		(ub4)0, (ub4 *)0, OCI_DEFAULT );
}

void OracleStatement::bind( const unsigned int idx, const types::Variant &value )
{
	// does boundary checking
	BaseStatement::bind( idx, value );

	// remember value
	OracleData data;
	data.v = value;
	m_data.push_back( data );
	
	// bind it
	switch( value.type( ) ) {
		case types::Variant::Null:
			bindNull( idx );
			break;
		
		case types::Variant::Int:
			if( value.data( ).value.Int <= 0x7FFFFFFF && value.data( ).value.Int >= -0x7FFFFFFFF ) {
				m_data.back( ).i32 = (boost::int32_t)m_data.back( ).v.data( ).value.Int;
				bindInt32( idx, m_data.back( ).i32 );
			} else {
				bindNumber( idx, m_data.back( ).v.data( ).value.Int );
			}
			break;

		case types::Variant::UInt:
			if( value.data( ).value.UInt <= 0x7FFFFFFF ) {
				m_data.back( ).ui32 = (boost::uint32_t)m_data.back( ).v.data( ).value.UInt;
				bindUInt32( idx, m_data.back( ).ui32 );
			} else {
				bindNumber( idx, m_data.back( ).v.data( ).value.UInt );
			}
			break;

		case types::Variant::Double:
			bindDouble( idx, m_data.back( ).v.data().value.Double );
			break;

		case types::Variant::Bool:
			bindBool( idx, m_data.back( ).v.data().value.Bool );
			break;

		case types::Variant::String:
			bindString( idx, m_data.back( ).v.charptr( ), m_data.back( ).v.charsize( ) );
			break;

		case types::Variant::Custom:
			throw std::logic_error( "Custom type in PostgreSQL database module not supported yet!");
			break;
		
		default:
			throw std::logic_error( "Binding unknown type '" + std::string( value.typeName( ) ) + "'" );
	}
}

const std::string OracleStatement::replace( const unsigned int idx ) const
{
	return ":" + boost::lexical_cast< std::string >( idx );
}

