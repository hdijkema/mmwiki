#ifndef QMMWIKI_H
#define QMMWIKI_H

#include <QObject>
#include <QHash>
#include <QVector>
#include <QList>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QTextStream>

#include <mmwiki_css.h>

#define QMMWIKI_PROP(type, v, setter) \
    private: \
        type    _##v;           \
        bool    _##v##_is_set;    \
    public: \
        const type &v() const; \
        type &v(); \
        void setter(const type &w); \
        bool v##IsSet() const; \
        void v##UnSet();

class QMMWiki : public QObject
{
    Q_OBJECT
private:
    class Line
    {
    private:
        QString _line_ci;
        QString _line;
        QString _value;

    public:
        Line(const QString &l);

    public:
        bool isKeyVal(const QString &key);
        QString value();
        bool operator ==(const QString &key);
        bool isEmpty() const;
        QString line() const;
    };

public:
    class Token
    {
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
        QMMWIKI_PROP(Tok, type, setType)
        QMMWIKI_PROP(QString, content, setContent)
        QMMWIKI_PROP(QString, modifier, setModifier)
        QMMWIKI_PROP(int, start, setStart)
        QMMWIKI_PROP(int, end, setEnd)
        QMMWIKI_PROP(QVector<Token>, subTokens, setSubTokens)
    public:
        Token();
        Token(Tok a);
    public:
        bool contains(const Token &other);
        bool equal(const Token &other);
        int  length() const;
    public:
        bool hasA(std::function<bool(const Token &t)> f) const;
    public:
        bool operator <(const Token &b) const;
    };

public:
    class Symptom
    {
    public:
        enum Type { NONE = 0, SYMPTOM = 1, CAUSE = 2, RELATION = 3 };
        class Graded
        {
            QMMWIKI_PROP(int, start, setStart)
            QMMWIKI_PROP(int, end, setEnd)
            QMMWIKI_PROP(int, grade, setGrade)
        public:
            Graded();
        public:
            int length() const;
        };

        QMMWIKI_PROP(Type, type, setType)
        QMMWIKI_PROP(QVector<Graded>, grades, setGrades)
        QMMWIKI_PROP(QString, symptom, setSymptom)
    public:
        Symptom();

    public:
        int maxGrade() const;
        int gradedCount() const;
        int gradeOf(int idx) const;
        const QString textOf(int idx) const;
    };

public:
    class RemedyProvider
    {
    public:
        virtual ~RemedyProvider();
    public:
        virtual QString getLatinName(const QString &uniform_abbrev);
        virtual QString getLink(const QString &uniform_abbrev);
        virtual bool hasLink(const QString &uniform_abbrev);
        virtual QString unifyAbbrev(const QString &abbrev);
    };

public:
    class CssProvider
    {
    public:
        virtual QString css() const;
        virtual QString escape(const QString &to_html);
    public:
        virtual ~CssProvider();
    };

public:
    class ImageSrc {
    public:
        ImageSrc();

        QMMWIKI_PROP(QString, author, setAuthor)
        QMMWIKI_PROP(QString, authorUrl, setAuthorUrl)
        QMMWIKI_PROP(QString, source, setSource)
        QMMWIKI_PROP(QString, sourceUrl, setSourceUrl)
        QMMWIKI_PROP(QString, license, setLicense)
        QMMWIKI_PROP(QString, licenseUrl, setLicenseUrl)
        QMMWIKI_PROP(QString, imageSrc, setImageSrc)
        QMMWIKI_PROP(QString, imageAlt, setImageAlt)
        QMMWIKI_PROP(QString, imgId, setImgId)
    };

public:
    class ImageProvider
    {
    public:
        virtual QString getImageSrc(const QString &image);
        virtual QVector<ImageSrc> getImageSrcs(const QString &uniform_abbrev);
    public:
        virtual ~ImageProvider();
    };

public:
    class IncludeProvider
    {
    public:
        virtual QString getPage(const QString &pagename);
    public:
        virtual ~IncludeProvider();
    };

public:
    class LinkProvider {
    private:
        mutable int link_id;
        QMMWIKI_PROP(QString, namePrefix, setNamePrefix)
    public:
        virtual QString mkLinkId(QString name) const;
        virtual QString mkLinkHRef(QString href, QString content) const;
    public:
        LinkProvider();
        virtual ~LinkProvider();
    };


public:
    class Source {
        QMMWIKI_PROP(QChar, letter, setLetter)
        QMMWIKI_PROP(QString, source, setSource)
        QMMWIKI_PROP(QString, htmlMark, setHtmlMark)
        QMMWIKI_PROP(QString, htmlPre, setHtmlPre)
        QMMWIKI_PROP(QString, htmlPost, setHtmlPost)
    public:
        Source();
    public:
        bool fromString(QString s);
    };

