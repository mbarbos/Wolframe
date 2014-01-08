#!/bin/sh
testname=`basename $0 ".tst"`				# name of the test
opt=""
modpath="../../src/modules"				# module directory relative from tests/temp
opt="$opt --module $modpath/cmdbind/lua/mod_command_lua"
luascript=`echo $testname | sed 's/lua_//' | sed 's/_oracle//'`.lua
opt="$opt --cmdprogram=$luascript"
ddltypeprg="simpleform.wnmp"
opt="$opt --program $ddltypeprg"			# normalization program for simpleform ddl types
modpath="../../src/modules/normalize/"			# module directory for normalizers relative from tests/temp
mod="$modpath/number/mod_normalize_number"		# module to load
opt="$opt --module $mod"
mod="$modpath/string/mod_normalize_string"		# module to load
opt="$opt --module $mod"
modpath="../../src/modules/cmdbind/directmap"		# module directory relative from tests/temp
opt="$opt --module $modpath/mod_command_directmap"
modpath="../wolfilter/modules/database"			# module directory relative from tests/temp
opt="$opt --module $modpath/oracle/mod_db_oracletest"
opt="$opt --database 'identifier=testdb,host=andreasbaumann.dyndns.org,port=1521,database=orcl,user=wolfusr,password=wolfpwd,dumpfile=DBDUMP,inputfile=DBDATA'"
opt="$opt --program=DBPRG.tdl"
testcmd="$opt run"					# command to execute by the test
testscripts="$luascript"				# list of scripts of the test
docin=$testname.in					# input document name
docout=$testname.out					# output document name
dumpout="program/$testname.dbdump.txt"			# resource dump to add to expected test output
testdata="
**file:$ddltypeprg
`cat program/$ddltypeprg`
**file: DBDATA
`cat program/first/tree_schema_oracle.sql`
**file:DBPRG.tdl
`cat program/first/tree_transactions_oracle.tdl`
**outputfile:DBDUMP"
csetlist="UTF-8"
. ./output_tst_all.sh