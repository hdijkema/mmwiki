<!DOCTYPE html>
<html>
  <head>
    <meta charset="utf-8"></meta>
	<link rel="stylesheet" href="mmwiki.css"></link>
	<script src="mmwiki.js"></script>
	<script src="mmwiki_test.js"></script>
	<title>Materia Medica Wiki</title>
  </head>
  <body>
  <div id="mmwiki"></div>
<?php
	$page = "index";
	$context = "";
	if (isset($_GET['page'])) { $page = $_GET['page']; }
	if (isset($_GET['context'])) { $context = $_GET['context']; }

	$d_context = $context;
	if ($d_context != "") { $d_context = "/" . $d_context; }
	
	$prefix = "c:/xampp/htdocs/mmwiki";
	$contents = file_get_contents("$prefix/mm$d_context/$page.mm");
	$contents = str_replace("\n", "\\n", $contents);
	$contents = str_replace("\r", "", $contents);
	$contents = str_replace("'", "\\'", $contents);
	
	echo "<script>\n";
	echo "var txt = '".$contents."';\n";
	echo "var m = new MMWiki();\n";
	echo "var lp = new MMLinkProvider();\n";
	echo "var rp = new MMRemedyProvider();\n";
	echo "lp.setContext('$context');\n";
	echo "rp.setContext('$context');\n";
	echo "m.setLinkProvider(lp);\n";
	echo "m.setRemedyProvider(rp);\n";
	echo "var contents = m.toHtml(txt);\n";
	echo "if (contents == \"\") { contents = 'No content'; }\n";
	echo "document.getElementById('mmwiki').innerHTML = contents;\n";
	echo "</script>\n";
?>
  </body>
</html>
