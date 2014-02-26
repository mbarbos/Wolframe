**
**requires:LUA
**requires:CJSON
**requires:TEXTWOLF
**input
 { 
 	 " a s s i g n m e n t l i s t " : 	 { 
 	 	 " a s s i g n m e n t " : 	 [ { 
 	 	 	 	 " t a s k " : 	 [ { 
 	 	 	 	 	 	 " t i t l e " : 	 " j o b   1 " , 
 	 	 	 	 	 	 " k e y " : 	 " A 1 2 3 " , 
 	 	 	 	 	 	 " c u s t o m e r n u m b e r " : 	 " 3 2 4 " 
 	 	 	 	 	 } ,   { 
 	 	 	 	 	 	 " t i t l e " : 	 " j o b   2 " , 
 	 	 	 	 	 	 " k e y " : 	 " V 4 5 6 " , 
 	 	 	 	 	 	 " c u s t o m e r n u m b e r " : 	 " 5 6 7 " 
 	 	 	 	 	 } ] , 
 	 	 	 	 " e m p l o y e e " : 	 { 
 	 	 	 	 	 " f i r s t n a m e " : 	 " J u l i a " , 
 	 	 	 	 	 " s u r n a m e " : 	 " T e g e l - S a c h e r " , 
 	 	 	 	 	 " p h o n e " : 	 " 0 9 8   7 6 5   4 3   2 1 " 
 	 	 	 	 } , 
 	 	 	 	 " i s s u e d a t e " : 	 " 1 3 . 5 . 2 0 0 6 " 
 	 	 	 } ,   { 
 	 	 	 	 " t a s k " : 	 [ { 
 	 	 	 	 	 	 " t i t l e " : 	 " j o b   3 " , 
 	 	 	 	 	 	 " k e y " : 	 " A 4 5 6 " , 
 	 	 	 	 	 	 " c u s t o m e r n u m b e r " : 	 " 5 6 7 " 
 	 	 	 	 	 } ,   { 
 	 	 	 	 	 	 " t i t l e " : 	 " j o b   4 " , 
 	 	 	 	 	 	 " k e y " : 	 " V 7 8 9 " , 
 	 	 	 	 	 	 " c u s t o m e r n u m b e r " : 	 " 8 9 0 " 
 	 	 	 	 	 } ] , 
 	 	 	 	 " e m p l o y e e " : 	 { 
 	 	 	 	 	 " f i r s t n a m e " : 	 " J a k o b " , 
 	 	 	 	 	 " s u r n a m e " : 	 " S t e g e l i n " , 
 	 	 	 	 	 " p h o n e " : 	 " 0 1 2   3 4 5   6 7   8 9 " 
 	 	 	 	 } , 
 	 	 	 	 " i s s u e d a t e " : 	 " 1 3 . 5 . 2 0 0 6 " 
 	 	 	 } ] 
 	 } 
 }**config
--input-filter cjson --output-filter cjson --module ../../src/modules/filter/cjson/mod_filter_cjson -c wolframe.conf run

**file:wolframe.conf
LoadModules
{
	module ../../src/modules/cmdbind/lua/mod_command_lua
	module ../wolfilter/modules/employee_assignment_convert/mod_employee_assignment_convert
}
Processor
{
	program program_formfunc.lua
	cmdhandler
	{
		lua
		{
			program script.lua
		}
	}
}
**file:program_formfunc.lua
function toupper_table( tb)
	for i,v in pairs(tb)
	do
		if type(v) == "table" then
			tb[ i] = toupper_table(v)
		else
			tb[ i] = string.upper(v)
		end
	end
	return tb
end

function toupper_formfunc( inp)
	return toupper_table( inp:table())
end
**file:script.lua

function run()
	res = provider.formfunction( "toupper_formfunc")( input:table())
	output:print( res:table())
end
**requires:DISABLED NETBSD

**output
 { 
 	 " a s s i g n m e n t l i s t " : 	 { 
 	 	 " a s s i g n m e n t " : 	 [ { 
 	 	 	 	 " i s s u e d a t e " : 	 " 1 3 . 5 . 2 0 0 6 " , 
 	 	 	 	 " e m p l o y e e " : 	 { 
 	 	 	 	 	 " f i r s t n a m e " : 	 " J U L I A " , 
 	 	 	 	 	 " p h o n e " : 	 " 0 9 8   7 6 5   4 3   2 1 " , 
 	 	 	 	 	 " s u r n a m e " : 	 " T E G E L - S A C H E R " 
 	 	 	 	 } , 
 	 	 	 	 " t a s k " : 	 [ { 
 	 	 	 	 	 	 " c u s t o m e r n u m b e r " : 	 " 3 2 4 " , 
 	 	 	 	 	 	 " k e y " : 	 " A 1 2 3 " , 
 	 	 	 	 	 	 " t i t l e " : 	 " J O B   1 " 
 	 	 	 	 	 } ,   { 
 	 	 	 	 	 	 " c u s t o m e r n u m b e r " : 	 " 5 6 7 " , 
 	 	 	 	 	 	 " k e y " : 	 " V 4 5 6 " , 
 	 	 	 	 	 	 " t i t l e " : 	 " J O B   2 " 
 	 	 	 	 	 } ] 
 	 	 	 } ,   { 
 	 	 	 	 " i s s u e d a t e " : 	 " 1 3 . 5 . 2 0 0 6 " , 
 	 	 	 	 " e m p l o y e e " : 	 { 
 	 	 	 	 	 " f i r s t n a m e " : 	 " J A K O B " , 
 	 	 	 	 	 " p h o n e " : 	 " 0 1 2   3 4 5   6 7   8 9 " , 
 	 	 	 	 	 " s u r n a m e " : 	 " S T E G E L I N " 
 	 	 	 	 } , 
 	 	 	 	 " t a s k " : 	 [ { 
 	 	 	 	 	 	 " c u s t o m e r n u m b e r " : 	 " 5 6 7 " , 
 	 	 	 	 	 	 " k e y " : 	 " A 4 5 6 " , 
 	 	 	 	 	 	 " t i t l e " : 	 " J O B   3 " 
 	 	 	 	 	 } ,   { 
 	 	 	 	 	 	 " c u s t o m e r n u m b e r " : 	 " 8 9 0 " , 
 	 	 	 	 	 	 " k e y " : 	 " V 7 8 9 " , 
 	 	 	 	 	 	 " t i t l e " : 	 " J O B   4 " 
 	 	 	 	 	 } ] 
 	 	 	 } ] 
 	 } 
 } 
**end
