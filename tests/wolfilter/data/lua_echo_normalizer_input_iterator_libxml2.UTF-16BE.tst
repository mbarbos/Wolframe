**
**requires:LUA
**requires:LIBXML2
**input
 < ? x m l   v e r s i o n = " 1 . 0 "   e n c o d i n g = " U T F - 1 6 B E "   s t a n d a l o n e = " y e s " ? > 
 < d o c > 
 	 < i t e m > I m   F r a n z � s i s c h e n   g i b t   e s   B u c h s t a b e n   w i e   " � "   " � "   " � "   " � "   ( e   t r � m a ) < / i t e m > 
 	 < i t e m > I m   D e u t s c h e n   s i n d   e s   d i e   U m l a t e   w i e   " � "   " � "   " � " < / i t e m > 
 < / d o c >**config
--input-filter libxml2 --output-filter libxml2 --module ../../src/modules/filter/libxml2/mod_filter_libxml2 --module ../../src/modules/doctype/xml/mod_doctype_xml -c wolframe.conf run

**file:wolframe.conf
LoadModules
{
	module ../../src/modules/cmdbind/lua/mod_command_lua
	module ../../src/modules/normalize/string/mod_normalize_string
}
Processor
{
	program normalize.wnmp
	cmdhandler
	{
		lua
		{
			program script.lua
			filter libxml2
		}
	}
}
**file:script.lua
function run()
	for v,t in input:get()
	do
		if not t and v then
			output:print( provider.type("name")(v),t)
		else
			output:print( v,t)
		end
	end
end
**file:normalize.wnmp
name=ucname;

**output
 < ? x m l   v e r s i o n = " 1 . 0 "   e n c o d i n g = " U T F - 1 6 B E "   s t a n d a l o n e = " y e s " ? > 
 < d o c > < i t e m > I M   F R A N Z O S I S C H E N   G I B T   E S   B U C H S T A B E N   W I E   C   E   A   E   E   T R E M A < / i t e m > < i t e m > I M   D E U T S C H E N   S I N D   E S   D I E   U M L A T E   W I E   U   A   O < / i t e m > < / d o c > 
**end
