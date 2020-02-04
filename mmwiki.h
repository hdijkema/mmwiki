#ifndef MMWIKI_H
#define MMWIKI_H

#include <string>
#include <list>
#include <map>
#include <functional>
#include <regex>
#include <algorithm>
#include <cctype>
#include <sstream>
#include <mmwiki_css.h>

#define MMWIKI_UNUSED(a)   (void(a))

inline std::string mmwiki_toLower(std::string s)
{
    std::transform(s.begin(),
                   s.end(),
                   s.begin(),
                   [](char c){ return std::tolower(c); }
    );
    return s;
}

inline std::string mmwiki_trim(std::string s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
         return !std::isspace(static_cast<unsigned char>(ch));
     }));
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !std::isspace(static_cast<unsigned char>(ch));
    }).base(), s.end());

    return s;
}

inline std::string mmwiki_ltrim(std::string s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
         return !std::isspace(static_cast<unsigned char>(ch));
     }));
    return s;
}

inline bool mmwiki_replace(std::string& str, const std::string& from, const std::string& to)
{
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}

inline bool mmwiki_replaceAll(std::string &str, const std::string &from, const std::string &to)
{
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos) return false;

    size_t len = from.length();
    size_t tlen = to.length();
    while(start_pos != std::string::npos) {
        str.replace(start_pos, len, to);
        start_pos = str.find(from, start_pos + tlen);
    }
    return true;
}

inline bool mmwiki_startsWith(const std::string &s, const std::string &with)
{
    return (s.find(with) == 0);
}

