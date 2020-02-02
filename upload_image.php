<?php
require_once('config.php');

if (loggedIn()) {
	$ok = true;
	$page = "";
	$context = "";
	$name = "";
	
	if (!isset($_GET['context'])) { $ok = false; error_log("no context"); }
	else { $context = trim($_GET['context']); }
	
	if (!isset($_GET['page'])) { $ok = false; error_log("no page"); }
	else { $page = trim($_GET['page']); }
	
	if (!isset($_GET['name'])) { $ok = false; error_log("no name"); }
	else { $name = trim($_GET['name']); }
	
	if (!isset($_FILES['image'])) { $ok = false; error_log("no image"); }
	
	if ($page == "" || $name == "") { $ok = false; error_log("empty page or name"); }
	
	if ($ok) {
		$d_context = "";
		if ($context != "") { $d_context = "/" . $context; }
		$dir = $MMWIKI_PREFIX . "/mm" . $d_context;
		$file = $dir . "/" . $name;
		
		$uploaded = $_FILES['image'];
		$name = $uploaded['tmp_name'];
		$check = filesize($uploaded['tmp_name']);
		if (!$check) {
			error_log("no getimagesize = $check"); 
			http_response_code(400);
		} else {
			if (move_uploaded_file($uploaded['tmp_name'], $file)) {
				// do nothing
			} else {
				error_log("cannot move image to $file"); 
				http_response_code(400);
			}
		}
	} else {
		http_response_code(400);
	}
}

?>
