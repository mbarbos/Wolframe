**
**requires:CJSON
**requires:TEXTWOLF
**requires:PGSQL
**input
{ 
         " - d o c t y p e " :   " P e r s o n " , 
         " n a m e " :   " H u g i   T u r b o " , 
         " a d d r e s s " :   " G u r k e n s t r a s s e   7 a   3 1 4 5   G u m l i g e n " 
 } **config
--input-filter cjson --output-filter cjson --module ../../src/modules/filter/cjson/mod_filter_cjson --module ../../src/modules/doctype/json/mod_doctype_json -c wolframe.conf InsertPerson

**file:wolframe.conf
LoadModules
{
	module ./../wolfilter/modules/database/postgresql/mod_db_postgresqltest
	module ./../../src/modules/cmdbind/directmap/mod_command_directmap
	module ./../../src/modules/cmdbind/aamap/mod_command_aamap
	module ./../../src/modules/cmdbind/tdl/mod_command_tdl
	module ./../wolfilter/modules/functions/audit/mod_audit
}
Database
{
	PostgreSQLTest
	{
		identifier testdb
		host localhost
		port 5432
		database wolframe
		user wolfusr
		password wolfpwd
		dumpfile DBDUMP
		inputfile DBDATA
	}
}
Processor
{
	database testdb
	program audit.tdl
	program audit.aamap

	cmdhandler
	{
		directmap
		{
			program audit.dmap
			filter cjson
		}
	}
}
**file:DBDATA
CREATE TABLE Person
(
ID SERIAL NOT NULL PRIMARY KEY,
 name TEXT,
 address TEXT
);

INSERT INTO Person (name,address) VALUES ('Aufru','Amselstrasse 12 Aulach');
INSERT INTO Person (name,address) VALUES ('Beno','Butterweg 23 Bendorf');
INSERT INTO Person (name,address) VALUES ('Carla','Camelstreet 34 Carassa');
INSERT INTO Person (name,address) VALUES ('Dorothe','Demotastrasse 45 Durnfo');
INSERT INTO Person (name,address) VALUES ('Erik','Erakimolstrasse 56 Enden');
**file:audit.dmap
COMMAND (Insert Person) SKIP CALL insertPerson;
**file:audit.aamap

AUDIT auditMutation1 auditMutation1( user=username );
**file:audit.tdl

TRANSACTION insertPerson
BEGIN
	DO INSERT INTO Person (name, address) VALUES ($(name), $(address));
	DO UNIQUE SELECT id from Person WHERE name = $(name);
END
AUDIT CRITICAL auditMutation1( operation="insert", $RESULT.id, newvalue=$(name))
**outputfile:DBDUMP
**outputfile:audit.log
**output
person:
id, name, address
'1', 'Aufru', 'Amselstrasse 12 Aulach'
'2', 'Beno', 'Butterweg 23 Bendorf'
'3', 'Carla', 'Camelstreet 34 Carassa'
'4', 'Dorothe', 'Demotastrasse 45 Durnfo'
'5', 'Erik', 'Erakimolstrasse 56 Enden'
'6', 'Hugi Turbo', 'Gurkenstrasse 7a 3145 Gumligen'
audit insert user 'wolfilter' id 6 old '' new 'Hugi Turbo'
**end
