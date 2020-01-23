class MMWiki
{
    class Line
    {
      constructor(l)
      {
        this.line = l;
        this.line_ci = toLowerCase(l.trim());
      }

      isKeyVal(key)
      {
         if (this.line_ci.startsWith(key)) {
           idx = key.length;
           len = this.line_ci.length;
           while(idx < len && this.line_ci[idx] == ' ') { idx += 1; }
           if (idx == len) {
             this.value = """;
             return true;
           } else if (this.line_ci[idx] == '=') {
             s = this.line.trimStart();
             this.value = s.substr(idx + 1);
             return true;
           }
         } else {
           return false;
         }
      }

      value() { return this.value; }

      isKey(key) { return this.line_ci.startsWith(key); }

      isEmpty() { return this.line_ci == ""; }

      line() { return this.line; }
    }

    ///////////////////////////////////////////////////////////////

    constructor()
    {
      this.css_provider = new CssProvider();
      this.re_prepare_modalities = /[<>]\[/;
      this.re_html_prepare = /[<>&']/;
      this.re_symptom = /([CSGZR])([A-Z]*)[{]([^}{]*)[}]/;
      this.re_section = /^\s*:begin\[([a-z, ]+)\]\s*$/;
    }

    toHtml(mmwiki, one_per_line = false, language = 'en')
    {
        this.mmwiki = mmwiki;
        var contents = extractContents(language)
    }

    extractContents(lang)
    {
      var res = this.mmwiki.match(this.re_section);
      var has_language = false;
      while(res && !has_language) {
        var l = res[1];
        var languages = l.split(','');
        if (languages.includes(lang)) {
          has_language = true;
        }
      }
      while(this.mmwiki.match(this.re_section))

    }

}
