// vim: ts=3 sts=3 sw=4 noet :
/****************************************************************
* LICENSE CC0 1.0 Universal
* (c) 2020 Hans Dijkema
* 
* These classes can be used with mmwiki.js to provide remedies 
* and links for the Materia Medica Wiki pages. 
*****************************************************************/

function tr(txt)
{
	return txt;
}

class MMLinkProvider
{
  constructor()
  {
    this._context = "";
    this._prefix = "";
	 this._old_link_prov = new MMWiki_LinkProvider();
  }

  setContext(c)
  {
    this._context = c;
  }
  
  context()
  {
	  return this._context;
  }

  setPrefix(p) 
  {
    this._prefix = p;
  }

  mkLinkHRef(link, content, tab_target)
  {
	  var c = this._context;
	  var p = this._prefix;
	  
	  if (link.startsWith("wiki://")) {
		  link = link.replace("wiki://", "");
		  var idx = link.lastIndexOf('/');
		  var page = link;
		  if (idx >= 0) {
			  page = link.substr(idx + 1);
			  c = link.substr(0, idx);
		  } else {
			  page = link;
		  } 
		  if (c == "_") { c = ""; }
		  if (c != "") { c = "/" + c; }
		  
		  if (p != "") { p = "/" + p; }


		  var nlink = p + c + "/" + page;
		  
		  link = nlink;
	  } 

	  var target = "";
	  if (tab_target) { 
			target = ' target="_blank"'; 
		}
	  
	  return "<a href=\"" + link + "\"" + target + ">" + content + "</a>";
  }

  mkLinkId(n) { return this._old_link_prov.mkLinkId(n); }
}

class MMRemedyProvider
{
	constructor() 
	{
		this._context = "";
		this._prev_prov = new MMWiki_RemedyProvider();
	}
	
	setContext(c)
	{
		this._context = c;
	}
	
	hasLink(uabbrev)
	{
		return true;
	}
	
	getLink(uabbrev) 
	{
		var a = uabbrev.toLowerCase();
		if (a == "con") { a = "_con"; }
		//return "index.php?context=" + this._context + "&page=" + a;
	   //return "/" + this._context + "/" + a;
		return a;
	}
	
	getLatinName(uabbrev)
	{
		var ln = mmwiki_remedies[uabbrev];
		if (ln) { return ln; }
		else { return uabbrev; }
	}
	
	unifyAbbrev(a)
	{
		return this._prev_prov.unifyAbbrev(a);
	}
}

class MMImageProvider
{
	constructor()
	{
		this._context = "";
	}
	
	setContext(c) { this._context = c; }
	
	getImageSrc(img) { 
		//return "mm/" + this._context + "/" + img; 
		return img;
	}
	
	getImageSrcs(uabbrev)
	{
		var abbrev = uabbrev.toLowerCase();
		var imgs = mmwiki_images[abbrev];
		if (imgs) {
			return imgs;
		} else {
			return new Array();
		}
	}

   getVideoSrcs(img) {
      var videos = new Array();
      videos.push({ link: img + ".mp4", ext: "mp4" });
      videos.push({ link: img + ".webm", ext: "webm" });
      videos.push({ link: img + ".ogv", ext: "ogg" });
      return videos;
   }
}

class MMIncludeProvider
{
	constructor()
	{
		this._context = "";
	}

	setContext(c) { this._context = c; }

	getPage(include_page, setter, f_error)
	{
		var xhr = new XMLHttpRequest();
		var url = "get_page.php?context=" + this._context + "&page=" + include_page;
		xhr.open("GET", url, true);
		xhr.onreadystatechange = function () {
			if (xhr.readyState === 4 && xhr.status === 200) {
				setter(xhr.responseText);
			} else if (xhr.readyState ===4) {
				f_error();
			}
		}
		xhr.send();
	}
}

class MMWikiMM 
{
		constructor(context)
		{
			this._mmwiki = new MMWiki();
			this._rem_prov = new MMRemedyProvider();
			this._link_prov = new MMLinkProvider();
			this._img_prov = new MMImageProvider();
			this._incl_prov = new MMIncludeProvider();
			this._rem_prov.setContext(context);
			this._link_prov.setContext(context);
			this._img_prov.setContext(context);
			this._incl_prov.setContext(context);
			this._mmwiki.setLinkProvider(this._link_prov);
			this._mmwiki.setRemedyProvider(this._rem_prov);
			this._mmwiki.setImageProvider(this._img_prov);
			this._mmwiki.setIncludeProvider(this._incl_prov);
			this._context = context;
			this._abbrev = "";
		}

