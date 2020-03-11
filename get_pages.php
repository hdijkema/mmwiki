<?php
# vim: ts=4 sw=4 sts=4 noet :

require_once('config.php');

$context = "";
$d_context = "";

if (isset($_GET['context'])) {
	$context = $_GET['context'];
	if ($context != "") { $d_context = "/" . $context; }
}

error_log("d_context=$d_context");

$files = glob("$MMWIKI_PREFIX/mm$d_context/*.mm");
$response_text =  "";
$comma = "";
error_log(print_r($files, true));
foreach($files as $file) {
	$f = basename($file);
	$f = str_replace(".mm", "", $f);
	$response_text .= $comma;
	$response_text .= $f;
	$comma = ",";
}

error_log("$response_text");

echo $response_text;

?>
