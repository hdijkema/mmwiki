<?php
require_once('config.php');

if (loggedIn()) {
	header("Content-Type: application/json");
	$data = file_get_contents("php://input");
	$v = json_decode($data);

	$page = $v->page;
	$context = $v->context;
	$content = $v->content;

	$d_context = "";
	if ($context != "" && $context != "_") { $d_context = "/" . $context; }
	
	$dir = "$MMWIKI_PREFIX/mm$d_context";
	if (!is_dir($dir)) { mkdir($dir); }

	$file = "$MMWIKI_PREFIX/mm$d_context/$page.mm";
	$fh = fopen($file, "w");
	fputs($fh, $content);
	fclose($fh);
	error_log("Written: $file");
} else {
	error_log("not logged in");
}


?>
