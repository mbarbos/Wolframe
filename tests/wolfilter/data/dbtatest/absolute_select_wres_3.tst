**
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<aa>1</aa><cc>3</cc><bb>2</bb>
**config
--module ../../src/modules/filter/textwolf/mod_filter_textwolf
--module ../wolfilter/modules/database/testtrace/mod_db_testtrace
--module ../../src/modules/transaction/preparedStatement/mod_transaction_pstm
--input-filter 'xml:textwolf'
--database 'id=testdb,outfile=DBOUT,file=DBRES'
--transaction 'testcall dbpstm doc/address=run(: /aa, /bb, /cc)'
testcall
**file: DBRES
#id name street#1 hugo "bahnhofstrasse 15"#2 miriam "zum gemsweg 3"#3 sara "tannensteig 12"
**outputfile:DBOUT
**output
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<doc><address><id>1</id><name>hugo</name><street>bahnhofstrasse 15</street></address><address><id>2</id><name>miriam</name><street>zum gemsweg 3</street></address><address><id>3</id><name>sara</name><street>tannensteig 12</street></address></doc>
begin();
start( 'run' );
bind( 1, '1' );
bind( 2, '2' );
bind( 3, '3' );
execute();
nofColumns(); returns 3
get( 1 ); returns 1
get( 2 ); returns hugo
get( 3 ); returns bahnhofstrasse 15
next(); returns 1
get( 1 ); returns 2
get( 2 ); returns miriam
get( 3 ); returns zum gemsweg 3
next(); returns 1
get( 1 ); returns 3
get( 2 ); returns sara
get( 3 ); returns tannensteig 12
next(); returns 0
getLastError(); returns 0
nofColumns(); returns 3
columnName( 1 ); returns id
columnName( 2 ); returns name
columnName( 3 ); returns street
commit();
**end