    typedef QHash<QChar, Source> SourceMap;

public:
    class Header {
        QMMWIKI_PROP(QString, book, setBook)
        QMMWIKI_PROP(QString, author, setAuthor)
        QMMWIKI_PROP(QString, edition, setEdition)
        QMMWIKI_PROP(QString, editor, setEditor)
        QMMWIKI_PROP(QString, abbrev, setAbbrev)
        QMMWIKI_PROP(QString, latinName, setLatinName)
        QMMWIKI_PROP(SourceMap, sources, setSources)
        QMMWIKI_PROP(QString, tendency, setTendency)
        QMMWIKI_PROP(QString, miasm, setMiasm)
        QMMWIKI_PROP(QString, classification, setClassification)
    public:
        Header();
    };

public:
    class MetaProvider
    {
    private:
        QMMWiki        *_mmwiki;
        int             _id;
    public:
        void setMMWiki(QMMWiki *m);
    public:
        virtual QString getMeta();

        virtual QString lbClassification();
        virtual QString lbTendency();
        virtual QString lbMiasm();
        virtual QString lbImage();
        virtual QString lbImageAuthor();
        virtual QString lbLicense();
        virtual QString lbSourceInfo();
    public:
        MetaProvider();
        virtual ~MetaProvider();
    public:
        QMMWIKI_PROP(QString, language, setLanguage)
    };

public:
    class HighlightWords
    {
    private:
        QList<QString> _words;
    public:
        QList<QString> words();
        void           addWord(const QString &word);
        void           clear();
    };

public:
    class Toc {
    private:
        QVector<int>        _toc_levels;
        QVector<QString>    _toc_txts;
        QVector<QString>    _toc_refs;
    public:
        Toc &append(int level, const QString &toc_txt, const QString &toc_ref);
        int size() const;
        bool empty() const;
        void clear();
        int level(int idx) const;
        QString tocTxt(int idx) const;
        QString tocRef(int idx) const;
    };

private:
    class RegExpMatch
    {
    private:
        QString                 &_s;
        QRegularExpressionMatch &_m;
    public:
        RegExpMatch(QString &s, QRegularExpressionMatch &m);
    public:
        QString prefix();
        QString suffix();
        QString str();
    public:
        bool    empty();
    public:
        int     position(int idx = 0);
        int     length(int idx = 0);
    public:
        int     caps();
        int size();
    public:
        QString operator[](int idx);
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

    QString     _mm_wiki;
    QString     _html;

    // Table of contents
    Toc             _toc;
    Toc             _pages;

    // Take symptoms of minimum grade
    int             _min_grade;

    // Symptoms
    QString                          _current_rubric;
    QHash<QString, QVector<Symptom>> _symptoms;
    QVector<Symptom>                 _empty_symptoms;

