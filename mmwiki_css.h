#ifndef MMWIKI_CSS_H
#define MMWIKI_CSS_H
/*
   vim: ts=4 sw=4 sts=4 noet :
*/

#define MMWIKI_CSS \
	"body {" \
	"  font-family: Times New Roman,serif;" \
	"  font-size: 12pt;" \
	"  margin-left: 5%;" \
	"  margin-right: 5%;" \
	"  color: black;" \
	" }" \
	"" \
	" h1, h2, h3, h4, h5 { font-family: Arial,sans-serif; color: #2d8659;margin-bottom: 1em;margin-top:1em; }" \
	" h1 { font-size: 150%;text-align: center; }" \
	" h2 { font-size: 125%;text-align: center; }" \
	" h3, h4, h5 { font-weight: bold;padding-bottom:0.5em;margin-bottom:0; }" \
	" h3 { font-size: 110%; }" \
	" h4 { font-size: 105%; }" \
	" h5 { font-size: 100%; }" \
	" div.rubric, div.rubric2 { margin-left: 1em; }" \
	" div.rubric p:first-of-type, div.rubric2 p:first-of-type { text-indent: -1em; }" \
	" span.rubric, span.rubric2 { font-size: 100%;font-weight: bold; }" \
	" span.rubric2 { font-style: italic; }" \
	"" \
	" p { margin-bottom: 0.5em;margin-top:0; }" \
	" ul, ol { margin-top: 0; margin-bottom: 0.5em; }" \
	" ul ul, ol ol, ul ol, ol ul { margin-bottom: 0; }" \
	"" \
	" span.symptom:hover { background: #d0d0d0;transition: background 1s; }" \
	"" \
	" span.tooltip, div.tooltip { display: none; }" \
	" span.hover:hover span.tooltip, div.hover:hover div.tooltip {" \
	" 	display: inline-block;position: absolute;background-color: #555;" \
	"    color: #fff;padding: 0.4em;z-index: 9999;" \
	"    margin-top: 1em;margin-left: 0.5em;margin-right: 1em;margin-bottom: 0;" \
	"    text-indent: 0;font-style: normal;" \
	" }" \
	" span.def-tooltip { min-width: 10em;max-width: 20em; }" \
	" span.tooltip a, .tooltip a { color: #fff; }" \
	"" \
	" span.link, span.link a { text-decoration:none;color:inherit; }" \
	" span.link_hover, span.link_hover a { text-decoration:underline;color:inherit; }" \
	"" \
	" span.grade2, span.grade2 a { color: #1B83FA; }" \
	" span.grade3, span.grade3 a { font-weight: bold; color: #aa664e; }" \
	" span.grade4, span.grade4 a { font-weight: bold; color: #a40fd6; }" \
	"" \
	" span.remedy1, span.remedy1 a { color: #008000; font-style: italic; }" \
	" span.remedy2, span.remedy2 a { color: #1b83fa; font-style: italic; }" \
	" span.remedy3, span.remedy3 a { color: #aa664e; font-style: italic; font-weight: bold; }" \
	" span.remedy4, span.remedy4 a { color: #a40fd6; font-style: italic; font-weight: bold; }" \
	"" \
	" span.relation { color: #341973; }" \
	" span.tendency { font-weight: bold; color: #b30047; }" \
	" span.clinical { font-style: italic;color: #a300cc; }" \
	" span.modality { }" \
	" span.radiating { }" \
	" span.code, .code { font-family: Courier,monospace;font-size: 85%; }" \
	" .code { margin-bottom: 0.5em; }" \
	" .note { font-size: 80%; }" \
	" .note p.first { margin-left: 10mm;text-indent: -5mm; }" \
	" .note p { margin-left: 10mm; }" \
	"" \
	" span.definition, span.definition-with-highlight { border-bottom: 1.1pt #505050 dotted; }" \
	" span.definition-with-highlight, span.highlight { background: yellow; }" \
	"" \
	" table { border-collapse: collapse; }" \
	" td, th { border: 1px solid #505050;padding: 0.2em; }" \
	" th { background: #e0e0e0;text-align: center;vertical-align:middle; }" \
	" td { vertical-align: top; }" \
	" td.center, th.center { text-align: center; }" \
	" td.left, th.left { text-align: left; }" \
	" td.right, th.right { text-align: right; }" \
	"" \
	" .meta { float: right;	font-size: 80%; border: 1px solid black; top: -5mm; margin-left: 5mm; }" \
	" .meta img { width: 5cm; }" \
	" .bigpicture { position:fixed; left: 50%; top: 10%; height: 80%; z-index: 9; display: none; }" \
	" .bigpicture img { position: relative; left: -50%; height: 100%; }" \
	"" \
	" .image-center { margin-left:auto; margin-right: auto; }" \
	" .image-float-left { margin:0;padding:0;float:left; }" \
	" .image-float-right { margin:0;padding:0;float:right; }" \
	" .image { margin: 0.5em;margin-top: 1em; }" \
	" .image .subscript { margin-top: 0.5em; text-align:center; font-size: 80%; font-style: italic; }" \
	" td .image { margin: 0; }" \
	"" \
	" .clear { clear:both; }" \
	"" \
	" span.dash { display:block;padding-left: 1em; }" \
	" span.dash::before { content: \"-\\00a0\"; }" \
	"" \
	" span.page-number { color: #ff0000;font-weight: bold;border: 1px solid #ff0000; }" \
	"" \
	" span.anchor { z-index:20;visibility:hidden;display:inline;position:relative; }" 

#endif
