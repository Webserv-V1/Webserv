#!/opt/homebrew/bin/php
<?php
echo "Status: 200 OK\r\n";
echo "Content-type:text/html\r\n\r\n";
echo $_SERVER['QUERY_STRING'];
echo "\n";
echo $_SERVER['REQUEST_METHOD'];
?>