    // State
    QString                 _req_language;
    bool                    _one_per_line;
    bool                    _no_dashes;
    QVector<QString>        _divs;
    QVector<int>            _div_levels;
    QString                 _seq;
    QString                 _seq_class;
    QString                 _seq_start;
    QVector<QString>        _bullits;
    QVector<int>            _bullit_levels;
    QVector<int>            _tables_cell;
    QVector<int>            _tables_cells;

private:
    // Regexes
    QRegularExpression re_prepare_modalities;
    QRegularExpression re_html_prepare;
    QRegularExpression re_symptom;
    QRegularExpression re_markup;
    QRegularExpression re_modalities;
    QRegularExpression re_literals;
    QRegularExpression re_html_implement;
    QRegularExpression re_symptom_grade;
    QRegularExpression re_anchor;
    QRegularExpression re_cleanup_open;
    QRegularExpression re_cleanup_sym_open;
    QRegularExpression re_cleanup_close;
    QRegularExpression re_section;
    QRegularExpression re_end;
    QRegularExpression re_bullits;
    QRegularExpression re_width;

public:
    explicit QMMWiki(QObject *parent = nullptr);
    ~QMMWiki();

public:
    void setCssProvider(CssProvider *provider);
    void setImageProvider(ImageProvider *provider);
    void setMetaProvider(MetaProvider *provider);
    void setLinkProvider(LinkProvider *l);
    void setRemedyProvider(RemedyProvider *r);
    void setIncludeProvider(IncludeProvider *i);

    void setHighlightWords(const HighlightWords &words);

    CssProvider *cssProvider();
    ImageProvider *imageProvider();
    MetaProvider *metaProvider();
    RemedyProvider *remedyProvider();
    LinkProvider *linkProvider();
    IncludeProvider *includeProvider();

    HighlightWords &highlightWords();
    Header &header();

    void setMinGrade(int grade);

public:
    void generateSymptoms(const QString &mmwiki, const QString &language = QString("en"));
    QVector<QString> symptomRubrics() const;
    QVector<Symptom> symptoms(const QString &rubric) const;

public:
    QString toHtml(const QString &mmwiki, bool one_per_line = false, const QString &language = QString("en"));
    const Toc &toc();
    const Toc &pages();

private:
    bool applyRe(const QRegularExpression &re, QString s,
                 std::function<void(RegExpMatch &, int)> f,
                 std::function<void(QString, int)> f_remains);

    QString extractContents(const QString &lang);
    Header extractHeader();

private:
    void addInclude(const QString &incl);
    void addHead(int toc, int level, const QString &h, const QString &l, const QString &cl = "");
    void addRubric(int toc, int level, const QString &d, const QString &r);

    void startDiv(int level, const QString &div, const QString &div_id = "");
    void endDiv(int level, const QString &div);
    void startSeq(const QString &cl = "", const QString &start = "");

    void endSeq();
    void addSeq(const Line &l);

    void startBullit(int level, const QString &b, const Line &l);
    void endBullit(int level, const QString &b);
    bool isBullit(Line &line, int &level, QString &b);

    QString addToc(int level, const QString &l);
    QString addPage(const QString &p);

    void addTable(const QString &l);
    void endTable();
    void addCell(const QString &value);

    void addImage(const QString &img_pars);
	
	void addDynamic();
	void addEndDynamic();
	void addJs(const QString &line);

private:
    QString processSeq(QString s, const QString &cl = "", const QString &start = "", bool par = false, int min_grade = 0);

private:
    QString generateHtml(Token &t);
    QString mkLink(Token &t);
    QString mkDefinition(Token &t);
    QString mkRemedy(int grade, QString &s);
    QString mkAnchor(QString &nm);
    bool mkHtmlSource(const QString &source_letter, QString &pre, QString &post) const;

private:
    Token filterOutGradedSymptoms(const Token &t, int min_grade);
    Token tokenize(QString s);
    Token extractTokens(Token &root, std::function<bool(const Token &t)> f);
    bool getTokenSubTree(Token &t, Token &nt, std::function<bool(const Token &t)> f);
    void insertToken(Token &t, Token &ins);
    void cleanupTokenTree(Token &t, QString &original);
    void sortTokenTree(Token &t);
    void doTokenize(Token &root);

private:
    QString p(const QString &cl);
    QString span(const QString &cl, const QString &l);
    // to use after wikitext has been prepared
    QString cleanupMM(QString s);
    QString prepareForHtml(QString mmwiki);
    QString implementHtml(QString pre_html);

private:
    QString prepareForSymptoms(QString s);
    QVector<Symptom> extractSymptoms(const Token &t);
    void extractSymptom(Symptom &s, Symptom::Graded &g, int &pos, const Token &t);
};

#endif // QMMWIKI_H
