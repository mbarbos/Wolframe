**
**input
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE assignmentlist PUBLIC "-//ABC//DTD XML 1.0 plus Software 2.0 plus ABC 1.1//EN" "http://www.wolframe.org/example-doc/employee_assignement_print.dtd">
<assignmentlist><assignment><task><title>job 1</title><key>A123</key><customernumber>324</customernumber></task><task><title>job 2</title><key>V456</key><customernumber>567</customernumber></task><employee><firstname>Julia</firstname><surname>Tegel-Sacher</surname><phone>098 765 43 21</phone></employee><issuedate>13.5.2006</issuedate></assignment><assignment><task><title>job 3</title><key>A456</key><customernumber>567</customernumber></task><task><title>job 4</title><key>V789</key><customernumber>890</customernumber></task><employee><firstname>Jakob</firstname><surname>Stegelin</surname><phone>012 345 67 89</phone></employee><issuedate>13.5.2006</issuedate></assignment></assignmentlist>**config
--script echo_input_doctype.lua run xml:textwolf
**file: echo_input_doctype.lua
function run()
	f = filter( "xml:textwolf")

	-- a W3C conform XML parser returns by default also tokens containing only spaces.
	-- these we eliminate to be able to create a table:
	f.empty = false

	input:as( f)
	type = input:doctype()
	logger.print( "INFO", "DOCTYPE ", type)
	output:as( f, 'assignmentlist')
	output:as( f, 'assignmentlist "http://www.wolframe.org/example-doc/employee_assignement_print.dtd"')
	output:as( f, 'assignmentlist "-//ABC//DTD XML 1.0 plus Software 2.0 plus ABC 1.1//EN" "http://www.wolframe.org/example-doc/employee_assignement_print.dtd"')
	output:as( f, type)
	output:print( input:get())
end
**output
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE assignmentlist PUBLIC "-//ABC//DTD XML 1.0 plus Software 2.0 plus ABC 1.1//EN" "http://www.wolframe.org/example-doc/employee_assignement_print.dtd">
<assignmentlist><assignment><task><title>job 1</title><key>A123</key><customernumber>324</customernumber></task><task><title>job 2</title><key>V456</key><customernumber>567</customernumber></task><employee><firstname>Julia</firstname><surname>Tegel-Sacher</surname><phone>098 765 43 21</phone></employee><issuedate>13.5.2006</issuedate></assignment><assignment><task><title>job 3</title><key>A456</key><customernumber>567</customernumber></task><task><title>job 4</title><key>V789</key><customernumber>890</customernumber></task><employee><firstname>Jakob</firstname><surname>Stegelin</surname><phone>012 345 67 89</phone></employee><issuedate>13.5.2006</issuedate></assignment></assignmentlist>
**end
