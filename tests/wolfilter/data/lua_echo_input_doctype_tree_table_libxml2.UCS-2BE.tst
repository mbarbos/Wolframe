**
**requires:LUA
**requires:LIBXML2
**input
 < ? x m l   v e r s i o n = " 1 . 0 "   e n c o d i n g = " U C S - 2 B E "   s t a n d a l o n e = " n o " ? > 
 < ! D O C T Y P E   r o o t   S Y S T E M   " h t t p : / / w w w . w o l f r a m e . o r g / e x a m p l e - d o c / t r e e s . d t d " > 
 < r o o t > 
 < m t r e e   i d = ' 1 ' > 
 < n a m e > e i n s < / n a m e > 
 < n o d e   i d = ' 1 . 1 ' > 
 < n a m e > E i n s   e i n s < / n a m e > 
 < / n o d e > 
 < n o d e   i d = ' 1 . 2 ' > 
 < n a m e > E i n s   z w e i < / n a m e > 
 < n o d e   i d = ' 1 . 2 . 1 ' > 
 < n a m e > E i n s   z w e i   e i n s < / n a m e > 
 < / n o d e > 
 < / n o d e > 
 < n o d e   i d = ' 1 . 3 ' > 
 < n a m e > E i n s   d r e i < / n a m e > 
 < / n o d e > 
 < / m t r e e > 
 < b t r e e   i d = ' 1 ' > 
 < n a m e > e i n s < / n a m e > 
 < l e f t   i d = ' 1 . 1 ' > 
 < i d > 1 . 1 < / i d > 
 < n a m e > E i n s   e i n s < / n a m e > 
 < / l e f t > 
 < r i g h t   i d = ' 1 . 2 ' > 
 < n a m e > E i n s   z w e i < / n a m e > 
 < l e f t   i d = ' 1 . 2 . 1 ' > 
 < n a m e > E i n s   z w e i   e i n s < / n a m e > 
 < / l e f t > 
 < / r i g h t > 
 < / b t r e e > 
 < / r o o t > 
**config
--input-filter xml:libxml2 --output-filter xml:libxml2 --module ../../src/modules/filter/libxml2/mod_filter_libxml2  --module ../../src/modules/cmdbind/lua/mod_command_lua --module ../../src/modules/ddlcompiler//simpleform/mod_ddlcompiler_simpleform --module ../../src/modules/normalize//number/mod_normalize_number --module ../../src/modules/normalize//string/mod_normalize_string --program simpleform_string.normalize --program trees.simpleform --cmdprogram echo_input_doctype_table.lua run

**file:simpleform_string.normalize
int=number:integer;
uint=number:unsigned;
float=number:float;
normname=string:convdia,ucname;
**file: echo_input_doctype_table.lua
function run()
	type = input:doctype()
	output:as( filter(), type)
	output:print( input:table())
end
**file: trees.simpleform
STRUCT MulTreeNode
{
	id		@int
	name		normname
	node		MulTreeNode[]
}

STRUCT BinTreeNode
{
	id		@int
	name		normname
	left		BinTreeNode
	right		BinTreeNode
}

FORM trees
{
	root
	{
		mtree	MulTreeNode
		btree	BinTreeNode
	}
}


**output
 < ? x m l   v e r s i o n = " 1 . 0 "   e n c o d i n g = " U C S - 2 B E "   s t a n d a l o n e = " n o " ? > 
 < ! D O C T Y P E   r o o t   S Y S T E M   " h t t p : / / w w w . w o l f r a m e . o r g / e x a m p l e - d o c / t r e e s . d t d " > 
 < r o o t > 
 < m t r e e > 
 < i d > 1 < / i d > 
 < n a m e > e i n s < / n a m e > 
 < n o d e > 
 < i d > 1 . 1 < / i d > 
 < n a m e > E i n s   e i n s < / n a m e > 
 < / n o d e > 
 < n o d e > 
 < i d > 1 . 2 < / i d > 
 < n a m e > E i n s   z w e i < / n a m e > 
 < n o d e > 
 < i d > 1 . 2 . 1 < / i d > 
 < n a m e > E i n s   z w e i   e i n s < / n a m e > 
 < / n o d e > 
 < / n o d e > 
 < n o d e > 
 < i d > 1 . 3 < / i d > 
 < n a m e > E i n s   d r e i < / n a m e > 
 < / n o d e > 
 < / m t r e e > 
 < b t r e e > 
 < i d > 1 < / i d > 
 < n a m e > e i n s < / n a m e > 
 < l e f t > 
 < i d > 1 . 1 < / i d > 
 < n a m e > E i n s   e i n s < / n a m e > 
 < / l e f t > 
 < r i g h t > 
 < i d > 1 . 2 < / i d > 
 < n a m e > E i n s   z w e i < / n a m e > 
 < l e f t > 
 < i d > 1 . 2 . 1 < / i d > 
 < n a m e > E i n s   z w e i   e i n s < / n a m e > 
 < / l e f t > 
 < / r i g h t > 
 < / b t r e e > 
 < / r o o t > 
 
**end
