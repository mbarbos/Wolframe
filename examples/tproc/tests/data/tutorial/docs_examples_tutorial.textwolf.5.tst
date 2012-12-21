**
**requires:LUA
**input
HELLO
RUN
 < ? x m l   v e r s i o n = " 1 . 0 "   e n c o d i n g = " U T F - 1 6 B E "   s t a n d a l o n e = " y e s " ? > 
 < d o c > < n a m e > m u m b a y < / n a m e > < c o u n t r y > i n d i a < / c o u n t r y > < c o n t i n e n t > a s i a < / c o n t i n e n t > < / d o c > 

.
QUIT
**file:example_5.lua

function run()
        f = filter( "XML:textwolf")
        -- filter out tokens containing only spaces:
        f.empty = false
        input:as( f)
        output:as( f)
        t = input:table()

        -- print the document from the table
        output:opentag( 'doc')
        output:opentag( 'name')
        output:print( t['doc']['name'])
        output:closetag()
        output:opentag( 'country')
        output:print( t['doc']['country'])
        output:closetag()
        output:closetag()
end

**config
provider
{
	cmdhandler
	{
		lua
		{
			program example_5.lua
		}
	}
}
proc
{
	cmd run
}
**output
OK enter cmd
 < ? x m l   v e r s i o n = " 1 . 0 "   e n c o d i n g = " U T F - 1 6 B E "   s t a n d a l o n e = " y e s " ? > 
 < d o c > < n a m e > m u m b a y < / n a m e > < c o u n t r y > i n d i a < / c o u n t r y > < / d o c > 

.
OK
BYE
**end

