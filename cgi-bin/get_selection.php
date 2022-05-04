<?php
$body = "";
$body .= "<!DOCTYPE html>\n";
$body .= "<html>\n";
$body .= "<head>\n";
$body .= "<title>GET_SELECTION</title>\n";
$body .= "<style>\n";
$body .= "html { color-scheme: light dark; }\n";
$body .= "body { width: 35em; margin: 0 auto;\n";
$body .= "font-family: Tahoma, Verdana, Arial, sans-serif; }\n";
$body .= "h1 { color:yellowgreen; }\n";
$body .= "</style>\n";
$body .= "</head>\n";
$body .= "<body>\n";

$body .= "<h1><em>get_selection</em> CGI program</h1>\n";
$body .= "<h2>[Result for 'would you rather' question]</h2>\n";
if (!isset($_REQUEST["first"]))
	$body .= "<h3>You didn't answer the first question.</h3>\n";
else if (strcmp($_REQUEST["first"], "1-1") == 0)
	$body .= "<h3>You selected 'Have more time' in first question!</h3>\n";
else
	$body .= "<h3>You selected 'Have more money' in first question!</h3>\n";
if (!isset($_REQUEST["second"]))
	$body .= "<h3>You didn't answer the second question.</h3>\n";
else if (strcmp($_REQUEST["second"], "2-1") == 0)
	$body .= "<h3>You selected 'Have many good friends' in second question!</h3>\n";
else
	$body .= "<h3>You selected 'Have one very best friend' in second question!</h3>\n";
if (!isset($_REQUEST["third"]))
	$body .= "<h3>You didn't answer the third question.</h3>\n";
else if (strcmp($_REQUEST["third"], "3-1") == 0)
	$body .= "<h3>You selected 'Hear the good news first' in third question!</h3>\n";
else
	$body .= "<h3>You selected 'Hear the bad news first' in third question!</h3>\n";
$body .= "<hr>\n";

$body .= "<h2>[Color of your choice]</h2>\n";
$color = "";
if (isset($_REQUEST['red']))
	$color .= "red ";
if (isset($_REQUEST['orange']))
	$color .= "orange ";
if (isset($_REQUEST['yellow']))
	$color .= "yellow ";
if (isset($_REQUEST['green']))
	$color .= "green ";
if (isset($_REQUEST['blue']))
	$color .= "blue ";
if (isset($_REQUEST['purple']))
	$color .= "purple ";
if (strlen($color) == 0)
	$body .= "<h3>You didn't select any color</h3>\n";
else
	$body .= ("<h3>" . $color . "</h3>\n");

$body .= "<hr>\n";
if (!isset($_REQUEST['dropdown']))
	$body .= "<h3>You didn't give a rating</h3>\n";
else
{
	$body .= "<h3>You gave me ";
	$body .= $_REQUEST['dropdown'];
	$body .= " stars</h3>\n";
}

$body .= "</body>\n";
$body .= "</html>\n";

$b_len = strlen($body);
header('Status: 200 Success\r\n');
header("Content-Length: $b_len\r\n");
print_r($body);
?>
