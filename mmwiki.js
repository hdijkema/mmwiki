class Line
{
  constructor(l)
  {
    this.line = l;
    this.line_ci = l.trim().toLowerCase();
    this._value = "";
  }

  isKeyVal(key)
  {
     if (this.line_ci.startsWith(key)) {
       var idx = key.length;
       var len = this.line_ci.length;
       while(idx < len && this.line_ci[idx] == ' ') { idx += 1; }
       if (idx == len) {
         this._value = "";
         return true;
       } else if (this.line_ci[idx] == '=') {
         var s = this.line.trimStart();
         this._value = s.substr(idx + 1);
         return true;
       }
     } else {
       return false;
     }
  }

  value() { return this._value; }
  isKey(key) { return this.line_ci.startsWith(key); }
  isEmpty() { return this.line_ci == ""; }
  line() { return this.line; }
}

class Source
{
  constructor()
  {
    this._letter = "";
    this._source = "";
    this._html_mark = "";
    this._html_pre = "";
    this._html_post = "";
  }

  fromString(s) {
    var parts = s.split(',');
    var size = parts.length;
    if (size >= 3) {
      this._letter = s[0].trim();
      this._source = s[1].trim();
      this._html_mark = s[2].trim();

      if (size >= 4) {
        this._html_pre = s[3].trim();
        if (size >= 5) {
          this._html_post = s[4].trim();
        }
      }
      return true;
    }
    return false;
  }

  letter() { return this._letter; }
  source() { return this._source; }
  htmlMark() { return this._html_mark; }
  htmlPost() { return this._html_post; }
  htmlPre() { return this._html_pre; }

}

class Header
{
  constructor()
  {
    this._book = "";
    this._edition = "";
    this._author = "";
    this._editor = "";
    this._abbrev = "";
    this._latin_name = "";
    this._sources = {};
    this._miasm = "";
    this._tendency = "";
    this._classification = "";
  }

  book() { return this._book; }
  setBook(b) { this._book = b; }
  edition() { return this._edition; }
  setEdition(e) { this._edition = e; }
  author() { return this._author; }
  setAuthor(a) { this._author = a; }
  editor() { return this._editor; }
  setEditor(e) { this._editor = e; }
  abbrev() { return this._abbrev; }
  setAbbrev(a) { this._abbrev = a; }
  latinName() { return this._latin_name; }
  setLatinName(l) { this._latin_name = l; }
  miasm() { return this._miasm; }
  setMiasm(m) { this._miasm = m; }
  tendency() { return this._tendency; }
  setTendency(t) { this._tendency = t; }
  classification() { return this._classification; }
  setClassification(c) { this._classification = c; }
  addSource(s) { var src = new Source();
                 if (src.fromString(s)) { this._sources[src.letter()] = s; }
               }
  source(letter) { return this._sources[letter]; }
}


class MMWiki
{
    ///////////////////////////////////////////////////////////////

    constructor()
    {
      this.re_prepare_modalities = /[<>]\[/;
      this.re_html_prepare = /[<>&']/;
      this.re_symptom = /([CSGZR])([A-Z]*)[{]([^}{]*)[}]/;
      this.re_section = /^\s*:begin\[([a-z, ]+)\]\s*$/m;
      this.re_end = /^\s*:end\s*$/m;
    }

    toHtml(mmwiki, one_per_line = false, language = 'en')
    {
        this.mmwiki = mmwiki;
        this.header = this.extractHeader();
        var contents = this.extractContents(language);
        return contents;
    }

    extractHeader()
    {
      var h = new Header();
      var in_head = true;
      var lines = this.mmwiki.split("\n");
      var i;
      for(i = 0; i < lines.length; i++) {
        var l = new Line(lines[i]);
        if (l.isKey(":begin")) {
          return h;
        } else if (l.isKeyVal(":book")) {
          h.setBook(l.value());
        }
      }
      return h;
    }

    extractContents(lang)
    {
      var matches = this.mmwiki.matchAll(this.re_section);

      var first_language = -1;
      var en_language = -1;
      var has_language = -1;

      for(const match of matches) {
          var l = match[1];
          var langs = l.split(',');
          var languages = new Array();
          for(lang in langs) {
            lang = lang.trim();
            languages.push(lang);
          }
          if (languages.includes(lang) && has_language < 0) {
            has_language = match.index + match[0].length;
          } else if (languages.includes("en") && en_language < 0) {
            en_language = match.index + match[0].length;
          } else if (first_language < 0) {
            first_language = match.index + match[0].length;
          }
      }

      var idx = -1;
      if (has_language >= 0) { idx = has_language; }
      else if (en_language >= 0) { idx = en_language; }
      else if (first_language >= 0) { idx = first_language; }

      if (idx >= 0) {
        var result = this.mmwiki.substr(idx);
        var m = result.match(this.re_end);
        if (m) {
          result = result.substr(0, m.index);
        }
        return result;
      } else {
        return "";
      }
    }

}
