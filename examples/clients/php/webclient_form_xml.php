<?php
header('Content-Type: text/xml');
require 'session.php';
use Wolframe\Session as Session;
/*
 Example that shows that HTTP/HTTPS requests can be mapped to calls of a Wolframe service.
 The emphasis is here more on 'that' rather than 'how' because security issues are only
 handled here when it is about the protocol between PhP and Wolframe. So do not take this
 script without caring about all aspects we did not show here, because we wanted to keep
 the example simple.

 The script forwards HTTP GET request as XML to a wolframe server via TCP/IP plain 
 or SSL and renders the result as XML.
 The HTTP REQUEST parameters without some reserved ones are transformed into 
 an XML request set sent to the server. The command is specified with the parameter
 'CMD' (command prefix) and 'DOCTYPE' (document type of the created XML). The 
 parameter 'CMD' is optional. 
 The authentication settings and wheter to use SSL or not are also selected by 
 request parameters. The SSL settings are minimal and must be revised for the use in a 
 productive system.
*/
try
{
	$cmd = NULL;			/* _REQUEST['CMD'] -> command to execute */
	$content = array();		/* request parameters without 'CMD' */
	$doctype = NULL;
	$root = NULL;
	$ssl_enabled = false;		/* SSL yes (true) / no (false) */
	$username = NULL;		/* Username for authorization with WOLFRAME-CRAM */
	$password = NULL;		/* Password for authorization with WOLFRAME-CRAM */

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

	$body = '<?xml version="1.0" encoding="UTF-8" standalone="no"?>' . "\n";
	$body .= '<!DOCTYPE ' . $root . " SYSTEM '$doctype.sfrm'>\n";
	$body .= "<$root>";

	foreach ($_REQUEST as $key => $value)
	{
		if ($key == "CMD")
		{
			$cmd = $value;
		}
		else if ($key == "SSL" && 0==strcmp($value,"yes"))
		{
			$ssl_enabled = true;
		}
		else if ($key == "DOCTYPE")
		{
		}
		else if ($key == "USERNAME")
		{
			$username = $value;
		}
		else if ($key == "PASSWORD")
		{
			$password = $value;
		}
		else
		{
			$body .= "<$key>" . $value . "</$key>";
		}
	}
	$body .= "</$root>\n";

	if ($ssl_enabled == true)
	{
		$sslpath = "./SSL";
		$sslopt = array(
			"local_cert" => "$sslpath/combinedcert.pem",
			"verify_peer" => false
		);
	}
	if ($password != NULL && $username != NULL)
	{
		$authopt = array(
			"mech" => "WOLFRAME-CRAM",
			"username" => $username,
			"password" => $password
		);
	}
	$port = 7661;
	if ($ssl_enabled)
	{
		$port = 7961;
	}
	$conn = new Session( "127.0.0.1", $port, $sslopt, $authopt);
	if (($result = $conn->request( $cmd, $body)) === FALSE)
	{
		echo "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>";
		echo "<error><class>ANSWER</class><message>" . $conn->lasterror() . "</message></error>";
	}
	else
	{
		echo $result;
	}
	unset( $conn);
}
catch ( \Exception $e)
{
	echo "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
	echo "<error><class>EXCEPTION</class><message>" . $e->getMessage() . "</message></error>";
}
?>


