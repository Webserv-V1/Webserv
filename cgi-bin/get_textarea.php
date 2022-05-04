<?php
$body = "";
$body .= "<!DOCTYPE html>\n";
$body .= "<html>\n";
$body .= "<head>\n";
$body .= "<title>GET_TEXTAREA</title>\n";
$body .= "<style>\n";
$body .= "html { color-scheme: light dark; }\n";
$body .= "body { width: 35em; margin: 0 auto;\n";
$body .= "font-family: Tahoma, Verdana, Arial, sans-serif; }\n";
$body .= "h1 { color:yellowgreen; }\n";
$body .= "</style>\n";
$body .= "</head>\n";
$body .= "<body>\n";

$body .= "<h1><em>get_textarea</em> CGI program</h1>\n";
if (!isset($_REQUEST['textcontent']) or strlen($_REQUEST['textcontent']) == 0)
	$body .= "<h3>Please leave a comment next time!</h3>\n";
else
{
	$body .= "<h2>You left the following comments:</h2>\n";
	$body .= ("<h3>" . $_REQUEST['textcontent'] . "</h3>\n");
}

$body .= "</body>\n";
$body .= "</html>\n";

$b_len = strlen($body);
header('Status: 200 Success\r\n');
header("Content-Length: $b_len\r\n");
print_r($body);
?>
