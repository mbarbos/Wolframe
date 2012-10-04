**
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<aa>1</aa><cc>3</cc><bb>2</bb>
**config
--module ../../src/modules/filter/textwolf/mod_filter_textwolf
--module ../wolfilter/modules/database/testtrace/mod_db_testtrace
--module ../../src/modules/transaction/databaseTransaction/mod_transaction_database
--input-filter 'xml:textwolf'
--database 'id=testdb,outfile=DBOUT'
--transaction 'testcall dbpstm run(: /aa, /bb, /cc)'
testcall
**outputfile:DBOUT
**output
run #1#2#3

start( 'run' );
bind( 1, '1' );
bind( 2, '2' );
bind( 3, '3' );
execute();
nofColumns(); returns 0
**end
