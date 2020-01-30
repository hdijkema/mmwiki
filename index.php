<?php
require_once("config.php");
?>
<!DOCTYPE html>
<html>
  <head>
    <meta charset="utf-8"></meta>
	<link rel="stylesheet" href="mmwiki.css"></link>
	<link rel="stylesheet" href="_style.css"></link>
	<link rel="stylesheet" href="mmwiki_mm.css"></link>
	<link rel="icon" href="favicon.png" sizes="32x32" />
	<link rel="icon" href="favicon.png" sizes="192x192" />	
	<script src="mmwiki.js" type="text/javascript" charset="utf-8"></script>
	<script src="remedies.js" type="text/javascript" charset="utf-8"></script>
	<script src="images.js" type="text/javascript" charset="utf-8"></script>
	<script src="mmwiki_mm.js" type="text/javascript" charset="utf-8"></script>
	<script src="ace/src-min-noconflict/ace.js" type="text/javascript" charset="utf-8"></script>
	<title>Materia Medica Wiki</title>
  </head>
  <body>
  <div class="mmwiki_page">
  <div id="mmwiki_hdr"><!-- This is where the generated HTML header goes--></div>
  <div class="mmwiki_edit" id="mm_edit">
	  <div id="editor_hdr">
			<button id="save_btn" type="button" disabled onclick="document.mmwiki_save();">Save</button>
			<input type="text" id="image_name" value="" placeholder="Image name" />
			<input id="image_upload_btn" type="file" name="image_file" />
			<button id="image_upload_now" type="button" onclick="document.mmwiki_upload_image();">Uploaden</button>
			<input type="text" id="language_code" value="" placeholder="Language" />
			<button id="close_btn" type="button" style="float:right;;margin-right:2em;" onclick="document.mmwiki_close_editor();">Close Editor</button>
			<button id="login_btn" type="button" style="display:none;float:right;" onclick="document.mmwiki_login();">Login</button>
			<button id="logout_btn" type="button" style="display:none;float:right;" onclick="document.mmwiki_logout();">Logout</button>
			<button id="open_btn" type="button" style="display:none;float:right;" onclick="document.mmwiki_open_editor();">Open Editor</button>
	  </div>
	  <div id="editor"><!-- This is where the editor goes--></div>
  </div>
  <div class="login" id="login-form">
	<h1>Login to MMWiki</h1>
	<table>
		<tr><td class="label">Account</td><td><input type="text" id="login-account"></td></tr>
		<tr><td class="label">Password</td><td><input type="password" id="login-passwd"></td></tr>
		<tr><td><button type="button" onclick="document.mmwiki_cancel_login();" >Cancel</button></td>
		    <td><button type="button" onclick="document.mmwiki_do_login();" id="do-login">Login</button>
		</td></tr>
		<p id="login-msg">&nbsp;</p>
	</table>
  </div>
  <div id="mmwiki"><!-- This is where the generated HTML goes --></div>
  </div>
