#!/usr/bin/php
<?php
$body = "";
$body .= "<!DOCTYPE html>\n";
$body .= "<html>\n";
$body .= "<head>\n";
$body .= "<title>GET_PROFILE</title>\n";
$body .= "<style>\n";
$body .= "html { color-scheme: light dark; }\n";
$body .= "body { width: 35em; margin: 0 auto;\n";
$body .= "font-family: Tahoma, Verdana, Arial, sans-serif; }\n";
$body .= "h1 { color:darksalmon; }\n";
$body .= "</style>\n";
$body .= "</head>\n";
$body .= "<body>\n";

$body .= "<h1><em>get_profile</em> CGI program</h1>\n";
if (strcmp($_SERVER["REQUEST_METHOD"], "GET") == 0)
	$data = $_SERVER["QUERY_STRING"];
else
{
	fseek(STDIN, 0, SEEK_SET);
	$data = fread(STDIN, $_SERVER["CONTENT_LENGTH"]);
}
parse_str($data, $res);
if (!isset($res["ID"]) or strlen($res["ID"]) == 0)
	$body .= "<h3>You didn't set the ID!</h3>\n";
else
{
	$body .= "<h3>Your 'ID' is ";
	$body .= $res["ID"];
	$body .= "</h3>\n";
}
if (!isset($res["PW"]) or strlen($res["PW"]) == 0)
	$body .= "<h3>You didn't set the PW!</h3>\n";
else
{
	$body .= "<h3>Your 'PW' is ";
	$body .= $res["PW"];
	$body .= "</h3>\n";
}

$body .= "</body>\n";
$body .= "</html>\n";

echo "Status: 200 Success\r\n";
echo "Content-type: text/html\r\n";
echo "Content-Length: ", strlen($body), "\r\n";
echo "\r\n";
echo $body;
?>
