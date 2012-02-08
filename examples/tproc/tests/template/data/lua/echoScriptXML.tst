#!/bin/sh
for example in\
	"ISO-8859-1=us=employee Sara Sample 0123456789 sara.sample@gmail.com engineer"\
	"UTF-8=us=employee Sara Sample 0123456789 sara.sample@gmail.com engineer"\
	"UTF-16=us=employee Sara Sample 0123456789 sara.sample@gmail.com engineer"\
	"UCS-2LE=us=employee Sara Sample 0123456789 sara.sample@gmail.com engineer"\
	"UCS-4BE=us=employee Sara Sample 0123456789 sara.sample@gmail.com engineer"\
	"UTF-8=cn=employee &#xE6&#x98&#x8E &#xE5&#xA7&#x98 021436587 yao.ming@gmail.com sportsman"\
	"UTF-16=cn=employee &#xE6&#x98&#x8E &#xE5&#xA7&#x98 021436587 yao.ming@gmail.com sportsman"\
	"UCS-2BE=cn=employee &#xE6&#x98&#x8E &#xE5&#xA7&#x98 021436587 yao.ming@gmail.com sportsman"\
	"UCS-2LE=cn=employee &#xE6&#x98&#x8E &#xE5&#xA7&#x98 021436587 yao.ming@gmail.com sportsman"\
	"UCS-4BE=cn=employee &#xE6&#x98&#x8E &#xE5&#xA7&#x98 021436587 yao.ming@gmail.com sportsman"\
	"UCS-4LE=cn=employee &#xE6&#x98&#x8E &#xE5&#xA7&#x98 021436587 yao.ming@gmail.com sportsman"
do
cset=`echo $example | cut -f1 -d=`
tag=`echo $example | cut -f2 -d=`
testdoc=`echo $example | cut -f3 -d=`

output="../`echo $0 | sed 's/template//' | sed 's/.tst$//'`.$tag.$cset.tst"
rm -f $output
echo "Expanding test file $output"

recode lat1..ibmpc >> $output <<!TEST
--
--requires:LUA
--requires:DISABLED
--input
HELLO
RUN
!TEST
./xmltestdoc.sh $cset $testdoc >> $output
recode lat1..ibmpc >> $output <<!TEST

.
QUIT
--file:echo.lua
function run( )
	f = filter( "xml:libxml2")
	f.empty = false

	input:as( f)
	output:as( f)

	for c,t in input:get() do
		output:print( c, t)
	end
end
--config
script {
	name RUN
	path echo.lua
	main run
}
--output
OK enter cmd
!TEST
./xmltestdoc.sh $cset $testdoc >> $output
recode lat1..ibmpc >> $output <<!TEST

.
OK
OK enter cmd
BYE
--end

!TEST
done
