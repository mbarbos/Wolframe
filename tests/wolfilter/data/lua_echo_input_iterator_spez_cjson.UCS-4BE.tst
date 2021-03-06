**
**requires:LUA
**requires:CJSON
**requires:TEXTWOLF
**input
   {   
   	   "   i   m   a   g   e   "   :       [   
   	   	   {       "   -   i   d   "   :       "   1   "       }   ,   
   	   	   {   
   	   	   	   "   -   i   d   "   :       "   1   "   ,   
   	   	   	   "   -   n   a   m   e   "   :       "   t   r   o   l   l   "   
   	   	   }   ,   
   	   	   {       "   -   i   d   "   :       "   2   "       }   ,   
   	   	   {   
   	   	   	   "   -   i   d   "   :       "   3   "   ,   
   	   	   	   "   #   t   e   x   t   "   :       "   X   "   
   	   	   }   
   	   ]   
   }**config
--input-filter cjson --output-filter cjson --module ../../src/modules/filter/cjson/mod_filter_cjson --module ../../src/modules/doctype/json/mod_doctype_json -c wolframe.conf run

**file:wolframe.conf
LoadModules
{
	module ../../src/modules/cmdbind/lua/mod_command_lua
}
Processor
{
	cmdhandler
	{
		lua
		{
			program script.lua
			filter cjson
		}
	}
}
**file:script.lua

local function process_image( itr)
	for v,t in itr do
		output:print( v, t)
	end
end

function run()
	local itr = input:get()
	for v,t in itr do
		if t == "image" then
			output:opentag( t)
			process_image( iterator.scope( itr))
			output:closetag()
		else
			output:print( v, t)
		end
	end
end
**output
   {   
   	   "   i   m   a   g   e   "   :   	   [   {   
   	   	   	   "   i   d   "   :   	   "   1   "   
   	   	   }   ,       {   
   	   	   	   "   i   d   "   :   	   "   1   "   ,   
   	   	   	   "   n   a   m   e   "   :   	   "   t   r   o   l   l   "   
   	   	   }   ,       {   
   	   	   	   "   i   d   "   :   	   "   2   "   
   	   	   }   ,       {   
   	   	   	   "   i   d   "   :   	   "   3   "   ,   
   	   	   	   "   #   t   e   x   t   "   :   	   "   X   "   
   	   	   }   ]   
   }   
**end
