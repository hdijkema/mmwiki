<?php
# vim: ts=4 sw=4 sts=4 noet :

require_once('config.php');

$context = "";
$page = "";
$d_context = "";

if (isset($_GET['context'])) {
	$context = $_GET['context'];
	if ($context != "") { $d_context = "/" . $context; }
}

if (isset($_GET['page'])) {
	$page = $_GET['page'];
}

if ($page == "") { $page = "index"; }

$file = "$MMWIKI_PREFIX/mm$d_context/$page.mm";

if (is_file($file)) {
	$content = file_get_contents($file);
	echo $content;
} else {
	echo "";
}

?>
