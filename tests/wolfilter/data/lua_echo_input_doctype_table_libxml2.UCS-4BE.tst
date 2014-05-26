**
**requires:LUA
**requires:LIBXML2
**input
   <   ?   x   m   l       v   e   r   s   i   o   n   =   "   1   .   0   "       e   n   c   o   d   i   n   g   =   "   U   C   S   -   4   B   E   "       s   t   a   n   d   a   l   o   n   e   =   "   n   o   "   ?   >   
   <   !   D   O   C   T   Y   P   E       a   s   s   i   g   n   m   e   n   t   l   i   s   t       S   Y   S   T   E   M       "   h   t   t   p   :   /   /   w   w   w   .   w   o   l   f   r   a   m   e   .   o   r   g   /   e   x   a   m   p   l   e   -   d   o   c   /   e   m   p   l   o   y   e   e   _   a   s   s   i   g   n   m   e   n   t   _   p   r   i   n   t   .   d   t   d   "   >   <   a   s   s   i   g   n   m   e   n   t   l   i   s   t   >   <   a   s   s   i   g   n   m   e   n   t   >   <   t   a   s   k   >   <   t   i   t   l   e   >   j   o   b       1   <   /   t   i   t   l   e   >   <   k   e   y   >   A   1   2   3   <   /   k   e   y   >   <   c   u   s   t   o   m   e   r   n   u   m   b   e   r   >   3   2   4   <   /   c   u   s   t   o   m   e   r   n   u   m   b   e   r   >   <   /   t   a   s   k   >   <   t   a   s   k   >   <   t   i   t   l   e   >   j   o   b       2   <   /   t   i   t   l   e   >   <   k   e   y   >   V   4   5   6   <   /   k   e   y   >   <   c   u   s   t   o   m   e   r   n   u   m   b   e   r   >   5   6   7   <   /   c   u   s   t   o   m   e   r   n   u   m   b   e   r   >   <   /   t   a   s   k   >   <   e   m   p   l   o   y   e   e   >   <   f   i   r   s   t   n   a   m   e   >   J   u   l   i   a   <   /   f   i   r   s   t   n   a   m   e   >   <   s   u   r   n   a   m   e   >   T   e   g   e   l   -   S   a   c   h   e   r   <   /   s   u   r   n   a   m   e   >   <   p   h   o   n   e   >   0   9   8       7   6   5       4   3       2   1   <   /   p   h   o   n   e   >   <   /   e   m   p   l   o   y   e   e   >   <   i   s   s   u   e   d   a   t   e   >   1   3   .   5   .   2   0   0   6   <   /   i   s   s   u   e   d   a   t   e   >   <   /   a   s   s   i   g   n   m   e   n   t   >   <   a   s   s   i   g   n   m   e   n   t   >   <   t   a   s   k   >   <   t   i   t   l   e   >   j   o   b       4   <   /   t   i   t   l   e   >   <   k   e   y   >   V   7   8   9   <   /   k   e   y   >   <   c   u   s   t   o   m   e   r   n   u   m   b   e   r   >   8   9   0   <   /   c   u   s   t   o   m   e   r   n   u   m   b   e   r   >   <   /   t   a   s   k   >   <   e   m   p   l   o   y   e   e   >   <   f   i   r   s   t   n   a   m   e   >   J   a   k   o   b   <   /   f   i   r   s   t   n   a   m   e   >   <   s   u   r   n   a   m   e   >   S   t   e   g   e   l   i   n   <   /   s   u   r   n   a   m   e   >   <   p   h   o   n   e   >   0   1   2       3   4   5       6   7       8   9   <   /   p   h   o   n   e   >   <   /   e   m   p   l   o   y   e   e   >   <   i   s   s   u   e   d   a   t   e   >   1   3   .   5   .   2   0   0   6   <   /   i   s   s   u   e   d   a   t   e   >   <   /   a   s   s   i   g   n   m   e   n   t   >   <   /   a   s   s   i   g   n   m   e   n   t   l   i   s   t   >**config
--input-filter libxml2 --output-filter libxml2 --module ../../src/modules/filter/libxml2/mod_filter_libxml2 -c wolframe.conf run

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
		}
	}
}
**file:normalize.wnmp
int=trim,integer(10 );
uint=trim,unsigned(10);
float=trim, floatingpoint( 10,10);
currency=bigfxp(  2);
percent_1=bigfxp(2 );
**file:form.sfrm
FORM Employee
{
	firstname string
	surname string
	phone string
}

