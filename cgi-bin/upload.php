#!/usr/bin/php

<?php
$body = "";
$body .= "<!DOCTYPE html>\n";
$body .= "<html>\n";
$body .= "<head>\n";
$body .= "<title>UPLOAD</title>\n";
$body .= "<style>\n";
$body .= "html { color-scheme: light dark; }\n";
$body .= "body { width: 35em; margin: 0 auto;\n";
$body .= "font-family: Tahoma, Verdana, Arial, sans-serif; }\n";
$body .= "h1 { color:mediumslateblue; }\n";
$body .= "</style>\n";
$body .= "</head>\n";
$body .= "<body>\n";
$body .= "<h1><em>upload</em> CGI program</h1>\n";

$target_dir = "uploads/";
$target_file = $target_dir . basename($_FILES["fileToUpload"]["name"]);
$uploadOk = 1;
$imageFileType = strtolower(pathinfo($target_file,PATHINFO_EXTENSION));

//입력값 뭔지 체크 - submit가 있는지?
// Check if image file is a actual image or fake image
if(isset($_POST["submit"])) {
  $check = getimagesize($_FILES["fileToUpload"]["tmp_name"]);
  if($check !== false) {
    $body .= ("<p>File is an image - " . $check["mime"] . ".</p>\n");
    //echo "File is an image - " . $check["mime"] . ".";
    $uploadOk = 1;
  } else {
    $body .= "<p>File is not an image.</p>\n";
    //echo "File is not an image.";
    $uploadOk = 0;
  }
}

// Check if file already exists
if (file_exists($target_file)) {
  $body .= "<p>Sorry, file already exists.</p>\n";
  //echo "Sorry, file already exists.";
  $uploadOk = 0;
}

// Check file size
if ($_FILES["fileToUpload"]["size"] > 1500000) {
  $body .= "<p>Sorry, your file is too large.</p>\n";
	//echo "Sorry, your file is too large.";
  $uploadOk = 0;
}

// Allow certain file formats
if($imageFileType != "jpg" && $imageFileType != "png" && $imageFileType != "jpeg"
&& $imageFileType != "gif" ) {
  $body .= "<p>Sorry, only JPG, JPEG, PNG & GIF files are allowed.</p>\n";
  //echo "Sorry, only JPG, JPEG, PNG & GIF files are allowed.";
  $uploadOk = 0;
}

// Check if $uploadOk is set to 0 by an error
if ($uploadOk == 0) {
  $body .= "<p>Sorry, your file was not uploaded.</p>\n";
  //echo "Sorry, your file was not uploaded.";
  echo "Status: 500 Internal Server Error";
// if everything is ok, try to upload file
} else {
  if (move_uploaded_file($_FILES["fileToUpload"]["tmp_name"], $target_file)) {
    $body .= ("<p>The file ". htmlspecialchars( basename( $_FILES["fileToUpload"]["name"])). " has been uploaded.</p>\n");
    //echo "The file ". htmlspecialchars( basename( $_FILES["fileToUpload"]["name"])). " has been uploaded.";
    echo "Status: 200 Success\r\n";

  } else {
    $body .= "<p>Sorry, there was an error uploading your file.</p>\n";
    //echo "Sorry, there was an error uploading your file.";
    echo "Status: 500 Internal Server Error";
  }
}
$body .= "</body>\n";
$body .= "</html>\n";

echo "Content-type: text/html\r\n";
echo "Content-Length: ", strlen($body), "\r\n";
echo "\r\n";
echo $body;
?>
