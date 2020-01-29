

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
		this._prev_prov = new RemedyProvider();
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
		return "index.php?context=" + this._context + "&page=" + uabbrev.toLowerCase();
	}
	
	getLatinName(uabbrev)
	{
		return uabbrev;
	}
	
	unifyAbbrev(a)
	{
		return this._prev_prov.unifyAbbrev(a);
	}
		
}



