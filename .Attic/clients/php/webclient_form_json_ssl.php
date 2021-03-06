<?php
require 'session.php';
use Wolframe\Session as Session;

/*
 Same as webclient.php but using SSL to communicate with the Wolframe server.
 Forwards HTTP GET request as JSON to a wolframe server via TCP/IP with SSL and returns the result
 as the HTTP request answer. The HTTP REQUEST parameters without 'CMD' are transformed into 
 a JSON request set sent to the server. The command prefix is specified with the parameter
 'CMD'. If the parameter 'CMD' is not specified then the document type only determines what is executed
 on the server. The document type of the content is defined in JSON as an attribute named 'doctype'
 and in XML as stem of the DOCYPE SYSTEM attribute or as stem of the xmlns:xsi root element
 attribute.
*/

try
{
	$sslpath = "./SSL";
	$sslopt = array(
		"local_cert" => "$sslpath/combinedcert.pem",
		"verify_peer" => false
	);

	$cmd = NULL;			/* _REQUEST['CMD'] -> command to execute */
	$content = array();		/* request parameters without 'CMD' */
	$doctype = NULL;
	$root = NULL;

	foreach ($_REQUEST as $key => $value)
	{
		if ($key == "DOCTYPE")
		{
			list($doctype, $root) = preg_split( "/[\s,;:]+/", $value, 2);
			if ($root == NULL)
			{
				$root = $doctype;
			}
		}
	}

	foreach ($_REQUEST as $key => $value)
	{
		if ($key == "CMD")
		{
			$cmd = $value;
		}
		else if ($key == "DOCTYPE")
		{
		}
		else
		{
			$content[ $key] = $value;
		}
	}
	$authopt = array(
		"mech" => "NONE",
		"username" => "gunibert",
		"password" => "bork123"
	);

	$doc = array();
	$doc[ '-doctype'] = $doctype;
	$doc[ $root] = $content;

	$body = json_encode( $doc);

	$conn = new Session( "127.0.0.1", 7962, $sslopt, $authopt);
	if (($result = $conn->request( $cmd, $body)) === FALSE)
	{
		echo "<html><head><title>FAILED</title></head><body>" . $conn->lasterror() . "</body></html>";
	}
	else
	{
		echo "<html><head><title>RESULT</title></head><body><p>" . $result . "</p></body></html>";
	}
	unset( $conn);
}
catch ( \Exception $e)
{
	echo "<html><head><title>ERROR</title></head><body>" . $e->getMessage() . "</body></html>";
}
?>


