--
--requires:LUA
--requires:LIBXML2
--input
HELLO
RUN
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<employee><firstname>姚</firstname><surname>明</surname><phone>021436587</phone><email>yao.ming@gmail.com</email><jobtitle>sportsman</jobtitle></employee>

.
QUIT
--file:echo.lua
function run( )
	f = provider.filter( "libxml2")
	f.empty = false

	input:as( f)
	output:as( f)

	for c,t in input:get() do
		logger.printc( "VISIT ", t, " = ", c)
		output:print( c, t)
	end
end
--config
provider
{
	cmdhandler
	{
		lua
		{
			program echo.lua
			filter char
		}
	}
}
proc
{
	cmd run
}
--output
OK enter cmd
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<employee><firstname>姚</firstname><surname>明</surname><phone>021436587</phone><email>yao.ming@gmail.com</email><jobtitle>sportsman</jobtitle></employee>

.
OK
BYE
--end

