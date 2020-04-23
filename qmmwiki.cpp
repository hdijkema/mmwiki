#include "qmmwiki.h"

#include <QStack>

static QString mkid(const QString &in, int n)
{
    return QString("%1_%2").arg(in, QString::number(n));
}

#define MMWIKI_UNUSED(a) ((void)a)

#define DEF_QMMWIKI_PROP(cl, type, v, setter) \
    const type &cl::v() const { return _##v; } \
    type &cl::v() { return _##v; } \
    void cl::setter(const type &w) { _##v= w;_##v##_is_set = true; } \
    bool cl::v##IsSet() const { return _##v##_is_set; } \
    void cl::v##UnSet() { _##v##_is_set = false; }

#define INIT_QMMWIKI_PROP(v) \
    _##v##_is_set = false;

////////////////////////////////////////////////////////////////////
//// MMWiki::Line
////////////////////////////////////////////////////////////////////

QMMWiki::Line::Line(const QString &l)
{
    _line = l;
    _line_ci = l.trimmed().toLower();
}

bool QMMWiki::Line::isKeyVal(const QString &key)
{
    if (_line_ci.startsWith(key)) {
        int idx = key.length();
        int len = _line_ci.length();
        while(idx < len && _line_ci[idx] == ' ') idx++;
        if (idx == len) {
            _value = "";
            return true;
        } else if (_line_ci[idx] == '=') {
            int i, N;
            for(i = 0, N = _line.length(); i < N && _line[i].isSpace(); i++);
            QString _line_ltrim = _line.mid(i);
            _value = _line_ltrim.mid(idx + 1);
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

QString QMMWiki::Line::value() const
{
    return _value;
}

bool QMMWiki::Line::operator ==(const QString &key) const
{
    return equal(key);
}

bool QMMWiki::Line::equal(const QString &key) const
{
    bool yes = _line_ci.startsWith(key);
    if (yes) {
        int i, N;
        i = key.length();
        N = _line_ci.length();

        if (i == N) {
            return true;
        } else {
            if (_line_ci[i] == ' ' || _line_ci[i] == '=') {
                return true;
            } else {
                return false;
            }
        }
    } else {
        return false;
    }
}

bool QMMWiki::Line::isEmpty() const
{
    return _line_ci == "";
}

QString QMMWiki::Line::line() const
{
    return _line;
}

bool QMMWiki::Line::has(const QStringList &l) const
{
    foreach(QString key, l) {
        if (equal(key)) { return true; }
    }
    return false;
}

bool QMMWiki::Line::startsWith(const QString &k) const
{
    return _line_ci.startsWith(k);
}


////////////////////////////////////////////////////////////////////
//// MMWiki::Token
////////////////////////////////////////////////////////////////////

DEF_QMMWIKI_PROP(QMMWiki::Token, QMMWiki::Token::Tok, type, setType)
DEF_QMMWIKI_PROP(QMMWiki::Token, QString, content, setContent)
DEF_QMMWIKI_PROP(QMMWiki::Token, QString, modifier, setModifier)
DEF_QMMWIKI_PROP(QMMWiki::Token, int, start, setStart)
DEF_QMMWIKI_PROP(QMMWiki::Token, int, end, setEnd)
DEF_QMMWIKI_PROP(QMMWiki::Token, QVector<QMMWiki::Token>, subTokens, setSubTokens)

QMMWiki::Token::Token()
{
    INIT_QMMWIKI_PROP(type)
    INIT_QMMWIKI_PROP(content)
    INIT_QMMWIKI_PROP(modifier)
    INIT_QMMWIKI_PROP(start)
    INIT_QMMWIKI_PROP(end)
    INIT_QMMWIKI_PROP(subTokens)
}

QMMWiki::Token::Token(QMMWiki::Token::Tok a)
{
    setType(a);
}

bool QMMWiki::Token::contains(const QMMWiki::Token &other)
{
    return other.start() >= start() &&
            other.end() <= end();
}

bool QMMWiki::Token::equal(const QMMWiki::Token &b)
{
    return b.start() == start() && b.end() == end();
}

int QMMWiki::Token::length() const
{
    return end() - start();
}

bool QMMWiki::Token::hasA(std::function<bool (const QMMWiki::Token &)> f) const
{
    if (f(*this)) { return true; }
    else {
        const QVector<Token> &v = subTokens();
        foreach(const Token &tt, v) {
            if (tt.hasA(f)) return true;
        }
        return false;
    }
}

bool QMMWiki::Token::operator <(const QMMWiki::Token &b) const
{
    return end() <= b.start();
}


DEF_QMMWIKI_PROP(QMMWiki::Symptom::Graded, int, start, setStart)
DEF_QMMWIKI_PROP(QMMWiki::Symptom::Graded, int, end, setEnd)
DEF_QMMWIKI_PROP(QMMWiki::Symptom::Graded, int, grade, setGrade)

QMMWiki::Symptom::Graded::Graded()
{
    INIT_QMMWIKI_PROP(start)
    INIT_QMMWIKI_PROP(end)
    INIT_QMMWIKI_PROP(grade)
}

int QMMWiki::Symptom::Graded::length() const
{
    return end() - start();
}

DEF_QMMWIKI_PROP(QMMWiki::Symptom, QMMWiki::Symptom::Type, type, setType)
DEF_QMMWIKI_PROP(QMMWiki::Symptom, QVector<QMMWiki::Symptom::Graded>, grades, setGrades)
DEF_QMMWIKI_PROP(QMMWiki::Symptom, QString, symptom, setSymptom)

QMMWiki::Symptom::Symptom()
{
    INIT_QMMWIKI_PROP(type)
    INIT_QMMWIKI_PROP(grades)
    INIT_QMMWIKI_PROP(symptom)
}

int QMMWiki::Symptom::maxGrade() const
{
    int max = 1;
    for(const Graded &g : grades())
        max = (g.grade() > max) ? g.grade() : max;
    return max;
}

int QMMWiki::Symptom::gradedCount() const
{
    return static_cast<int>(grades().size());
}

int QMMWiki::Symptom::gradeOf(int idx) const
{
    const QVector<Graded> &g = grades();
    return g[idx].grade();
}

const QString QMMWiki::Symptom::textOf(int idx) const
{
    const QVector<Graded> &g = grades();
    const Graded &gr = g[idx];
    QString s = symptom().mid(gr.start(), gr.length());
    return s;
}

QMMWiki::RemedyProvider::~RemedyProvider()
{
}

QString QMMWiki::RemedyProvider::getLatinName(const QString &uniform_abbrev)
{
    return uniform_abbrev;
}

QString QMMWiki::RemedyProvider::getLink(const QString &uniform_abbrev)
{
    ((void) uniform_abbrev);
    return "";
}

QString QMMWiki::RemedyProvider::unifyAbbrev(const QString &abbrev)
{
    QString s = abbrev.trimmed();
    if (s != "") {
        s = s.toLower();
        s[0] = s[0].toUpper();
        if (s.back() == '.') { s = s.left(s.length() - 1); }
    }
    return s;
}

bool QMMWiki::RemedyProvider::hasLink(const QString &uniform_abbrev)
{
    ((void)uniform_abbrev);
    return false;
}

QString QMMWiki::CssProvider::css() const
{
    return QString(MMWIKI_CSS);
}

QString QMMWiki::CssProvider::escape(const QString &to_html) { return to_html; }

QMMWiki::CssProvider::~CssProvider()
{
}

DEF_QMMWIKI_PROP(QMMWiki::ImageSrc, QString, author, setAuthor)
DEF_QMMWIKI_PROP(QMMWiki::ImageSrc, QString, authorUrl, setAuthorUrl)
DEF_QMMWIKI_PROP(QMMWiki::ImageSrc, QString, source, setSource)
DEF_QMMWIKI_PROP(QMMWiki::ImageSrc, QString, sourceUrl, setSourceUrl)
DEF_QMMWIKI_PROP(QMMWiki::ImageSrc, QString, license, setLicense)
DEF_QMMWIKI_PROP(QMMWiki::ImageSrc, QString, licenseUrl, setLicenseUrl)
DEF_QMMWIKI_PROP(QMMWiki::ImageSrc, QString, imageSrc, setImageSrc)
DEF_QMMWIKI_PROP(QMMWiki::ImageSrc, QString, imageAlt, setImageAlt)
DEF_QMMWIKI_PROP(QMMWiki::ImageSrc, QString, imgId, setImgId)

QMMWiki::ImageSrc::ImageSrc()
{
    INIT_QMMWIKI_PROP(author)
    INIT_QMMWIKI_PROP(authorUrl)
    INIT_QMMWIKI_PROP(source)
    INIT_QMMWIKI_PROP(sourceUrl)
    INIT_QMMWIKI_PROP(license)
    INIT_QMMWIKI_PROP(licenseUrl)
    INIT_QMMWIKI_PROP(imageSrc)
    INIT_QMMWIKI_PROP(imageAlt)
    INIT_QMMWIKI_PROP(imgId)
}

QString QMMWiki::ImageProvider::getImageSrc(const QString &image)
{
    return image;
}

QVector<QMMWiki::ImageSrc> QMMWiki::ImageProvider::getImageSrcs(const QString &)
{
    return QVector<ImageSrc>();
}

QMMWiki::ImageProvider::~ImageProvider() {}

QString QMMWiki::IncludeProvider::getPage(const QString &pagename)
{
    MMWIKI_UNUSED(pagename);
    return "";
}

QMMWiki::IncludeProvider::~IncludeProvider() {}

DEF_QMMWIKI_PROP(QMMWiki::LinkProvider, QString, namePrefix, setNamePrefix)

QString QMMWiki::LinkProvider::mkLinkId(QString name) const
{
    QString a = namePrefix();
    a.append(name);
    return a;
}

QString QMMWiki::LinkProvider::mkLinkHRef(QString href, QString content) const
{
    QString id = "link_id_"; id.append(QString::number(++link_id));
    QString a = "<span class=\"link\" id=\"";
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

QMMWiki::LinkProvider::LinkProvider()
{
    INIT_QMMWIKI_PROP(namePrefix)
}

QMMWiki::LinkProvider::~LinkProvider() {}


DEF_QMMWIKI_PROP(QMMWiki::Source, QChar, letter, setLetter)
DEF_QMMWIKI_PROP(QMMWiki::Source, QString, source, setSource)
DEF_QMMWIKI_PROP(QMMWiki::Source, QString, htmlMark, setHtmlMark)
DEF_QMMWIKI_PROP(QMMWiki::Source, QString, htmlPre, setHtmlPre)
DEF_QMMWIKI_PROP(QMMWiki::Source, QString, htmlPost, setHtmlPost)

QMMWiki::Source::Source()
{
    INIT_QMMWIKI_PROP(letter)
    INIT_QMMWIKI_PROP(source)
    INIT_QMMWIKI_PROP(htmlMark)
    INIT_QMMWIKI_PROP(htmlPre)
    INIT_QMMWIKI_PROP(htmlPost)
}

bool QMMWiki::Source::fromString(QString s)
{
    // letter,source,indicator in text,[pre_html],[post_html]

    QStringList parts = s.split(",");
    int size = parts.size();

    if (size >= 3) {
        QString letter = parts[0].trimmed();
        if (letter.length() == 1) {
            setLetter(letter[0]);
            setSource(parts[1].trimmed());
            setHtmlMark(parts[2].trimmed());

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


DEF_QMMWIKI_PROP(QMMWiki::Header, QString, book, setBook)
DEF_QMMWIKI_PROP(QMMWiki::Header, QString, author, setAuthor)
DEF_QMMWIKI_PROP(QMMWiki::Header, QString, edition, setEdition)
DEF_QMMWIKI_PROP(QMMWiki::Header, QString, editor, setEditor)
DEF_QMMWIKI_PROP(QMMWiki::Header, QString, abbrev, setAbbrev)
DEF_QMMWIKI_PROP(QMMWiki::Header, QString, latinName, setLatinName)
DEF_QMMWIKI_PROP(QMMWiki::Header, QMMWiki::SourceMap, sources, setSources)
DEF_QMMWIKI_PROP(QMMWiki::Header, QString, tendency, setTendency)
DEF_QMMWIKI_PROP(QMMWiki::Header, QString, miasm, setMiasm)
DEF_QMMWIKI_PROP(QMMWiki::Header, QString, classification, setClassification)

QMMWiki::Header::Header()
{
    INIT_QMMWIKI_PROP(book)
    INIT_QMMWIKI_PROP(author)
    INIT_QMMWIKI_PROP(edition)
    INIT_QMMWIKI_PROP(editor)
    INIT_QMMWIKI_PROP(abbrev)
    INIT_QMMWIKI_PROP(latinName)
    INIT_QMMWIKI_PROP(sources)
    INIT_QMMWIKI_PROP(tendency)
    INIT_QMMWIKI_PROP(miasm)
    INIT_QMMWIKI_PROP(classification)
}



void QMMWiki::MetaProvider::setMMWiki(QMMWiki *m) { _mmwiki = m; }

QString QMMWiki::MetaProvider::lbClassification() { return "Classification"; }

QString QMMWiki::MetaProvider::lbTendency() { return "Tendency"; }

QString QMMWiki::MetaProvider::lbMiasm() { return "Miasm"; }

QString QMMWiki::MetaProvider::lbImage() { return "Picture"; }

QString QMMWiki::MetaProvider::lbImageAuthor() { return "Author"; }

QString QMMWiki::MetaProvider::lbLicense() { return "License"; }

QString QMMWiki::MetaProvider::lbSourceInfo() { return "Source Information"; }

DEF_QMMWIKI_PROP(QMMWiki::MetaProvider, QString, language, setLanguage)


QMMWiki::MetaProvider::MetaProvider()
{
    _id = 0;
    INIT_QMMWIKI_PROP(language)
}

QMMWiki::MetaProvider::~MetaProvider() {}

QString QMMWiki::MetaProvider::getMeta()
{
    Header        &h = _mmwiki->header();
    ImageProvider *img = _mmwiki->imageProvider();
    CssProvider   *css = _mmwiki->cssProvider();

    QString html;

    auto addTblRow = [&css](QString &html, const QString &l, const QString &v) {
        html.append("<tr>");
        html.append("<td>");html.append(css->escape(l));html.append("</td>");
        html.append("<td>");html.append(css->escape(v));html.append("</td>");
        html.append("</tr>");
    };

    auto addRow = [&html, addTblRow](const QString &l, const QString &v) {
        addTblRow(html, l, v);
    };

    auto href = [&css](const QString &ref, const QString &txt) {
        QString s = "<a href=\"";
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

    QVector<ImageSrc> img_srcs;
    if (h.abbrevIsSet()) {
        QString abbrev = h.abbrev();
        img_srcs = img->getImageSrcs(abbrev);
        for(ImageSrc &img_src : img_srcs) {
            QString img_id = mkid("abbrev_img", ++_id);
            img_src.setImgId(img_id);

            QString image = "<img onclick=\"getElementById('";
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

    foreach(const ImageSrc &img_src, img_srcs) {
        QString img_id = img_src.imgId();
        html.append("<div class=\"bigpicture\" id=\"").append(img_id).append("\" ");
        html.append("onclick=\"getElementById('").append(img_id).append("').style.display='none';\" >");
        html.append("<img src=\"").append(img_src.imageSrc()).append("\" alt=\"").append(img_src.imageAlt()).
                append("\" />").append("</div>");
    }

    return html;
}

QList<QString> QMMWiki::HighlightWords::words() { return _words; }

void QMMWiki::HighlightWords::addWord(const QString &word) { _words.push_back(word); }

void QMMWiki::HighlightWords::clear() { _words.clear(); }

QMMWiki::Toc &QMMWiki::Toc::append(int level, const QString &toc_txt, const QString &toc_ref)
{
    _toc_levels.push_back(level);
    _toc_txts.push_back(toc_txt);
    _toc_refs.push_back(toc_ref);
    return *this;
}

int QMMWiki::Toc::size() const
{
    return _toc_levels.size();
}

bool QMMWiki::Toc::empty() const
{
    return _toc_levels.empty();
}

void QMMWiki::Toc::clear()
{
    _toc_levels.clear();
    _toc_txts.clear();
    _toc_refs.clear();
}

int QMMWiki::Toc::level(int idx) const
{
    return _toc_levels[idx];
}

QString QMMWiki::Toc::tocTxt(int idx) const
{
    return _toc_txts[idx];
}

QString QMMWiki::Toc::tocRef(int idx) const
{
    return _toc_refs[idx];
}

QMMWiki::QMMWiki(QObject *parent) : QObject(parent)
{
    _owns_providers = true;

    _image_provider = new ImageProvider();
    _css_provider = new CssProvider();
    _remedy_provider = new RemedyProvider();
    _link_provider = new LinkProvider();
    _incl_provider = new IncludeProvider();

    _meta_provider = new MetaProvider();
    _meta_provider->setMMWiki(this);

    re_prepare_modalities = QRegularExpression("[<>]\\[");
    re_html_prepare = QRegularExpression("([<>&'])");
    re_symptom = QRegularExpression("([CSGZR])([A-Z]*)[{]([^}{]*)[}]");
    re_markup = QRegularExpression("([234PQHNMITBRCLti^!+-]|R[234])\\[([^\\]\\[]*)\\]");
    re_modalities = QRegularExpression("([<>])\\[([^\\]\\[]*)\\]");
    re_literals = QRegularExpression("\\\\[:{}\\[\\]]|\\[BR\\]");
    re_html_implement = QRegularExpression("[@][!][@]([^@]+)[@][%][@]");
    re_symptom_grade = QRegularExpression("([234])\\[([^\\]\\[]*)\\]");
    re_section = QRegularExpression("^\\s*:begin\\[([a-z,]+)\\]\\s*$", QRegularExpression::MultilineOption);
    //re_section = QRegularExpression("\\s*:begin\\[([a-z,]+)\\]\\s*\n");
    re_end = QRegularExpression("^\\s*:end\\s*$", QRegularExpression::MultilineOption);
    //re_end = QRegularExpression("\\s*:end\\s*");
    re_anchor = QRegularExpression("N\\[[^\\]]+\\]");
    re_cleanup_sym_open = QRegularExpression("([CSGZRP])([A-Z]*)[{]");
    re_cleanup_open = QRegularExpression("([234PQHNMITBRCLti^!<>]|R[234])\\[([^|]+[|]){0,1}");
    re_cleanup_close = QRegularExpression("(\\]|[}])");
    re_bullits = QRegularExpression("^([*12]+)\\s");
    re_width = QRegularExpression("([0-9.]+)(%|em|pt|mm|cm){0,1}");

    _min_grade = 0;
}

QMMWiki::~QMMWiki()
{
    if (_owns_providers) {
        delete _meta_provider;
        delete _image_provider;
        delete _css_provider;
        delete _link_provider;
        delete _remedy_provider;
    }
}

void QMMWiki::setCssProvider(QMMWiki::CssProvider *provider) { delete _css_provider; _css_provider = provider; }

void QMMWiki::setImageProvider(QMMWiki::ImageProvider *provider) { delete _image_provider; _image_provider = provider; }

void QMMWiki::setMetaProvider(QMMWiki::MetaProvider *provider) { delete _meta_provider; _meta_provider = provider;_meta_provider->setMMWiki(this); }

void QMMWiki::setLinkProvider(QMMWiki::LinkProvider *l) { delete _link_provider; _link_provider = l; }

void QMMWiki::setRemedyProvider(QMMWiki::RemedyProvider *r) { delete _remedy_provider; _remedy_provider = r; }

void QMMWiki::setIncludeProvider(QMMWiki::IncludeProvider *i) { delete _incl_provider; _incl_provider = i; }

void QMMWiki::setHighlightWords(const QMMWiki::HighlightWords &words) { _highlight_words = words; }

QMMWiki::CssProvider *QMMWiki::cssProvider() { return _css_provider; }

QMMWiki::ImageProvider *QMMWiki::imageProvider() { return _image_provider; }

QMMWiki::MetaProvider *QMMWiki::metaProvider() { return _meta_provider; }

QMMWiki::RemedyProvider *QMMWiki::remedyProvider() { return _remedy_provider; }

QMMWiki::LinkProvider *QMMWiki::linkProvider() { return _link_provider; }

QMMWiki::IncludeProvider *QMMWiki::includeProvider() { return _incl_provider; }

QMMWiki::HighlightWords &QMMWiki::highlightWords() { return _highlight_words; }

QMMWiki::Header &QMMWiki::header() { return _header; }

void QMMWiki::setMinGrade(int grade) { _min_grade = grade; }

bool QMMWiki::applyRe(const QRegularExpression &re, QString s, std::function<void (QMMWiki::RegExpMatch &, int)> f, std::function<void (QString, int)> f_remains)
{
    bool applied = false;
    QRegularExpressionMatch match;
    int re_offset = 0;

    auto do_match = [&match, &s, &re]() {
        match = re.match(s);
        return match.hasMatch();
    };

    while(do_match()) {
        RegExpMatch m(s, match);
        f(m, re_offset);
        s = m.suffix();

        re_offset += m.position() + m.length();
        applied = true;
    }

    f_remains(s, re_offset);

    return applied;
}

QString QMMWiki::extractContents(const QString &lang)
{
    QRegularExpressionMatch match;

    QString first_language;
    int         first_position = -1;
    bool        has_first_language = false;

    QString en_language = QString("en");
    bool        has_en_language = false;
    int         en_position = -1;

    QString language = lang.toLower();
    bool        has_language = false;
    int         position = -1;

    QString mm_wiki_lang = _mm_wiki;

    auto do_match = [](const QString &s, QRegularExpressionMatch &match, QRegularExpression &re) {
        match = re.match(s);
        return match.hasMatch();
    };

    while(do_match(mm_wiki_lang, match, re_section) && !has_language) {
        RegExpMatch m(mm_wiki_lang, match);
        if (!m.empty()) {
            QString l = m[1].toLower();
            QStringList languages = l.split(",");

            auto contains = [&languages](const QString &l) {
                for(const QString &ll : languages) {
                    if (ll.trimmed() == l) return true;
                }
                return false;
            };

            if (contains(language)) {
                has_language = true;
                position = static_cast<int>(m.position() + m.length());
            } else if (contains(en_language)) {
                has_en_language = true;
                en_position = static_cast<int>(m.position() + m.length());
            } else if (!has_first_language) {
                has_first_language = true;
                first_position = static_cast<int>(m.position() + m.length());
                first_language = languages[0];
            }
        }
        mm_wiki_lang = m.suffix();
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
        metaProvider()->setLanguage(QString(""));
        return QString("");
    } else {
        metaProvider()->setLanguage(language);

        QString result = _mm_wiki.mid(position);

        if (do_match(result, match, re_end)) {
            RegExpMatch m(result, match);
            result = result.mid(0, m.position());
        }

        return result;
    }
}


#define QMMWIKI_SP(p, set) \
    { \
        if (ln.isKeyVal(p) || ln == p) { \
            set(ln.value().trimmed()); \
        } \
    }

QMMWiki::Header QMMWiki::extractHeader()
{
    QString head;
    QTextStream in(&_mm_wiki);
    QString line;

    Header h;

    bool in_head = true;

    auto setEndHead = [&in_head](QString ) {
        in_head = false;
    };

    auto addSource = [&h](QString value) {
        Source s;
        if (s.fromString(value)) {
            SourceMap m;
            if (h.sourcesIsSet()) {
                m = h.sources();
            }
            m.insert(s.letter(), s);
            h.setSources(m);
        }
    };

    auto getline = [](QTextStream &in, QString &line) {
        if (!in.atEnd()) {
            line = in.readLine();
            return true;
        } else {
            return false;
        }
    };

    while(in_head && getline(in, line)) {
        Line ln(line);
        QMMWIKI_SP(":begin", setEndHead)
        if (in_head) {
            QMMWIKI_SP(":book", h.setBook)
            QMMWIKI_SP(":edition", h.setEdition)
            QMMWIKI_SP(":author", h.setAuthor)
            QMMWIKI_SP(":editor", h.setEditor)
            QMMWIKI_SP(":abbrev", h.setAbbrev)
            QMMWIKI_SP(":latin-name", h.setLatinName)
            QMMWIKI_SP(":source", addSource)
            QMMWIKI_SP(":miasm", h.setMiasm)
            QMMWIKI_SP(":tendency", h.setTendency)
            QMMWIKI_SP(":class", h.setClassification)
        }
    }

    return h;
}


QString QMMWiki::prepareForHtml(QString mmwiki)
{
    QString n_mmwiki;
    // Create _mm_wiki while resolving literals
    applyRe(re_literals, mmwiki, [&n_mmwiki](RegExpMatch &m, int) {
        n_mmwiki.append(m.prefix());
        QString s_literal = m.str();
        QChar literal = s_literal[0];
        if (literal == '\\') {
            literal = s_literal[1];
            switch(literal.toLatin1()) {
            case ':': n_mmwiki.append("@!@#58@%@");break;
            case '{': n_mmwiki.append("@!@#123@%@");break;
            case '}': n_mmwiki.append("@!@#125@%@");break;
            case '[': n_mmwiki.append("@!@#91@%@");break;
            case ']': n_mmwiki.append("@!@#93@%@");break;
            }
        } else { // BR
            n_mmwiki.append("@!@br@%@");
        }
    }, [&n_mmwiki](QString remain, int) {
        n_mmwiki.append(remain);
    });

    return n_mmwiki;
}

QString QMMWiki::implementHtml(QString pre_html)
{
    QString n_html;
    applyRe(re_html_implement, pre_html, [&n_html](RegExpMatch &m, int) -> void {
        n_html.append(m.prefix());
        QString literal = m[1];
        if (literal == "br") {
            n_html.append("<br />");
        } else if (literal[0].isDigit()) {
            n_html.append("#").append(literal).append(";");
        } else {
            n_html.append("&").append(literal).append(";");
        }
    }, [&n_html](QString remain, int) -> void {
        n_html.append(remain);
    });
    return n_html;
}

void QMMWiki::addInclude(const QString &incl)
{
    QStringList l = incl.split(",");
    QString incl_page = "";
    QString div_class = "";
    if (l.size() > 0) incl_page = l[0].trimmed();
    if (l.size() > 1) div_class = l[1].trimmed();

    if (div_class != "") _html.append("<div class=\"").append(div_class).append("\">");
    if (incl_page != "") {
        QMMWiki *m = new QMMWiki();
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

void QMMWiki::addHead(int toc, int level, const QString &h, const QString &l, const QString &cl)
{
    endDiv(level, h);
    QString toc_ref = "";
    if (toc >= 0) { toc_ref = addToc(toc, l); }
    QString dash = (_one_per_line && !_no_dashes) ? " dash" : "";
    QString html_class = "";
    if (cl != "") {
        html_class.append(" class=\"").append(cl).append(dash).append("\" ");
    }
    if (toc_ref != "") {
        QString a;a.append("<span class=\"anchor\" id=\"").append(toc_ref).append("\">").append("</span>");toc_ref = a;
    }
    _html.append("<").append(h).append(html_class).
            append(" >").append(toc_ref).
            append(processSeq(l)).
            append("</").append(h).append(">");
    startDiv(level, h);
}

void QMMWiki::addRubric(int toc, int level, const QString &d, const QString &r)
{
    QString rubr = r.trimmed();
    if (rubr != "") {
        rubr[0] = rubr[0].toUpper();
        if (rubr.back() == '.') {
            rubr = rubr.left(rubr.length() - 1);
        }
        if (rubr.back() != ':') {
            rubr.append(": ");
        } else {
            rubr.append(" ");
        }
    }
    endDiv(level, d);
    startDiv(level, d);
    QString toc_ref = "";
    if (toc >= 0) { toc_ref=addToc(toc, r); }
    if (toc_ref != "") {
        QString a;a.append("<span class=\"anchor\" id=\"").append(toc_ref).append("\" >").append("</span>");toc_ref=a;
    }

    QString start;
    start.append("<span class=\"").append(d).append("\"").append(">").append(toc_ref).
            append(processSeq(rubr)).
            append("</span>");
    startSeq("", start);
}

void QMMWiki::startDiv(int level, const QString &div, const QString &div_id)
{
    QString p_div = (div_id == "") ? div : div_id;
    _divs.push_back(p_div);
    _div_levels.push_back(level);
    _html.append("<div class=\"").append(div).append("\" >");
    startSeq();
}

void QMMWiki::endDiv(int level, const QString &div)
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

void QMMWiki::startSeq(const QString &cl, const QString &start)
{
    _seq = "";
    _seq_class = cl;
    _seq_start = start;
}

void QMMWiki::endSeq()
{
    if (_seq != "" || _seq_start != "") {
        _html.append(processSeq(_seq, _seq_class, _seq_start, true, _min_grade));
        _seq = "";
        _seq_class = "";
        _seq_start = "";
    }
}

void QMMWiki::addSeq(const QMMWiki::Line &l)
{
    _seq.append(l.line());
    _seq.append(" ");   // each line has an '\n' to compensate for
}

void QMMWiki::startBullit(int level, const QString &b, const QMMWiki::Line &l)
{
    auto open = [this](const QString &b) {
        if (b == "*") _html.append("<ul>");
        else if (b == "1") _html.append("<ol>");
        else if (b == "2") _html.append("<ol class=\"a\">");
    };

    QString current;
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

void QMMWiki::endBullit(int level, const QString &b)
{
    auto close = [this](const QString &b) {
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

bool QMMWiki::isBullit(QMMWiki::Line &line, int &level, QString &b)
{
    auto do_match = [](QString l, QRegularExpressionMatch &m, QRegularExpression &re) {
        m = re.match(l);
        return m.hasMatch();
    };

    QRegularExpressionMatch match;
    QString l = line.line();
    if (do_match(l, match, re_bullits)) {
        RegExpMatch m(l, match);
        QString s = m[1];
        b = s.back();
        level = static_cast<int>(s.length());
        line = Line(m.suffix());
        return true;
    }
    return false;
}

QString QMMWiki::addToc(int level, const QString &l)
{
    QString toc_ref = mkid("head_ref", _toc.size() + 1);
    _toc.append(level, cleanupMM(l), toc_ref);
    return toc_ref;
}

QString QMMWiki::addPage(const QString &p)
{
    QString pg_ref = mkid("page_ref", _pages.size() + 1);
    _pages.append(0, cleanupMM(p), pg_ref);
    QString ref;
    ref.append("<span class=\"anchor\" id=\"").append(pg_ref).append("\"></span>");
    return ref;
}

void QMMWiki::addTable(const QString &l)
{
    QStringList parts = l.split(":");
    QString percentages;
    QString headers;
    QString table_width_percentage;
    QString table_class;
    if (parts.size() >= 1) percentages = parts[0].trimmed();
    if (parts.size() >= 2) headers = parts[1].trimmed();
    if (parts.size() >= 3) table_width_percentage = parts[2].trimmed();
    if (parts.size() >= 4) table_class = parts[3].trimmed();

    if (table_class != "") {
        QString c = " class=\"";
        c.append(table_class).append("\" ");
        table_class=c;
    }

    if (table_width_percentage != "") {
        QString c = " style=\"width:";
        c.append(table_width_percentage).append("%;\" ");
        table_width_percentage = c;
    }

    _html.append("<table").append(table_class).append(table_width_percentage).append("><tr>");

    QStringList v_percentages = percentages.split(",");
    QStringList v_headers = headers.split(",");
    QStringList::iterator it_p = v_percentages.begin();
    QStringList::iterator it_h = v_headers.begin();
    int cells = 0;
    while(it_p != v_percentages.end()) {
        QString p = (*it_p).trimmed();
        QString h = processSeq((it_h != v_headers.end()) ? (*it_h).trimmed() : "");
        p.replace("%", "");
        _html.append("<th width=\"").append(p.trimmed()).append("%\">").
                append(h).append("</th>");
        cells++;
        it_p++;
        if (it_h != v_headers.end()) it_h++;
    }
    _html.append("</tr>");
    _tables_cell.push_back(0);
    _tables_cells.push_back(cells);
}

void QMMWiki::endTable()
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

void QMMWiki::addCell(const QString &value)
{
    QStringList parts = value.split(":");
    QString align;
    QString cell_class;

    if (parts.size() > 0) align = parts[0].trimmed().toLower();
    if (parts.size() > 1) cell_class = parts[1].trimmed().toLower();

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

void QMMWiki::addImage(const QString &img_pars)
{
    QStringList v = img_pars.split(",");

    auto get = [&v](int idx, QString def) {
        if (v.size() > idx) { return v[idx].trimmed(); }
        else { return def; }
    };

    QString img = get(0, "");
    QString width_perc = get(1, "100");
    QString subscript = get(2, "");
    QString align = get(3, "center");
    QString cl = align;
    if (align == "left") { cl = "image-float-left"; }
    else if (align == "right") { cl = "image-float-right"; }
    else if (align == "center") { align = "margin-left:auto;margin-right:auto;";cl = "image-center"; }

    if (cl == "image-float-left" || cl == "image-float-right" || cl == "image-center") {
        if (width_perc == "") { width_perc = "100%"; }
    }

    QString src = _image_provider->getImageSrc(img);

    auto do_match = [](QString s, QRegularExpressionMatch &match, QRegularExpression &re) {
        match = re.match(s);
        return match.hasMatch();
    };

    if (width_perc != "") {
        QString a = " style=\"width:";
        QRegularExpressionMatch match;
        if (do_match(width_perc, match, re_width)) {
            RegExpMatch m(width_perc, match);
            QString w = m[1];
            QString unit = m[2];
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

QString QMMWiki::processSeq(QString s, const QString &cl, const QString &start, bool par, int min_grade)
{
    QString html;
    if (par) html.append(p(cl));

    html.append(start);

    QString ss;
    applyRe(re_prepare_modalities, s, [&ss](RegExpMatch &match, int) -> void {
        QChar c = match.str()[0];
        QString prefix = match.prefix();
        ss.append(prefix);
        if (c == '<') { ss.append("-["); }
        else if (c == '>') { ss.append("+["); }
    },
    [&ss](QString remain, int) { ss.append(remain); }
    );

    s = ss;
    ss = "";
    applyRe(re_html_prepare, s, [&ss](RegExpMatch &match, int) -> void {
        ss.append(match.prefix());
        QChar c = match.str()[0];
        switch(c.toLatin1()) {
        case '<': ss.append("@!@lt@%@"); break;
        case '>': ss.append("@!@gt@%@"); break;
        case '&': ss.append("@!@amp@%@"); break;
        case '"': ss.append("@!@quot@%@"); break;
        case '\'': ss.append("@!@apos@%@"); break;
        }
    },
    [&ss](QString remain, int) { ss.append(remain); }
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

QString QMMWiki::generateHtml(QMMWiki::Token &t)
{
    QString o, c, cl, m;
    QString content = t.contentIsSet() ? t.content() : "";
    QString modifier = t.modifier();
    QString pre;
    QString post;

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
        QString c = "<img src=\"";
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

    QString html;

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

QString QMMWiki::mkLink(QMMWiki::Token &t)
{
    Token &link = t.subTokens()[0];
    Token &content = t.subTokens()[2];
    return linkProvider()->mkLinkHRef(generateHtml(link), generateHtml(content));
}

QString QMMWiki::mkDefinition(QMMWiki::Token &t)
{
    Token &word = t.subTokens()[0];
    Token &definition = t.subTokens()[2];

    QString cl = "definition";
    if (t.content().indexOf("class=\"highlight\"") >= 0) {
        cl = "definition-with-highlight";
    }
    QString r = "<span class=\"";
    r.append(cl).append(" hover\">").
            append("<span class=\"tooltip def-tooltip\">").append(generateHtml(definition)).append("</span>").
            append(generateHtml(word)).
            append("</span>");
    return r;
}

QString QMMWiki::mkRemedy(int grade, QString &s)
{
    QString cl = "remedy";
    cl.append(QString::number(grade));
    QString uabbrev = remedyProvider()->unifyAbbrev(s);
    QString rem = uabbrev;
    if (remedyProvider()->hasLink(rem)) {
        QString link = "<a href=\"";
        link.append(remedyProvider()->getLink(rem)).
                append("\">").
                append(rem).
                append("</a>");
        rem = link;
    }

    QString r = "<span class=\"";
    r.append(cl).append(" hover\">").
            append("<span class=\"tooltip\">").
            append(remedyProvider()->getLatinName(uabbrev)).
            append("</span>").
            append(rem).
            append("</span>");
    return r;
}

QString QMMWiki::mkAnchor(QString &nm)
{
    return _link_provider->mkLinkId(nm);
}

bool QMMWiki::mkHtmlSource(const QString &source_letter, QString &pre, QString &post) const
{
    if (source_letter == "") { return false; }

    const SourceMap &sm = _header.sources();
    if (sm.contains(source_letter[0])) {
        const Source &src = sm.value(source_letter[0]);
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

QMMWiki::Token QMMWiki::filterOutGradedSymptoms(const QMMWiki::Token &t, int min_grade)
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
        QVector<Token> v;
        for(const Token &tt : t.subTokens()) {
            v.push_back(filterOutGradedSymptoms(tt, min_grade));
        }
        nt.setSubTokens(v);
        return nt;
    }
}

QMMWiki::Token QMMWiki::tokenize(QString s)
{
    Token root(Token::ROOT);
    root.setStart(0);
    root.setEnd(static_cast<int>(s.length()));
    root.setContent(s);

    doTokenize(root);
    cleanupTokenTree(root, root.content());

    return root;
}

QMMWiki::Token QMMWiki::extractTokens(QMMWiki::Token &root, std::function<bool (const QMMWiki::Token &)> f)
{
    Token n_root(Token::ROOT);
    getTokenSubTree(root, n_root, f);
    return n_root;
}

bool QMMWiki::getTokenSubTree(QMMWiki::Token &t, QMMWiki::Token &nt, std::function<bool (const QMMWiki::Token &)> f)
{
    nt = t;
    QVector<Token> nv;
    QVector<Token> &v = t.subTokens();
    for(Token &tt : v) {
        Token tt1;
        if (getTokenSubTree(tt, tt1, f)) {
            nv.push_back(tt1);
        }
    }
    nt.setSubTokens(nv);
    return f(nt);
}

void QMMWiki::insertToken(QMMWiki::Token &t, QMMWiki::Token &ins)
{
    if (t.contains(ins)) {
        QVector<Token> &v = t.subTokens();
        {
            QVector<Token> vv;
            int i, N;
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
            QVector<Token> vv;
            bool inserted = false;
            int i, N;
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

void QMMWiki::cleanupTokenTree(QMMWiki::Token &t, QString &original)
{
    //sortTokenTree(t);
    QVector<Token> &v = t.subTokens();

    if (v.size() == 0) {
        return; // content is oke.
    } else {
        QVector<Token> nv;
        int last_end = t.start();
        for(Token &tt : v) {
            int start = tt.start();

            if (start > last_end) {
                Token nt(Token::NONE);
                nt.setStart(last_end);
                nt.setEnd(start);
                nt.setContent(original.mid(nt.start(),nt.length()));
                nv.push_back(nt);
            }
            last_end = tt.end();

            nv.push_back(tt);
        }

        if (last_end < t.end()) {
            Token nt(Token::NONE);
            nt.setStart(last_end);
            nt.setEnd(t.end());
            nt.setContent(original.mid(nt.start(),nt.length()));
            nv.push_back(nt);
        }

        t.contentUnSet();

        for(Token &tt : nv) {
            cleanupTokenTree(tt, original);
        }

        t.setSubTokens(nv);
    }
}

void QMMWiki::sortTokenTree(QMMWiki::Token &t)
{
    QVector<Token> &v = t.subTokens();
    for(Token &tt : v) sortTokenTree(tt);
    std::sort(v.begin(), v.end(), [](const Token &a, const Token &b) {
        return a.start() < b.start();
    });
}

void QMMWiki::doTokenize(QMMWiki::Token &root)
{
    QString s = root.content();

    bool go_on = true;
    while(go_on) {
        QString ss;
        go_on = applyRe(re_markup, s, [this, &ss, &root](RegExpMatch &m, int re_offset) -> void {
            const QString &t = m[1];
            const QChar cmd = t[0];
            QString content = m[2];
            QString modifier;
            bool has_modifier = false;

            Token::Tok type = Token::NONE;

            switch(cmd.toLatin1()) {
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
                int idx = content.indexOf("|");
                if (idx >= 0) {
                    has_modifier = true;
                    modifier = content.mid(0, idx);
                    content = content.mid(idx + 1);
                }
            }
                break;
            case 'R': {
                int grade = 1;
                if (t != "R") grade = t.mid(1).toInt();
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
            case 'l':
            case 'L': {
                type = Token::LINK;
                int idx = content.indexOf("|");
                if (idx >= 0) {
                    has_modifier = true;
                    modifier = content.mid(0, idx);
                    content = content.mid(idx + 1);
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
        [&ss](QString remain, int) -> void {
                ss.append(remain);
    });

        s = ss;
    }

    go_on = true;
    while(go_on) {
        QString ss;
        go_on = applyRe(re_symptom, s, [this, &ss, &root](RegExpMatch &m, int re_offset) -> void {
            const QString &m_type = m[1];
            const QString &src = m[2];
            const QString &content = m[3];

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
            ss.append("  ").append((src == "") ? "" : " ");
            ss.append(content);
            ss.append(" ");
        },
        [&ss](QString remain, int) -> void {
                ss.append(remain);
    });
        s = ss;
    }
}

QString QMMWiki::p(const QString &cl)
{
    QString c(cl);

    if (_one_per_line && !_no_dashes) {
        c.append(" dash");
    }

    QString pp("<p ");
    if (c != "") {
        pp.append("class=\"").append(c).append("\"");
    }
    pp.append(">");
    return pp;
}

QString QMMWiki::span(const QString &cl, const QString &l)
{
    QString s("<span class=\"");
    return s.append(cl).append("\">").
            append(l).append("</span>");
}

void QMMWiki::generateSymptoms(const QString &mmwiki, const QString &language)
{
    _mm_wiki = mmwiki;
    QString contents = extractContents(language);
    QVector<std::pair<int, char>> literals;
    contents = prepareForSymptoms(contents);

    QStringList lines = contents.split("\n");

    QString rubric = "";
    QString seq;

    _symptoms.clear();


    auto processSeq = [this, &rubric, &seq]() {
        if (seq == "") return;

        Token root = tokenize(seq);
        QVector<Symptom> v = extractSymptoms(root);

        _symptoms.insert(rubric, v);

        seq = "";
    };


    for(QString ln : lines) {
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

QVector<QString> QMMWiki::symptomRubrics() const
{
    QVector<QString> rubrics;
    QHash<QString, QVector<Symptom>>::const_iterator it = _symptoms.begin();
    while(it != _symptoms.end()) {
        if (it.value().size() > 0)
            rubrics.push_back(it.key());
        it++;
    }
    return rubrics;
}

QVector<QMMWiki::Symptom> QMMWiki::symptoms(const QString &rubric) const
{
    if (_symptoms.contains(rubric)) {
        return _symptoms.value(rubric);
    } else {
        return _empty_symptoms;
    }
}

QString QMMWiki::toHtml(const QString &mmwiki, bool one_per_line, const QString &language)
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
    QString contents = extractContents(language);

    contents = prepareForHtml(contents);

    // Prepare highlighting
    QStringList highlight_words = _highlight_words.words();
    if (highlight_words.size() > 0) {
        QString re = "\\b(";
        QString delim = "";
        for(QString word : highlight_words) {
            re.append(delim).append(word);
            delim = "|";
        }
        re.append(")\\b");
        QRegularExpression hre(re, QRegularExpression::CaseInsensitiveOption);
        contents = contents.replace(hre, "H[\\1]");
    }

    // Create HTML

    // iterate over all the content

    QStringList lines = contents.split("\n");
    foreach(QString ln, lines) {
        Line line(ln);
        int level;
        QString bullit;

        if (line.isKeyVal(":include")) {
            addInclude(line.value());
        } else if (line == ":no-dash-begin") {
            _no_dashes = true;
        } else if (line == ":no-dash-end") {
            _no_dashes = false;
        } else if (line.isKeyVal(":book")) {
            _header.setBook(line.value());
        } else if (line.isKeyVal(":h1")) {
            addHead(-1, 15, "h1", line.value(), "h1");
        } else if (line.isKeyVal(":h2")) {
            addHead(-1, 15, "h2", line.value(), "h2");
        } else if (line.isKeyVal(":h3")) {
            addHead(-1, 15, "h3", line.value(), "h3");
        } else if (line.isKeyVal(":h4")) {
            addHead(-1, 15, "h4", line.value(), "h4");
        } else if (line.isKeyVal(":h5")) {
            addHead(-1, 15, "h5", line.value(), "h5");
        } else if (line.isKeyVal(":local-name")) {
            addHead(-1, 20, "h2", line.value(), "local-name");
        } else if (line.isKeyVal(":source")) {
            addHead(-1, 30, "h3", line.value(), "source");
        } else if (line.isKeyVal(":head")) {
            addHead(0, 40, "h4", line.value());
        } else if (line.isKeyVal(":head2")) {
            addHead(1, 50, "h5", line.value());
        } else if (line.isKeyVal(":rubric")) {
            addRubric(2, 60, "rubric", line.value());
        } else if (line.isKeyVal(":rubric2")) {
            addRubric(3, 70, "rubric2", line.value());
        } else if (line.isKeyVal(":div-begin") || line.isKeyVal(":div")) {
            startDiv(-1, line.value().trimmed(), "div-begin");
        } else if (line == ":div-end" || line == ":end-div") {
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

const QMMWiki::Toc &QMMWiki::toc()
{
    return _toc;
}

const QMMWiki::Toc &QMMWiki::pages()
{
    return _pages;
}

bool QMMWiki::checkMMText(const QString &mm, QStringList &results)
{
    QString mmp = prepareForHtml(mm);

    QStringList keywords;
    keywords << ":begin"
             << ":include"
             << ":no-dash-begin" << ":no-dash-end"
             << ":local-name"
             << ":source"
             << ":head" << ":head2"
             << ":rubric" << ":rubric2"
             << ":div-begin" << ":div-end"
             << ":image"
             << ":table" << ":table-begin" << ":end-table" << ":table-end" << ":cell"
             << ":note" << ":note-begin" << ":note-end" << ":end-note"
             << ":code" << ":end-code"
             << ":end";

    QStringList header_words;
    header_words << ":book" << ":edition" << ":author" << ":editor"
                 << ":abbrev" << ":latin-name" << ":source"
                 << ":miasm" << ":tendency" << ":class";

    auto checkSeq = [](bool ok, QString &seq, QStringList &results, int &symptoms, int line_nr) {
        QStack<QChar> opens;

        auto chk_close = [&opens, &results, &line_nr](int i, QChar c) {
            if (opens.isEmpty()) {
                results.append(tr("%1: Unexpected closer at character %2, got %3").arg(
                                   QString::asprintf("%04d", line_nr),
                                   QString::asprintf("%d", i),
                                   c
                                   )
                               );
                return false;

            } else if (opens.top() != c) {
                results.append(tr("%1: Wrong closer at character %2 expected %3, got %4").arg(
                                   QString::asprintf("%04d", line_nr),
                                   QString::number(i),
                                   opens.top(),
                                   c
                                   )
                               );
                return false;
            } else {
                return true;
            }
        };

        QString s = seq;
        seq = "";
        int c_nr = 0;

        int i, N;
        for(i = 0, N = s.length(); i < N; i++) {
            c_nr += 1;
            if (s[i] == '{') { opens.push('}');symptoms += 1; }
            else if (s[i] == '[') { opens.push(']'); }
            else if ((s[i] == '}' || s[i] == ']')) {
                if (chk_close(c_nr, s[i])) {
                    opens.pop();
                } else {
                    ok = false;
                }
            } else if (s[i] == '\n') {
                line_nr += 1;
                c_nr = 0;
            }
        }

        if (opens.isEmpty()) {
            return ok;
        } else {
            QString closers = "";
            while(!opens.isEmpty()) { closers.append(opens.top()); opens.pop(); }
            results.append(tr("%1: Not enough closers at character %2, expected %3").arg(
                               QString::asprintf("%04d", line_nr),
                               QString::asprintf("%d", c_nr),
                               closers
                               )
                           );
            return false;
        }
    };

    bool ok = true;
    bool in_head = true;
    bool in_content = false;
    int line_nr = 0;
    int seq_start_line_nr = -1;
    QStringList lines = mmp.split("\n");
    QString seq = "";
    int symptoms = 0;

    foreach(QString ln, lines) {
        Line l(ln);
        line_nr += 1;
        if (in_head) {
            if (!l.has(header_words)) {
                if (l.startsWith(":begin[")) {
                    in_head = false;
                    in_content = true;
                    seq_start_line_nr = line_nr;
                } else if (!l.isEmpty()) {
                    results.append(tr("%1: Unknown keyword in header (%2)").arg(
                                       QString::asprintf("%04d", line_nr),
                                       l.line()
                                       )
                                   );
                    ok = false;
                }
            }
        } else {
            if (l.isEmpty()) {
                ok = checkSeq(ok, seq, results, symptoms, seq_start_line_nr);
                seq_start_line_nr = -1;
            } else if (l.has(keywords)) {
                if (l.startsWith(":begin[")) {
                    if (in_content) {
                        results.append(tr("%1: Missing :end (%2)").arg(
                                           QString::asprintf("%04d", line_nr),
                                           l.line()
                                           )
                                       );
                        ok = false;
                    } else {
                        in_content = true;
                    }
                } else if (l == ":end") {
                    in_content = false;
                }
                ok = checkSeq(ok, seq, results, symptoms, seq_start_line_nr);
                seq_start_line_nr = -1;
            } else {
                if (seq_start_line_nr < 0) { seq_start_line_nr = line_nr; }
                seq += l.line() + "\n";
            }
        }
    }

    if (in_content) {
        results.append(tr("%1: Missing trailing :end").arg(QString::asprintf("%04d", line_nr)));
        ok = checkSeq(false, seq, results, symptoms, seq_start_line_nr);
    }

    results.append(tr("%1: Symptoms = %2").arg(QString::asprintf("%04d", 10), QString::asprintf("%d", symptoms)));
    if (symptoms == 0) { ok = false; }

    return ok;
}

QString QMMWiki::prepareForSymptoms(QString s)
{
    QString ns;
    applyRe(re_literals, s, [&ns](RegExpMatch &m, int ) {
        ns.append(m.prefix());
        QString s_literal = m.str();
        QChar literal = s_literal[0];
        if (literal == '\\') {
            literal = s_literal[1];
            ns.append(literal);
        } else {
            // BR
            ns.append(" ");
        }
    }, [&ns](QString remain, int) {
        ns.append(remain);
    });

    s = ns;
    ns = "";
    applyRe(re_modalities, s, [&ns](RegExpMatch &m, int) {
        ns.append(m.prefix());
        QString open = m[1];
        if (open == "<") { ns.append("-"); }
        else if (open == ">") { ns.append("+"); }
        ns.append("[");
        ns.append(m[2]);
        ns.append("]");
    }, [&ns](QString remain, int) {
        ns.append(remain);
    });

    return ns;
}

QString QMMWiki::cleanupMM(QString s)
{
    s.replace(re_anchor, "");
    s.replace(re_cleanup_open, "");
    s.replace(re_cleanup_sym_open, "");
    s.replace(re_cleanup_close, "");

    QString ns;
    applyRe(re_html_implement, s, [&ns](RegExpMatch &m, int) -> void {
        ns.append(m.prefix());
        QString literal = m[1];
        if (literal == "br") ns.append(" ");
        else if (literal == "gt") ns.append(">");
        else if (literal == "lt") ns.append("<");
        else if (literal == "apos") ns.append("'");
        else if (literal == "quot") ns.append("\"");
        else if (literal == "amp") ns.append("&");
        else { // assume ascii value
            int asc = literal.toInt();
            ns.push_back(static_cast<char>(asc));
        }
    }, [&ns](QString remain, int) {
        ns.append(remain);
    });

    return ns;
}

QVector<QMMWiki::Symptom> QMMWiki::extractSymptoms(const QMMWiki::Token &t)
{
    Token::Tok type = t.type();
    Symptom::Type st = Symptom::NONE;
    switch(type) {
    case Token::SYMPTOM: st = Symptom::SYMPTOM;break;
    case Token::RELATION: st = Symptom::RELATION;break;
    case Token::CAUSE: st = Symptom::CAUSE;break;
    default: break;
    }
    QVector<Symptom> v;
    if (st != Symptom::NONE) {
        Symptom s;
        Symptom::Graded g;

        s.setType(st);
        g.setStart(0);g.setGrade(1);
        int pos = 0;

        extractSymptom(s, g, pos, t);

        g.setEnd(pos);

        QVector<Symptom::Graded> &vg = s.grades();
        vg.push_back(g);

        std::sort(vg.begin(), vg.end(), [](const Symptom::Graded &a, const Symptom::Graded &b) {
            if (a.start() < b.start()) return true;
            else if (a.start() > b.start()) return false;
            else return a.grade() < b.grade();
        });

        QVector<Symptom::Graded> nvg;
        QVector<Symptom::Graded>::iterator it, it1;
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
            QVector<Symptom> vv = extractSymptoms(tt);
            v.append(vv);
        }
    }
    return v;
}

void QMMWiki::extractSymptom(QMMWiki::Symptom &s, QMMWiki::Symptom::Graded &g, int &pos, const QMMWiki::Token &t)
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
            const QVector<Token> &v = t.subTokens();
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

QMMWiki::RegExpMatch::RegExpMatch(QString &s, QRegularExpressionMatch &m)
    : _s(s), _m(m)
{
}

QString QMMWiki::RegExpMatch::prefix()
{
    return _s.left(position());
}

QString QMMWiki::RegExpMatch::suffix()
{
    return _s.mid(position() + length());
}

QString QMMWiki::RegExpMatch::str()
{
    return _m.captured(0);
}

bool QMMWiki::RegExpMatch::empty()
{
    return !_m.hasMatch();
}

int QMMWiki::RegExpMatch::position(int idx)
{
    return _m.capturedStart(idx);
}

int QMMWiki::RegExpMatch::length(int idx)
{
    return _m.capturedLength(idx);
}

int QMMWiki::RegExpMatch::size()
{
    return _m.lastCapturedIndex() + 1;
}

QString QMMWiki::RegExpMatch::operator[](int idx)
{
    return _m.captured(idx);
}
