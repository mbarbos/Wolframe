**
**requires:LUA
**requires:CJSON
**requires:TEXTWOLF
**input
{   
                   "   -   d   o   c   t   y   p   e   "   :       "   P   e   r   s   o   n   "   ,   
                   "   n   a   m   e   "   :       "   H   u   g   i       T   u   r   b   o   "   ,   
                   "   a   d   d   r   e   s   s   "   :       "   G   u   r   k   e   n   s   t   r   a   s   s   e       7   a       3   1   4   5       G   u   m   l   i   g   e   n   "   
   }   **config
--input-filter cjson --output-filter cjson --module ../../src/modules/filter/cjson/mod_filter_cjson --module ../../src/modules/doctype/json/mod_doctype_json -c wolframe.conf run

**file:wolframe.conf
LoadModules
{
	module ./../../src/modules/cmdbind/lua/mod_command_lua
}
Processor
{
	cmdhandler
	{
		lua
		{
			program no_operation.lua
			filter cjson
		}
	}
}
**file:no_operation.lua
function run( input )
end
**output
{   
   	   "   -   d   o   c   t   y   p   e   "   :   	   "   P   e   r   s   o   n   "   
   }   
   **end
