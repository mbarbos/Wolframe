**
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
--input-filter cjson --output-filter cjson --module ../../src/modules/filter/cjson/mod_filter_cjson --module ../../src/modules/doctype/json/mod_doctype_json -c wolframe.conf employee_assignment_print

**file:wolframe.conf
LoadModules
{
	module ./../../src/modules/ddlcompiler/simpleform/mod_ddlcompiler_simpleform
	module ./../../src/modules/normalize/number/mod_normalize_number
	module ./../../src/modules/normalize/string/mod_normalize_string
	module ./../../src/modules/datatype/bcdnumber/mod_datatype_bcdnumber
}
Processor
{
	program ../wolfilter/scripts/employee_assignment_print.sfrm
	program ../wolfilter/template/program/simpleform.wnmp
}
**output
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
 } 
**end
