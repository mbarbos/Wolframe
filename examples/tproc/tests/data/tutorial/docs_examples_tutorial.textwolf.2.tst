**
**requires:LUA
**input
HELLO
RUN
姚 明
yao ming
.
QUIT
**file:example_2.lua

function run( )
        input:as( filter( "line:UTF-8"))
        output:as( filter( "line:UTF-8"))
        for c in input:get() do
                output:print( c)
        end
end
**config
script {
	cmd RUN
	path example_2.lua
	main run
}
**output
OK enter cmd
姚 明
yao ming

.
OK
BYE
**end