<?php
	$page = "index";
	$context = "";

	if (isset($_GET['context'])) { $context = $_GET['context']; }
	$d_context = $context;
	if ($d_context != "") { $d_context = "/" . $d_context; }

	if (isset($_GET['page'])) { $page = $_GET['page']; }
	if (isset($_GET['generate-index']) && $d_context != "") { generateIndex($d_context); }

	$file = "$MMWIKI_PREFIX/mm$d_context/$page.mm";

	if (file_exists($file)) {
		$contents = file_get_contents($file);
	} else {
		$contents = ":book=Materia Medica Wiki\n:begin[en]\n\nNo content available for page '$page'.\n\n:end\n\n";
	}
	$contents = str_replace("\n", "\\n", $contents);
	$contents = str_replace("\r", "", $contents);
	$contents = str_replace("'", "\\'", $contents);
	
	?>
	<script>
		document.mmwiki_updater = function(txt) {
			var abbrev = <?php echo "'$page'"; ?>; 
			var m = new MMWikiMM(<?php echo "'$context'"; ?>);
			var lang = mmwikiLanguage();
			var contents = m.toHtml(abbrev, txt, false, lang);
			if (contents == "") { contents = 'No content'; }
			document.getElementById('mmwiki').innerHTML = contents;
			document.getElementById('mmwiki_hdr').innerHTML = m.headerHtml();
		}
		var txt = <?php echo "'$contents'"; ?>;
		document.mmwiki_updater(txt);
	</script>
	<?php
	
	createEditor($context, $page, $file); 
	
	function generateIndex($context) 
	{
		global $MMWIKI_PREFIX;
		$dir = "$MMWIKI_PREFIX/mm/$context";
		
		$g_files = glob("$dir/*.mm");
		$files = array();
		foreach($g_files as $file) {
			$bn = basename($file);
			if ($bn == "_con.mm") { $bn = "con.mm"; }
			array_push($files, $bn);
		}
		sort($files);
		$letter = "";
		$comma = "";
		
		$pre = file_get_contents("$dir/index.pre");
		
		$fh = fopen("$dir/index.mm", "w");
		fputs($fh, $pre); 
		foreach($files as $file) {
			$abbrev = str_replace(".mm", "", $file);
			if ($abbrev != "index") {
				if ($abbrev == "_con") { $abbrev = "con"; }
				$abbrev = strtoupper(substr($abbrev, 0, 1)) . strtolower(substr($abbrev, 1));
				$nletter = substr($abbrev, 0, 1);
				if ($nletter != $letter) {
					if ($letter != "") {
						fputs($fh, "\n:table-end\n");
					}
					fputs($fh, "\n\n:head=$nletter\n");

					$letter = $nletter;
					$comma = "";

					fputs($fh, "\n:table-begin=5,5,5,5,5,5,5,5,5,5::80\n");
				}
				fputs($fh, ":cell=l\nR[$abbrev]\n");
				$comma = ", ";
			}
		}
		fputs($fh, "\n\n:table-end\n\n:div-end\n\n:end\n\n");
		fclose($fh);
	}
	
	function escape($a) {
		$a = str_replace("'", "\\'", $a);
		return $a;
	}
	
	function createEditor($context, $page, $file)
	{
		if (file_exists($file)) {
			$contents = file_get_contents($file);
		} else {
			$contents = ":book=Materia Medica Wiki\n:author=Hans Dijkema\n:editor=(c) 2020 Homeopathie Nederland B.V.\n:edition=2020-1\n\n:begin[en]\n\nEmpty page\n\n:end\n\n";
		}
		$contents = str_replace("\n", "\\n", $contents);
		$contents = str_replace("\r", "", $contents);
		$contents = str_replace("'", "\\'", $contents);
		$contents = str_replace("<", "&lt;", $contents);
		?>
		<script>
			var context = <?php echo "'$context'"; ?>;
			var page = <?php echo "'$page'"; ?>;
			var contents = <?php echo "'$contents'"; ?>;
			var elem_editor = document.getElementById('editor');
			var div_editor = document.getElementById('mm_edit'); 
			var open_btn = document.getElementById('open_btn');
			var close_btn = document.getElementById('close_btn');
			var elem_mmwiki = document.getElementById('mmwiki');
			var save_btn = document.getElementById('save_btn');
			var img_upload_btn = document.getElementById('image_upload_btn');
			var img_upload_now = document.getElementById('image_upload_now');
			var image_name = document.getElementById('image_name');
			var lang_code = document.getElementById('language_code');
			var login_btn = document.getElementById('login_btn');
			var logout_btn = document.getElementById('logout_btn');

			var logged_in = mmwikiCookie('logged-in');
			
			document.mmwiki_login = function() {
				var form = document.getElementById('login-form');
				form.style.display = 'block';
				document.getElementById('login-account').focus();
			};
			
			document.mmwiki_logout = function() {
				mmwikiLogout(
					function() { 
						logged_in = false;
						document.mmwiki_main();
					}, 
					function() {
						alert("Could not logout");
					}
				);
			};
			
			document.mmwiki_main = function() {
				if (logged_in == "yes") {
					var language = mmwikiLanguage();
					lang_code.onchange = function() { 
						mmwikiSetLanguage(lang_code.value); 
						document.mmwiki_updater(editor.getValue());
					}
					lang_code.value = language;
					
					elem_editor.innerHTML = contents;
					div_editor.style.display = 'block';
					
					var editor = ace.edit("editor");
					var session = editor.getSession();
					
					session.setMode("ace/mode/mmwiki");
					session.setUseWrapMode(true);
					
					var timeout_handle ;
					session.on('change', function() { 
											if (timeout_handle) {
												clearTimeout(timeout_handle);
											}
											timeout_handle = setTimeout(function() {
												elem_mmwiki.innerHTML = "Wait...";
												var txt = editor.getValue();
												document.mmwiki_updater(txt);
												save_btn.disabled = false; 
												dirty = false;
												}, 2000);
											}
							  );
					document.mmwiki_save = function() { 
						mmwikiSave(context, page, editor.getValue(), function() { 
							save_btn.disabled = true;
						}, function() { 
							alert("Page cannot be saved");
						}); 
					}
					document.mmwiki_upload_image = function() {
						if (img_upload_btn.files.length > 0) {
							var img_file = img_upload_btn.files[0];
							var path = img_upload_btn.value;
							var img_name = path.split(/[\\/]/).pop();
							var requested_name = image_name.value.trim();
							mmwikiUploadImage(context, page, requested_name, img_file, img_name, function(name) {
									img_upload_btn.value = "";
									alert("Image '" + name + "' has been uploaded");
								}, function() {
									alert("Image " + img_name + " could not be uploaded to context '" + context + "'");
								}
							);
						} else {
							alert("Choose an image to upload first");
						}
					};
					document.mmwiki_close_editor = function() { 
						div_editor.style.height = "1.5em";
						div_editor.style.background = "transparent";
						div_editor.style.border = "none";
						elem_editor.style.display = 'none';
						close_btn.style.display = 'none';
						save_btn.style.display = 'none';
						img_upload_btn.style.display = 'none';
						img_upload_now.style.display = 'none';
						image_name.style.display = 'none';
						lang_code.style.display = 'none';
						logout_btn.style.display = 'inline';
						login_btn.style.display = 'none';
						open_btn.style.display = 'inline';
					};
					document.mmwiki_open_editor = function() {
						div_editor.style.height = "50%";
						div_editor.style.background = "#a0a0a0";
						div_editor.style.border = "1px solid black";
						elem_editor.style.display = 'block';
						close_btn.style.display = 'inline';
						save_btn.style.display = 'inline';
						img_upload_btn.style.display = 'inline';
						img_upload_now.style.display = 'inline';
						image_name.style.display = 'inline';
						lang_code.style.display = 'inline';
						logout_btn.style.display = 'inline';
						login_btn.style.display = 'none';
						open_btn.style.display = 'none';
					};
					document.mmwiki_close_editor();
				} else {
					div_editor.style.height = "1.5em";
					div_editor.style.background = "transparent";
					div_editor.style.border = "none";
					elem_editor.style.display = 'none';
					close_btn.style.display = 'none';
					save_btn.style.display = 'none';
					img_upload_btn.style.display = 'none';
					img_upload_now.style.display = 'none';
					image_name.style.display = 'none';
					lang_code.style.display = 'none';
					open_btn.style.display = 'none';
					logout_btn.style.display = 'none';
					login_btn.style.display = 'inline';
					
					var login_account = document.getElementById('login-account');
					var login_passwd = document.getElementById('login-passwd');
					login_account.addEventListener('keyup', function(event) {
							document.getElementById('login-msg').innerHTML = '&nbsp;';
							if (event.keyCode === 13) {
								event.preventDefault();
								login_passwd.focus();
							} else if (event.keyCode === 27) {
								event.preventDefault();
								document.mmwiki_cancel_login();
							}
						}
					);
					login_passwd.addEventListener('keyup', function(event) {
							document.getElementById('login-msg').innerHTML = '&nbsp;';
							if (event.keyCode === 13) {
								event.preventDefault();
								var do_login_btn = document.getElementById('do-login');
								do_login_btn.click();
							} else if (event.keyCode === 27) {
								event.preventDefault();
								document.mmwiki_cancel_login();
							}
						}
					);
					
					document.mmwiki_cancel_login = function() {
						var form = document.getElementById('login-form');
						form.style.display = 'none';
						var pwd = document.getElementById('login-passwd');
						pwd.value = '';
						document.getElementById('login-msg').innerHTML = '&nbsp;';
					};
					
					document.mmwiki_do_login = function() {
						var login_account = document.getElementById('login-account');
						var login_passwd = document.getElementById('login-passwd');
						mmwikiLogin(login_account.value, login_passwd.value, 
							function() { 
								document.mmwiki_cancel_login();
								logged_in = "yes";
								document.mmwiki_main();
							}, function() {
								document.getElementById('login-msg').innerHTML = 
									'<span class="error">Could not login to MMWiki, try again</span>';
							}
						);
					}
				}
			};
			
			document.mmwiki_main();
		</script>
		<?php
	}
?>
  </body>
</html>
