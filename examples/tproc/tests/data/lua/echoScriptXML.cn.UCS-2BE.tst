--
--requires:LUA
--requires:DISABLED
--input
HELLO
RUN
<?xml version="1.0" encoding="UCS-2BE"?>
�� < e m p l o y e e > 
�� < f i r s t n a m e > � � � < / f i r s t n a m e > 
�� < s u r n a m e > � � � < / s u r n a m e > 
�� < p h o n e > 0 2 1 4 3 6 5 8 7 < / p h o n e > 
�� < e m a i l > y a o . m i n g @ g m a i l . c o m < / e m a i l > 
�� < j o b t i t l e > s p o r t s m a n < / j o b t i t l e > 
�� < / e m p l o y e e > 

.
QUIT
--file:echo.lua
function run( )
	f = filter( "xml:libxml2")
	f.empty = false

	input:as( f)
	output:as( f)

	for c,t in input:get() do
		output:print( c, t)
	end
end
--config
script {
	name RUN
	path echo.lua
	main run
}
--output
OK enter cmd
<?xml version="1.0" encoding="UCS-2BE"?>
�� < e m p l o y e e > 
�� < f i r s t n a m e > � � � < / f i r s t n a m e > 
�� < s u r n a m e > � � � < / s u r n a m e > 
�� < p h o n e > 0 2 1 4 3 6 5 8 7 < / p h o n e > 
�� < e m a i l > y a o . m i n g @ g m a i l . c o m < / e m a i l > 
�� < j o b t i t l e > s p o r t s m a n < / j o b t i t l e > 
�� < / e m p l o y e e > 

.
OK
OK enter cmd
BYE
--end

