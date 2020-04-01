:book=ZC4 Manual
:edition=2020-1
:author=Homeopathie Nederland B.V.
:editor=Hans Dijkema
:source=C,<small>By Homeopathie Nederland</small>,<sup>δ</sup>,<b>,</b>
:source=P,Phatak,•

:begin[nl,dut,nld]

:book=Een Quick Reference voor de MM Wiki taal

:source=Quick Reference voor de MM Wiki taal

:head=Pagina indeling

Iedere pagina van een website bestaat uit een kop  en inhoud. De onderstaande code geeft dit weer.
De B[Q[:book]] tag wordt de titel van de web-pagina, zoals die wordt getoond in je browser.

Je kunt per taal inhoud maken. Iedere B[taalsectie] begint met B[Q[:begin![taal,...]]] en eindigt met B[Q[:end]].

De B[Q[:book]] tag kun je herhalen in een taalsectie, zodat de titel van je webpagina in de taal komt die de webbrowser gebruikt.

Als je niet een specifieke taal hebt voor een gegeven B[taal-afkorting] (nl, de, en, fr, etc.), dan zal er worden teruggevallen op engels (en), tenzij die taalsectie er niet is, dan wordt de 1e taalsectie genomen. 

:div=indent
:code
2[:book]=B[ZC4 Manual][BR]
2[:edition]=B[2020-1][BR]
2[:author]=B[Homeopathie Nederland B.V.][BR]
2[:editor]=B[Hans Dijkema][BR]
2[:source]=B[C,<small>By Homeopathie Nederland</small>,<sup>•</sup>,<b>,</b>][BR]

2[:begin][en][BR]
(...)[BR]
2[:end][BR]

2[:begin][nl,nld,dut][BR]
2[:book]=B[ZC4 Handleiding][BR]
(...)[BR]
2[:end][BR]
:end-code
:end-div

:head=Koppen

De volgende koppen worden ondersteund. Onderstaand voor echte Materia Medica pagina's. 

:div=indent
:code
2[:local-name]=B[Header niveau 2 in HTML][BR]
2[:source]=B[Header niveau 3 in HTML][BR]
2[:head]=B[Header niveau 4 in HTML][BR]
2[:head2]=B[Header niveau 5 in HTML][BR]
2[:rubric]=B[Een kop voor een rubriek (bijvoorbeeld 'Relaties')][BR]
2[:rubric2]=B[Een sub-rubriek voor een kop (bijvoorbeeld 'Beter')][BR]
:end-code
:end-div

Voor gewone Web-pagina's, kun je simpelweg gebruik maken van Q[:h1] t/m Q[:h5] voor koppen.

:div=indent
:code
2[:h1]=B[Header niveau 1][BR]
2[:h2]=B[Header niveau 2][BR]
2[:h3]=B[Header niveau 3][BR]
2[:h4]=B[Header niveau 4][BR]
2[:h5]=B[Header niveau 5][BR]
:end-code
:end-div

:head=Opmaak Secties

Een opmaak sectie begin je met B[Q[:div]] en eindig je met B[Q[:end-div]]. Een opmaak sectie representeert in HTML een zogenaamde 'class'. Dit is een L[https://www.w3schools.com/css/|Cascading Stylesheets (CSS) term]. 

Voorbeeld:

:div=indent
:code
2[:div]=B[indent][BR]
Geïndenteerde tekst.[BR]
2[:end-div][BR]
:end-code

Je moet weten welke CSS classes er beschikbaar zijn, anders kun je er niet veel mee. In bovenstaand voorbeeld is Q[indent] een CSS class. In HTML wordt er Q[<div class="indent">] toegevoegd. In CSS wordt dat bijvoorbeeld gerepresenteerd door Q[.indent { margin-left: 2em; }].

:head=Opmaak in de tekst.

Je kunt B[vetgedrukt] en I[schuingedrukt] eenvoudig toevoegen. Die kun je ook B[I[combineren]]. 

Voorbeeld:

:div=indent
:code
Je kunt B[vetgedrukt] en I[schuingedrukt] eenvoudig toevoegen. Die kun je ook B[I[combineren]]. 
:end-code
:end-div

:head=Plaatjes

Een plaatje toevoegen kan ook vrij eenvoudig:

:div=indent
:code
2[:div]=img-normal[BR]
2[:image]=plaatje.jpg,25%,onderschrift,right[BR]
2[:end-div][BR]
2[:div]=img-mobiel[BR]
2[:image]=plaatje.jpg,100%,onderschrift,right[BR]
2[:end-div][BR]
:end-code
:end-div

De bovenstaande code voegt twee keer hetzelfde plaatje in, alleen is de ene op een normale web-pagina te zien, terwijl de andere op de mobiele site te zien is. Dit wordt geregeld met de eerder genoemde CSS. Het verschil zit hem in de B[25%] en B[100%]. 

:head=Typische Materia Medica onderdelen

De volgende dingen:

:div=indent
:code
B[S{]Dit is een symptoom.B[}][BR]
B[S{]Dit is een symptoom, met een B[2[]2[tweewaardig deel]B[]}][BR]
B[S{]Dit is een symptoom, met een B[2[]3[driewaardig deel]B[]}][BR]
B[S{]Dit is een symptoom, met een B[2[]4[vierwaardig deel]B[]}][BR]
[BR]
B[C{]Dit is een oorzaak.B[}][BR]
[BR]
B[R{]Dit is een vergelijkend deel (relaties naar andere middelen), bijvoorbeeld naar B[R[]ArnB[]].B[}][BR]
[BR]
B[G{]B[S{]Dit is een symptoom in een groep.B[}]B[S{]Dit is nog een een symptoom in die groep.B[}]B[}]
[BR]
B[GP{]GP{B[S{]Dit is een symptoom in een Phatak groep.B[}]}B[}]

:end

