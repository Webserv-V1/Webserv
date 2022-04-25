#!/opt/homebrew/bin/php
<?php
$body = "";
$body .= "<!DOCTYPE html>\n";
$body .= "<html>\n";
$body .= "<head>\n";
$body .= "<title>Outputs for <em>get_profile</em> CGI program</title>\n";
$body .= "</head>\n";
$body .= "<body>\n";

if (isset($_REQUEST['ID']))
{
	$body .= "<p>Your ID is ";
	$body .= $_REQUEST['ID'];
	$body .= "</p>\n";
}
else
	$body .= "<p>You didn't set the ID!</p>\n";
if (isset($_REQUEST['PW']))
{
	$body .= "<p>Your password is ";
	$body .= $_REQUEST['PW'];
	$body .= "</p>\n";
}
else
	$body .= "<p>You didn't set the password!</p>\n";

$body .= "</body>\n";
$body .= "</html>\n";

echo "Status: 200 Success\r\n";
echo "Content-type: text/html\r\n";
echo "Content-Length: ", strlen($body), "\r\n";
echo "\r\n";
echo $body;
?>
