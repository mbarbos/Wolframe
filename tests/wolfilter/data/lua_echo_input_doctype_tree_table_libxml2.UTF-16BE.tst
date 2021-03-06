**
**requires:LUA
**requires:LIBXML2
**input
 < ? x m l   v e r s i o n = " 1 . 0 "   e n c o d i n g = " U T F - 1 6 B E "   s t a n d a l o n e = " n o " ? > 
 < ! D O C T Y P E   r o o t   S Y S T E M   " h t t p : / / w w w . w o l f r a m e . o r g / e x a m p l e - d o c / t r e e s . d t d " > 
 < r o o t > 
 < m t r e e   i d = ' 1 ' > 
 < n a m e > e i n s < / n a m e > 
 < n o d e   i d = ' 1 1 ' > 
 < n a m e > E i n s   e i n s < / n a m e > 
 < / n o d e > 
 < n o d e   i d = ' 1 2 ' > 
 < n a m e > E i n s   z w e i < / n a m e > 
 < n o d e   i d = ' 1 2 1 ' > 
 < n a m e > E i n s   z w e i   e i n s < / n a m e > 
 < / n o d e > 
 < / n o d e > 
 < n o d e   i d = ' 1 3 ' > 
 < n a m e > E i n s   d r e i < / n a m e > 
 < / n o d e > 
 < / m t r e e > 
 < b t r e e   i d = ' 1 ' > 
 < n a m e > e i n s < / n a m e > 
 < l e f t   i d = ' 1 1 ' > 
 < n a m e > E i n s   e i n s < / n a m e > 
 < / l e f t > 
 < r i g h t   i d = ' 1 2 ' > 
 < n a m e > E i n s   z w e i < / n a m e > 
 < l e f t   i d = ' 1 2 1 ' > 
 < n a m e > E i n s   z w e i   e i n s < / n a m e > 
 < / l e f t > 
 < / r i g h t > 
 < / b t r e e > 
 < / r o o t > 
**config
--input-filter libxml2 --output-filter libxml2 --module ../../src/modules/filter/libxml2/mod_filter_libxml2 --module ../../src/modules/doctype/xml/mod_doctype_xml -c wolframe.conf run

**file:wolframe.conf
LoadModules
{
	module ../../src/modules/cmdbind/lua/mod_command_lua
	module ../../src/modules/ddlcompiler/simpleform/mod_ddlcompiler_simpleform
	module ../../src/modules/normalize/number/mod_normalize_number
	module ../../src/modules/normalize/string/mod_normalize_string
	module ../../src/modules/datatype/bcdnumber/mod_datatype_bcdnumber
}
Processor
{
	program		normalize.wnmp
	program		form.sfrm
	cmdhandler
	{
		lua
		{
			program script.lua
			filter libxml2
		}
	}
}
**file:normalize.wnmp
int=integer;
uint=unsigned;
float=floatingpoint ;
currency= bigfxp(   2  );
percent_1=bigfxp(  2  );
normname =convdia,ucname;
**file:form.sfrm
STRUCT MulTreeNode
{
	id		@int
	name		normname
	node		^MulTreeNode[]
}

STRUCT BinTreeNode
{
	id		@int
	name		normname
	left		^BinTreeNode
	right		^BinTreeNode
}

FORM trees
	-root root
{
	mtree	MulTreeNode
	btree	BinTreeNode
}
**file:script.lua
function printTable( tab)
	-- deterministic print of a table (since lua 5.2.1 table keys order is non deterministic)

	keys = {}
	for key,val in pairs( tab) do
		table.insert( keys, key)
	end
	table.sort( keys)

	for i,t in ipairs( keys) do
		local v = tab[ t]

		if type(v) == "table" then
			if v[ #v] then
				-- print array (keys are indices)
				for eidx,elem in ipairs( v) do
					output:opentag( t)
					if type(elem) == "table" then
						printTable( elem)
					else
						output:print( elem)
					end
					output:closetag()
				end
			else
				-- print table (keys are values)
				output:opentag( t)
				printTable( v)
				output:closetag()
			end
		else
			output:opentag( t)
			output:print( v)
			output:closetag()
		end
	end
end

function run()
	local doctype = input:doctype()
	output:as( provider.filter(), doctype)
	printTable( input:table())
end
**output
 < ? x m l   v e r s i o n = " 1 . 0 "   e n c o d i n g = " U T F - 1 6 B E "   s t a n d a l o n e = " n o " ? > 
 < ! D O C T Y P E   r o o t   S Y S T E M   " h t t p : / / w w w . w o l f r a m e . o r g / e x a m p l e - d o c / t r e e s . d t d " > < r o o t > < b t r e e > < i d > 1 < / i d > < l e f t > < i d > 1 1 < / i d > < n a m e > E I N S   E I N S < / n a m e > < / l e f t > < n a m e > E I N S < / n a m e > < r i g h t > < i d > 1 2 < / i d > < l e f t > < i d > 1 2 1 < / i d > < n a m e > E I N S   Z W E I   E I N S < / n a m e > < / l e f t > < n a m e > E I N S   Z W E I < / n a m e > < / r i g h t > < / b t r e e > < m t r e e > < i d > 1 < / i d > < n a m e > E I N S < / n a m e > < n o d e > < i d > 1 1 < / i d > < n a m e > E I N S   E I N S < / n a m e > < / n o d e > < n o d e > < i d > 1 2 < / i d > < n a m e > E I N S   Z W E I < / n a m e > < n o d e > < i d > 1 2 1 < / i d > < n a m e > E I N S   Z W E I   E I N S < / n a m e > < / n o d e > < / n o d e > < n o d e > < i d > 1 3 < / i d > < n a m e > E I N S   D R E I < / n a m e > < / n o d e > < / m t r e e > < / r o o t > 
**end
