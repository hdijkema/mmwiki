// Class stack
class Stack
{
  constructor() {
    this._stack = new Array();
  }

  back() { return this._stack[this._stack.length - 1]; }
  empty() { return this._stack.length == 0; }
  push_back(a) { this._stack.push(a); return this; }
  pop_back() { this._stack.pop(); return this; }
}

// Class Line
class Line
{
  constructor(l)
  {
    this.setLine(l);
    this._value = "";
  }

  isKeyVal(key)
  {
     if (this._line_ci.startsWith(key)) {
       var idx = key.length;
       var len = this._line_ci.length;
       while(idx < len && this._line_ci[idx] == ' ') { idx += 1; }
       if (idx == len) {
         this._value = "";
         return true;
       } else if (this._line_ci[idx] == '=') {
         var s = this._line.trimStart();
         this._value = s.substr(idx + 1);
         return true;
       }
     } else {
       return false;
     }
  }

  value() { return this._value; }
  isKey(key) { return this._line_ci.startsWith(key); }
  isEmpty() { return this._line_ci == ""; }
  line() { return this._line; }
  setLine(l) { this._line = l; this._line_ci = l.trim().toLowerCase(); }
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
      this._letter = parts[0].trim();
      this._source = parts[1].trim();
      this._html_mark = parts[2].trim();

