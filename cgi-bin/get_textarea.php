#!/opt/homebrew/bin/php
<?php
$body = "";
$body .= "<!DOCTYPE html>\n";
$body .= "<html>\n";
$body .= "<head>\n";
$body .= "<title>Outputs for <em>get_profile</em> CGI program</title>\n";
$body .= "</head>\n";
$body .= "<body>\n";

if (isset($_REQUEST['textcontent']))
{
	$body .= "<p>You left the following comments:<br/>";
	$body .= $_REQUEST['textcontent'];
	$body .= "</p>\n";
}
else
	$body .= "<p>Please leave a comment next time!</p>\n";

$body .= "</body>\n";
$body .= "</html>\n";

echo "Status: 200 Success\r\n";
echo "Content-type: text/html\r\n";
echo "Content-Length: ", strlen($body), "\r\n";
echo "\r\n";
echo $body;
?>
