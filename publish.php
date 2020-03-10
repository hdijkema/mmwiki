<?php
require_once('config.php');

if (loggedIn()) {
	header("Content-Type: application/json");
	$data = file_get_contents("php://input");
	$v = json_decode($data);

	$page = $v->page;
	$context = $v->context;
	$content = $v->content;
	$languages = $v->languages;
	$book = $v->book;

	$styles = "";
	foreach($languages as $langs) {
		$id = "lang";
		foreach($langs as $lang) {
			$id .= "_";
			$id .= $lang;
		}
		$styles .= "#$id { display: none; }\n";
	}

	$default_lang = "";
	$js = "function selectLanguage() {\n";
	$js .= "var selected = false;\n";
	$js .= "var lang = navigator.language || navigator.userLanguage;\n";
	foreach($languages as $langs) {
		$id = "lang";
		foreach($langs as $lang) {
			$id .= "_";
			$id .= $lang;
		}
		foreach($langs as $lang) {
			if ($default_lang == "") { $default_lang = $id; }
			$js .= "if (lang == '$lang') { selected=true;document.getElementById('$id').style.display = 'block'; }\n";
		}
	}
	$js .= "if (!selected) { document.getElementById('$default_lang').style.display = 'block'; }\n";
	$js .= "}\n";
	

	$d_context = "";
	if ($context != "" && $context != "_") { $d_context = "/" . $context; }
	
	$dir = "$MMWIKI_PREFIX/html$d_context";
	if (!is_dir($dir)) { mkdir($dir); }

	$prefix_file = "$MMWIKI_PREFIX/html.pre";
	$prefix_html = file_get_contents($prefix_file);

	$prefix_html = str_replace("@@title@@", $book, $prefix_html);
	$prefix_html = str_replace("@@langstyles@@", $styles, $prefix_html);
	$prefix_html = str_replace("@@langscript@@", $js, $prefix_html);

	$postfix_file = "$MMWIKI_PREFIX/html.post";
	$postfix_html = file_get_contents($postfix_file);

	$file = "$MMWIKI_PREFIX/html$d_context/$page.html";
	$fh = fopen($file, "w");

	fputs($fh, $prefix_html);
	fputs($fh, $content);
	fputs($fh, $postfix_html);

	fclose($fh);
	error_log("Written: $file");
} else {
	error_log("not logged in");
}


?>
