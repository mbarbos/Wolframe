**
**input
< ? x m l   v e r s i o n = " 1 . 0 "   e n c o d i n g = " U T F - 1 6 L E "   s t a n d a l o n e = " y e s " ? > 
 < a s s i g n m e n t l i s t > < a s s i g n m e n t > < t a s k > < t i t l e > j o b   1 < / t i t l e > < k e y > A 1 2 3 < / k e y > < c u s t o m e r n u m b e r > 3 2 4 < / c u s t o m e r n u m b e r > < / t a s k > < t a s k > < t i t l e > j o b   2 < / t i t l e > < k e y > V 4 5 6 < / k e y > < c u s t o m e r n u m b e r > 5 6 7 < / c u s t o m e r n u m b e r > < / t a s k > < e m p l o y e e > < f i r s t n a m e > J u l i a < / f i r s t n a m e > < s u r n a m e > T e g e l - S a c h e r < / s u r n a m e > < p h o n e > 0 9 8   7 6 5   4 3   2 1 < / p h o n e > < / e m p l o y e e > < i s s u e d a t e > 1 3 . 5 . 2 0 0 6 < / i s s u e d a t e > < / a s s i g n m e n t > < a s s i g n m e n t > < t a s k > < t i t l e > j o b   3 < / t i t l e > < k e y > A 4 5 6 < / k e y > < c u s t o m e r n u m b e r > 5 6 7 < / c u s t o m e r n u m b e r > < / t a s k > < t a s k > < t i t l e > j o b   4 < / t i t l e > < k e y > V 7 8 9 < / k e y > < c u s t o m e r n u m b e r > 8 9 0 < / c u s t o m e r n u m b e r > < / t a s k > < e m p l o y e e > < f i r s t n a m e > J a k o b < / f i r s t n a m e > < s u r n a m e > S t e g e l i n < / s u r n a m e > < p h o n e > 0 1 2   3 4 5   6 7   8 9 < / p h o n e > < / e m p l o y e e > < i s s u e d a t e > 1 3 . 5 . 2 0 0 6 < / i s s u e d a t e > < / a s s i g n m e n t > < / a s s i g n m e n t l i s t > **config
--input-filter textwolf --output-filter textwolf --module ../../src/modules/filter/textwolf/mod_filter_textwolf -c wolframe.conf employee_assignment_print
**requires:TEXTWOLF
**file:wolframe.conf
LoadModules
{
	module ./../wolfilter/modules/database/testtrace/mod_db_testtrace
	module ./../../src/modules/normalize/number/mod_normalize_number
	module ./../../src/modules/normalize/string/mod_normalize_string
	module ./../../src/modules/cmdbind/directmap/mod_command_directmap
	module ./../../src/modules/ddlcompiler/simpleform/mod_ddlcompiler_simpleform
	module ./../../src/modules/datatype/bcdnumber/mod_datatype_bcdnumber
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
	database testdb
	program DBIN.tdl
	program ../wolfilter/scripts/employee_assignment_print.sfrm
	program ../wolfilter/template/program/simpleform.wnmp

	cmdhandler
	{
		directmap
		{
			program test.dmap
			filter textwolf
		}
	}
}
**file: test.dmap
COMMAND(employee_assignment_print) CALL(test_transaction) RETURN STANDALONE doc;
**file: DBRES
#id task start end#11 'bla bla' '12:04:19 1/3/2012' '12:41:34 1/3/2012'#12 'bli blu' '07:14:23 1/3/2012' '08:01:51 1/3/2012'
#id task start end#21 'gardening' '09:24:28 1/3/2012' '11:11:07 1/3/2012'#22 'helo' '11:31:01 1/3/2012' '12:07:55 1/3/2012'
#id task start end#31 'hula hop' '19:14:38 1/4/2012' '20:01:12 1/4/2012'#32 'hula hip' '11:31:01 1/3/2012' '12:07:55 1/3/2012'#33 'hula hup' '11:31:01 1/3/2012' '12:07:55 1/3/2012'
**file:DBIN.tdl
TRANSACTION test_transaction
BEGIN
	INTO task FOREACH //task DO SELECT run( title);
END
**output
< ? x m l   v e r s i o n = " 1 . 0 "   e n c o d i n g = " U T F - 1 6 L E "   s t a n d a l o n e = " y e s " ? > 
 < a s s i g n m e n t l i s t > < t a s k > < i d > 1 1 < / i d > < t a s k > b l a   b l a < / t a s k > < s t a r t > 1 2 : 0 4 : 1 9   1 / 3 / 2 0 1 2 < / s t a r t > < e n d > 1 2 : 4 1 : 3 4   1 / 3 / 2 0 1 2 < / e n d > < / t a s k > < t a s k > < i d > 1 2 < / i d > < t a s k > b l i   b l u < / t a s k > < s t a r t > 0 7 : 1 4 : 2 3   1 / 3 / 2 0 1 2 < / s t a r t > < e n d > 0 8 : 0 1 : 5 1   1 / 3 / 2 0 1 2 < / e n d > < / t a s k > < t a s k > < i d > 2 1 < / i d > < t a s k > g a r d e n i n g < / t a s k > < s t a r t > 0 9 : 2 4 : 2 8   1 / 3 / 2 0 1 2 < / s t a r t > < e n d > 1 1 : 1 1 : 0 7   1 / 3 / 2 0 1 2 < / e n d > < / t a s k > < t a s k > < i d > 2 2 < / i d > < t a s k > h e l o < / t a s k > < s t a r t > 1 1 : 3 1 : 0 1   1 / 3 / 2 0 1 2 < / s t a r t > < e n d > 1 2 : 0 7 : 5 5   1 / 3 / 2 0 1 2 < / e n d > < / t a s k > < t a s k > < i d > 3 1 < / i d > < t a s k > h u l a   h o p < / t a s k > < s t a r t > 1 9 : 1 4 : 3 8   1 / 4 / 2 0 1 2 < / s t a r t > < e n d > 2 0 : 0 1 : 1 2   1 / 4 / 2 0 1 2 < / e n d > < / t a s k > < t a s k > < i d > 3 2 < / i d > < t a s k > h u l a   h i p < / t a s k > < s t a r t > 1 1 : 3 1 : 0 1   1 / 3 / 2 0 1 2 < / s t a r t > < e n d > 1 2 : 0 7 : 5 5   1 / 3 / 2 0 1 2 < / e n d > < / t a s k > < t a s k > < i d > 3 3 < / i d > < t a s k > h u l a   h u p < / t a s k > < s t a r t > 1 1 : 3 1 : 0 1   1 / 3 / 2 0 1 2 < / s t a r t > < e n d > 1 2 : 0 7 : 5 5   1 / 3 / 2 0 1 2 < / e n d > < / t a s k > < / a s s i g n m e n t l i s t > 
 **end
