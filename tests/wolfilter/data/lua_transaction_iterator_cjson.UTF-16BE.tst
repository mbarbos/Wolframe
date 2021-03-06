**
**requires:LUA
**requires:CJSON
**requires:TEXTWOLF
**input
 { 
 	 " a s s i g n m e n t " : 	 [ { 
 	 	 	 " t a s k " : 	 [ { 
 	 	 	 	 	 " t i t l e " : 	 " j o b   1 " , 
 	 	 	 	 	 " k e y " : 	 " A 1 2 3 " , 
 	 	 	 	 	 " c u s t o m e r n u m b e r " : 	 " 3 2 4 " 
 	 	 	 	 } ,   { 
 	 	 	 	 	 " t i t l e " : 	 " j o b   2 " , 
 	 	 	 	 	 " k e y " : 	 " V 4 5 6 " , 
 	 	 	 	 	 " c u s t o m e r n u m b e r " : 	 " 5 6 7 " 
 	 	 	 	 } ] , 
 	 	 	 " e m p l o y e e " : 	 { 
 	 	 	 	 " f i r s t n a m e " : 	 " J u l i a " , 
 	 	 	 	 " s u r n a m e " : 	 " T e g e l - S a c h e r " , 
 	 	 	 	 " p h o n e " : 	 " 0 9 8   7 6 5   4 3   2 1 " 
 	 	 	 } , 
 	 	 	 " i s s u e d a t e " : 	 " 1 3 . 5 . 2 0 0 6 " 
 	 	 } ,   { 
 	 	 	 " t a s k " : 	 [ { 
 	 	 	 	 	 " t i t l e " : 	 " j o b   3 " , 
 	 	 	 	 	 " k e y " : 	 " A 4 5 6 " , 
 	 	 	 	 	 " c u s t o m e r n u m b e r " : 	 " 5 6 7 " 
 	 	 	 	 } ,   { 
 	 	 	 	 	 " t i t l e " : 	 " j o b   4 " , 
 	 	 	 	 	 " k e y " : 	 " V 7 8 9 " , 
 	 	 	 	 	 " c u s t o m e r n u m b e r " : 	 " 8 9 0 " 
 	 	 	 	 } ] , 
 	 	 	 " e m p l o y e e " : 	 { 
 	 	 	 	 " f i r s t n a m e " : 	 " J a k o b " , 
 	 	 	 	 " s u r n a m e " : 	 " S t e g e l i n " , 
 	 	 	 	 " p h o n e " : 	 " 0 1 2   3 4 5   6 7   8 9 " 
 	 	 	 } , 
 	 	 	 " i s s u e d a t e " : 	 " 1 3 . 5 . 2 0 0 6 " 
 	 	 } ] 
 }**config
--input-filter cjson --output-filter cjson --module ../../src/modules/filter/cjson/mod_filter_cjson --module ../../src/modules/doctype/json/mod_doctype_json -c wolframe.conf test_transaction

**file:wolframe.conf
LoadModules
{
	module ../wolfilter/modules/database/testtrace/mod_db_testtrace
	module ../../src/modules/cmdbind/tdl/mod_command_tdl
	module ../../src/modules/cmdbind/lua/mod_command_lua
}
Database
{
	test
	{
		identifier testdb
		outfile DBOUT
		file DBRES
	}
}
Processor
{
	program		DBIN.tdl
	program		script.lua
	database	testdb
}
**file:script.lua

function run()
	r = provider.formfunction("test_transaction")( input:get())
	for v,t in r:get() do
		output:print( v,t)
	end
end
**file: DBRES
#name#job1#job2#job3#job4
**file:DBIN.tdl
TRANSACTION test_transaction
BEGIN
	INTO title FOREACH //task DO SELECT run( title);
END
**output
 { 
 	 " t i t l e " : 	 [ { 
 	 	 	 " n a m e " : 	 " j o b 1 " 
 	 	 } ,   { 
 	 	 	 " n a m e " : 	 " j o b 2 " 
 	 	 } ,   { 
 	 	 	 " n a m e " : 	 " j o b 3 " 
 	 	 } ,   { 
 	 	 	 " n a m e " : 	 " j o b 4 " 
 	 	 } ] 
 } 
**end