FORM employee_assignment_print
	:root assignmentlist
{
	assignment []
	{
		task []
		{
			title string
			key string
			customernumber int
		}
		employee Employee
		issuedate string
	}
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
**requires:DISABLED NETBSD

**output
   <   ?   x   m   l       v   e   r   s   i   o   n   =   "   1   .   0   "       e   n   c   o   d   i   n   g   =   "   U   C   S   -   4   B   E   "       s   t   a   n   d   a   l   o   n   e   =   "   n   o   "   ?   >   
   <   !   D   O   C   T   Y   P   E       a   s   s   i   g   n   m   e   n   t   l   i   s   t       S   Y   S   T   E   M       "   h   t   t   p   :   /   /   w   w   w   .   w   o   l   f   r   a   m   e   .   o   r   g   /   e   x   a   m   p   l   e   -   d   o   c   /   e   m   p   l   o   y   e   e   _   a   s   s   i   g   n   m   e   n   t   _   p   r   i   n   t   .   d   t   d   "   >   <   a   s   s   i   g   n   m   e   n   t   l   i   s   t   >   <   a   s   s   i   g   n   m   e   n   t   >   <   e   m   p   l   o   y   e   e   >   <   f   i   r   s   t   n   a   m   e   >   J   u   l   i   a   <   /   f   i   r   s   t   n   a   m   e   >   <   p   h   o   n   e   >   0   9   8       7   6   5       4   3       2   1   <   /   p   h   o   n   e   >   <   s   u   r   n   a   m   e   >   T   e   g   e   l   -   S   a   c   h   e   r   <   /   s   u   r   n   a   m   e   >   <   /   e   m   p   l   o   y   e   e   >   <   i   s   s   u   e   d   a   t   e   >   1   3   .   5   .   2   0   0   6   <   /   i   s   s   u   e   d   a   t   e   >   <   t   a   s   k   >   <   c   u   s   t   o   m   e   r   n   u   m   b   e   r   >   3   2   4   <   /   c   u   s   t   o   m   e   r   n   u   m   b   e   r   >   <   k   e   y   >   A   1   2   3   <   /   k   e   y   >   <   t   i   t   l   e   >   j   o   b       1   <   /   t   i   t   l   e   >   <   /   t   a   s   k   >   <   t   a   s   k   >   <   c   u   s   t   o   m   e   r   n   u   m   b   e   r   >   5   6   7   <   /   c   u   s   t   o   m   e   r   n   u   m   b   e   r   >   <   k   e   y   >   V   4   5   6   <   /   k   e   y   >   <   t   i   t   l   e   >   j   o   b       2   <   /   t   i   t   l   e   >   <   /   t   a   s   k   >   <   /   a   s   s   i   g   n   m   e   n   t   >   <   a   s   s   i   g   n   m   e   n   t   >   <   e   m   p   l   o   y   e   e   >   <   f   i   r   s   t   n   a   m   e   >   J   a   k   o   b   <   /   f   i   r   s   t   n   a   m   e   >   <   p   h   o   n   e   >   0   1   2       3   4   5       6   7       8   9   <   /   p   h   o   n   e   >   <   s   u   r   n   a   m   e   >   S   t   e   g   e   l   i   n   <   /   s   u   r   n   a   m   e   >   <   /   e   m   p   l   o   y   e   e   >   <   i   s   s   u   e   d   a   t   e   >   1   3   .   5   .   2   0   0   6   <   /   i   s   s   u   e   d   a   t   e   >   <   t   a   s   k   >   <   c   u   s   t   o   m   e   r   n   u   m   b   e   r   >   8   9   0   <   /   c   u   s   t   o   m   e   r   n   u   m   b   e   r   >   <   k   e   y   >   V   7   8   9   <   /   k   e   y   >   <   t   i   t   l   e   >   j   o   b       4   <   /   t   i   t   l   e   >   <   /   t   a   s   k   >   <   /   a   s   s   i   g   n   m   e   n   t   >   <   /   a   s   s   i   g   n   m   e   n   t   l   i   s   t   >   
**end
