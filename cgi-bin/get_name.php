#!/opt/homebrew/bin/php
<?php
$body = "";
$body .= "<!DOCTYPE html>\n";
$body .= "<html>\n";
$body .= "<head>\n";
$body .= "<title>Outputs for <em>get_name</em> CGI program</title>\n";
$body .= "</head>\n";
$body .= "<body>\n";

if (isset($_REQUEST['first_name']))
{
	$body .= "<p>Your first name is ";
	$body .= $_REQUEST['first_name'];
	$body .= "</p>\n";
}
else
	$body .= "<p>You didn't set the first_name!</p>\n";
if (isset($_REQUEST['last_name']))
{
	$body .= "<p>Your last name is ";
	$body .= $_REQUEST['last_name'];
	$body .= "</p>\n";
}
else
	$body .= "<p>You didn't set the last_name!</p>\n";

$body .= "</body>\n";
$body .= "</html>\n";

echo "Status: 200 Success\r\n";
echo "Content-type: text/html\r\n";
echo "Content-Length: ", strlen($body), "\r\n";
echo "\r\n";
echo $body;
?>