		setPrefix(prefix)
		{
			this._mmwiki.linkProvider().setPrefix(prefix);
		}

		getLanguages(mmwiki)
		{
			return this._mmwiki.getLanguages(mmwiki);
		}

		book()
		{
			var hdr = this._mmwiki.header();
			return hdr.book();
		}
		
		headerHtml()
		{
			var hdr = this._mmwiki.header();
			var lp = this._link_prov;
			var author = hdr.author().trim();
			if (author != "") { author = " (" + author + ")"; }
			document.title = hdr.book() + author;
			var h_html = '<div class="mmwiki_hdr">' + 
						   '<div class="left">' + 
								lp.mkLinkHRef("wiki://index", 
												 '<span class="index">' + tr(hdr.book() + author) + '</span>' +
												 '<span class="mobile-index">&#x2616;</span>', false
												 ) +
							'</div>' +
							'<div class="right">' +
								lp.mkLinkHRef("wiki://_/index", 
												  '<span class="home">' + tr("Main index of Materia Medica Wiki") + '</span>' +
											     '<span class="mobile-home">&#x2617;</span>', false
											    ) + 
							'</div>' +
						 '</div>';
			return h_html;
		}
		
		toHtml(abbrev, mmwiki, one_per_line = false, language = "en")
		{
			var html = this._mmwiki.toHtml(mmwiki, one_per_line, language);
			var hdr = this._mmwiki.header();
			var lp = this._link_prov;
			var rp = this._rem_prov;
			
			this._abbrev = abbrev;

			if (abbrev == "_con") { abbrev = "con"; }

			var uabbrev = rp.unifyAbbrev(abbrev);

			hdr.setAbbrev(uabbrev);
			hdr.setLatinName(rp.getLatinName(uabbrev));
						 
			var h1_html = "";
			var h_meta = "";
			var t_html = "";
			var context = this._context;
			if (context.startsWith("mm_")) {
				if (uabbrev == "Index") {
					h1_html = '<h1>Index of ' + hdr.book() + '</h1>';
				} else {
					h1_html = '<h1>' + 
								  rp.getLatinName(uabbrev) + ' (' + uabbrev + ')' +
								  '</h1>';
				}
			
				var meta = this._mmwiki.metaProvider();
				h_meta = meta.getMeta();
		   } else {
		   }
						  
			var d_html = h1_html + h_meta + html;
			
			return d_html;
		}

		addIncludesRaw(ready_f, error_f, finish_f) {
			this._mmwiki.addIncludes(ready_f, error_f, finish_f);
		}

		addIncludes() 
		{
			this._mmwiki.addIncludes(
								function(id, html) {
									document.getElementById(id).innerHTML = html;
								},
								function(id, include_page) {
									console.log("Error including id '" + id + "', page '" + include_page + "'");
								},
								function() { 
									// finish includes, does nothing here
								}
							);
		}
		
		tocHtml() 
		{
 		   var context = this._context;
		   var page = this._abbrev.toLowerCase().trim();
		   if (context.startsWith("mm_")) {
			   return "";
		   } else if (page == "index") {
			   return "";
		   }

		   var toc = this._mmwiki.toc();
		   var i;
		   var level = -1;
		   var t_html = "";
		   var lang = mmwikiLanguage();
		   var h = "";

			if (toc.size() == 0) { 
				return "";
			}

		   if (lang == "nl") { h = "Inhoud"; }
		   else { h = "Contents"; }
		   
		   t_html += "<h3>" + h + "</h3>"
		   for(i = 0; i < toc.size(); i++) {
			   var lvl = toc.level(i);
			   if (lvl > level) {
				   while(level < lvl) { 
						t_html += "<ul>";
						level += 1;
				   }
			   } else if (lvl == level) {
				   // do nothing
			   } else {
				   while(level > lvl) {
					   t_html += "</ul>";
					   level -= 1;
				   }
			   }
			   t_html += "<li><a href=\"#" + toc.tocRef(i) + "\">" + toc.tocTxt(i) + "</a></li>";
		   }
		   while (level >= 0) {
				t_html += "</ul>";
				level -= 1; 
		   }
		   t_html = "<div class=\"toc\">" + t_html + "</div>";		
		   return t_html;
		}
}

