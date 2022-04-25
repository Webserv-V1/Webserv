#!/opt/homebrew/bin/php
<?php
$body = "";
$body .= "<!DOCTYPE html>\n";
$body .= "<html>\n";
$body .= "<head>\n";
$body .= "<title>Outputs for get_selection CGI program</title>\n";
$body .= "</head>\n";
$body .= "<body>\n";

$body .= "<h1>Outputs for <em>get_selection</em> CGI program</h1>\n";
/*$body .= "<p>You selected '";
$body .= $_REQUEST['first'];
$body .= "' in first question!</p>\n";
$body .= "<p>You selected '";
$body .= $_REQUEST['second'];
$body .= "' in second question!</p>\n";
$body .= "<p>You selected '";
$body .= $_REQUEST['third'];
$body .= "' in third question!</p>\n";

$body .= "<p>Color of your choice - ";
if (isset($_REQUEST['red']))
	$body .= "red ";
if (isset($_REQUEST['orange']))
	$body .= "orange ";
if (isset($_REQUEST['yellow']))
	$body .= "yellow ";
if (isset($_REQUEST['green']))
	$body .= "green ";
if (isset($_REQUEST['blue']))
	$body .= "blue ";
if (isset($_REQUEST['purple']))
	$body .= "purple ";
$body .= "</p>\n";

$body .= "<p>You gave me ";
$body .= $_REQUEST['dropdown'];
$body .= "stars</p>\n";*/

$body .= "</body>\n";
$body .= "</html>\n";

echo "Status: 200 Success\r\n";
echo "Content-type: text/html\r\n";
echo "Content-Length: ", strlen($body), "\r\n";
echo "\r\n";
echo $body;
?>
