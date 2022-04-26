#!/opt/homebrew/bin/php
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
$body .= "</style>\n";
$body .= "</head>\n";
$body .= "<body>\n";

$body .= "<h1><em>get_name</em> CGI program</h1>\n";
if (strcmp($_SERVER["REQUEST_METHOD"], "GET") == 0)
	$data = $_SERVER["QUERY_STRING"];
else
{
	fseek(STDIN, 0, SEEK_SET);
	$data = fread(STDIN, $_SERVER["CONTENT_LENGTH"]);
}
parse_str($data, $res);
if (strlen($res["first_name"]) == 0)
	$body .= "<h3>You didn't set the first_name!</h3>\n";
else
{
	$body .= "<h3>Your 'first name' is ";
	$body .= $res["first_name"];
	$body .= "</h3>\n";
}
if (strlen($res["last_name"]) == 0)
	$body .= "<h3>You didn't set the last_name!</h3>\n";
else
{
	$body .= "<h3>Your 'last name' is ";
	$body .= $res["last_name"];
	$body .= "</h3>\n";
}

$body .= "</body>\n";
$body .= "</html>\n";

echo "Status: 200 Success\r\n";
echo "Content-type: text/html\r\n";
echo "Content-Length: ", strlen($body), "\r\n";
echo "\r\n";
echo $body;
?>
