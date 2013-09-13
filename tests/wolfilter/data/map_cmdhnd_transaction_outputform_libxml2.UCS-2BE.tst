**
**requires:LIBXML2
**input
 < ? x m l   v e r s i o n = " 1 . 0 "   e n c o d i n g = " U C S - 2 B E "   s t a n d a l o n e = " y e s " ? > 
 < a s s i g n m e n t l i s t > < a s s i g n m e n t > < t a s k > < t i t l e > j o b   1 < / t i t l e > < k e y > A 1 2 3 < / k e y > < c u s t o m e r n u m b e r > 3 2 4 < / c u s t o m e r n u m b e r > < / t a s k > < t a s k > < t i t l e > j o b   2 < / t i t l e > < k e y > V 4 5 6 < / k e y > < c u s t o m e r n u m b e r > 5 6 7 < / c u s t o m e r n u m b e r > < / t a s k > < e m p l o y e e > < f i r s t n a m e > J u l i a < / f i r s t n a m e > < s u r n a m e > T e g e l - S a c h e r < / s u r n a m e > < p h o n e > 0 9 8   7 6 5   4 3   2 1 < / p h o n e > < / e m p l o y e e > < i s s u e d a t e > 1 3 . 5 . 2 0 0 6 < / i s s u e d a t e > < / a s s i g n m e n t > < a s s i g n m e n t > < t a s k > < t i t l e > j o b   3 < / t i t l e > < k e y > A 4 5 6 < / k e y > < c u s t o m e r n u m b e r > 5 6 7 < / c u s t o m e r n u m b e r > < / t a s k > < t a s k > < t i t l e > j o b   4 < / t i t l e > < k e y > V 7 8 9 < / k e y > < c u s t o m e r n u m b e r > 8 9 0 < / c u s t o m e r n u m b e r > < / t a s k > < e m p l o y e e > < f i r s t n a m e > J a k o b < / f i r s t n a m e > < s u r n a m e > S t e g e l i n < / s u r n a m e > < p h o n e > 0 1 2   3 4 5   6 7   8 9 < / p h o n e > < / e m p l o y e e > < i s s u e d a t e > 1 3 . 5 . 2 0 0 6 < / i s s u e d a t e > < / a s s i g n m e n t > < / a s s i g n m e n t l i s t >**config
--input-filter xml:libxml2 --output-filter xml:libxml2 --module ../../src/modules/filter/libxml2/mod_filter_libxml2  --program map_cmdhnd_transaction_outputform.sfrm --program employee_assignment_print.sfrm --module ../../src/modules/ddlcompiler//simpleform/mod_ddlcompiler_simpleform --module ../../src/modules/normalize//number/mod_normalize_number --module ../../src/modules/normalize//string/mod_normalize_string --program simpleform.wnmp --module ../../src/modules/cmdbind/directmap/mod_command_directmap --module ../wolfilter/modules/database/testtrace/mod_db_testtrace --database 'identifier=testdb,outfile=DBOUT,file=DBRES' --program=DBIN.tdl --cmdprogram=test.dmap run

**file: test.dmap
run = test_transaction( xml :employee_assignment_print) :map_cmdhnd_transaction_outputform;
**file:simpleform.wnmp
int=number:integer;
uint=number:unsigned;
float=number:float;
**file: DBRES
#id Task start end#11 'bla bla' '12:04:19 1/3/2012' '12:41:34 1/3/2012'#12 'bli blu' '07:14:23 1/3/2012' '08:01:51 1/3/2012'
#id task Start end#21 'gardening' '09:24:28 1/3/2012' '11:11:07 1/3/2012'#22 'helo' '11:31:01 1/3/2012' '12:07:55 1/3/2012'
#ID task start End#31 'hula hop' '19:14:38 1/4/2012' '20:01:12 1/4/2012'#32 'hula hip' '11:31:01 1/3/2012' '12:07:55 1/3/2012'#33 'hula hup' '11:31:01 1/3/2012' '12:07:55 1/3/2012'
**file:DBIN.tdl
TRANSACTION test_transaction
BEGIN
	INTO task FOREACH //task DO run( title);
END
**file: employee_assignment_print.sfrm
FORM Employee
{
	firstname string
	surname string
	phone string
}

FORM employee_assignment_print
{
	assignmentlist
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
}