inline bool mmwiki_key(const std::string &s_ci, const std::string &key,
                       const std::string & s, std::string &val)
{
    if (mmwiki_startsWith(s_ci, key)) {
        size_t idx = static_cast<size_t>(key.size());
        size_t len = s_ci.length();
        while(idx < len && s_ci[idx] == ' ') {
            idx += 1;
        }
        if (idx == len) {
            val = "";
            return true;
        } else if (s_ci[idx] == '=') {
            std::string ltrim_s = mmwiki_ltrim(s);
            val = ltrim_s.substr(idx + 1);
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

inline bool mmwiki_begin(const std::string &s)
{
    return mmwiki_startsWith(s, ":begin[");
}


inline std::vector<std::string> mmwiki_split(std::string v, char delim)
{
    std::vector<std::string> parts;
    std::istringstream f(v);
    std::string s;
    while (std::getline(f, s, delim)) {
        parts.push_back(s);
    }
    return parts;
}

inline std::string mmwiki_mkid(std::string prefix, int nr)
{
    char id[100];
    sprintf(id, "%s%08d", prefix.c_str(), nr);
    std::string s(id);
    return s;
}


#define MMWIKI_PROP(type, v, setter) \
    private: \
      Prop<type> _##v; \
    public: \
      const type &v() const { return _##v.get(); } \
      type &v() { return _##v.get(); } \
      void setter(const type &w) { _##v.set(w); } \
      bool v##IsSet() const { return _##v.isSet(); } \
      void v##UnSet() { _##v.unSet(); }

#define MMWIKI_SP(p, set) \
    { \
        std::string val; \
        if (mmwiki_key(lineCi, p, line, val) || mmwiki_begin(lineCi)) { \
            set(mmwiki_trim(val)); \
        } \
    }

class MMWiki
{
public:
    template <class T> class Prop
    {
    private:
        bool _is_set;
        T    _v;
    public:
        Prop() { _is_set = false; }
    public:
        const T &get() const     { return _v; }
        T &get()                 { return _v; }
        void set(T v)            { _is_set = true; _v = v; }
        bool isSet() const       { return _is_set; }
        void unSet()             { _is_set = false; }
    };

private:
    class Line
    {
    private:
        std::string _line_ci;
        std::string _line;
        std::string _value;

    public:
        Line(const std::string &l)
        {
            _line = l;
            _line_ci = mmwiki_trim(mmwiki_toLower(l));
        }

    public:
        bool isKeyVal(const std::string &key)       { return mmwiki_key(_line_ci, key, _line, _value); }
        std::string value()                         { return _value; }
        bool operator ==(const std::string &key)    { return mmwiki_startsWith(_line_ci, key); }
        bool isEmpty() const                        { return _line_ci == ""; }
        std::string line() const                    { return _line; }
    };

public:
    class Token {
    public:
        enum Tok    { NOOP = -2, ROOT = -1, NONE = 0, MODIFIER = -3, CONTENT = -4,
                      SYMPTOM = 'S' + 100, GROUP = 'G' + 100, RELATION = 'R' + 100, CAUSE = 'C' + 100, SENTENCE = 'Z' + 100,
                      GRADE2 = '2' + 200, GRADE3 = '3' + 200, GRADE4 = '4' + 200,
                      MODALITY = 'M' + 200, CLASSIFICATION = 'C' + 200, TENDENCY = 'T' + 200,
                      REMEDY1 = 'R' + 300, REMEDY2 = 'R' + 301, REMEDY3 = 'R' + 302, REMEDY4 = 'R'+ 303,
                      DEFINITION = 't' + 200, INLINE_IMAGE = 'i' + 200,
                      RADIATES = '^' + 200, BETTER = '+' + 200, WORSE = '-' + 200,
                      BOLD = 'B' + 200, ITALIC = 'I' + 200,
                      PAGE = 'P' + 200, CODE = 'Q' + 200, HIGHLIGHT = 'H' + 200,
                      LINK = 'L' + 200, LITERAL = '!' + 200, ANCHOR = 'N' + 200
                    };
    public:
        MMWIKI_PROP(Tok, type, setType)
        MMWIKI_PROP(std::string, content, setContent)
        MMWIKI_PROP(std::string, modifier, setModifier)
        MMWIKI_PROP(int, start, setStart)
        MMWIKI_PROP(int, end, setEnd)
        MMWIKI_PROP(std::vector<Token>, subTokens, setSubTokens)
    public:
        Token() {}
        Token(Tok a) { setType(a); }

    public:
        bool contains(const Token &other) const { return other.start() >= start() &&
                                                   other.end() <= end();
                                          }
        bool equal(const Token &b) const { return b.start() == start() && b.end() == end(); }
        int length() const { return end() - start(); }
    public:
        bool hasA(std::function<bool(const Token &t)> f) const {
            if (f(*this)) { return true; }
            else {
                for(const Token &tt: subTokens()) {
                    if (tt.hasA(f)) return true;
                }
                return false;
            }
        }

    public:
        bool operator< (const Token &b) const { return end() <= b.start(); }
    };

public:
    class Symptom {
    public:
        enum Type { NONE = 0, SYMPTOM = 1, CAUSE = 2, RELATION = 3 };
        class Graded
        {
            MMWIKI_PROP(int, start, setStart)
            MMWIKI_PROP(int, end, setEnd)
            MMWIKI_PROP(int, grade, setGrade)
            public:
                int length() const { return end() - start(); }
        };

        MMWIKI_PROP(Type, type, setType)
        MMWIKI_PROP(std::vector<Graded>, grades, setGrades)
        MMWIKI_PROP(std::string, symptom, setSymptom)
    public:
        int maxGrade() const
        {
            int max = 1;
            for(const Graded &g : grades())
                max = (g.grade() > max) ? g.grade() : max;
            return max;
        }
        int gradedCount() const
        {
            return static_cast<int>(grades().size());
        }
        int gradeOf(int idx) const
        {
            const std::vector<Graded> &g = grades();
            return g[static_cast<size_t>(idx)].grade();
        }
        const std::string textOf(int idx) const
        {
            const std::vector<Graded> &g = grades();
            const Graded &gr = g[static_cast<size_t>(idx)];
            std::string s = symptom().substr(static_cast<size_t>(gr.start()),
                                             static_cast<size_t>(gr.length())
                                             );
            return s;
        }
    };

    class RemedyProvider
    {
    public:
        RemedyProvider() {}
        RemedyProvider(const RemedyProvider &other) = default;
        virtual ~RemedyProvider() {}

    public:
        RemedyProvider &operator =(const RemedyProvider &other) = default;

    public:
        virtual std::string getLatinName(const std::string &uniform_abbrev)
        {
            return uniform_abbrev;
        }

        virtual std::string getLink(const std::string &uniform_abbrev)
        {
            ((void) uniform_abbrev);
            return "";
        }

        virtual bool hasLink(const std::string &uniform_abbrev)
        {
            ((void)uniform_abbrev);
            return false;
        }

        virtual std::string unifyAbbrev(const std::string &abbrev)
        {
            std::string s = mmwiki_trim(abbrev);
            if (s != "") {
                s = mmwiki_toLower(s);
                s[0] = static_cast<char>(std::toupper(s[0]));
                if (s.back() == '.') {
                    s.pop_back();
                }
            }
            return s;
        }
    };

    class CssProvider
    {
    public:
        virtual std::string css() const
        {
            return std::string(MMWIKI_CSS);
        }

    public:
        virtual std::string escape(const std::string &to_html) { return to_html; }
    public:
        CssProvider() {}
        CssProvider(const CssProvider &other) = default;
        virtual ~CssProvider() {}
    public:
        CssProvider &operator =(const CssProvider &other) = default;
    };

    class ImageSrc {
        MMWIKI_PROP(std::string, author, setAuthor)
        MMWIKI_PROP(std::string, authorUrl, setAuthorUrl)
        MMWIKI_PROP(std::string, source, setSource)
        MMWIKI_PROP(std::string, sourceUrl, setSourceUrl)
        MMWIKI_PROP(std::string, license, setLicense)
        MMWIKI_PROP(std::string, licenseUrl, setLicenseUrl)
        MMWIKI_PROP(std::string, imageSrc, setImageSrc)
        MMWIKI_PROP(std::string, imageAlt, setImageAlt)
        MMWIKI_PROP(std::string, imgId, setImgId)
    };

    class ImageProvider
    {
    public:
        virtual std::string getImageSrc(const std::string &image)
        {
            return image;
        }
        virtual std::vector<ImageSrc> getImageSrcs(const std::string &uniform_abbrev)
        {
            MMWIKI_UNUSED(uniform_abbrev);
            return std::vector<ImageSrc>();
        }
    public:
        ImageProvider() {}
        ImageProvider(const ImageProvider &other) = default;
        virtual ~ImageProvider() {}
    public:
        ImageProvider &operator = (const ImageProvider &other) = default;
    };

    class IncludeProvider
    {
    public:
        virtual std::string getPage(const std::string &pagename)
        {
            MMWIKI_UNUSED(pagename);
            return "";
        }
    public:
        IncludeProvider() {}
        IncludeProvider(const IncludeProvider &other) = default;
        virtual ~IncludeProvider() {}
    public:
        IncludeProvider &operator = (const IncludeProvider &other)= default;
    };

    class LinkProvider {
    private:
        mutable int link_id;
        MMWIKI_PROP(std::string, namePrefix, setNamePrefix)
    public:
        virtual std::string mkLinkId(std::string name) const
        {
            std::string a = namePrefix();
            a.append(name);
            return a;
        }
        virtual std::string mkLinkHRef(std::string href, std::string content) const
        {
            std::string id = "link_id_"; id.append(std::to_string(++link_id));
            std::string a = "<span class=\"link\" id=\"";
            a.append(id).append("\" >").
                    append("<a href=\"").append(href).append("\"").
                    append("onmouseenter=\"getElementById('").append(id).append("').className='link_hover';\" ").
                    append("onmouseleave=\"getElementById('").append(id).append("').className='link';\" ").
                    append(">").
                    append(content).
                    append("&#8599;").append("</a>").
                    append("</span>");
            if (link_id > 1000000000) { link_id = 0; }

            return a;
        }
    public:
        LinkProvider() { link_id = 0; }
        LinkProvider(const LinkProvider &other) = default;
        virtual ~LinkProvider() {}
    public:
        LinkProvider &operator = (const LinkProvider &other) = default;
    };

    class Source {
        MMWIKI_PROP(char, letter, setLetter)
        MMWIKI_PROP(std::string, source, setSource)
        MMWIKI_PROP(std::string, htmlMark, setHtmlMark)
        MMWIKI_PROP(std::string, htmlPre, setHtmlPre)
        MMWIKI_PROP(std::string, htmlPost, setHtmlPost)
    public:
        bool fromString(std::string s)
        {
            // letter,source,indicator in text,[pre_html],[post_html]

            std::vector<std::string> parts = mmwiki_split(s, ',');
            size_t size = parts.size();

            if (size >= 3) {
                std::string letter = mmwiki_trim(parts[0]);
                if (letter.length() == 1) {
                    setLetter(letter[0]);
                    setSource(mmwiki_trim(parts[1]));
                    setHtmlMark(mmwiki_trim(parts[2]));

                    if (size >= 4) {
                        setHtmlPre(parts[3]);
                        if (size >= 5) {
                            setHtmlPost(parts[4]);
                        }
                    }

                    return true;
                }
            }

            return false;
        }
    };

    typedef std::map<char, Source> SourceMap;

    class Header {
        MMWIKI_PROP(std::string, book, setBook)
        MMWIKI_PROP(std::string, author, setAuthor)
        MMWIKI_PROP(std::string, edition, setEdition)
        MMWIKI_PROP(std::string, editor, setEditor)
        MMWIKI_PROP(std::string, abbrev, setAbbrev)
        MMWIKI_PROP(std::string, latinName, setLatinName)
        MMWIKI_PROP(SourceMap, sources, setSources)
        MMWIKI_PROP(std::string, tendency, setTendency)
        MMWIKI_PROP(std::string, miasm, setMiasm)
        MMWIKI_PROP(std::string, classification, setClassification)
    };

    class MetaProvider
    {
    private:
        MMWiki        *_mmwiki;
        int _id;
    public:
        void setMMWiki(MMWiki *m) { _mmwiki = m; }
    public:
        virtual std::string getMeta()
        {
            Header        &h = _mmwiki->header();
            ImageProvider *img = _mmwiki->imageProvider();
            CssProvider   *css = _mmwiki->cssProvider();

            std::string html;

            auto addTblRow = [&css](std::string &html, const std::string &l, const std::string &v) {
                html.append("<tr>");
                html.append("<td>");html.append(css->escape(l));html.append("</td>");
                html.append("<td>");html.append(css->escape(v));html.append("</td>");
                html.append("</tr>");
            };

            auto addRow = [&html, addTblRow](const std::string &l, const std::string &v) {
                addTblRow(html, l, v);
            };

            auto href = [&css](const std::string &ref, const std::string &txt) {
                std::string s = "<a href=\"";
                s.append(ref);
                s.append("\">");
                s.append(css->escape(txt));
                s.append("</a>");
                return s;
            };

            html.append("<div class=\"meta\">");
            html.append("<table>");

            if (h.classificationIsSet() && h.classification() != "") { addRow(lbClassification(), h.classification()); }
            if (h.tendencyIsSet() && h.tendency() != "") { addRow(lbTendency(), h.tendency()); }
            if (h.miasmIsSet() && h.miasm() != "") { addRow(lbMiasm(), h.miasm()); }

            std::vector<ImageSrc> img_srcs;
            if (h.abbrevIsSet()) {
                std::string abbrev = h.abbrev();
                img_srcs = img->getImageSrcs(abbrev);
                for(ImageSrc &img_src : img_srcs) {
                    std::string img_id = mmwiki_mkid("abbrev_img", ++_id);
                    img_src.setImgId(img_id);

                    std::string image = "<img onclick=\"getElementById('";
                    image.append(img_id);
                    image.append("').style.display='block';\" ");
                    image.append("src=\"");
                    image.append(img_src.imageSrc());
                    image.append("\" alt=\"");
                    image.append(img_src.imageAlt());
                    image.append("\" />");
                    image.append("<div class=\"hover\">");
                    image.append("<div class=\"tooltip\">");
                    image.append("<table>");
                    addTblRow(image, lbImageAuthor(),
                              href(img_src.authorUrl(), img_src.author()).append(" ").
                              append(href(img_src.sourceUrl(), img_src.source())));
                    addTblRow(image, lbLicense(),
                              href(img_src.licenseUrl(), img_src.license()));
                    image.append("</table>");
                    image.append("</div>");
                    image.append(lbSourceInfo());
                    image.append("</div>");

                    addRow(lbImage(), image);
                }
            }

            html.append("</table>");
            html.append("</div>");

            for(ImageSrc &img_src : img_srcs) {
                std::string img_id = img_src.imgId();
                html.append("<div class=\"bigpicture\" id=\"").append(img_id).append("\" ");
                html.append("onclick=\"getElementById('").append(img_id).append("').style.display='none';\" >");
                html.append("<img src=\"").append(img_src.imageSrc()).append("\" alt=\"").append(img_src.imageAlt()).
                        append("\" />").append("</div>");
            }

            return html;
        }

        virtual std::string lbClassification() { return "Classification"; }
        virtual std::string lbTendency() { return "Tendency"; }
        virtual std::string lbMiasm() { return "Miasm"; }
        virtual std::string lbImage() { return "Picture"; }
        virtual std::string lbImageAuthor() { return "Author"; }
        virtual std::string lbLicense() { return "License"; }
        virtual std::string lbSourceInfo() { return "Source Information"; }
    public:
        MetaProvider() { _id = 0; }
        MetaProvider(const MetaProvider &other) = default;
        virtual ~MetaProvider() {}
    public:
        MetaProvider &operator = (const MetaProvider &other) = default;
    public:
        MMWIKI_PROP(std::string, language, setLanguage)
    };

    class HighlightWords
    {
    private:
        std::list<std::string> _words;
    public:
        std::list<std::string> words() { return _words; }
        void                   addWord(const std::string &word) { _words.push_back(word); }
        void                   clear() { _words.clear(); }
    };

    class Toc {
    private:
        std::vector<int>            _toc_levels;
        std::vector<std::string>    _toc_txts;
        std::vector<std::string>    _toc_refs;
    public:
        Toc &append(int level, const std::string &toc_txt, const std::string &toc_ref)
        {
            _toc_levels.push_back(level);
            _toc_txts.push_back(toc_txt);
            _toc_refs.push_back(toc_ref);
            return *this;
        }

        size_t size() const
        {
            return _toc_levels.size();
        }

        bool empty() const
        {
            return _toc_levels.empty();
        }

        void clear()
        {
            _toc_levels.clear();
            _toc_txts.clear();
            _toc_refs.clear();
        }

        int level(int idx) const
        {
            return _toc_levels[static_cast<size_t>(idx)];
        }

        std::string tocTxt(int idx) const
        {
            return _toc_txts[static_cast<size_t>(idx)];
        }

        std::string tocRef(int idx) const
        {
            return _toc_refs[static_cast<size_t>(idx)];
        }
    };

private:
    bool             _owns_providers;

    CssProvider     *_css_provider;
    ImageProvider   *_image_provider;
    MetaProvider    *_meta_provider;
    RemedyProvider  *_remedy_provider;
    LinkProvider    *_link_provider;
    IncludeProvider *_incl_provider;

    HighlightWords  _highlight_words;
    Header          _header;

    std::string     _mm_wiki;
    std::string     _html;

    // Table of contents
    Toc             _toc;
    Toc             _pages;

    // Take symptoms of minimum grade
    int             _min_grade;

    // Symptoms
    std::string                                 _current_rubric;
    std::map<std::string, std::vector<Symptom>> _symptoms;
    std::vector<Symptom>                        _empty_symptoms;

    // State
    std::string                 _req_language;
    bool                        _one_per_line;
    bool                        _no_dashes;
    std::vector<std::string>    _divs;
    std::vector<int>            _div_levels;
    std::string                 _seq;
    std::string                 _seq_class;
    std::string                 _seq_start;
    std::vector<std::string>    _bullits;
    std::vector<int>            _bullit_levels;
    std::vector<int>            _tables_cell;
    std::vector<int>            _tables_cells;

private:
    // Regexes
    std::regex re_prepare_modalities;
    std::regex re_html_prepare;
    std::regex re_symptom;
    std::regex re_markup;
    std::regex re_modalities;
    std::regex re_literals;
    std::regex re_html_implement;
    std::regex re_symptom_grade;
    std::regex re_anchor;
    std::regex re_cleanup_open;
    std::regex re_cleanup_sym_open;
    std::regex re_cleanup_close;
    std::regex re_section;
    std::regex re_end;
    std::regex re_bullits;
    std::regex re_width;

public:
    MMWiki()
    {
        _owns_providers = true;

        _image_provider = new ImageProvider();
        _css_provider = new CssProvider();
        _remedy_provider = new RemedyProvider();
        _link_provider = new LinkProvider();
        _incl_provider = new IncludeProvider();

        _meta_provider = new MetaProvider();
        _meta_provider->setMMWiki(this);

        re_prepare_modalities = std::regex("[<>]\\[");
        re_html_prepare = std::regex("([<>&'])");
        re_symptom = std::regex("([CSGZR])([A-Z]*)[{]([^}{]*)[}]");
        re_markup = std::regex("([234PQHNMITBRCLti^!+-]|R[234])\\[([^\\]\\[]*)\\]");
        re_modalities = std::regex("([<>])\\[([^\\]\\[]*)\\]");
        re_literals = std::regex("\\\\[:{}\\[\\]]|\\[BR\\]");
        re_html_implement = std::regex("[@][!][@]([^@]+)[@][%][@]");
        re_symptom_grade = std::regex("([234])\\[([^\\]\\[]*)\\]");
        //re_section = std::regex("^\\s*:begin\\[([a-z,]+)\\]\\s*$");
        re_section = std::regex("\\s*:begin\\[([a-z,]+)\\]\\s*\n");     // cannot use ^ and $. on OS X std::regex does no multiline
        //re_end = std::regex("^\\s*:end\\s*$");
        re_end = std::regex("\\s*:end\\s*");                            // cannot use ^ and $. on OS X std::regex does no multiline
																		// however, :end could not be terminated by a '\n'.
        re_anchor = std::regex("N\\[[^\\]]+\\]");
        re_cleanup_sym_open = std::regex("([CSGZRP])([A-Z]*)[{]");
        re_cleanup_open = std::regex("([234PQHNMITBRCLti^!<>]|R[234])\\[([^|]+[|]){0,1}");
        re_cleanup_close = std::regex("(\\]|[}])");
        re_bullits = std::regex("^([*12]+)\\s");
        re_width = std::regex("([0-9.]+)(%|em|pt|mm|cm){0,1}");

        _min_grade = 0;
    }
    MMWiki(const MMWiki &other) = default;
    ~MMWiki()
    {
        if (_owns_providers) {
            delete _meta_provider;
            delete _image_provider;
            delete _css_provider;
            delete _link_provider;
            delete _remedy_provider;
        }
    }

public:
    MMWiki &operator = (const MMWiki &other) = default;

public:
    void setCssProvider(CssProvider *provider) { delete _css_provider; _css_provider = provider; }
    void setImageProvider(ImageProvider *provider) { delete _image_provider; _image_provider = provider; }
    void setMetaProvider(MetaProvider *provider) { delete _meta_provider; _meta_provider = provider;_meta_provider->setMMWiki(this); }
    void setLinkProvider(LinkProvider *l) { delete _link_provider; _link_provider = l; }
    void setRemedyProvider(RemedyProvider *r) { delete _remedy_provider; _remedy_provider = r; }
    void setIncludeProvider(IncludeProvider *i) { delete _incl_provider; _incl_provider = i; }

    void setHighlightWords(const HighlightWords &words) { _highlight_words = words; }

    CssProvider *cssProvider() { return _css_provider; }
    ImageProvider *imageProvider() { return _image_provider; }
    MetaProvider *metaProvider() { return _meta_provider; }
    RemedyProvider *remedyProvider() { return _remedy_provider; }
    LinkProvider *linkProvider() { return _link_provider; }
    IncludeProvider *includeProvider() { return _incl_provider; }

    HighlightWords &highlightWords() { return _highlight_words; }
    Header &header() { return _header; }

    void setMinGrade(int grade) { _min_grade = grade; }

public:
    void generateSymptoms(const std::string &mmwiki, const std::string &language = std::string("en"))
    {
        _mm_wiki = mmwiki;
        std::string contents = extractContents(language);
        std::vector<std::pair<int, char>> literals;
        contents = prepareForSymptoms(contents);

        std::vector<std::string> lines = mmwiki_split(contents, '\n');

        std::string rubric = "";
        std::string seq;

        _symptoms.clear();


        auto processSeq = [this, &rubric, &seq]() {
            if (seq == "") return;

            Token root = tokenize(seq);
            std::vector<Symptom> v = extractSymptoms(root);

            _symptoms.insert(std::pair<std::string, std::vector<Symptom>>(rubric, v));

            seq = "";
        };


        for(std::string ln : lines) {
            Line line(ln);

            if (line == ":head" || line == ":head2" || line==":source") {
                rubric = "";
            } else if (line.isKeyVal(":rubric")) {
                processSeq();
                rubric = line.value();
            } else if (line.isKeyVal(":rubric2")) {
                processSeq();
                rubric = line.value();
            } else {
                seq.append(line.line());
            }
        }

        processSeq();
    }

    std::vector<std::string> symptomRubrics() const
    {
        std::vector<std::string> rubrics;
        std::map<std::string, std::vector<Symptom>>::const_iterator it = _symptoms.begin();
        while(it != _symptoms.end()) {
            if (it->second.size() > 0)
                rubrics.push_back(it->first);
            it++;
        }
        return rubrics;
    }

    const std::vector<Symptom> &symptoms(const std::string &rubric) const
    {
        std::map<std::string, std::vector<Symptom>>::const_iterator it = _symptoms.find(rubric);
        if (it != _symptoms.end()) {
            return it->second;
        } else {
            return _empty_symptoms;
        }
    }

private:
    std::string prepareForSymptoms(std::string s)
    {
        std::string ns;
        applyRe(re_literals, s, [&ns](std::smatch &m, int ) {
            ns.append(m.prefix());
            std::string s_literal = m.str();
            char literal = s_literal[0];
            if (literal == '\\') {
                literal = s_literal[1];
                ns.append(1u, literal);
            } else {
                // BR
                ns.append(" ");
            }
        }, [&ns](std::string remain, int) {
            ns.append(remain);
        });

        s = ns;
        ns = "";
        applyRe(re_modalities, s, [&ns](std::smatch &m, int) {
            ns.append(m.prefix());
            std::string open = m[1];
            if (open == "<") { ns.append("-"); }
            else if (open == ">") { ns.append("+"); }
            ns.append("[");
            ns.append(m[2]);
            ns.append("]");
        }, [&ns](std::string remain, int) {
            ns.append(remain);
        });

        return ns;
    }

    std::vector<Symptom> extractSymptoms(const Token &t)
    {
        Token::Tok type = t.type();
        Symptom::Type st = Symptom::NONE;
        switch(type) {
        case Token::SYMPTOM: st = Symptom::SYMPTOM;break;
        case Token::RELATION: st = Symptom::RELATION;break;
        case Token::CAUSE: st = Symptom::CAUSE;break;
        default: break;
        }
        std::vector<Symptom> v;
        if (st != Symptom::NONE) {
            Symptom s;
            Symptom::Graded g;

            s.setType(st);
            g.setStart(0);g.setGrade(1);
            int pos = 0;

            extractSymptom(s, g, pos, t);

            g.setEnd(pos);

            std::vector<Symptom::Graded> &vg = s.grades();
            vg.push_back(g);

            std::sort(vg.begin(), vg.end(), [](const Symptom::Graded &a, const Symptom::Graded &b) {
                if (a.start() < b.start()) return true;
                else if (a.start() > b.start()) return false;
                else return a.grade() < b.grade();
            });

            std::vector<Symptom::Graded> nvg;
            std::vector<Symptom::Graded>::iterator it, it1;
            for(it = vg.begin(); it != vg.end(); it++) {
                it1 = it;it1++;
                if (it1 == vg.end()) {
                    nvg.push_back(*it);
                } else {
                    while (it1 != vg.end()) {
                        if (it->start() == it1->start() && it->end() == it1->end()) {
                            it++; it1++;
                            if (it1 == vg.end()) nvg.push_back(*it);
                        } else {
                            nvg.push_back(*it);
                            break;
                        }
                    }
                }
            }
            s.setGrades(nvg);

            v.push_back(s);

            return v;
        } else {
            for(const Token &tt : t.subTokens()) {
                std::vector<Symptom> vv = extractSymptoms(tt);
                v.insert(v.end(), vv.begin(), vv.end());
            }
        }
        return v;
    }

    void extractSymptom(Symptom &s, Symptom::Graded &g, int &pos, const Token &t)
    {
        Token::Tok type = t.type();
        int grade = -1;
        switch(type) {
        case Token::GRADE2: grade = 2;break;
        case Token::GRADE3: grade = 3;break;
        case Token::GRADE4: grade = 4;break;
        default: break;
        }

        auto add = [this, &s, &pos, t](Symptom::Graded &g) {
            const Token *c = &t;

            Token::Tok type = t.type();
            if (type == Token::LINK || type == Token::DEFINITION) {
                const std::vector<Token> &v = t.subTokens();
                c = &v[2];
            } else if (type == Token::WORSE) {
                s.symptom().append("<");
            } else if (type == Token::BETTER) {
                s.symptom().append(">");
            } else if (type == Token::RADIATES) {
                s.symptom().append("^");
            }

            if (c->contentIsSet()) {
                s.symptom().append(c->content());
                pos = static_cast<int>(s.symptom().length());
            }

            for(const Token &tt : c->subTokens()) {
                extractSymptom(s, g, pos, tt);
            }
        };

        if (grade != -1) {
            Symptom::Graded g1;
            g1.setStart(pos);
            g1.setGrade(grade);
            add(g1);
            g1.setEnd(pos);
            s.grades().push_back(g1);
        } else {
            add(g);
        }
    }

public:
    std::string toHtml(const std::string &mmwiki, bool one_per_line = false, const std::string &language = std::string("en"))
    {
        // Init state
        _divs.clear();
        _div_levels.clear();
        _bullits.clear();
        _bullit_levels.clear();
        _tables_cell.clear();
        _tables_cells.clear();

        _toc.clear();
        _pages.clear();

        _req_language = language;
        _one_per_line = one_per_line;
        _no_dashes = false;

        _seq = "";
        _seq_class = "";
        _seq_start = "";

        // Init wiki / html
        _mm_wiki = mmwiki;
        _html = "";

        // Extract the header
        _header = extractHeader();

        // Extract the contents for this language
        std::string contents = extractContents(language);

        contents = prepareForHtml(contents);

        // Prepare highlighting
        std::list<std::string> highlight_words = _highlight_words.words();
        if (highlight_words.size() > 0) {
            std::string re = "\\b(";
            std::string delim = "";
            for(std::string word : highlight_words) {
                re.append(delim).append(word);
                delim = "|";
            }
            re.append(")\\b");
            std::regex hre(re, std::regex_constants::icase);
            contents = std::regex_replace(contents, hre, "H[$1]");
        }

        // Create HTML

        // iterate over all the content

        std::vector<std::string> lines = mmwiki_split(contents, '\n');
        for(std::string ln : lines) {
            Line line(ln);
            int level;
            std::string bullit;

            if (line.isKeyVal(":include")) {
                addInclude(line.value());
            } else if (line == ":no-dash-begin") {
                _no_dashes = true;
            } else if (line == ":no-dash-end") {
                _no_dashes = false;
            } else if (line.isKeyVal(":local-name")) {
                addHead(-1, 20, "h2", line.value());
            } else if (line.isKeyVal(":source")) {
                addHead(-1, 30, "h3", line.value());
            } else if (line.isKeyVal(":head")) {
                addHead(0, 40, "h4", line.value());
            } else if (line.isKeyVal(":head2")) {
                addHead(1, 50, "h5", line.value());
            } else if (line.isKeyVal(":rubric")) {
                addRubric(2, 60, "rubric", line.value());
            } else if (line.isKeyVal(":rubric2")) {
                addRubric(3, 70, "rubric2", line.value());
            } else if (line.isKeyVal(":div-begin")) {
                startDiv(-1, mmwiki_trim(line.value()), "div-begin");
            } else if (line == ":div-end") {
                endDiv(-1, "div-begin");
            } else if (line.isKeyVal(":image")) {
                endSeq();
                addImage(line.value());
            } else if (line.isKeyVal(":table") || line.isKeyVal(":table-begin")) {
                endSeq();
                addTable(line.value());
            } else if (line == ":end-table" || line == ":table-end") {
                endSeq();
                endTable();
            } else if (line.isKeyVal(":cell")) {
                endSeq();
                addCell(line.value());
            } else if (line.isKeyVal(":note") || line.isKeyVal(":note-begin")) {
                endSeq();
                startDiv(-1, "note");
                _seq_start.append("<span class=\"note\">").
                        append(processSeq(line.value())).append("&nbsp;</span>");
                _seq_class = "first";
            } else if (line == ":end-note" || line == ":note-end") {
                endDiv(-1, "note");
            } else if (line == ":code") {
                endSeq();
                startDiv(-1, "code");
            } else if (line == ":end-code") {
                endSeq();
                endDiv(-1 , "code");
            } else if (isBullit(line, level, bullit)) {
                endSeq();
                startBullit(level, bullit, line);
            } else {
                endBullit(0, "");
                if (line.isEmpty()) {
                    endSeq();
                } else {
                    addSeq(line);
                }
            }
        }

        endSeq();

        _html = implementHtml(_html);

        return _html;
    }

    const Toc &toc()
    {
        return _toc;
    }

    const Toc &pages()
    {
        return _pages;
    }



private:
    void startDiv(int level, const std::string &div, const std::string &div_id = "")
    {
        std::string p_div = (div_id == "") ? div : div_id;
        _divs.push_back(p_div);
        _div_levels.push_back(level);
        _html.append("<div class=\"").append(div).append("\" >");
        startSeq();
    }

    void endDiv(int level, const std::string &div)
    {
        endSeq();
        if (level < 0) {
            while(!_divs.empty() && _divs.back() != div) {
                _html.append("</div> <!--").append(_divs.back()).append("-->");
                _divs.pop_back();
                _div_levels.pop_back();
            }
            if (!_divs.empty()) {
                _html.append("</div> <!--").append(div).append("-->");
                _divs.pop_back();
                _div_levels.pop_back();
            }
        } else {
            while(!_div_levels.empty() && _div_levels.back() >= level) {
                _html.append("</div> <!--").append(_divs.back()).append("-->");
                _divs.pop_back();
                _div_levels.pop_back();
            }
            if (!_div_levels.empty() && _div_levels.back() == level) {
                _html.append("</div> <!--").append(div).append("-->");
                _divs.pop_back();
                _div_levels.pop_back();
            }
        }
    }

    void startSeq(const std::string &cl = "", const std::string &start = "")
    {
        _seq = "";
        _seq_class = cl;
        _seq_start = start;
    }

    void endSeq()
    {
        if (_seq != "" || _seq_start != "") {
            _html.append(processSeq(_seq, _seq_class, _seq_start, true, _min_grade));
            _seq = "";
            _seq_class = "";
            _seq_start = "";
        }
    }

    void addSeq(const Line &l)
    {
        _seq.append(l.line());
        _seq.append(" ");   // each line has an '\n' to compensate for
    }

    void startBullit(int level, const std::string &b, const Line &l)
    {
        auto open = [this](const std::string &b) {
            if (b == "*") _html.append("<ul>");
            else if (b == "1") _html.append("<ol>");
            else if (b == "2") _html.append("<ol class=\"a\">");
        };

        std::string current;
        int c_level = 0;

        if (_bullits.size() != 0) {
            current = _bullits.back();
            c_level = _bullit_levels.back();
        }

        endBullit(level, b);
        if (b != current && c_level == level) {
            _bullits.push_back(b);
            _bullit_levels.push_back(level);
            open(b);
        } else if (c_level < level) {
            _bullits.push_back(b);
            _bullit_levels.push_back(level);
            open(b);
        }

        _html.append("<li>");
        _html.append(processSeq(l.line(), "", "", false, _min_grade));
        _html.append("</li>");
    }

    void endBullit(int level, const std::string &b)
    {
        auto close = [this](const std::string &b) {
            if (b == "*") _html.append("</ul>");
            else if (b == "1" || b == "2") _html.append("</ol>");
        };

        while (!_bullit_levels.empty() && _bullit_levels.back() > level) {
            close(_bullits.back());
            _bullits.pop_back();
            _bullit_levels.pop_back();
        }

        if (!_bullit_levels.empty() && _bullits.back() != b && _bullit_levels.back() == level) {
            close(_bullits.back());
            _bullit_levels.pop_back();
            _bullits.pop_back();
        }
    }

    bool isBullit(Line &line, int &level, std::string &b)
    {
        std::smatch m;
        std::string l = line.line();
        if (std::regex_search(l, m, re_bullits)) {
            std::string s = m[1];
            b = s.back();
            level = static_cast<int>(s.length());
            line = Line(m.suffix());
            return true;
        }
        return false;
    }

    std::string addToc(int level, const std::string &l)
    {
        std::string toc_ref = mmwiki_mkid("head_ref", static_cast<int>(_toc.size() + 1));
        _toc.append(level, cleanupMM(l), toc_ref);
        return toc_ref;
    }

    std::string addPage(const std::string &p)
    {
        std::string pg_ref = mmwiki_mkid("page_ref", static_cast<int>(_pages.size() + 1));
        _pages.append(0, cleanupMM(p), pg_ref);
        std::string ref;
        ref.append("<span class=\"anchor\" id=\"").append(pg_ref).append("\"></span>");
        return ref;
    }

    void addTable(const std::string &l)
    {
        std::vector<std::string> parts = mmwiki_split(l, ':');
        std::string percentages;
        std::string headers;
        std::string table_width_percentage;
        std::string table_class;
        if (parts.size() >= 1) percentages = mmwiki_trim(parts[0]);
        if (parts.size() >= 2) headers = mmwiki_trim(parts[1]);
        if (parts.size() >= 3) table_width_percentage = mmwiki_trim(parts[2]);
        if (parts.size() >= 4) table_class = mmwiki_trim(parts[3]);

        if (table_class != "") {
            std::string c = " class=\"";
            c.append(table_class).append("\" ");
            table_class=c;
        }

        if (table_width_percentage != "") {
            std::string c = " style=\"width:";
            c.append(table_width_percentage).append("%;\" ");
            table_width_percentage = c;
        }

        _html.append("<table").append(table_class).append(table_width_percentage).append("><tr>");

        std::vector<std::string> v_percentages = mmwiki_split(percentages, ',');
        std::vector<std::string> v_headers = mmwiki_split(headers, ',');
        std::vector<std::string>::iterator it_p = v_percentages.begin();
        std::vector<std::string>::iterator it_h = v_headers.begin();
        int cells = 0;
        while(it_p != v_percentages.end()) {
            std::string p = mmwiki_trim(*it_p);
            std::string h = processSeq(mmwiki_trim((it_h != v_headers.end()) ? *it_h : ""));
            mmwiki_replaceAll(p, "%", "");
            _html.append("<th width=\"").append(mmwiki_trim(p)).append("%\">").
                    append(h).append("</th>");
            cells++;
            it_p++;
            if (it_h != v_headers.end()) it_h++;
        }
        _html.append("</tr>");
        _tables_cell.push_back(0);
        _tables_cells.push_back(cells);
    }

    void endTable()
    {
        if (_tables_cell.size() > 0) {
            int c = _tables_cell.back();
            int n = _tables_cells.back();
            if (c > 0) {
                while (c++ < n) { _html.append("<td />"); }
                _html.append("</tr>");
            }
            _tables_cell.pop_back();
            _tables_cells.pop_back();
            _html.append("</table>");
        }
    }

    void addCell(const std::string &value)
    {
        std::vector<std::string> parts = mmwiki_split(value, ':');
        std::string align;
        std::string cell_class;

        if (parts.size() > 0) align = mmwiki_trim(mmwiki_toLower(parts[0]));
        if (parts.size() > 1) cell_class = mmwiki_trim(mmwiki_toLower(parts[1]));

        if (align == "r") { align = "right"; }
        else if (align == "c") { align = "center"; }
        else { align = "left"; }

        if (cell_class != "") align.append(" ").append(cell_class);

        if (_tables_cell.size() > 0) {
            int c = _tables_cell.back();
            int n = _tables_cells.back();
            if (c == 0) {
                _html.append("<tr><td class=\"").append(align).append("\">");
                c++;
            } else if (c == n) {
                _html.append("</td></tr><tr><td class=\"").append(align).append("\">");
                c = 1;
            } else {
                _html.append("</td><td class=\"").append(align).append("\">");
                c++;
            }
            _tables_cell.back() = c;
        }
    }

    void addInclude(const std::string &incl)
    {
        std::vector<std::string> l = mmwiki_split(incl, ',');
        std::string incl_page = "";
        std::string div_class = "";
        if (l.size() > 0) incl_page = mmwiki_trim(l[0]);
        if (l.size() > 1) div_class = mmwiki_trim(l[1]);

        if (div_class != "") _html.append("<div class=\"").append(div_class).append("\">");
        if (incl_page != "") {
            MMWiki *m = new MMWiki();
            m->_owns_providers = false;
            m->setImageProvider(imageProvider());
            m->setCssProvider(cssProvider());
            m->setRemedyProvider(remedyProvider());
            m->setLinkProvider(linkProvider());
            m->setIncludeProvider(includeProvider());
            m->setMetaProvider(metaProvider());
            metaProvider()->setMMWiki(this);

            _html.append(m->toHtml(incl_page, _one_per_line, _req_language));

            delete m;
        }
        if (div_class != "") _html.append("</div>");
    }

    void addHead(int toc, int level, const std::string &h, const std::string &l, const std::string &cl = "")
    {
        endDiv(level, h);
        std::string toc_ref = "";
        if (toc >= 0) { toc_ref = addToc(toc, l); }
        std::string dash = (_one_per_line && !_no_dashes) ? " dash" : "";
        std::string html_class = "";
        if (cl != "") {
            html_class.append(" class=\"").append(cl).append(dash).append("\" ");
        }
        if (toc_ref != "") {
            std::string a;a.append("<span class=\"anchor\" id=\"").append(toc_ref).append("\">").append("</span>");toc_ref = a;
        }
        _html.append("<").append(h).append(html_class).
                append(" >").append(toc_ref).
            append(processSeq(l)).
            append("</").append(h).append(">");
        startDiv(level, h);
    }

    void addRubric(int toc, int level, const std::string &d, const std::string &r)
    {
        std::string rubr = mmwiki_trim(r);
        if (rubr != "") {
            rubr[0] = static_cast<char>(std::toupper(r[0]));
            if (rubr.back() == '.') {
                rubr.pop_back();
            }
            if (rubr.back() != ':') {
                rubr.append(": ");
            } else {
                rubr.append(" ");
            }
        }
        endDiv(level, d);
        startDiv(level, d);
        std::string toc_ref = "";
        if (toc >= 0) { toc_ref=addToc(toc, r); }
        if (toc_ref != "") {
            std::string a;a.append("<span class=\"anchor\" id=\"").append(toc_ref).append("\" >").append("</span>");toc_ref=a;
        }

        std::string start;
        start.append("<span class=\"").append(d).append("\"").append(">").append(toc_ref).
                append(processSeq(rubr)).
                append("</span>");
        startSeq("", start);
    }

    void addImage(const std::string &img_pars)
    {
        std::vector<std::string> v = mmwiki_split(img_pars, ',');

        auto get = [&v](size_t idx, std::string def) {
            if (v.size() > idx) { return mmwiki_trim(v[idx]); }
            else { return def; }
        };

        std::string img = get(0, "");
        std::string width_perc = get(1, "100");
        std::string subscript = get(2, "");
        std::string align = get(3, "center");
        std::string cl = "image-center";
        if (align == "left") { cl = "image-float-left"; }
        else if (align == "right") { cl = "image-float-right"; }
        else { align = "margin-left:auto;margin-right:auto;"; }

        std::string src = _image_provider->getImageSrc(img);

        if (width_perc != "") {
            std::string a = " style=\"width:";
            std::smatch m;
            if (std::regex_search(width_perc, m, re_width)) {
                std::string w = m[1];
                std::string unit = m[2];
                if (unit == "") { unit = "%"; }
                a.append(w);
                a.append(unit);
            } else {
                a.append(width_perc);
            }
            a.append(";\" ");
            width_perc = a;
        } else {
            width_perc = "";
        }

        _html.append("<div ").append(width_perc).
                append("class=\"").append(cl).append("\"").
                append(" >").
                append("<div class=\"image\">").
                  append("<img src=\"").append(src).
                    append("\" style=\"width:100%;\" />");
        if (subscript != "") {
            _html.append("<div class=\"subscript\">").append(subscript).append("</div>");
        }
        _html.append("</div></div>");
    }

    std::string p(const std::string &cl)
    {
        std::string c(cl);

        if (_one_per_line && !_no_dashes) {
            c.append(" dash");
        }

        std::string pp("<p ");
        if (c != "") {
            pp.append("class=\"").append(c).append("\"");
        }
        pp.append(">");
        return pp;
    }

    std::string span(const std::string &cl, const std::string &l)
    {
        std::string s("<span class=\"");
        return s.append(cl).append("\">").
                append(l).append("</span>");
    }

    std::string prepareForHtml(std::string mmwiki)
    {
        std::string n_mmwiki;
        // Create _mm_wiki while resolving literals
        applyRe(re_literals, mmwiki, [&n_mmwiki](std::smatch &m, int) {
            n_mmwiki.append(m.prefix());
            std::string s_literal = m.str();
            char literal = s_literal[0];
            if (literal == '\\') {
                literal = s_literal[1];
                switch(literal) {
                case ':': n_mmwiki.append("@!@#58@%@");break;
                case '{': n_mmwiki.append("@!@#123@%@");break;
                case '}': n_mmwiki.append("@!@#125@%@");break;
                case '[': n_mmwiki.append("@!@#91@%@");break;
                case ']': n_mmwiki.append("@!@#93@%@");break;
                }
            } else { // BR
                n_mmwiki.append("@!@br@%@");
            }
        }, [&n_mmwiki](std::string remain, int) {
            n_mmwiki.append(remain);
        });

        return n_mmwiki;
    }

    std::string implementHtml(std::string pre_html)
    {
        std::string n_html;
        applyRe(re_html_implement, pre_html, [&n_html](std::smatch &m, int) -> void {
            n_html.append(m.prefix());
            std::string literal = m[1];
            if (literal == "br") {
                n_html.append("<br />");
            } else if (std::isdigit(literal[0])) {
                n_html.append("#").append(literal).append(";");
            } else {
                n_html.append("&").append(literal).append(";");
            }
        }, [&n_html](std::string remain, int) -> void {
            n_html.append(remain);
        });
        return n_html;
    }

    // to use after wikitext has been prepared
    std::string cleanupMM(std::string s)
    {
        s = std::regex_replace(s, re_anchor, "");
        s = std::regex_replace(s, re_cleanup_open, "");
        s = std::regex_replace(s, re_cleanup_sym_open, "");
        s = std::regex_replace(s, re_cleanup_close, "");

        std::string ns;
        applyRe(re_html_implement, s, [&ns](std::smatch &m, int) -> void {
            ns.append(m.prefix());
            std::string literal = m[1];
            if (literal == "br") ns.append(" ");
            else if (literal == "gt") ns.append(">");
            else if (literal == "lt") ns.append("<");
            else if (literal == "apos") ns.append("'");
            else if (literal == "quot") ns.append("\"");
            else if (literal == "amp") ns.append("&");
            else { // assume ascii value
                int asc = std::stoi(literal);
                ns.push_back(static_cast<char>(asc));
            }
        }, [&ns](std::string remain, int) {
            ns.append(remain);
        });

        return ns;
    }

    bool applyRe(const std::regex &re, std::string s,
                 std::function<void(std::smatch &, int re_offset)> f,
                 std::function<void(std::string, int re_offset)> f_remains)
    {
        bool applied = false;
        std::smatch match;
        int re_offset = 0;
        while(std::regex_search(s, match, re)) {
            f(match, re_offset);
            s = match.suffix();

            re_offset += static_cast<int>(match.position() + match.length());
            applied = true;
        }
        f_remains(s, re_offset);
        return applied;
    }

    std::string processSeq(std::string s, const std::string &cl = "", const std::string &start = "", bool par = false, int min_grade = 0)
    {
        std::string html;
        if (par) html.append(p(cl));

        html.append(start);

        std::string ss;
        applyRe(re_prepare_modalities, s, [&ss](std::smatch &match, int) -> void {
            char c = match.str()[0];
            std::string prefix = match.prefix();
            ss.append(prefix);
            if (c == '<') { ss.append("-["); }
            else if (c == '>') { ss.append("+["); }
        },
        [&ss](std::string remain, int) { ss.append(remain); }
        );

        s = ss;
        ss = "";
        applyRe(re_html_prepare, s, [&ss](std::smatch &match, int) -> void {
            ss.append(match.prefix());
            char c = match.str()[0];
            switch(c) {
                case '<': ss.append("@!@lt@%@"); break;
                case '>': ss.append("@!@gt@%@"); break;
                case '&': ss.append("@!@amp@%@"); break;
                case '"': ss.append("@!@quot@%@"); break;
                case '\'': ss.append("@!@apos@%@"); break;
            }
        },
        [&ss](std::string remain, int) { ss.append(remain); }
        );
        s = ss;

        Token root = tokenize(s);
        if (min_grade > 1) {
            root = filterOutGradedSymptoms(root, min_grade);
        }

        html.append(generateHtml(root));

        if (par) html.append("</p>");

        return html;
    }

    std::string generateHtml(Token &t)
    {
        std::string o, c, cl, m;
        std::string content = t.contentIsSet() ? t.content() : "";
        std::string modifier = t.modifier();
        std::string pre;
        std::string post;

        c = "</span>";
        bool content_made = false;

        bool sym = false;

        Token::Tok type = t.type();

        switch(type) {
            case Token::NOOP: return "";
            case Token::NONE: content_made = true; break;
            case Token::ROOT: content_made = true; break;
            case Token::SYMPTOM: cl = "symptom";mkHtmlSource(modifier, pre, post);sym = true;break;
            case Token::RELATION: cl = "relation";mkHtmlSource(modifier, pre, post);sym = true;break;
            case Token::GROUP: cl = "group";mkHtmlSource(modifier, pre, post);break;
            case Token::SENTENCE: cl = "sentence";mkHtmlSource(modifier, pre, post);break;
            case Token::CAUSE: cl = "cause";mkHtmlSource(modifier, pre, post);sym = true;break;
            case Token::GRADE2: cl = "grade2";break;
            case Token::GRADE3: cl = "grade3";break;
            case Token::GRADE4: cl = "grade4";break;
            case Token::MODALITY: cl = "modality";break;
            case Token::ITALIC: o = "<i>"; c = "</i>"; break;
            case Token::BOLD: o = "<b>"; c = "</b>"; break;
            case Token::PAGE: cl = "page-number";c.append(addPage(content));break;
            case Token::CODE: cl = "code";break;
            case Token::TENDENCY: cl = "tendency";break;
            case Token::INLINE_IMAGE: {
                   cl = "inline-image";
                   std::string c = "<img src=\"";
                   c.append(imageProvider()->getImageSrc(content)).append("\" />");
                   content = c;
            }
            break;
            case Token::DEFINITION: return mkDefinition(t);
            case Token::REMEDY1: content = mkRemedy(1, content);content_made = true; break;
            case Token::REMEDY2: content = mkRemedy(2, content);content_made = true; break;
            case Token::REMEDY3: content = mkRemedy(3, content);content_made = true; break;
            case Token::REMEDY4: content = mkRemedy(4, content);content_made = true; break;
            case Token::ANCHOR: o = "<span class=\"anchor\" id=\"";o.append(mkAnchor(content));o.append("\" >");content = "";break;
            case Token::LINK: return mkLink(t);
            case Token::RADIATES: o = "<span class=\"radiating\">@!@#187@%@@!@nbsp@%@";break;
            case Token::WORSE: o = "<span class=\"modality\">@!@lt@%@@!@nbsp@%@";break;
            case Token::BETTER: o = "<span class=\"modality\">@!@gt@%@@!@nbsp@%@";break;
            case Token::LITERAL: o = "["; c = "]";break;
            case Token::HIGHLIGHT: cl = "highlight";break;
            case Token::CLASSIFICATION: cl = "classification"; break;
            case Token::MODIFIER:
            case Token::CONTENT: content_made = true; break;
        }

        if (_one_per_line && !_no_dashes && sym) {
            cl.append(" dash");
        }

        std::string html;

        auto generateRest = [this, &html, &t]() {
            for(Token &tt : t.subTokens()) {
                html.append(generateHtml(tt));
            }
        };

        html.append(pre);
        if (!content_made) {
            if (cl != "") { o = "<span class=\"";o.append(cl);o.append("\">"); }
            html.append(o).append(content);
            generateRest();
            html.append(c);
        } else {
            html.append(content);
            generateRest();
        }
        html.append(post);

        return html;
    }

    std::string mkLink(Token &t)
    {
        Token &link = t.subTokens()[0];
        Token &content = t.subTokens()[2];
        return linkProvider()->mkLinkHRef(generateHtml(link), generateHtml(content));
    }

    std::string mkDefinition(Token &t)
    {
        Token &word = t.subTokens()[0];
        Token &definition = t.subTokens()[2];

        std::string cl = "definition";
        if (t.content().find("class=\"highlight\"") != std::string::npos) {
            cl = "definition-with-highlight";
        }
        std::string r = "<span class=\"";
        r.append(cl).append(" hover\">").
                append("<span class=\"tooltip def-tooltip\">").append(generateHtml(definition)).append("</span>").
                append(generateHtml(word)).
                append("</span>");
        return r;
    }

    std::string mkRemedy(int grade, std::string &s)
    {
        std::string cl = "remedy";
        cl.append(std::to_string(grade));
        std::string uabbrev = remedyProvider()->unifyAbbrev(s);
        std::string rem = uabbrev;
        if (remedyProvider()->hasLink(rem)) {
            std::string link = "<a href=\"";
            link.append(remedyProvider()->getLink(rem)).
                    append("\">").
                    append(rem).
                    append("</a>");
            rem = link;
        }

        std::string r = "<span class=\"";
        r.append(cl).append(" hover\">").
                append("<span class=\"tooltip\">").
                append(remedyProvider()->getLatinName(uabbrev)).
                append("</span>").
                append(rem).
                append("</span>");
        return r;
    }

    std::string mkAnchor(std::string &nm)
    {
        return _link_provider->mkLinkId(nm);
    }


    Token filterOutGradedSymptoms(const Token &t, int min_grade)
    {
        Token::Tok type = t.type();
        if (type == Token::SYMPTOM ||
                type ==Token::RELATION ||
                type == Token::CAUSE) {
            if (t.hasA([min_grade](const Token &t) {
                       Token::Tok type = t.type();
                       int gr = (type == Token::GRADE2) ? 2 :
                                    ((type == Token::GRADE3) ? 3 :
                                       ((type == Token::GRADE4) ? 4 : 1)
                                     );
                       return (gr >= min_grade);
            })) {
                return t;
            } else {
                return Token(Token::NOOP);
            }
        } else {
            Token nt(Token::NONE);
            nt.contentUnSet();
            std::vector<Token> v;
            for(const Token &tt : t.subTokens()) {
               v.push_back(filterOutGradedSymptoms(tt, min_grade));
            }
            nt.setSubTokens(v);
            return nt;
        }
    }

    Token tokenize(std::string s)
    {
        Token root(Token::ROOT);
        root.setStart(0);
        root.setEnd(static_cast<int>(s.length()));
        root.setContent(s);

        doTokenize(root);
        cleanupTokenTree(root, root.content());

        return root;
    }

    Token extractTokens(Token &root, std::function<bool(const Token &t)> f)
    {
        Token n_root(Token::ROOT);
        getTokenSubTree(root, n_root, f);
        return n_root;
    }

    bool getTokenSubTree(Token &t, Token &nt, std::function<bool(const Token &t)> f)
    {
        nt = t;
        std::vector<Token> nv;
        std::vector<Token> &v = t.subTokens();
        for(Token &tt : v) {
            Token tt1;
            if (getTokenSubTree(tt, tt1, f)) {
                nv.push_back(tt1);
            }
        }
        nt.setSubTokens(nv);
        return f(nt);
    }

    void insertToken(Token &t, Token &ins)
    {
        /*if (t.contains(ins)) {
            std::vector<Token> &v = t.subTokens();

            auto in_ins = std::equal_range(v.begin(), v.end(), ins);

            std::vector<Token> not_in_ins(v.begin(), in_ins.first);
            not_in_ins.insert(not_in_ins.end(), in_ins.second, v.end());

            std::vector<Token> &ins_v = ins.subTokens();
            ins_v.insert(ins_v.end(), in_ins.first, in_ins.second);

            t.setSubTokens(not_in_ins);
            std::vector<Token> &vv = t.subTokens();

            std::vector<Token>::iterator findIt = std::lower_bound(vv.begin(), vv.end(), ins);
            if (findIt == vv.end()) {
                vv.push_back(ins);
            } else if (findIt->contains(ins)) {
                insertToken(*findIt, ins);
            } else {
                vv.insert(findIt, ins);
            }
        }*/
		if (t.contains(ins)) {
			std::vector<Token> &v = t.subTokens();
			{
				std::vector<Token> vv;
                size_t i, N;
				for(i = 0, N = v.size(); i < N; i++) {
					Token &tt = v[i];
					if (ins.equal(tt)) {
						vv.push_back(tt);
					} else if (ins.contains(tt)) {
						insertToken(ins, tt);
					} else {
						vv.push_back(tt);
					}
				}
				t.setSubTokens(vv);
			}
			
			{
				v = t.subTokens();
				std::vector<Token> vv;
				bool inserted = false;
                size_t i, N;
				for(i = 0, N = v.size(); i < N; i++) {
					Token &tt = v[i];
					if (tt.equal(ins)) {
						vv.push_back(tt);
						inserted = true;
					} else if (tt.contains(ins)) {
						insertToken(tt, ins);
						vv.push_back(tt);
						inserted = true;
					} else if (tt.end() <= ins.start()) {
						vv.push_back(tt);
					} else { // tt.end() > ins.start()
						if (!inserted) { inserted = true; vv.push_back(ins); }
						vv.push_back(tt);
					}
				}
				if (!inserted) { vv.push_back(ins); }
				t.setSubTokens(vv);
			}
		}
    }

    void cleanupTokenTree(Token &t, std::string &original)
    {
        //sortTokenTree(t);
        std::vector<Token> &v = t.subTokens();

        if (v.size() == 0) {
            return; // content is oke.
        } else {
            std::vector<Token> nv;
            int last_end = t.start();
            for(Token &tt : v) {
                int start = tt.start();

                if (start > last_end) {
                    Token nt(Token::NONE);
                    nt.setStart(last_end);
                    nt.setEnd(start);
                    nt.setContent(original.substr(static_cast<size_t>(nt.start()), 
								  static_cast<size_t>(nt.length()))
								  );
                    nv.push_back(nt);
                }
                last_end = tt.end();

                nv.push_back(tt);
            }

            if (last_end < t.end()) {
                Token nt(Token::NONE);
                nt.setStart(last_end);
                nt.setEnd(t.end());
                nt.setContent(original.substr(static_cast<size_t>(nt.start()), 
							  static_cast<size_t>(nt.length()))
							  );
                nv.push_back(nt);
            }

            t.contentUnSet();

            for(Token &tt : nv) {
                cleanupTokenTree(tt, original);
            }

            t.setSubTokens(nv);
        }
    }

    void sortTokenTree(Token &t)
    {
        std::vector<Token> &v = t.subTokens();
        for(Token &tt : v) sortTokenTree(tt);
        std::sort(v.begin(), v.end(), [](const Token &a, const Token &b) {
            return a.start() < b.start();
        });
    }

    void doTokenize(Token &root)
    {
        std::string s = root.content();

        bool go_on = true;
        while(go_on) {
            std::string ss;
            go_on = applyRe(re_markup, s, [this, &ss, &root](std::smatch &m, int re_offset) -> void {
                const std::string &t = m[1];
                const char cmd = t[0];
                std::string content = m[2];
                std::string modifier;
                bool has_modifier = false;

                Token::Tok type = Token::NONE;

                switch(cmd) {
                    case '2': type = Token::GRADE2;break;
                    case '3': type = Token::GRADE3;break;
                    case '4': type = Token::GRADE4;break;
                    case 'M': type = Token::MODALITY;break;
                    case 'I': type = Token::ITALIC;break;
                    case 'B': type = Token::BOLD;break;
                    case 'P': type = Token::PAGE;break;
                    case 'Q': type = Token::CODE;break;
                    case 'T': type = Token::TENDENCY;break;
                    case 'i': type = Token::INLINE_IMAGE;break;
                    case 't': {
                        type = Token::DEFINITION;
                        std::size_t idx = content.find("|");
                        if (idx != std::string::npos) {
                            has_modifier = true;
                            modifier = content.substr(0, idx);
                            content = content.substr(idx + 1);
                        }
                    }
                    break;
                    case 'R': {
                        int grade = 1;
                        if (t != "R") grade = std::stoi(t.substr(1));
                        switch(grade) {
                        case 1: type = Token::REMEDY1;break;
                        case 2: type = Token::REMEDY2;break;
                        case 3: type = Token::REMEDY3;break;
                        case 4: type = Token::REMEDY4;break;
                        }
                    }
                    break;
                    case 'H': type = Token::HIGHLIGHT;break;
                    case 'C': type = Token::CLASSIFICATION;break;
                    case '!': type = Token::LITERAL;break;
                    case 'L': {
                        type = Token::LINK;
                        std::size_t idx = content.find("|");
                        if (idx != std::string::npos) {
                            has_modifier = true;
                            modifier = content.substr(0, idx);
                            content = content.substr(idx + 1);
                        }
                    }
                    break;
                    case 'N': type = Token::ANCHOR;break;
                    case '^': type = Token::RADIATES;break;
                    case '-': type = Token::WORSE;break;
                    case '+': type = Token::BETTER;break;
                }

                Token tok(type);
                int c_start = static_cast<int>(m.position(2));

                if (has_modifier) {
                    Token t_modifier(Token::MODIFIER);
                    t_modifier.setStart(re_offset + c_start);
                    t_modifier.setEnd(re_offset + c_start + static_cast<int>(modifier.length()));
                    t_modifier.setContent(modifier);

                    Token t_pipe(Token::NOOP);
                    t_pipe.setStart(t_modifier.end());
                    t_pipe.setEnd(t_modifier.end() + 1);

                    Token t_content(Token::CONTENT);
                    t_content.setStart(t_pipe.end());
                    t_content.setEnd(t_pipe.end() + static_cast<int>(content.length()));
                    t_content.setContent(content);

                    insertToken(root, t_modifier);
                    insertToken(root, t_pipe);
                    insertToken(root, t_content);
                } else {
                    tok.setContent(content);
                }

                int c_end = c_start + static_cast<int>(m.length(2));
                tok.setStart(re_offset + c_start);
                tok.setEnd(re_offset + c_end);

                Token pre(Token::NOOP);
                int pre_start = static_cast<int>(m.position(0));
                int pre_end = c_start;
                pre.setStart(re_offset + pre_start);
                pre.setEnd(re_offset + pre_end);

                Token post(Token::NOOP);
                int post_start = c_end;
                int post_end = pre_start + static_cast<int>(m.length(0));
                post.setStart(re_offset + post_start);
                post.setEnd(re_offset + post_end);

                insertToken(root, tok);
                insertToken(root, pre);
                insertToken(root, post);

                ss.append(m.prefix());
                ss.append((t.length() == 2) ? "   " : "  ");
                if (has_modifier) {
                    ss.append(modifier);
                    ss.append(" ");
                }
                ss.append(content);
                ss.append(" ");
            },
            [&ss](std::string remain, int) -> void {
                ss.append(remain);
            });

            s = ss;
        }

        go_on = true;
        while(go_on) {
            std::string ss;
            go_on = applyRe(re_symptom, s, [this, &ss, &root](std::smatch &m, int re_offset) -> void {
                const std::string &m_type = m[1];
                const std::string &src = m[2];
                const std::string &content = m[3];

                Token::Tok type = Token::NONE;
                if (m_type == "S") { type = Token::SYMPTOM; }
                else if (m_type == "R") { type = Token::RELATION; }
                else if (m_type == "C") { type = Token::CAUSE; }
                else if (m_type == "G") { type = Token::GROUP; }
                else if (m_type == "Z") { type = Token::SENTENCE; }

                Token tok(type);
                tok.setContent(content);
                tok.setModifier(src);
                int c_start = static_cast<int>(m.position(3));
                int c_end = c_start + static_cast<int>(m.length(3));
                tok.setStart(re_offset + c_start);
                tok.setEnd(re_offset + c_end);

                Token pre(Token::NOOP);
                int pre_start = static_cast<int>(m.position(0));
                int pre_end = c_start;
                pre.setStart(re_offset + pre_start);
                pre.setEnd(re_offset + pre_end);

                Token post(Token::NOOP);
                int post_start = c_end;
                int post_end = pre_start + static_cast<int>(m.length(0));
                post.setStart(re_offset + post_start);
                post.setEnd(re_offset + post_end);

                insertToken(root, tok);
                insertToken(root, pre);
                insertToken(root, post);

                ss.append(m.prefix());
                ss.append("  ").append(src.empty() ? "" : " ");
                ss.append(content);
                ss.append(" ");
            },
            [&ss](std::string remain, int) -> void {
                 ss.append(remain);
            });
            s = ss;
        }
    }

    bool mkHtmlSource(const std::string &source_letter, std::string &pre, std::string &post) const
    {
        if (source_letter == "") { return false; }

        const SourceMap &sm = _header.sources();
        SourceMap::const_iterator it;
        char sl = source_letter[0];
        it = sm.find(sl);
        if (it != sm.cend()) {
            const Source &src = it->second;
            pre.append("<span class=\"source hover\">").
                       append("<span class=\"tooltip\">").append(src.source()).append("</span>").
                       append(src.htmlMark()).
                    append("</span>").append(src.htmlPre());
            post = src.htmlPost();
            return true;
        } else {
            return false;
        }
    }

    std::string extractContents(const std::string &lang)
    {
        std::smatch match;

        std::string first_language;
        int         first_position = -1;
        bool        has_first_language = false;

        std::string en_language = std::string("en");
        bool        has_en_language = false;
        int         en_position = -1;

        std::string language = mmwiki_toLower(lang);
        bool        has_language = false;
        int         position = -1;

        std::string mm_wiki_lang = _mm_wiki;
        while(std::regex_search(mm_wiki_lang, match, re_section) && !has_language) {
            if (!match.empty()) {
                std::string l = mmwiki_toLower(match[1]);
                std::vector<std::string> languages = mmwiki_split(l, ',');

                auto contains = [&languages](const std::string &l) {
                    for(const std::string &ll : languages) {
                        if (mmwiki_trim(ll) == l) return true;
                    }
                    return false;
                };

                if (contains(language)) {
                    has_language = true;
                    position = static_cast<int>(match.position() + match.length());
                } else if (contains(en_language)) {
                    has_en_language = true;
                    en_position = static_cast<int>(match.position() + match.length());
                } else if (!has_first_language) {
                    has_first_language = true;
                    first_position = static_cast<int>(match.position() + match.length());
                    first_language = languages[0];
                }
            }
            mm_wiki_lang = match.suffix().str();
        }

        if (!has_language) {
            if (has_en_language) {
                position = en_position;
                language = en_language;
            } else if (has_first_language) {
                position = first_position;
                language = first_language;
            }
        }

        if (position < 0) {
            metaProvider()->setLanguage(std::string(""));
            return std::string("");
        } else {
            metaProvider()->setLanguage(language);

            std::string result = _mm_wiki.substr(static_cast<unsigned long>(position));

            if (std::regex_search(result, match, re_end)) {
                result = result.substr(0, static_cast<unsigned long>(match.position()));
            }

            return result;
        }
    }

    Header extractHeader()
    {
        std::string head;
        std::stringstream in(_mm_wiki);
        std::string line;

        Header h;

        bool in_head = true;

        auto setEndHead = [&in_head](std::string value) {
            MMWIKI_UNUSED(value);
            in_head = false;
        };

        auto addSource = [&h](std::string value) {
            Source s;
            if (s.fromString(value)) {
                SourceMap m;
                if (h.sourcesIsSet()) {
                    m = h.sources();
                }
                m.insert(std::pair<char, Source>(s.letter(), s));
                h.setSources(m);
            }
        };

        while(in_head && std::getline(in, line, '\n')) {
            std::string lineCi = mmwiki_trim(mmwiki_toLower(line));
            MMWIKI_SP(":begin", setEndHead)
            if (in_head) {
                MMWIKI_SP(":book", h.setBook)
                MMWIKI_SP(":edition", h.setEdition)
                MMWIKI_SP(":author", h.setAuthor)
                MMWIKI_SP(":editor", h.setEditor)
                MMWIKI_SP(":abbrev", h.setAbbrev)
                MMWIKI_SP(":latin-name", h.setLatinName)
                MMWIKI_SP(":source", addSource)
                MMWIKI_SP(":miasm", h.setMiasm)
                MMWIKI_SP(":tendency", h.setTendency)
                MMWIKI_SP(":class", h.setClassification)
            }
        }

        return h;
    }

private:
    std::string getValue(std::string s) const
    {
        size_t p = s.find("=");
        if (p == std::string::npos) {
            return std::string("");
        } else {
            return mmwiki_trim(s.substr(p + 1));
        }
    }
};


#endif // MMWIKI_H
