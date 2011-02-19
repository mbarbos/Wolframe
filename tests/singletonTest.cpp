//
// tests singleton, especially cleanup and live time issues when destructing
//

#include "singleton.hpp"

#include <gtest/gtest.h>

#include <string>

class TestObject : public Singleton< TestObject > {
private:
	std::string *somedata;
	
public:
	TestObject( ) {
		somedata = new std::string( "bla" );
	}
	
	virtual ~TestObject( ) {
		delete somedata;
	}
};

// The fixture for testing SMERP::Singleton
class SingletonFixture : public ::testing::Test	{
protected:
	SingletonFixture( ) {
	}		
	
	virtual ~SingletonFixture( ) {
	}
};

// Tests to check basic functionality of the singleton
// (implicitly tests for memory leaks in the destructor of
// the test object)
TEST_F( SingletonFixture, basic ) {
	TestObject *o1 = &TestObject::instance( );
	TestObject *o2 = &TestObject::instance( );
	
	ASSERT_EQ( o1, o2 );
}

int main( int argc, char *argv[] )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS( );
}
