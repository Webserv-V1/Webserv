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
if (!isset($_REQUEST["ID"]))
	$body .= "<h3>You didn't set the ID!</h3>\n";
else
{
	$body .= "<h3>Your 'ID' is ";
	$body .= $_REQUEST["ID"];
	$body .= "</h3>\n";
}
if (!isset($_REQUEST["PW"]))
	$body .= "<h3>You didn't set the PW!</h3>\n";
else
{
	$body .= "<h3>Your 'PW' is ";
	$body .= $_REQUEST["PW"];
	$body .= "</h3>\n";
}

$body .= "</body>\n";
$body .= "</html>\n";

$b_len = strlen($body);
header('Status: 200 Success\r\n');
header("Content-Length: $b_len\r\n");
print_r($body);
?>
