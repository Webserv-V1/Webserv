#!/opt/homebrew/bin/php
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
if (strcmp($_SERVER["REQUEST_METHOD"], "GET") == 0)
	$data = $_SERVER["QUERY_STRING"];
else
{
	fseek(STDIN, 0, SEEK_SET);
	$data = fread(STDIN, $_SERVER["CONTENT_LENGTH"]);
}
parse_str($data, $res);
if (!isset($res['textcontent']) or strlen($res['textcontent']) == 0)
	$body .= "<h3>Please leave a comment next time!</h3>\n";
else
{
	$body .= "<h2>You left the following comments:</h2>\n";
	$body .= ("<h3>" . $res['textcontent'] . "</h3>\n");
}

$body .= "</body>\n";
$body .= "</html>\n";

echo "Status: 200 Success\r\n";
echo "Content-type: text/html\r\n";
echo "Content-Length: ", strlen($body), "\r\n";
echo "\r\n";
echo $body;
?>
