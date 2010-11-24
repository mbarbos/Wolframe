//
// tests a simple protocol handler (pechoHandler) std input

#include "pechoHandler.hpp"
#include "testHandlerTemplates.hpp"
#include <stdio.h>

using namespace _SMERP;

LogBackend logBack;

int main( int, const char**)
{
   Network::LocalTCPendpoint ep( "127.0.0.1", 12345);
   pecho::Connection connection( ep);
   return test::runTestFileIO( stdin, stdout, connection);
}




