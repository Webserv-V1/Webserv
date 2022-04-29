#!/usr/bin/php
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
if (strcmp($_SERVER["REQUEST_METHOD"], "GET") == 0)
	$data = $_SERVER["QUERY_STRING"];
else
{
	fseek(STDIN, 0, SEEK_SET);
	$data = fread(STDIN, $_SERVER["CONTENT_LENGTH"]);
}
parse_str($data, $res);
$body .= "<h2>[Result for 'would you rather' question]</h2>\n";
if (!isset($res["first"]))
	$body .= "<h3>You didn't answer the first question.</h3>\n";
else if (strcmp($res["first"], "1-1") == 0)
	$body .= "<h3>You selected 'Have more time' in first question!</h3>\n";
else
	$body .= "<h3>You selected 'Have more money' in first question!</h3>\n";
if (!isset($res["second"]))
	$body .= "<h3>You didn't answer the second question.</h3>\n";
else if (strcmp($res["second"], "2-1") == 0)
	$body .= "<h3>You selected 'Have many good friends' in second question!</h3>\n";
else
	$body .= "<h3>You selected 'Have one very best friend' in second question!</h3>\n";
if (!isset($res["third"]))
	$body .= "<h3>You didn't answer the third question.</h3>\n";
else if (strcmp($res["third"], "3-1") == 0)
	$body .= "<h3>You selected 'Hear the good news first' in third question!</h3>\n";
else
	$body .= "<h3>You selected 'Hear the bad news first' in third question!</h3>\n";
$body .= "<hr>\n";

$body .= "<h2>[Color of your choice]</h2>\n";
$color = "";
if (isset($res['red']))
	$color .= "red ";
if (isset($res['orange']))
	$color .= "orange ";
if (isset($res['yellow']))
	$color .= "yellow ";
if (isset($res['green']))
	$color .= "green ";
if (isset($res['blue']))
	$color .= "blue ";
if (isset($res['purple']))
	$color .= "purple ";
if (strlen($color) == 0)
	$body .= "<h3>You didn't select any color</h3>\n";
else
	$body .= ("<h3>" . $color . "</h3>\n");

$body .= "<hr>\n";
if (!isset($res['dropdown']))
	$body .= "<h3>You didn't give a rating</h3>\n";
else
{
	$body .= "<h3>You gave me ";
	$body .= $res['dropdown'];
	$body .= " stars</h3>\n";
}

$body .= "</body>\n";
$body .= "</html>\n";

echo "Status: 200 Success\r\n";
echo "Content-type: text/html\r\n";
echo "Content-Length: ", strlen($body), "\r\n";
echo "\r\n";
echo $body;
?>