function mmwikiPublishAllPage(incl_provider, context, pages, page_idx, f_progress, f_ok, f_error)
{
	if (page_idx >= pages.length) {
		f_ok();
	} else {
		var page = pages[page_idx];
		incl_provider.getPage(page, 
									 function(content) {
										 f_progress(context, page, page_idx + 1, pages);
										 mmwikiPublish(context, page, content,
															function() {
																page_idx += 1;
																mmwikiPublishAllPage(incl_provider, context, pages, page_idx, f_progress, f_ok, f_error);
															},
															f_error
														  );
									 },
									 f_error
									);
	}
}

function mmwikiPublishAll(context, f_progress, f_ok, f_error)
{
   var xhr = new XMLHttpRequest();
   var url = "get_pages.php";
	var incl_provider = new MMIncludeProvider();
	incl_provider.setContext(context);

	var url = "get_pages.php?context=" + context;
	xhr.open("GET", url, true);

   xhr.onreadystatechange = function () {
		if (xhr.readyState === 4 && xhr.status === 200) {
			var pages_str = xhr.responseText;
			var pages = pages_str.split(",");

			if (pages.length > 0) {
				var page_idx = 0;
				mmwikiPublishAllPage(incl_provider, context, pages, page_idx, f_progress, f_ok, f_error);
			}
       } else if (xhr.readyState ===4) {
          f_error();
       }
    }

    xhr.send();
}

function mmwikiPublish(context, page, content, f_ok, f_error)
{
	var m = new MMWikiMM(context);
	var l = m.getLanguages(content);

	var html = "";
   var book = "";

   var i;
   for(i = 0; i < l.length; i++) {
		var languages = l[i];
		var lang = languages[0];
		var j;
		var sep = "";
		var cl = "";
		for(j = 0; j < languages.length; j++) {
			cl += sep;
			cl += languages[j];
			sep = "_";
		}

		html += '<div id="lang_' + cl + '">';
		var contents = m.toHtml(page, content, false, lang);
		var toc = m.tocHtml();
		var header = m.headerHtml();
		book = m.book();
		html += '<div id="mmwiki_hdr">' + header + '</div>';
		html += '<div id="mmwiki_toc"><div class="mmwiki_toc">' + toc + '</div></div>';
	   html += '<div id="mmwiki"><div class="mmwiki">';
		html += "<!-- begin contents -->";
		html += contents  
		html += "<!-- end contents -->";
		html += '</div></div>';
		html += '</div>';
	}

	m.addIncludesRaw(
			function(id, incl_html) {
				var needle = "<div id=\"" + id + "\"></div>";
				html = html.replace(needle, "<!-- " + id + "--> " + incl_html);
			},
			function(id, incl_page) {
				console.log("Error including page '" + incl_page + "', id '" + id + "'");
			}, 
			function() {
				var xhr = new XMLHttpRequest();
				var url = "publish.php";
			
				xhr.open("POST", url, true);
				xhr.setRequestHeader("Content-Type", "application/json");
			
				xhr.onreadystatechange = function () {
					if (xhr.readyState === 4 && xhr.status === 200) {
						f_ok();
					} else if (xhr.readyState ===4) {
						f_error();
					}
				}

				if (page == "con") { page = "_con"; }	
				var obj = { "book": book, "context": context, "page": page, "content": html, "languages": l };
				var data = JSON.stringify(obj);
				xhr.send(data);
			}
	);
}

function mmwikiGetCss(f_ok, f_error)
{
	var incl_prov = new MMIncludeProvider();
	incl_prov.getPage("custom.css", f_ok, f_error);
}

function mmwikiSaveCss(css, f_ok, f_error)
{
	mmwikiSave("css", "custom.css", css, f_ok, f_error);
}

