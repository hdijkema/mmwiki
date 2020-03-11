:book=ZC4 Manual
:edition=2020-1
:author=Homeopathie Nederland B.V.
:editor=Hans Dijkema
:source=C,<small>By Homeopathie Nederland</small>,<sup>δ</sup>,<b>,</b>

:begin[en]

:head=A Standard Materia Medica Language 

ZC4 introduces a standard and open source GC{L[https://homeopathie-nederland.nl/mmwiki|Materia Medica Wiki language (mmwiki format)]} that can be used to store Materia Medica in a structured and easy to edit way. An mmwiki reference parser implementation in C++ is available at L[github|github]. 

The MMWiki format for Materia Medica and Wiki content consists of language elements that make it possible to construct complex pages which can be used to display materia medica (and other material) that contain symptoms, graded-symptoms, etc. in a structured way. 

This manual is used to explain the MMWiki format and is written in this same format. 

:head2=A page
 
Each page consists of two parts: A B[I[header]] and B[I[content]]. Example:

:div-begin=indent
:code
2[:book]=B[ZC4 Manual][BR]
2[:edition]=B[2020-1][BR]
2[:author]=B[Homeopathie Nederland B.V.][BR]
2[:editor]=B[Hans Dijkema][BR]
2[:source]=B[C,<small>By Homeopathie Nederland</small>,<sup>*</sup>,<b>,</b>][BR]

2[:begin][en][BR]
(...)[BR]
2[:end][BR]

2[:begin][nl,nld,dut][BR]
(...)[BR]
2[:end][BR]
:end-code
:div-end

:rubric=Header
The 2[header] contains 2[parameters]. Parameters can contain information about the book, edition, author, etc. of a Materia Medica (or other) page. Also sources for information can be given. 

Each parameter starts with a colon (2[:]). 

:rubric=Content
2[Content] I[must] be provided in at least one language. It I[may] be provided in multiple languages. Each language-part starts with a Q[begin[<language,...>]] statement. 

The 2[mmwiki parser] I[must] be able to extract the contents for a wanted language and fall back to B[en] (first option) or B[the first language of the page] (second option) if the wanted language is not provided by the page.

The language parameter I[must] follow the international L[https://www.iso.org/iso-639-language-codes.html|ISO649] language codes (see L[https://www.loc.gov/standards/iso639-2/php/code_list.php|list of codes]). It I[may] consist of multiple language codes that indicate the same language, which is considered best practice.  

:head2=Header

A header can contain the following parameters:

:table=30,70:Parameter,Explanation::center compact

:cell=L:def
2[:book]=B[<Title of book>]
:cell=L
Denotes the title of the book of which this (materia medica) page is part.
:cell=L:def
2[:author]=B[<Author of book>]
:cell=L
Denotes the original author(s) of the book of which this (materia medica) page is part.
:cell=L:def
2[:edition]=B[<Edition of the book>]
:cell=L
Denotes the edition of the book given by 2[:book].
:cell=L:def
2[:editor]=B[<Editor of the book>]
:cell=L
Denotes the editor of the book given by 2[:book].
:cell=L:def
2[:abbrev]=B[<Standard Uniform Abbreviation of a remedy>]
:cell=L
Gives the L[wiki://zc4_en_standard_uniform_abbreviation|Standard Uniform Abbreviation] of the remedy for which materia medica is provided with this page.
:cell=L:def
2[:latin-name]=B[<Latin name of the given 2[:abbrev]>]
:cell=L
Gives the latin name of the given abbreviation. See the list of L[wiki://zc4_en_standard_uniform_abbreviation|Standard Uniform Abbreviations] for the latin names.
:cell=L:def
2[:source]=B[<letter>,<tooltip-html>,<indicator>,![,<prefix-html>![,<postfix-html>]]{0..*}]
:cell=L
Provides a source for the content. A source consists of:
1 a 2[single letter], which I[must] be in the range 'A' to 'Z';
1 a 2[source indicator], which I[must] be shown before the symptom, cause, relation, group or sentence in which it is used;
1 a 2[tooltip] specified in HTML, which I[must] be shown on hovering the 2[source indicator];
1 I[optional] a HTML prefix, which I[must] be shown before the symptom, cause, relation, group or sentence;
1 I[optional] a HTML postfix, which I[must] be shown after the symptom, cause, relation, group or sentence.
:cell=L:def
2[:miasm]=B[<main miasm(s) of the remedy>]
:cell=L
Indicates the main miasm(s) of the remedy, i.e. Psoric (R[Psor]), Medhorrenic (R[Med]) and/or Syphilitic (R[Syph]). If present, I[can] be shown as metadata.
:cell=L:def
2[:tendency]=B[<main tendency (tendencies) of the remedy>]
:cell=L
Indicates the main tendency(ies) of the remedy, i.e. Carcinogenic (R[Carc]) or Tuberculinic (R[Tub]). If present, I[can] be shown as metadata.
:cell=L:def
2[:class]=B[<most probable classification of the remedy>]
:cell=L
Indicates possible classifications of the remedy (exogenic, endogenic, psoric, epidemic, etc.). If present, I[can] be shown as metadata.
:end-table

:end

