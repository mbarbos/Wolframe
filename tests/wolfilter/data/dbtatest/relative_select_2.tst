**
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<baum>hier ist ein baum text</baum><haus>hier ist ein haus text</haus>
**config
--module ../../src/modules/filter/textwolf/mod_filter_textwolf
--module ../wolfilter/modules/database/testtrace/mod_db_testtrace
--module ../../src/modules/transaction/preparedStatement/mod_transaction_pstm
--input-filter 'xml:textwolf'
--database 'id=testdb,outfile=DBOUT'
--transaction 'testcall dbpstm run(: haus, baum)'
testcall
**outputfile:DBOUT
**output
run #hier ist ein haus text#hier ist ein baum text

start( 'run' );
bind( 1, 'hier ist ein haus text' );
bind( 2, 'hier ist ein baum text' );
execute();
nofColumns(); returns 0
**end