function mmwikiSave(context, page, content, f_ok, f_error)
{
	var xhr = new XMLHttpRequest();
	var url = "save.php";
	
	xhr.open("POST", url, true);
	xhr.setRequestHeader("Content-Type", "application/json");
	
	xhr.onreadystatechange = function () {
		if (xhr.readyState === 4 && xhr.status === 200) {
			f_ok();
		} else if (xhr.readyState ===4) {
			f_error();
		}
	}

   if (page == "con") { page = "_con"; }	
	var obj = { "context": context, "page": page, "content": content };
	var data = JSON.stringify(obj);
	xhr.send(data);
}

function mmwikiLogin(account, passwd, f_ok, f_error)
{
	var xhr = new XMLHttpRequest();
	var url = "login.php";
	
	xhr.open("POST", url, true);
	xhr.setRequestHeader("Content-Type", "application/json");
	
	xhr.onreadystatechange = function () {
		if (xhr.readyState === 4 && xhr.status === 200) {
			f_ok();
		} else if (xhr.readyState ===4) {
			f_error();
		}
	}
	
	var obj = { "account": account, "passwd": passwd };
	var data = JSON.stringify(obj);
	xhr.send(data);
}

function mmwikiPing(f_ok, f_error)
{
  var xhr = new XMLHttpRequest();
   var url = "ping.php";
   xhr.open("GET", url, true);
   xhr.onreadystatechange = function () {
      if (xhr.readyState === 4 && xhr.status === 200) {
         f_ok();
      } else if (xhr.readyState ===4) {
         f_error();
      }
   }
   xhr.send();
}

function mmwikiLogout(f_ok, f_error) 
{
	var xhr = new XMLHttpRequest();
	var url = "logout.php";
	xhr.open("GET", url, true);
	xhr.onreadystatechange = function () {
		if (xhr.readyState === 4 && xhr.status === 200) {
			f_ok();
		} else if (xhr.readyState ===4) {
			f_error();
		}
	}
	xhr.send();
}


function mmwikiUploadImage(context, page, req_name, image_file, image_name, f_ok, f_error)
{
	var req = new XMLHttpRequest();
	var formData = new FormData();
	
	req_name = req_name.trim();
	image_name = image_name.trim();
	if (req_name != "") { image_name = req_name; }
	var url = "upload_image.php?context=" + context + "&page=" + page + "&name=" + image_name;

	formData.append("image", image_file);                                
	req.open("POST", url, true);
	req.send(formData);	
	
	req.onreadystatechange = function () {
		if (req.readyState === 4 && req.status === 200) {
			f_ok(image_name);
		} else if (req.readyState ===4) {
			f_error();
		}
	}
}

var mmwiki_custom_language = "";

function mmwikiLanguage()
{
	if (mmwiki_custom_language != "") {
		return mmwiki_custom_language;
	} else {
		var userLang = navigator.language || navigator.userLanguage; 
		if (userLang) {
			var idx = userLang.indexOf('-');
			if (idx >= 0) { userLang = userLang.substr(0, idx); }
			return userLang;
		} else {
			return "en";
		}
	}
}

function mmwikiSetLanguage(lang)
{
	mmwiki_custom_language = lang.trim();
}


function mmwikiCookie(cname) {
  var name = cname + "=";
  var decodedCookie = decodeURIComponent(document.cookie);
  var ca = decodedCookie.split(';');
  for(var i = 0; i <ca.length; i++) {
    var c = ca[i];
    while (c.charAt(0) == ' ') {
      c = c.substring(1);
    }
    if (c.indexOf(name) == 0) {
      return c.substring(name.length, c.length);
    }
  }
  return "";
}

function mmwikiMakeMovable(id)
{
	var mousePosition;
	var offset = [0,0];
	var div = document.getElementById(id);
	var isDown = false;

	div.addEventListener('mousedown', function(e) {
         var y = event.clientY;
         if (y >= div.offsetTop && y <= (div.offsetTop + 30)) {
    		  isDown = true;
    		  offset = [
        		  div.offsetLeft - e.clientX,
        		  div.offsetTop - e.clientY
    		  ];
         }
		}, true);

	document.addEventListener('mouseup', function() {
    		isDown = false;
	}, true);

	document.addEventListener('mousemove', function(event) {
    		if (isDown) {
    		   event.preventDefault();
        		mousePosition = { x : event.clientX, y : event.clientY };
        		div.style.left = (mousePosition.x + offset[0]) + 'px';
        		div.style.top  = (mousePosition.y + offset[1]) + 'px';
    		}
	}, true);
}
