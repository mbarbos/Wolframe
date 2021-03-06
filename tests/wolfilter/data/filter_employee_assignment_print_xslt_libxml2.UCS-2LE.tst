**
**requires:LIBXML2
**input
< ? x m l   v e r s i o n = " 1 . 0 "   e n c o d i n g = " U C S - 2 L E "   s t a n d a l o n e = " y e s " ? > 
 < a s s i g n m e n t l i s t > < a s s i g n m e n t > < t a s k > < t i t l e > j o b   1 < / t i t l e > < k e y > A 1 2 3 < / k e y > < c u s t o m e r n u m b e r > 3 2 4 < / c u s t o m e r n u m b e r > < / t a s k > < t a s k > < t i t l e > j o b   2 < / t i t l e > < k e y > V 4 5 6 < / k e y > < c u s t o m e r n u m b e r > 5 6 7 < / c u s t o m e r n u m b e r > < / t a s k > < e m p l o y e e > < f i r s t n a m e > J u l i a < / f i r s t n a m e > < s u r n a m e > T e g e l - S a c h e r < / s u r n a m e > < p h o n e > 0 9 8   7 6 5   4 3   2 1 < / p h o n e > < / e m p l o y e e > < i s s u e d a t e > 1 3 . 5 . 2 0 0 6 < / i s s u e d a t e > < / a s s i g n m e n t > < a s s i g n m e n t > < t a s k > < t i t l e > j o b   3 < / t i t l e > < k e y > A 4 5 6 < / k e y > < c u s t o m e r n u m b e r > 5 6 7 < / c u s t o m e r n u m b e r > < / t a s k > < t a s k > < t i t l e > j o b   4 < / t i t l e > < k e y > V 7 8 9 < / k e y > < c u s t o m e r n u m b e r > 8 9 0 < / c u s t o m e r n u m b e r > < / t a s k > < e m p l o y e e > < f i r s t n a m e > J a k o b < / f i r s t n a m e > < s u r n a m e > S t e g e l i n < / s u r n a m e > < p h o n e > 0 1 2   3 4 5   6 7   8 9 < / p h o n e > < / e m p l o y e e > < i s s u e d a t e > 1 3 . 5 . 2 0 0 6 < / i s s u e d a t e > < / a s s i g n m e n t > < / a s s i g n m e n t l i s t > **config
--input-filter employee_assignment_print_map --output-filter libxml2 --module ../../src/modules/filter/libxml2/mod_filter_libxml2 --module ../../src/modules/doctype/xml/mod_doctype_xml -c wolframe.conf -

**requires:LIBXSLT
**file:wolframe.conf
Processor
{
	program employee_assignment_print_map.xslt
}
**file:employee_assignment_print_map.xslt
<?xml version="1.0" standalone="yes"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="/assignmentlist">
  <assignmentlist>
    <xsl:for-each select="assignment">
      <assignment>
      <xsl:for-each select="task">
	<task>
	<title>title:<xsl:value-of select="title"/></title>
	<key>key:<xsl:value-of select="key"/></key>
	<customernumber>customernumber:<xsl:value-of select="customernumber"/></customernumber>
	</task>
      </xsl:for-each>
      <xsl:for-each select="employee">
	<employee>
	<firstname>firstname:<xsl:value-of select="firstname"/></firstname>
	<surname>surname:<xsl:value-of select="surname"/></surname>
	<phone>phone:<xsl:value-of select="phone"/></phone>
	</employee>
      </xsl:for-each>
      <issuedate>issuedate:<xsl:value-of select="issuedate"/></issuedate>
      </assignment>
    </xsl:for-each>
  </assignmentlist>
</xsl:template>

</xsl:stylesheet>
**output
< ? x m l   v e r s i o n = " 1 . 0 "   e n c o d i n g = " U C S - 2 L E "   s t a n d a l o n e = " y e s " ? > 
 < a s s i g n m e n t l i s t > < a s s i g n m e n t > < t a s k > < t i t l e > t i t l e : j o b   1 < / t i t l e > < k e y > k e y : A 1 2 3 < / k e y > < c u s t o m e r n u m b e r > c u s t o m e r n u m b e r : 3 2 4 < / c u s t o m e r n u m b e r > < / t a s k > < t a s k > < t i t l e > t i t l e : j o b   2 < / t i t l e > < k e y > k e y : V 4 5 6 < / k e y > < c u s t o m e r n u m b e r > c u s t o m e r n u m b e r : 5 6 7 < / c u s t o m e r n u m b e r > < / t a s k > < e m p l o y e e > < f i r s t n a m e > f i r s t n a m e : J u l i a < / f i r s t n a m e > < s u r n a m e > s u r n a m e : T e g e l - S a c h e r < / s u r n a m e > < p h o n e > p h o n e : 0 9 8   7 6 5   4 3   2 1 < / p h o n e > < / e m p l o y e e > < i s s u e d a t e > i s s u e d a t e : 1 3 . 5 . 2 0 0 6 < / i s s u e d a t e > < / a s s i g n m e n t > < a s s i g n m e n t > < t a s k > < t i t l e > t i t l e : j o b   3 < / t i t l e > < k e y > k e y : A 4 5 6 < / k e y > < c u s t o m e r n u m b e r > c u s t o m e r n u m b e r : 5 6 7 < / c u s t o m e r n u m b e r > < / t a s k > < t a s k > < t i t l e > t i t l e : j o b   4 < / t i t l e > < k e y > k e y : V 7 8 9 < / k e y > < c u s t o m e r n u m b e r > c u s t o m e r n u m b e r : 8 9 0 < / c u s t o m e r n u m b e r > < / t a s k > < e m p l o y e e > < f i r s t n a m e > f i r s t n a m e : J a k o b < / f i r s t n a m e > < s u r n a m e > s u r n a m e : S t e g e l i n < / s u r n a m e > < p h o n e > p h o n e : 0 1 2   3 4 5   6 7   8 9 < / p h o n e > < / e m p l o y e e > < i s s u e d a t e > i s s u e d a t e : 1 3 . 5 . 2 0 0 6 < / i s s u e d a t e > < / a s s i g n m e n t > < / a s s i g n m e n t l i s t > 
 **end
