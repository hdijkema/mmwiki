<?php
# vim: ts=4 sw=4 sts=4 noet :

$contents = file_get_contents("mmwiki.css");
$contents = preg_replace('/\\\\/', "\\\\\\\\", $contents);
$contents = preg_replace('/"/', "\\\"", $contents);
$contents = preg_replace('/^/m', "\t\"", $contents);
$contents = preg_replace('/$/m', "\" \\", $contents);
$contents = preg_replace('/\\\\\\s*["]\\s*[\\\\]\\s*$/', "", $contents);

$contents = "#ifndef MMWIKI_CSS_H\n" . 
			"#define MMWIKI_CSS_H\n" . 
			"/*\n   vim: ts=4 sw=4 sts=4 noet :\n*/\n\n" .
			"#define MMWIKI_CSS \\\n" . $contents;
$contents = $contents . "\n\n#endif\n";

$fh = fopen("mmwiki_css.h", "w");
fputs($fh, $contents);
fclose($fh);

?>
