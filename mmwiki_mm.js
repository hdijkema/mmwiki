/****************************************************************
* LICENSE CC0 1.0 Universal
* (c) 2020 Hans Dijkema
* 
* These classes can be used with mmwiki.js to provide remedies 
* and links for the Materia Medica Wiki pages. 
*****************************************************************/

class MMLinkProvider
{
  constructor()
  {
    this._context = "";
  }

  setContext(c)
  {
    this._context = c;
  }
  
  context()
  {
	  return this._context;
  }

  mkLinkHRef(link, content)
  {
	  var c = this._context;
	  
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
		  if (c != "") { c = "context=" + c + "&"; }
		  var nlink = "index.php?" + c + "page=" + page;
		  link = nlink;
	  } 
	  
	  return "<a href=\"" + link +"\">" + content + "</a>";
  }
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
		return "index.php?context=" + this._context + "&page=" + a;
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
		return "mm/" + this._context + "/" + img; 
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
}

class MMWikiMM 
{
		constructor(context)
		{
			this._mmwiki = new MMWiki();
			this._rem_prov = new MMRemedyProvider();
			this._link_prov = new MMLinkProvider();
			this._img_prov = new MMImageProvider();
			this._rem_prov.setContext(context);
			this._link_prov.setContext(context);
			this._img_prov.setContext(context);
			this._mmwiki.setLinkProvider(this._link_prov);
			this._mmwiki.setRemedyProvider(this._rem_prov);
			this._mmwiki.setImageProvider(this._img_prov);
		}
		
		headerHtml()
		{
			var hdr = this._mmwiki.header();
			var lp = this._link_prov;
			var h_html = '<div class="mmwiki_hdr">' + 
						 '<div class="left">' + 
							lp.mkLinkHRef("wiki://index", hdr.book()) + 
						 '</div>' +
						 '<div class="right">' +
							lp.mkLinkHRef("wiki://_/index", 'Main index of Materia Medica Wiki') +
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

			if (abbrev == "_con") { abbrev = "con"; }

			var uabbrev = rp.unifyAbbrev(abbrev);

			hdr.setAbbrev(uabbrev);
			hdr.setLatinName(rp.getLatinName(uabbrev));
						 
			var h1_html = "";
			if (uabbrev == "Index") {
				h1_html = '<h1>Index of ' + hdr.book() + '</h1>';
			} else {
				h1_html = '<h1>' + 
							  rp.getLatinName(uabbrev) + ' (' + uabbrev + ')' +
							  '</h1>';
			}
			
			var meta = this._mmwiki.metaProvider();
			var h_meta = meta.getMeta();
						  
			var d_html = h1_html + h_meta + html;
			
			return d_html;
		}
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