      if (size >= 4) {
        this._html_pre = parts[3].trim();
        if (size >= 5) {
          this._html_post = parts[4].trim();
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

const TOKEN_NOOP = -2,
      TOKEN_ROOT = -1,
      TOKEN_NONE = 0,
      TOKEN_MODIFIER = -3,
      TOKEN_CONTENT = -4,
      TOKEN_SYMPTOM = 100,
      TOKEN_GROUP = 101,
      TOKEN_RELATION = 102,
      TOKEN_CAUSE = 103,
      TOKEN_SENTENCE = 104,
      TOKEN_GRADE2 = 200,
      TOKEN_GRADE3 = 201,
      TOKEN_GRADE4 = 202,
      TOKEN_MODALITY = 203,
      TOKEN_CLASSIFICATION = 204,
      TOKEN_TENDENCY = 205,
      TOKEN_REMEDY1 = 206,
      TOKEN_REMEDY2 = 207,
      TOKEN_REMEDY3 = 208,
      TOKEN_REMEDY4 = 209,
      TOKEN_DEFINITION = 210,
      TOKEN_RADIATES = 211,
      TOKEN_BETTER = 212,
      TOKEN_WORSE = 213,
      TOKEN_BOLD = 214,
      TOKEN_ITALIC = 215,
      TOKEN_PAGE = 216,
      TOKEN_CODE = 217,
      TOKEN_HIGHLIGHT = 218,
      TOKEN_LINK = 219,
      TOKEN_LITERAL = 220,
      TOKEN_ANCHOR = 221;

class Token
{
   init() {
     this._type = TOKEN_ROOT;
     this._content = "";
     this._modifier = "";
     this._start = 0;
     this._end = 0;
     this._subtokens = new Array();
     this._content_set = false;
     this._modifier_set = false;
   }

   constructor(a = TOKEN_ROOT)
   {
     this.init();
     this.setType(a);
   }

   type() { return this._type; }
   setType(t) { this._type = t; }

   content() { return this._content; }
   setContent(c) { this._content = c;this._content_set = true; }
   contentUnSet() { this._content_set = false; }
   contentIsSet() { return this._content_set; }

   modifier() { return this._modifier; }
   setModifier(m) { this._modifier = m;this._modifier_set = true; }
   modifierUnSet() { this._modifier_set = false;}
   modifierIsSet() { return this._modifier_set; }

   start() { return this._start; }
   setStart(s) { this._start = s; }

   end() { return this._end; }
   setEnd(e) { this._end = e; }

   subTokens() { return this._subtokens; }
   setSubTokens(s) { this._subtokens = s; }

   contains(other) {
     return other.start() >= this.start() && other.end() <= this.end();
   }

   equal(other) {
     return other.start() == this.start() && other.end() == this.end();
   }

   length() { return this.end() - this.start(); }

   hasA(f) {
     if (f(this)) { return true; }
     else {
       var i;
       for(i = 0; i < this._subtokens.length; i++) {
         if (this._subtokens[i].hasA(f)) return true;
       }
       return false;
     }
   }
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
                 if (src.fromString(s)) { this._sources[src.letter()] = src; }
               }
  source(letter) { return this._sources[letter]; }
}

class LinkProvider
{
  mkLinkHRef(link, content)
  {
    return "<a href=\"" + link + "\">" + content + "</a>";
  }
}

class RemedyProvider
{
  unifyAbbrev(a)
  {
    var s = a.trim();
    if (s != "") {
      s = s.substr(0, 1).toUpperCase() + s.substr(1).toLowerCase();
      if (s.substr(s.length - 1, 1) == ".") s = s.substr(0, s.length - 1);
    }
    return s;
  }

  hasLink(uabbrev) { return false; }
  getLink(uabbrev) { return ""; }
  getLatinName(uabbrev) { return uabbrev; }
}

class MMWiki
{
    ///////////////////////////////////////////////////////////////

    constructor()
    {
      this.re_prepare_modalities = /[<>]\[/;
      this.re_html_prepare = /[<>&']/;
      this.re_html_implement = /[@][!][@]([^@]+)[@][%][@]/;

      this.re_symptom = /([CSGZR])([A-Z]*)[{]([^}{]*)[}]/;
      this.re_markup = /([234PQHNMITBRCLt^!+-]|R[234])\[([^\[\]]*)\]/;
      this.re_modalities = /([<>])\[([^\[\]]*)\]/;
      this.re_symptom_grade = /([234])\[([^\[\]]*)\]/;
      this.re_anchor = /N\[[^\]]+\]/;

      this.re_section = /^\s*:begin\[([a-z, ]+)\]\s*$/m;
      this.re_end = /^\s*:end\s*$/m;

      this.re_bullits = /^([*12]+)\s/;
      this.re_literals = /\\[:{}\[\]]|\[BR\]/;

      this.re_cleanup_sym_open = /([CSGZR])([A-Z]*)[{]/;
      this.re_cleanup_open = /([234PQHNMITBRCLt^!+-]|R[234])\[([^|]+[|]){0, 1}/;
      this.re_cleanup_close = /(\]|[}])/;

      this.re_width = /([0-9.]+)(%|em|pt|mm|cm){0,1}/;

      this._highlight_words = new Array();

      this._min_grade = 0;

      this._link_provider = new LinkProvider();
      this._remedy_provider = new RemedyProvider();
    }

    toHtml(mmwiki, one_per_line = false, language = 'en')
    {
        // init State
        this._divs = new Stack();
        this._div_levels = new Stack();
        this._bullits = new Stack();
        this._bullit_levels = new Stack();
        this._tables_cell = new Stack();
        this._tables_cells = new Stack();

        this._one_per_line = one_per_line;
        this._no_dashes = false;

        this._seq = "";
        this._seq_class = "";
        this._seq_start = "";

        // Init wiki / html
        this._mmwiki = mmwiki;
        this._html = "";

        // extract header
        this._header = this.extractHeader();

        // extract contents
        var contents = this.extractContents(language);
        contents = this.prepareForHtml(contents);

        // highlighting
        if (this._highlight_words.length > 0) {
          var i;
          var re = "\\b(";
          var delim = "";
          for(i = 0; i < this._highlight_words.length; i++) {
            re = re + delim;
            re = re + this._highlight_words[i];
            delim = "|";
          }
          re += ")\\b";
          var rre = new RegExp(re, "gi");
          contents = contents.replace(rre, "H[$1]");
        }

        // Generate HTML
        // iterate over all the content
        var lines = contents.split('\n');
        var i;
        for(i = 0; i < lines.length; i++) {
          var line = new Line(lines[i]);
          var bullit_state = { level: 0, bullit: "*" };

          if (line.isKey(":no-dash-begin")) {
            this._no_dashes = true;
          } else if (line.isKey(":no-dash-end")) {
            this._no_dashes = false;
          } else if (line.isKeyVal(":local-name")) {
            this.addHead(-1, 20, "h2", line.value());
          } else if (line.isKeyVal(":source")) {
            this.addHead(-1, 30, "h3", line.value());
          } else if (line.isKeyVal(":head")) {
            this.addHead(0, 40, "h4", line.value());
          } else if (line.isKeyVal(":head2")) {
            this.addHead(1, 50, "h5", line.value());
          } else if (line.isKeyVal(":rubric")) {
            this.addRubric(2, 60, "rubric", line.value());
          } else if (line.isKeyVal(":rubric2")) {
            this.addRubric(3, 70, "rubric2", line.value());
          } else if (line.isKeyVal(":div-begin")) {
            this.startDiv(-1, line.value().trim(), "div-begin");
          } else if (line.isKeyVal(":div-end")) {
            this.endDiv(-1, "div-begin");
          } else if (this.isBullit(line, bullit_state)) {
            this.endSeq();
            this.startBullit(bullit_state, line);
          } else {
            this.endBullit(0, "");
            if (line.isEmpty()) {
              this.endSeq();
            } else {
              this.addSeq(line);
            }
          }
        }

        this.endSeq();

        this._html = this.implementHtml(this._html);

        return this._html;
    }

    setHighlightWords(h)
    {
      if (!Array.isArray(h)) {
        var a = new Array();
        a.push(h);
        this._highlight_words = a;
      } else {
        this._highlight_words = h;
      }
    }

    setMinGrade(g)
    {
      this._min_grade = g;
    }

    applyRe(re, s, f, f_remain) {
      var applied = false;
      var re_offset = 0;
      var m = s.match(re);
      while(m) {
        m.prefix = s.substr(0, m.index);
        f(m, re_offset);
        s = s.substr(m.index + m[0].length);
        re_offset += m.index + m[0].length;
        applied = true;
        m = s.match(re);
      }
      f_remain(s, re_offset);
      return applied;
    }

    prepareForHtml(contents)
    {
      var n_mmwiki = "";
      this.applyRe(this.re_literals, contents,
                  function(m, offset) {
                    n_mmwiki += m.prefix;
                    var s_literal = m[0];
                    var literal = s_literal.substr(0, 1);
                    if (literal == "\\") {
                      literal = s_literal.substr(1, 1);
                      if (literal == ":") n_mmwiki += "@!@#58@%@";
                      else if (literal == "{") n_mmwiki += "@!@#123@%@";
                      else if (literal == "}") n_mmwiki += "@!@#125@%@";
                      else if (literal == "[") n_mmwiki += "@!@#91@%@";
                      else if (literal == "]") n_mmwiki += "@!@#93@%@";
                    } else {
                      n_mmwiki += "@!@br@%@";
                    }
                  },
                function(s, offset) {
                  n_mmwiki += s;
                }
              );
      return n_mmwiki;
    }

    implementHtml(contents)
    {
      var n_html = "";
      this.applyRe(this.re_html_implement, contents, function(m, offset) {
        n_html += m.prefix;
        var literal = m[1];
        var c = literal[0];
        if (literal == "br") { n_html += "<br />"; }
        else if (c >= '0' && c <= '9') { n_html += "#" + literal + ";"; }
        else { n_html += "&" + literal + ";"; }
      }, function(remain, offset) {
        n_html += remain;
      });
      return n_html;
    }

    startDiv(level, div, div_id = "")
    {
      var p_div = (div_id == "") ? div : div_id;
      this._divs.push_back(p_div);
      this._div_levels.push_back(level);
      this._html += "<div class=\"" + div + "\" >";
      this.startSeq();
    }

    endDiv(level, div)
    {
      this.endSeq();
      if (level < 0) {
        while(!this._divs.empty() && this._divs.back() != div) {
          this._html += "</div> <!--";
          this._html += this._divs.back();
          this._html += "-->";
          this._divs.pop();
          this._div_levels.pop();
        }
        if (_this._divs.length != 0) {
          this._html += "</div><!--";
          this._html += div;
          this._html += "-->";
          this._divs.pop_back();
          this._div_levels.pop_back();
        }
      } else {
        while(!this._div_levels.empty() && this._div_levels.back()  >= level) {
          this._html += "</div><!--";
          this._html += this._divs.back();
          this._html += "-->";
          this._divs.pop_back();
          this._div_levels.pop_back();
        }
        if (!this._div_levels.empty() && this._div_levels.back() == level) {
          this._html += "</div><!--";
          this._html += div;
          this._html += "-->";
          this._divs.pop_back();
          this._div_levels.pop_back();
        }
      }
    }

    startSeq(cl = "", start = "")
    {
      this._seq = "";
      this._seq_class = cl;
      this._seq_start = start;
    }

    endSeq()
    {
      if (this._seq != "" || this._seq_start != "") {
        this._html += this.processSeq(this._seq, this._seq_class, this._seq_start, true, this._min_grade);
        this._seq = "";
        this._seq_class = "";
        this._seq_start = "";
      }
    }

    addSeq(l)
    {
      this._seq += l.line();
      this._seq += " "; // each line has an '\n' to compensate for
    }

    startBullit(bullit_state, l)
    {
	  var level = bullit_state.level;
	  var b = bullit_state.bullit;
		
	  var me = this;
      var open = function(b) {
        if (b == "*") me._html += "<ul>";
        else if (b == "1") me._html += "<ol>";
        else if (b == "2") me._html += "<ol class=\"a\">";
      };

      var current;
      var c_level = 0;
      if (!this._bullits.empty()) {
        current = this._bullits.back();
        c_level = this._bullit_levels.back();
      }

      this.endBullit(level, b);
      if (b != current && c_level == level) {
        this._bullits.push_back(b);
        this._bullit_levels.push_back(level);
        open(b);
      } else if (c_level < level) {
        this._bullits.push_back(b);
        this._bullit_levels.push_back(level);
        open(b);
      }

      this._html += "<li>";
      this._html += this.processSeq(l.line(), "", "", false, this._min_grade);
      this._html += "</li>";
    }

    endBullit(level, b)
    {
	  var me = this;
      var close = function(b) {
        if (b == "*") me._html += "</ul>";
        else if (b == "1" || b == "2") me._html += "</ol>";
      };

      while(!this._bullit_levels.empty() && this._bullit_levels.back() > level) {
        close(this._bullits.back());
        this._bullits.pop_back();
        this._bullit_levels.pop_back();
      }

      if (!this._bullit_levels.empty() && this._bullits.back() != b && this._bullit_levels.back() == level) {
        close(this._bullits.back());
        this._bullit_levels.pop_back();
        this._bullits.pop_back();
      }
    }

    isBullit(line, bullit_state)
    {
      var l = line.line();
      var m = l.match(this.re_bullits);
      if (m) {
        var s = m[1];
        var b = s.substr(s.length - 1, 1);
        var level = s.length;
		bullit_state.level = level;
		bullit_state.bullit = b;
        line.setLine(l.substr(m.index + m[0].length));
        return true;
      }
      return false;
    }

    addHead(toc, level, h, l, cl = "")
    {
      this.endDiv(level, h);
      var dash = (this._one_per_line && !this._no_dashes) ? " dash" : "";
      var html_class = "";
      if (cl != "") {
        html_class = " class=\"" + cl + dash + "\" ";
      }
      this._html += "<" + h + html_class + ">" + this.processSeq(l) + "</" + h + ">";
      this.startDiv(level, h);
    }

    addRubric(toc, level, d, r)
    {
      var rubr = r.trim();
      if (rubr != "") {
        rubr = rubr.substr(0, 1).toUpperCase() + rubr.substr(1).toLowerCase();
        if (rubr.substr(rubr.length - 1, 1) == ".") { rubr = rubr.substr(0, rubr.length - 1); }
        if (rubr.substr(rubr.length - 1, 1) == ":") { rubr += " "; }
        else { rubr += ": "; }
      }
      this.endDiv(level, d);
      this.startDiv(level, d);
      var start = "<span class=\"" + d + "\"" + ">" + this.processSeq(rubr) + "</span>";
      this.startSeq("", start);
    }

    processSeq(s, cl = "", start = "", par = false, min_grade = 0)
    {
      var html = "";
      if (par) html += this.p(cl);

      html += start;

      var ss = "";
      this.applyRe(this.re_prepare_modalities, s, function(m, offset) {
        var c = m[0].substr(0, 1);
        ss += m.prefix;
        if (c == "<") { ss += "-["; }
        else if (c == ">") { ss += "+["; }
      }, function(remain, offset) {
        ss += remain;
      });

      s = ss;
      ss = "";
      this.applyRe(this.re_html_prepare, s, function(m, offset) {
        ss += m.prefix;
        var c = m[0].substr(0, 1);
        if (c == "<") ss += "@!@lt@%@";
        else if (c == ">") ss += "@!@gt@%@";
        else if (c == "&") ss += "@!@amp@%@";
        else if (c == "\"") ss += "@!@quot@%@";
        else if (c == "'") ss += "@!@apos@%@";
      }, function(remain, offset) {
        ss += remain;
      });

      s = ss;
      var root = this.tokenize(s);
      if (min_grade > 1) root = this.filterOutGradedSymptoms(root, min_grade);

      html += this.generateHtml(root);

      if (par) html += "</p>";

      html = this.implementHtml(html);

      return html;
    }

    p(cl)
    {
      if (this._one_per_line && !this._no_dashes) {
        cl += " dash";
      }
      var pp = "<p";
      if (cl != "") { pp += " class=\"" + cl + "\""; }
      pp += ">";
      return pp;
    }

    filterOutGradedSymptoms(root, min_grade)
    {
      return root;
    }

    generateHtml(t)
    {
      var o = "", c = "", cl = "", m = "";
      var content = t.contentIsSet() ? t.content() : "";
      var modifier = t.modifierIsSet() ? t.modifier() : "";
      var pre = "";
      var post = "";

      c = "</span>";
      var content_made = false;
      var sym = false;

      var type = t.type();
      if (type == TOKEN_NOOP) { return ""; }
      else if (type == TOKEN_NONE) { content_made = true; }
      else if (type == TOKEN_ROOT) { content_made = true; }
      else if (type == TOKEN_SYMPTOM) { cl = "symptom"; this.mkHtmlSource(modifier, function(s,e) { pre=s; post=e; }); sym = true; }
      else if (type == TOKEN_RELATION) { cl = "relation"; this.mkHtmlSource(modifier, function(s,e) { pre=s; post=e; }); sym=true; }
      else if (type == TOKEN_CAUSE) { cl = "cause"; this.mkHtmlSource(modifier, function(s,e) { pre=s; post=e; }); sym = true; }
      else if (type == TOKEN_SENTENCE) { cl = "sentence"; this.mkHtmlSource(modifier, function(s,e) { pre=s; post=e; }); }
      else if (type == TOKEN_GROUP) { cl = "group"; this.mkHtmlSource(modifier, function(s,e) { pre=s; post=e; }); }
      else if (type == TOKEN_GRADE2) { cl = "grade2"; }
      else if (type == TOKEN_GRADE3) { cl = "grade3"; }
      else if (type == TOKEN_GRADE4) { cl = "grade4"; }
      else if (type == TOKEN_MODALITY) { cl = "modality"; }
      else if (type == TOKEN_ITALIC) { o = "<i>"; c = "</i>"; }
      else if (type == TOKEN_BOLD) { o = "<b>"; c = "</b>"; }
      else if (type == TOKEN_PAGE) { cl = "page-number"; }
      else if (type == TOKEN_CODE) { cl = "code"; }
      else if (type == TOKEN_TENDENCY) { cl = "tendency"; }
      else if (type == TOKEN_DEFINITION) { return this.mkDefinition(t); }
      else if (type == TOKEN_REMEDY1) { content = this.mkRemedy(1, content); content_made = true; }
      else if (type == TOKEN_REMEDY2) { content = this.mkRemedy(2, content); content_made = true; }
      else if (type == TOKEN_REMEDY3) { content = this.mkRemedy(3, content); content_made = true; }
      else if (type == TOKEN_REMEDY4) { content = this.mkRemedy(4, content); content_made = true; }
      else if (type == TOKEN_ANCHOR) { o = "<span id=\"" + this.mkAnchor(content) + "\" >"; content = ""; }
      else if (type == TOKEN_LINK) { return this.mkLink(t); }
      else if (type == TOKEN_RADIATES) { o = "<span class=\"radiating\">@!@#1878@%@@!@nbsp@%@"; }
      else if (type == TOKEN_WORSE) { o = "<span class=\"modality\">@!@lt@%@@!@nbsp@%@"; }
      else if (type == TOKEN_BETTER) { o = "<span class=\"modality\">@!@gt@%@@!@nbsp@%@"; }
      else if (type == TOKEN_LITERAL) {  o = "["; c = "]"; }
      else if (type == TOKEN_HIGHLIGHT) { cl = "highlight"; }
      else if (type == TOKEN_CLASSIFICATION) { cl = "classification"; }
      else if (type == TOKEN_MODIFIER) { content_made = true; }
      else if (type == TOKEN_CONTENT) { content_made = true; }

      if (this._one_per_line && !this._no_dashes && sym) {
        cl += " dash";
      }

      var html = "";
      var me = this;
      var generateRest = function() {
        var i;
        var v = t.subTokens();
        for(i = 0; i < v.length; i++) {
          html += me.generateHtml(v[i]);
        }
      };

      html += pre;
      if (!content_made) {
        if (cl != "") { o = "<span class=\"" + cl + "\">"; }
        html += o;
        html += content;
        generateRest();
        html += c;
      } else {
        html += content;
        generateRest();
      }
      html += post;

      return html;
    }

    mkLink(t)
    {
      var link = t.subTokens()[0];
      var content = t.subTokens()[2];
	  var href = this.generateHtml(link);
	  var a = this.generateHtml(content);
      return this.linkProvider().mkLinkHRef(href, a);
    }

    linkProvider() { return this._link_provider; }
    setLinkProvider(l) { this._link_provider = l; }

    remedyProvider() { return this._remedy_provider; }
    setRemedyProvider(r) { this._remedy_provider = r; }

    mkDefinition(t)
    {
      var word = t.subTokens()[0];
      var definition = t.subTokens()[2];

      var cl = "definition";
      if (t.content().indexOf("class=\"highlight\"") >= 0) {
        cl = "definition-with-hightlight";
      }
      var r = "<span class=\"" + cl + " hover\">" +
              "<span class=\"tooltip def-tooltip\">" + this.generateHtml(definition) + "</span>" +
              this.generateHtml(word) +
              "</span>";
      return r;
    }

    mkRemedy(grade, s)
    {
      var cl = "remedy";
      cl += grade.toString();
      var uabbrev = this.remedyProvider().unifyAbbrev(s);
      var rem = uabbrev;
      if (this.remedyProvider().hasLink(rem)) {
        var link = "<a href=\"" + this.remedyProvider().getLink(rem) + "\">" + rem + "</a>";
        rem = link;
      }

      var r = "<span class=\"" + cl + " hover\">" +
              "<span class=\"tooltip\">" +
              this.remedyProvider().getLatinName(uabbrev) +
              "</span>" +
              rem +
              "</span>";
      return r;
    }

    mkHtmlSource(source_letter, setter)
    {
      var src = this._header.source(source_letter);
      var pre = "";
      var post = "";
      var ok = false;
      if (src) {
        pre = "<span class=\"source hover\">" +
               "<span class=\"tooltip\">" + src.source() + "</span>" +
               src.htmlMark() +
               "</span>" + src.htmlPre();
        post = src.htmlPost();
        ok = true;
      }
      setter(pre, post);
      return ok;
    }

    tokenize(s)
    {
        var root = new Token(TOKEN_ROOT);
        root.setStart(0);
        root.setEnd(s.length);
        root.setContent(s);

        this.doTokenize(root);
        this.cleanupTokenTree(root, root.content());

        return root;
    }

    doTokenize(root)
    {
      var s = root.content();
      var go_on = true;
      var me = this;

      while(go_on) {
        var ss = "";
        go_on = this.applyRe(this.re_markup, s, function(m, offset) {
          var t = m[1];
          var cmd = t.substr(0, 1);
          var content = m[2];
          var modifier = "";
          var has_modifier = false;
          var type = TOKEN_NONE;

          if (cmd == "2") type = TOKEN_GRADE2;
          else if (cmd == "3") type = TOKEN_GRADE3;
          else if (cmd == "4") type = TOKEN_GRADE4;
          else if (cmd == "M") type = TOKEN_MODALITY;
          else if (cmd == "I") type = TOKEN_ITALIC;
          else if (cmd == "B") type = TOKEN_BOLD;
          else if (cmd == "P") type = TOKEN_PAGE;
          else if (cmd == "Q") type = TOKEN_CODE;
          else if (cmd == "T") type = TOKEN_TENDENCY;
          else if (cmd == "t") {
            type = TOKEN_DEFINITION;
            var idx = content.indexOf("|");
            if (idx >= 0) {
              has_modifier = true;
              modifier = content.substr(0, idx);
              content = content.substr(idx + 1);
            }
          }
          else if (cmd == "R") {
            var grade = 1;
            if (t != "R") grade = parseInt(t.substr(1));
            if (grade ==1) type = TOKEN_REMEDY1;
            else if (grade == 2) type = TOKEN_REMEDY2;
            else if (grade == 3) type = TOKEN_REMEDY3;
            else if (grade == 4) type = TOKEN_REMEDY4;
          }
          else if (cmd == "H") type = TOKEN_HIGHLIGHT;
		  else if (cmd == "C") type = TOKEN_CLASSIFICATION;
		  else if (cmd == "!") type = TOKEN_LITERAL;
		  else if (cmd == "L") {
			  type = TOKEN_LINK;
			  var idx = content.indexOf("|");
			  if (idx >= 0) {
				  has_modifier = true;
				  modifier = content.substr(0, idx);
				  content = content.substr(idx + 1);
			  }
		  }
          else if (cmd == "^") type = TOKEN_RADIATES;
          else if (cmd == "-") type = TOKEN_WORSE;
          else if (cmd == "+") type = TOKEN_BETTER;

          var tok = new Token(type);
          var c_start = m.index + m[1].length;

          if (has_modifier) {
            var t_modifier = new Token(TOKEN_MODIFIER);
            t_modifier.setStart(offset + c_start);
            t_modifier.setEnd(offset + c_start + modifier.length);
            t_modifier.setContent(modifier);

            var t_pipe = new Token(TOKEN_NOOP);
            t_pipe.setStart(t_modifier.end());
            t_pipe.setEnd(t_modifier.end() + 1);

            var t_content = new Token(TOKEN_CONTENT);
            t_content.setStart(t_pipe.end());
            t_content.setEnd(t_pipe.end() + content.length);
            t_content.setContent(content);

            me.insertToken(root, t_modifier);
            me.insertToken(root, t_pipe);
            me.insertToken(root, t_content);
          } else {
            tok.setContent(content);
          }

          var c_end = c_start + m[2].length;
          tok.setStart(offset + c_start);
          tok.setEnd(offset + c_end);

          var pre = new Token(TOKEN_NOOP);
          var pre_start = m.index;
          var pre_end = c_start;
          pre.setStart(offset + pre_start);
          pre.setEnd(offset + pre_end);

          var post = new Token(TOKEN_NOOP);
          var post_start = c_end;
          var post_end = pre_start + m[0].length;
          post.setStart(offset + post_start);
          post.setEnd(offset + post_end);

          me.insertToken(root, tok);
          me.insertToken(root, pre);
          me.insertToken(root, post);

          ss += m.prefix;
          ss += (t.length == 2) ? "   " : "  ";
          if (has_modifier) {
            ss += modifier;
            ss += " ";
          }
          ss += content;
          ss += " ";
        }, function(remain, offset) {
          ss += remain;
        });

        s = ss;
      }

      go_on = true;
      while(go_on) {
        ss = "";
        go_on = this.applyRe(this.re_symptom, s, function(m, offset) {
          var m_type = m[1];
          var src = m[2];
          var content = m[3];

          var type = TOKEN_NONE;
          if (m_type == "S") type = TOKEN_SYMPTOM;
          else if (m_type == "R") type = TOKEN_RELATION;
          else if (m_type == "C") type = TOKEN_CAUSE;
          else if (m_type == "G") type = TOKEN_GROUP;
          else if (m_type == "Z") type = TOKEN_SENTENCE;

          var tok = new Token(type);
          tok.setContent(content);
          tok.setModifier(src);
          var c_start = m.index + m[1].length + 1 + m[2].length;
          var c_end = c_start + m[3].length;
          tok.setStart(offset + c_start);
          tok.setEnd(offset + c_end);

          var pre = new Token(TOKEN_NOOP);
          var pre_start = m.index;
          var pre_end = c_start;
          pre.setStart(offset + pre_start);
          pre.setEnd(offset + pre_end);

          var post = new Token(TOKEN_NOOP);
          var post_start = c_end;
          var post_end = pre_start + m[0].length;
          post.setStart(offset + post_start);
          post.setEnd(offset + post_end);

          me.insertToken(root, tok);
          me.insertToken(root, pre);
          me.insertToken(root, post);

          ss += m.prefix;
          ss += "  ";
          ss += (src == "") ? "" : " ";
          ss += content;
          ss += " ";
        }, function(remain, offset) {
          ss += remain;
        });

        s = ss;
      }
    }

    insertToken(t, ins)
    {
      if (t.contains(ins)) {
        var v = t.subTokens();
        var i;
        var vv = new Array();

        for(i = 0; i < v.length; i++) {
          var tt = v[i];
          if (ins.equal(tt)) {
            // already inserted, do nothing
            vv.push(tt);
          } else if (ins.contains(tt)) {
            this.insertToken(ins, tt);
          } else {
            vv.push(tt);
          }
        }

        t.setSubTokens(vv);
        v = t.subTokens();
        vv = new Array();

        var inserted = false;
        for(i = 0; i < v.length; i++) {
          var tt = v[i];
          if (tt.equal(ins)) {
            // already inserted, do nothing
            vv.push(tt);
            inserted = true;
          } else if (tt.contains(ins)) {
            this.insertToken(tt, ins);
            vv.push(tt);
            inserted = true;
          } else if (tt.end() <= ins.start()) {
            vv.push(tt);
          } else { // ins.end() < tt.start()
            if (!inserted) { inserted = true; vv.push(ins); }
            vv.push(tt);
          }
        }
        if (!inserted) { vv.push(ins); }
        t.setSubTokens(vv);
      }
    }

    cleanupTokenTree(t, original)
    {
      var v = t.subTokens();

      if (v.length == 0) {
        return;
      } else {
        var nv = new Array();
        var last_end = t.start();
        var i;
        for(i = 0; i < v.length; i++) {
          var tt = v[i];
          var start = tt.start();
          if (start > last_end) {
            var nt = new Token(TOKEN_NONE);
            nt.setStart(last_end);
            nt.setEnd(start);
            nt.setContent(original.substr(nt.start(), nt.length()));
            nv.push(nt);
          }
          last_end = tt.end();
          nv.push(tt);
        }

        if (last_end < t.end()) {
          var nt = new Token(TOKEN_NONE);
          nt.setStart(last_end);
          nt.setEnd(t.end());
          nt.setContent(original.substr(nt.start(), nt.length()));
          nv.push(nt);
        }

        t.contentUnSet();

        for(i = 0; i < nv.length; i++) {
          this.cleanupTokenTree(nv[i], original);
        }

        t.setSubTokens(nv);
      }
    }

    extractHeader()
    {
      var h = new Header();
      var in_head = true;
      var lines = this._mmwiki.split("\n");
      var i;
      for(i = 0; i < lines.length; i++) {
        var l = new Line(lines[i]);
        if (l.isKey(":begin")) { return h; }
        else if (l.isKeyVal(":book")) { h.setBook(l.value()); }
        else if (l.isKeyVal(":edition")) { h.setEdition(l.value()); }
        else if (l.isKeyVal(":author")) { h.setAuthor(l.value()); }
        else if (l.isKeyVal(":editor")) { h.setEditor(l.value()); }
        else if (l.isKeyVal(":latin-name")) { h.setLatinName(l.value()); }
        else if (l.isKeyVal(":source")) { h.addSource(l.value()); }
        else if (l.isKeyVal(":miasm")) { h.setMiasm(l.value()); }
        else if (l.isKeyVal(":tendency")) { h.setTendency(l.value()); }
        else if (l.isKeyVal(":class")) { h.setClassification(l.value()); }
      }
      return h;
    }

    extractContents(lang)
    {
      var matches = this._mmwiki.matchAll(this.re_section);

      var first_language = -1;
      var en_language = -1;
      var has_language = -1;

      for(const match of matches) {
          var l = match[1];
          var langs = l.split(',');
          var languages = new Array();
          var i;
          for(i = 0; i < langs.length; i++)  {
            var lng = langs[i].trim();
            languages.push(lng);
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
        var result = this._mmwiki.substr(idx);
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
