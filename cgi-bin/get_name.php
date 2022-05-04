<?php
$body = "";
$body .= "<!DOCTYPE html>\n";
$body .= "<html>\n";
$body .= "<head>\n";
$body .= "<title>GET_NAME</title>\n";
$body .= "<style>\n";
$body .= "html { color-scheme: light dark; }\n";
$body .= "body { width: 35em; margin: 0 auto;\n";
$body .= "font-family: Tahoma, Verdana, Arial, sans-serif; }\n";
$body .= "h1 { color:darksalmon; }\n";
$body .= "</style>\n";
$body .= "</head>\n";
$body .= "<body>\n";

$body .= "<h1><em>get_name</em> CGI program</h1>\n";

if (!isset($_REQUEST["first_name"]))
	$body .= "<h3>You didn't set the first_name!</h3>\n";
else
{
	$body .= "<h3>Your 'first name' is ";
	$body .= $_REQUEST["first_name"];
	$body .= "</h3>\n";
}
if (!isset($_REQUEST["last_name"]))
	$body .= "<h3>You didn't set the last_name!</h3>\n";
else
{
	$body .= "<h3>Your 'last name' is ";
	$body .= $_REQUEST["last_name"];
	$body .= "</h3>\n";
}

$body .= "</body>\n";
$body .= "</html>\n";

$b_len = strlen($body);
header('Status: 200 Success\r\n');
header("Content-Length: $b_len\r\n");
print_r($body);
?>