**file: map_cmdhnd_transaction_outputform.sfrm
FORM map_cmdhnd_transaction_outputform
{
	doc
	{
		task []
		{
			task string
			start string
			end string
			id int
		}
	}
}
**output
 < ? x m l   v e r s i o n = " 1 . 0 "   e n c o d i n g = " U C S - 2 B E "   s t a n d a l o n e = " n o " ? > 
 < ! D O C T Y P E   d a t a   S Y S T E M   " D a t a . s i m p l e f o r m " > < d a t a > < p e r s o n > < c o m p a n y > < n a m e > B a l u b a   I n c . < / n a m e > < / c o m p a n y > < c o m p a n y > < n a m e > C a r i m b a   I n c . < / n a m e > < / c o m p a n y > < c o m p a n y > < n a m e > D e r e n o   I n c . < / n a m e > < / c o m p a n y > < c o m p a n y > < n a m e > H u r a t z   I n c . < / n a m e > < / c o m p a n y > < i d > 1 < / i d > < c h i l d > < p r e n a m e > B e n o < / p r e n a m e > < s u r n a m e > B e r e t < / s u r n a m e > < / c h i l d > < c h i l d > < p r e n a m e > C a r l a < / p r e n a m e > < s u r n a m e > C a r l s o n < / s u r n a m e > < / c h i l d > < c h i l d > < p r e n a m e > D o r o t h e < / p r e n a m e > < s u r n a m e > D u b i < / s u r n a m e > < / c h i l d > < c h i l d > < p r e n a m e > H u b e r t < / p r e n a m e > < s u r n a m e > H a u e r < / s u r n a m e > < / c h i l d > < p r e n a m e > A u f r u < / p r e n a m e > < s u r n a m e > A l a n o < / s u r n a m e > < l o c a t i o n > < s t r e e t > B u t t e r w e g   2 3 < / s t r e e t > < t o w n > B e n d o r f < / t o w n > < / l o c a t i o n > < l o c a t i o n > < s t r e e t > C a m e l s t r e e t   3 4 < / s t r e e t > < t o w n > C a r a s s a < / t o w n > < / l o c a t i o n > < l o c a t i o n > < s t r e e t > D e m o t a s t r a s s e   4 5 < / s t r e e t > < t o w n > D u r n f o < / t o w n > < / l o c a t i o n > < l o c a t i o n > < s t r e e t > H u r t i k a   8 9 < / s t r e e t > < t o w n > H o f < / t o w n > < / l o c a t i o n > < t a g > 1 0 0 1 < / t a g > < / p e r s o n > < p e r s o n > < c o m p a n y > < n a m e > C a r i m b a   I n c . < / n a m e > < / c o m p a n y > < c o m p a n y > < n a m e > D e r e n o   I n c . < / n a m e > < / c o m p a n y > < c o m p a n y > < n a m e > E t u n g o   I n c . < / n a m e > < / c o m p a n y > < c o m p a n y > < n a m e > H u r a t z   I n c . < / n a m e > < / c o m p a n y > < i d > 2 < / i d > < c h i l d > < p r e n a m e > C a r l a < / p r e n a m e > < s u r n a m e > C a r l s o n < / s u r n a m e > < / c h i l d > < c h i l d > < p r e n a m e > D o r o t h e < / p r e n a m e > < s u r n a m e > D u b i < / s u r n a m e > < / c h i l d > < c h i l d > < p r e n a m e > E r i k < / p r e n a m e > < s u r n a m e > E r t k i < / s u r n a m e > < / c h i l d > < c h i l d > < p r e n a m e > H u b e r t < / p r e n a m e > < s u r n a m e > H a u e r < / s u r n a m e > < / c h i l d > < p r e n a m e > B e n o < / p r e n a m e > < s u r n a m e > B e r e t < / s u r n a m e > < l o c a t i o n > < s t r e e t > C a m e l s t r e e t   3 4 < / s t r e e t > < t o w n > C a r a s s a < / t o w n > < / l o c a t i o n > < l o c a t i o n > < s t r e e t > D e m o t a s t r a s s e   4 5 < / s t r e e t > < t o w n > D u r n f o < / t o w n > < / l o c a t i o n > < l o c a t i o n > < s t r e e t > E r a k i m o l s t r a s s e   5 6 < / s t r e e t > < t o w n > E n d e n < / t o w n > < / l o c a t i o n > < l o c a t i o n > < s t r e e t > H u r t i k a   8 9 < / s t r e e t > < t o w n > H o f < / t o w n > < / l o c a t i o n > < t a g > 1 0 0 1 < / t a g > < / p e r s o n > < p e r s o n > < c o m p a n y > < n a m e > D e r e n o   I n c . < / n a m e > < / c o m p a n y > < c o m p a n y > < n a m e > E t u n g o   I n c . < / n a m e > < / c o m p a n y > < c o m p a n y > < n a m e > F i g a j i   I n c . < / n a m e > < / c o m p a n y > < c o m p a n y > < n a m e > H u r a t z   I n c . < / n a m e > < / c o m p a n y > < i d > 3 < / i d > < c h i l d > < p r e n a m e > D o r o t h e < / p r e n a m e > < s u r n a m e > D u b i < / s u r n a m e > < / c h i l d > < c h i l d > < p r e n a m e > E r i k < / p r e n a m e > < s u r n a m e > E r t k i < / s u r n a m e > < / c h i l d > < c h i l d > < p r e n a m e > F r a n < / p r e n a m e > < s u r n a m e > F u i o k o < / s u r n a m e > < / c h i l d > < c h i l d > < p r e n a m e > H u b e r t < / p r e n a m e > < s u r n a m e > H a u e r < / s u r n a m e > < / c h i l d > < p r e n a m e > C a r l a < / p r e n a m e > < s u r n a m e > C a r l s o n < / s u r n a m e > < l o c a t i o n > < s t r e e t > D e m o t a s t r a s s e   4 5 < / s t r e e t > < t o w n > D u r n f o < / t o w n > < / l o c a t i o n > < l o c a t i o n > < s t r e e t > E r a k i m o l s t r a s s e   5 6 < / s t r e e t > < t o w n > E n d e n < / t o w n > < / l o c a t i o n > < l o c a t i o n > < s t r e e t > F a b e l w e g   6 7 < / s t r e e t > < t o w n > F o r m k o n < / t o w n > < / l o c a t i o n > < l o c a t i o n > < s t r e e t > H u r t i k a   8 9 < / s t r e e t > < t o w n > H o f < / t o w n > < / l o c a t i o n > < t a g > 1 0 0 1 < / t a g > < / p e r s o n > < p e r s o n > < c o m p a n y > < n a m e > E t u n g o   I n c . < / n a m e > < / c o m p a n y > < i d > 4 < / i d > < c h i l d > < p r e n a m e > E r i k < / p r e n a m e > < s u r n a m e > E r t k i < / s u r n a m e > < / c h i l d > < p r e n a m e > D o r o t h e < / p r e n a m e > < s u r n a m e > D u b i < / s u r n a m e > < l o c a t i o n > < s t r e e t > E r a k i m o l s t r a s s e   5 6 < / s t r e e t > < t o w n > E n d e n < / t o w n > < / l o c a t i o n > < t a g > 1 0 0 1 < / t a g > < / p e r s o n > < p e r s o n > < c o m p a n y > < n a m e > F i g a j i   I n c . < / n a m e > < / c o m p a n y > < i d > 5 < / i d > < c h i l d > < p r e n a m e > F r a n < / p r e n a m e > < s u r n a m e > F u i o k o < / s u r n a m e > < / c h i l d > < p r e n a m e > E r i k < / p r e n a m e > < s u r n a m e > E r t k i < / s u r n a m e > < l o c a t i o n > < s t r e e t > F a b e l w e g   6 7 < / s t r e e t > < t o w n > F o r m k o n < / t o w n > < / l o c a t i o n > < t a g > 1 0 0 1 < / t a g > < / p e r s o n > < p e r s o n > < c o m p a n y / > < i d > 6 < / i d > < c h i l d / > < p r e n a m e > F r a n < / p r e n a m e > < s u r n a m e > F u i o k o < / s u r n a m e > < l o c a t i o n / > < t a g > 1 0 0 1 < / t a g > < / p e r s o n > < p e r s o n > < c o m p a n y > < n a m e > H u r a t z   I n c . < / n a m e > < / c o m p a n y > < i d > 7 < / i d > < c h i l d > < p r e n a m e > H u b e r t < / p r e n a m e > < s u r n a m e > H a u e r < / s u r n a m e > < / c h i l d > < p r e n a m e > G e r d < / p r e n a m e > < s u r n a m e > G o l t o < / s u r n a m e > < l o c a t i o n > < s t r e e t > H u r t i k a   8 9 < / s t r e e t > < t o w n > H o f < / t o w n > < / l o c a t i o n > < t a g > 1 0 0 1 < / t a g > < / p e r s o n > < p e r s o n > < c o m p a n y / > < i d > 8 < / i d > < c h i l d / > < p r e n a m e > H u b e r t < / p r e n a m e > < s u r n a m e > H a u e r < / s u r n a m e > < l o c a t i o n / > < t a g > 1 0 0 1 < / t a g > < / p e r s o n > < / d a t a > 
**end
