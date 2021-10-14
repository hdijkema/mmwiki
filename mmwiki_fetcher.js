
import { mmwiki_remedies } from './remedies.js';
import { mmwikiSave } from './mmwiki_mm.js';

export class MMWikiFetcher
{
	constructor(url, context, params, progress)
	{
		this.url = url;
		
		this.context = context;
		
		this.title = ('title' in params) ? params.title : '';
		this.author = ('author' in params) ? params.author : '';
		this.edition = ('edition' in params) ? params.edition : '';
		this.editor = ('editor' in params) ? params.editor : '';
		this.version = ('version' in params) ? params.version : '';
		this.id = ('id' in params) ? params.id : '';
		this.lang = ('lang' in params) ? params.lang : 'en';
		
		this.progr_el = progress;
		
		this.parser = function() {
			var self = {};
			
			self.parse = function(html) {};
			self.init = function(fetcher) {};
		};
		
		
	}
	
	setParser(parser) 
	{
		this.parser = parser;	
	}
	
	setRemReplacer(repl) {
		this.rem_replacer = repl;
	}
	
	fetchAndParse(state, url)
	{
		var self = this;
		var u = '/fetch.php?url=' + url;
		
		$.ajaxSetup({
    		'beforeSend' : function(xhr) {
        		xhr.overrideMimeType('text/html; charset=iso-8859-1');
    		},
		});

		$.get( u, 
			   function (html) {
				 var el = document.createElement('div');
				 $(el).addClass('mmwiki_fetcher');
				 $(el).html(html);
				 self.parser.parse(state, url, el);
			   }
		);
	}
	
	cleanup(mmwiki)
	{
		mmwiki = mmwiki.replaceAll(/(2\[([^\]])+]((\s|[.,])*))+/g, function(s) {
			s = s.replaceAll('2[', '');
			s = s.replaceAll(']', '');
			return '2[' + s + ']';
		});
		
		mmwiki = mmwiki.replaceAll(/(2\[[^\]]+$)/gm, function(s) {
			return s.substr(0, s.length - 1) + ']' + '\n';
		});
		
		mmwiki = mmwiki.replaceAll(/^]/gm, '');
		
		mmwiki = mmwiki.replaceAll(' ]', '] ');
		
		mmwiki = mmwiki.replaceAll(/\[([^\]]+)]([a-zA-Z]+)/g, '[$1$2]');
		
		mmwiki = mmwiki.replaceAll(/([a-zA-Z]+)([I2])\[([^\]]+)]/g, '$2[$1$3]');
		
		return mmwiki;
	}
	
	setPage(name, mmwiki, ok_f, err_f) {
		
		mmwiki = ':book=' + this.title  + '\n' + 
		         ':edition=' + this.edition + '\n' + 
		         ':author=' + this.author + '\n' +
		         ':editor=' + this.editor + '\n' +
		         ':version=' + this.version + '\n' +
		         ':id=' + this.id + '\n' + 
		         '\n' + 
		         ':begin[' + this.lang + ']\n' +
		         '\n' + 
		         mmwiki +
		         '\n' + 
		         ':end\n' + 
		         '\n';
		         
		mmwikiSave(this.context, name, mmwiki, ok_f, err_f);
	}
	
	parseRemedy(el) 
	{
		var maybe_rem = $(el).text().trim();
		maybe_rem = this.rem_replacer(maybe_rem);
		
		for (const [key, value] of Object.entries(mmwiki_remedies)) {
			if (key == maybe_rem) { return key; }
			else if (value.toLowerCase().startsWith(maybe_rem)) { return key; }
		}
		
		return undefined;
	}
	
	getRemedy(maybe_rem)
	{
		maybe_rem = maybe_rem.toLowerCase();
		
		var match_length = -1;
		var rkey = maybe_rem;
		
		for (const [key, value] of Object.entries(mmwiki_remedies)) {
			var k = key.toLowerCase();
			if (k == maybe_rem) { return key; }
			else if (k.startsWith(maybe_rem)) { 
				if (maybe_rem.length > match_length) { 
					match_length = maybe_rem.length;
					rkey = key;
				}
			} else if (maybe_rem.startsWith(k)) { 
				if (k.length > match_length) {
					match_length = k.length;
					rkey = key;
				}
			}
		}
		return rkey;
	}
	
	parseText(el) 
	{
		var html = $(el).html();
		html = html.replaceAll(/[&]nbsp[;]/g, ' ');
		html = html.replaceAll(/\s+/g, ' ');
		html = html.replaceAll('<i>', 'I[');
		html = html.replaceAll('</i>', ']');
		html = html.replaceAll('<br>', '[BR]');
		html = html.replaceAll('ď', 'di');
		html = html.replaceAll('Ś', 'eo');
		html = html.replaceAll('Benzoďc', 'Benzodic');
		$(el).html(html);
		var mmwiki = $(el).text();
		
		mmwiki = mmwiki.replaceAll(/\][.]/g, '.]');
		mmwiki = mmwiki.replaceAll(/,\s+\]/g, '], ');
		mmwiki = mmwiki.replaceAll(/\s+\]/g, '] ');
		
		mmwiki = mmwiki.replaceAll(/(\[[^\[\]]*?)$/gm, '$1]');

		return mmwiki;
	};
	
	mkNewUrl(url, page) 
	{
		if (url.endsWith('.htm')) {
			url = url.replace(/[/][^/]+$/, '/' + page);
		} else {
			url = url.replace(/[/]$/, '/' + page);
			if (!url.endsWith(page)) {
				url += page;
			}
		}
		
		return url;
	}


	run() 
	{
		this.parser.init(this);
		this.fetchAndParse('start', this.url);
	}
}

export function kentLecturesParser()
{
	var self = {};
	
	self.fetcher = null;

	self.findRemedy = function(remedy) 
	{
		
	}

	self.init = function(fetcher) {
		self.fetcher = fetcher;
		self.state = 'start';
		
		self.fetcher.setRemReplacer(function(rem) {
			return rem;
		});
	};

	self.parse = function(state, url, el) {
		if (state == 'start') {
			self.parseFront(url, el);
		} else if (state == 'preface') {
			self.parsePreface(url, el);
		} else {
			self.parseRemedy(state, url, el);
		}
	};
	
	self.parseFront = function(url, el) {
		var elements = $(el).find('a');
		var i, N;
		
		var mmwiki = '';
		
		for(i = 0, N = elements.length; i < N; i++) {
			var a_el = elements[i];
			var href = $(a_el).attr('href');
			
			if (href == 'preface.htm') {
				self.state = 'preface';
				var new_url = self.fetcher.mkNewUrl(url, href);
				self.fetcher.fetchAndParse('preface', new_url);
			} else if (href == 'conclusion.htm' || href == 'intro.htm'|| href == '../../index.htm' || href == '../../english/index.htm') {
				// do nothing
				console.log('noting');
			} else {
				var remedy = href;
				remedy = remedy.replace('.htm', '');
				remedy = remedy.toLowerCase();
				
				var rem = self.getRemedy(remedy);
				mmwiki += '* R[' + rem + ']\n';
				
				var new_url = self.fetcher.mkNewUrl(url, href);
				self.fetcher.fetchAndParse(remedy, new_url);
			}
		}
		
		self.fetcher.setPage('index', mmwiki, function() { }, function() { console.log('err'); });
	};
	
	self.parsePreface = function(url, dom_el)
	{
		var size = '';
		
		var textOut = function(el) {
			var txt = self.fetcher.parseText(el).trim();
			var lc = txt.toLowerCase();
			if (lc != 'main' && lc != 'index') {
				if (size == '4') {
					mmwiki += ':head=' + txt + '\n';
				} else if (size == '5') {
					mmwiki += ':head2=' + txt + '\n';
				} else {
					mmwiki += txt + '\n\n';
				}
			}
		};

		var mmwiki = '';
		var elements = $(dom_el).find('p');
		var i, N;
		for(i = 0, N = elements.length; i < N; i++) {
			var el = elements[i];
			
			var parent_el = $(el).parent();
			size = $(parent_el).attr('size');
			textOut(el);
		}
		
		self.fetcher.setPage('preface', mmwiki, 
						function() {  }, 
						function() { console.log('err'); }
						);
	}
	
	self.repl = [
		'bryonia:Bry', 'lycopodium:Lyc', 'kalmia:Kalm', 'ledum:Led', 'rhus tox.:Rhus-t', 'pulsatilla:Puls', 'carbo v.:Carb-v',
		'natrum mur.:Nat-m', 'psorinum:Psor', 'aurum:aur', 'Abrot:Abrot', 'Phosphorus:Phos', 
		'coffea:Coff', 'nux vomica:Nux-v', 'ignatia:Ign', 'chamomilla:Cham', 'nux:nux-v', 'arnica:Arn', 'Cham:Cham', 
		'kreosote:Kreos', 'arsenic:Ars', 'Rhus:Rhus-t', 'gelsemium:Gels', 'aconite:Acon', 'Apis:Apis', 'Staph:Staph', 
		'arum triphyllum:Arum-t', 'nux moshata:Nux-m', 'Puls:Puls', 'Lyc:Lyc', 'rhus tox:Rhus-t', 'calcarea:Calc', 
		'petroleum:Petr', 'chelidonium:Chel', 'Bellis:Bell-p', 'chloroform:Chlf', 'arsenicum:Ars', 'sulphur:Sulph',
		'tuberculinum:Tub', 'agaricus:Agar', 'fluoric acid:Fl-ac', 'Sep:Sep', 'Sepia:Sep', 'Arg. n:Arg-n', 
		'mercurius:Merc', 'Merc:Merc', 'Sul:Sulph', 'Aloe:Aloe', 'zincum:Zinc', 'lachesis:Lach', 
		'cicuta:Cic', 'natrum muriaticum:Nat-m', 'calcarea carb:Calc', 'anacardium:Anac', 'hyos:Hyos', 
		'Argentum:Arg-n', 'Stram:Stram', 'stramonium:Stram', 'Bell:Bell', 'belladonna:Bell', 'Anac:Anac', 
		'eupator:Eup-per', 'Arn:Arn', 'Bry:Bry', 'Ars:Ars', 'arum triph:Arum-t', 'Phos:Phos', 'baptisia:Bapt', 
		'calc. carb:Calc', 'baryta carb:Bar-c', 'natrum mur:Nat-m', 'borax:Bor', 'graph:Graph', 'Calc.:Calc',
		'carbo veg:Carb-v', 'carbo an:Carb-an', 'natr. mur:Nat-m', 'Bar. m:Bar-m', 'Tub:Tub', 'Hepar:Hep', 
		'Kali c:Kali-c', 'Merc. cor:Merc-c', 'Merc cor:Merc-c', 'iodine:Iod', 
		'Agnus:Agn', 'Silica:Sil', 'Sanic:Sanic', 'Thuja:Thuj', 'Ammonium carb:Am-c', 'Amm. carb:Am-c',
		'Stannum:Stann', 'Ant. tart:Ant-t', "Lach:Lach", 'Sulph:Sulph', 'Calc:Calc', 'Nat. mur:Nat-m',
		"senega:Seneg", 'ammoniacum:Ammc', 'baryta muriatica:Bar-m', 'aethusa:Aeth', 'Natrum carb:Nat-c',
		'cuprum:Cupr', 'phosph:Phos', 'carbolic acid:Carb-ac', 'cantharis:Canth', 'Ergot:Ergot', 
		'aesculus:Aesc', 'kali carb:Kali-c', 'camphora:Camph', 'ferrum:Ferr', 'opium:Opium',
		'spongia:Spong', 'kali bi:Kali-bi', 'podophyllum:Pod', 'natrum sulph:Nat-s', 'graphites:Graph',
		'alumina:Alum', 'alumen:Alumn', 'calc iod:Calc-i', 'cactus:Cact', 'plumbum:Plb', 
		'antimonium tartaricum:Ant-t', 'Sang:Sang', 'Psor:Psor', 'magn. mur:Mag-m', 'bar. carb:Bar-c',
		'bar. c:Bar-c', 'china:Chin', 'natr. mur:Nat-m', 'nux mosch:Nux-m', 'calc. sul:Calc-s', 
		'Berb:Berb', 'Caust:Caust', 'calc phos:Calc-p', 'Agar:Agar', 'Rhod:Rhod', 'nat. sul:Nat-s',
		'ant. cr:Ant-cr', 'Med:Med', 'Ruta:Ruta', 'Sil:Sil', 'Con:Con', 'Natr. mur:Nat-m', 'Nux vom:Nux-v',
		'Sulf:Sulph', 'Calc. c:Calc', 'calc. phos:Calc-p', 'Natr.] mur.:Nat-m]', 'Zinc:Zinc', 'Euphr:Euphr',
		'cimicifuga:Cimic', 'hyosc:Hyos', 'camphor:Camph', 'veratr:Verat', 'canth:Canth', 'veratrum:Verat',
		'secale:Sec', 'Podo:Podo', 'senecio:Senec', 'hamamelis:Ham', 'causticum:Caust', 
		'kali bichromicum:Kali-bi', 'pyrogen:pyrog', 'mercurius corrosivus:Merc-c', 'rumex:Rumx',
		'silicea:Sil', 'natr. sulph:Nat-s', 'Cina:Cina', 'Colch:Colch', 'oleander:Olnd', 'digitalis:Dig',
		'murex:Murx', 'ipecac:Ip', 'phos. ac:Ph-ac', 'squilla:Squil', 'Squil:Squil', 'Alum:Alum', 
		'can. ind:Cann-i', 'Ign:Ign', 'Gels:Gels', 'tart. emet:Ant-t', 'eupatorium:Eup-per',
		'tarent:Tarent', 'anthr:Anthr', 'tarent. cubensis:Tarent-c', 'Bapt:Bapt', 'Ant. t:Ant-t', 
		'olean:Olnd', 'Thuya:Thuj', 'nat-mur:Nat-m', 'curare:Cur', 'vespa:Vesp', 'Aeth:Aeth', 
		'mur. ac":Mur-ac', 'Natrum 2[sulph:Nat-s', 'acetic acid:Acet-ac', 'Ip:Ip', 'natrum mur.:Nat-m',
		'carbo v:Carb-v', '2[Rhus] 2[tox]:Rhus-t', 'dulcamara:Dulc', 'abrotanum:Abrot',
		'caulophyllum:Caul', 'Actea:Cimic', 'Lit. t:Lil-t', 'allium cepa:All-c', 'Allium:All-c',
		'euphrasia:Euphr', 'Cepa:All-c', 'sulph. ac:Sul-ac', 'calcarea iod:Calc-i', 'aluminum:Alum',
		'nux moschata:Nux-m', 'aluminum metallicum:Alum-met', 'conium:Con', 'sulfur:Sulph', 
		'nitric acid:Nit-ac', 'arg. met:Arg', 'ambra grisea:Ambr', ' Am. c:Am-c', 'antimonium tart:Ant-t', 
		'Ant. crud:Ant-c', 'kali sulph:Kali-s', 'antimonium crudum:Ant-c', 'antimonium crud:Ant-c', 'ant. tart:Ant-t',
		'carbolic acid:Carb-ac', 'apocynum:Apoc', 'tartar emetic:Ant-t', 'argentum met:Arg', 'argentum nitricum:Arg-n',
		'badiaga:Bad', 'kali n:Kali-n', 'lil. t:Lil-t', 'platina:Plat', 'hypericum:Hyper', 'staphysagria:Staph',
		'ipeca:Ip', 'CalI[carea]:Calc', 'asa foetida:Asaf', 'ambra:Ambr', 'mercury:Merc', 'kalium sulphuricum:Kali-s',
		'baryta carbonica:Bar-c', 'Silic:Sil', 'colocynth:Coloc', 'dioscorea:dios', 'sabina:sabin', 'Arum:Arum-t', 
		'hippuricum acid:Hip-ac', 'benzoic acid:Benz-ac', '2[Hippuricum] 2[acid]:Hip-ac', '2[Hippuricum] acid:Hip-ac',
		'2[Benzo�c] acid:Benz-ac', 'berberis:Berb', 'natr. carb:Nat-c', 'bromium:Brom', 'bufo:Bufo', 'cadmium sulph:Cadm-s',
		'ipec:Ip', 'asarum:Asar', 'caladium:Calad', 'kali ars:Kali-ar', 'kali phos:Kali-p', 
		'chemical union of lime and fluoric acid:-chemical union of lime and fluoric acid-',
		'ph. ac:Ph-ac', 'nat. m:Nat-m', 'mag. p:Mag-p', 'baryta c:Bar-c', 'origanum:Orig', 'gratiola:Grat',
		'calc. p:Calc-p', '2[Benzoïc] acid:Benz-ac', 'Camph:Camph', 'verat:Verat', 'am. carb:Am-c',
		'ant. c:Ant-c', 'cannabis sativa:Cann-s', 'cannabis indica:Cann-i', 'capsicum:Caps',
		'kali bich:Kali-bi', 'Acon:Acon', 'Hep:Hep', 'Caps:Caps', 'carbo animalis:Carb-an', 'aur. m. n:Aur-m-n',
		'kali iodatum:Kali-i', 'chel:Chel', 'aesc:Aesc', 'cocculus:Cocc', 'kali carbonicum:Kali-c',
		'sarsaparilla:Sars', 'carbo vegetabilis:Carb-v', 'Led:Led', 'Hyper:Hyper', 'Cupr:Cupr', 'Natr. m:Nat-m',
		'nux mos:Nux-m', 'strych:Stry', 'Op:Op', 'Plat:Plat', 'Podoph:Podo', 'cistus:Cist', 'clematis:Clem',
		'croton tig:Croto-t', 'ranunculus:Ran-b', 'colchicum:Colch', 'coccus cacti:Coc-c', 
		'platinum:Plat', 'magnesia phosphorica:Mag-p', 'crotalus:crot-h', 'crotalus horridus:Crot-h',
		'naja:Naja', 'elaps:Elaps', 'anthracinum:Anthr', 'crot. tig:Croto-t', 'anagallis:Anag', 'Nit. ac:Nit-ac', 
		'Sab.:Sabin', 'Mang:Mang', 'ferrum metallicum:Ferr', 'Sars:Sars', 'coloc:Coloc', 'fluor. ac:Fl-ac',
		'picric acid:Pic-ac', 'sil.:Sil', 'glonoine:Glon', 'Merc. c:Merc-c', 'lac c:Lac-c',
		'hellebore:Hell', 'helleborus:Hell', 'asa f:Asaf', 'nit. acid:Nit-ac', 'hydrastis:Hydr',
		'ferr:Ferr', 'hyoscyamus:Hyos', 'hydrophobinum:Lyss', 'strontium carb:Stront-c', 'Moschus:Mosch',
		'Hell:Hell', 'iodide of potassium:Kali-i', 'ant. tart:Ant-t', 'ipecacuanha:Ip', 'Kali. bichromicum:Kali-bi',
		'kali i:Kali-i', 'mur. ac:Mur-ac', 'nit ac:Nit-ac', 'tab:Tab', 'crot. h:Crot-h','kali iod:Kali-i',
		'Iodide of potash:Kali-i', 'asar europaeum:Asar', 'lac can:Lac-c', 'kali bich:Kali-bi', 'Mag. carb:Mag-c',
		'Dig:Dig', 'calendula:Calen', 'lilium tigrinum:Lil-t', 'lilium tig:Lil-t', 'lycopod:Lyc', 'magnesia carb:Mag-c',
		'tuberc:Tub', 'magnesia mur:Mag-m', 'bromine:Brom', 'ptelea:Ptel', 'magnesia phos:Mag-p', 'manganum:Mang',
		'dulc:Dulc', 'syphilinum:Syph', 'Nat. s:Nat-s', 'iodoform:Iodof', 'corrosive mercury:Merc-c', 'Merc. viv.:Merc-v',
		'Merc. sol.:Merc-sol', 'Merc. sulph:Merc-s', 'Mez:Mez', 'Cal.:Calc', 'mill:Mill', 'millefolium:Mill', 'mosch:Mosch',
		'Plb:Plb', 'muriatic acid:Mur-ac', 'phos. acid:Ph-ac', 'Ignat:Ign', 'Nitric ac:Nit-ac', 'nitricum acidum:Nit-ac',
		'selen:Sel', 'Nat. c:Nat-c', 'mag. m:Mag-m', 'card. m:Card-m', 'ptel:Ptel', 'natrum sulphuricum:Nat-s',
		'petr:Petr', 'inula:Inul', 'abies nig:Abies-n', 'Kreos:Kreos', 'phosphoric acid:Ph-ac', 'Nux v:Nux-v', 
		'selenium:Sel', 'phytolacca:Phyt', 'phyto:Phyt', 'merc. cyanide:Merc-cy', 'onosmodium:Onos',
		'fl. ac:Fl-ac', 'calcarea phos:Calc-p', 'tarantula cubensis:Tarent-c', 'arg. nit:Arg-n', 'gnaphalium:Gnaph',
		'mezereum:Mez', 'sabadilla:Sabad', 'wyethia:Wye', 'sinapis nigra:Sin-n','sanguinaria:Sang',
		'senecio gracilis:Senec', 'hamam:Ham', 'ergot:Ergot', 'spigelia:Spig', 'samb:Samb', 'Kali s:Kali-s',
		'lauroc:Laur', 'grindelia robusta:Grin', 'dros:Dros', 'CoIoc.:Coloc', 'coff:Coff', 'petrol:Petr',
		'tarent. hisp:Tarent', 'hydroph:Lyss', 'sulphuric acid:Sul-ac', 'Kali hydr:Kali-hox', 'Petros:Petr', 
		'Cann. sat:Cann-s', 'medorrhinum:Med', 'bacillinum:Bac', 'tuberculinum bovinum:Tub', 'Ox. ac:Ox-ac',
		
		];
		
	self.rem_repl = [ 'plumb:plb', 'act-r:cimic', 'alm:alum', 'alum:alumn', 'antim-crud:ant-c', 'antim-tart:ant-t',
		'cad-s:cadm-s', 'can-ind:cann-i', 'can-sat:cann-s', 'carbo-sul:carbn-s', 'china:chin', 'cinnab:cinnb',
		'croton-tig:croto-t', 'eupat-perf:eup-per', 'fer-phos:ferr-p', 'fluor-ac:fl-ac', 'hepar:hep',
		'kali-ca:kali-c', 'kali-sul:kali-s', 'lac-can:lac-c', 'merc-iod:merc-i', 'merc-iod-r:merc-i-r', 
		'merc-sul:merc-s', 'nat-sul:nat-s', 'oxal-ac:ox-ac', 'pho-ac:ph-ac', 'pho:phos', 'pyro:pyrog',
		'rh-t:rhus-t', 'rx-c:rumx', 'saba:sabad', 'stan:stann', 'sul:sulph'
	];

	self.getRemedy = function(_remedy) {
		
		var i, N;
		for(i = 0, N = self.rem_repl.length; i < N; i++) {
			var kv = self.rem_repl[i].split(':');
			if (_remedy == kv[0]) { _remedy = kv[1]; i = N; }
		}
		
		var remedy = self.fetcher.getRemedy(_remedy);
		
		var rlc = remedy.toLowerCase();
		if (rlc == 'amm-carb') { remedy = "Am-c"; }
		else if (rlc == "amm-mur") { remedy = "Am-m"; }
		else if (rlc == "ver-a") { remedy = "Verat"; }

		console.log('Parsing: ' + _remedy + ' -> ' + remedy);
		
		return remedy;
	};

	self.repl.sort(function(a, b) {
		var kv_a = a.split(':');
		var kv_b = b.split(':');
		var kv_a_l = kv_a[0].length;
		var kv_b_l = kv_b[0].length;
		if (kv_a_l < kv_b_l) { return 1; }
		else if (kv_a_l > kv_b_l) { return -1; }
		else { return 0; }
	});
	
	self.replaceRemedies = function(txt) {
		var prot = {};
		var prot_i = 0;
		
		var protect = function() {
			var idx = txt.indexOf('R[');
			while(idx >= 0) {
				var idx1 = txt.indexOf(']', idx);
				var id = 'id_' + prot_i;
				prot_i += 1;
				prot[id] = txt.substring(idx, idx1 + 1);
				txt = txt.substr(0, idx) + '@@' + id + '@@' + txt.substr(idx1 + 1); 
				idx = txt.indexOf('R[');
			}
		};

		var r = function(regexp, rem) {
		   txt = ' ' + txt + ' ';
		   txt = txt.replace(regexp, '$1R[' + rem + ']$2');	
		   txt = txt.substr(1);
		   txt = txt.substr(0, txt.length - 1);
		   protect();
		};

		var i, N;
		for(i =0, N = self.repl.length; i < N; i++) {
			var kv = self.repl[i].split(':');
			
			var key = kv[0];
			var mod = '';
			if (key.toLowerCase() == key) { mod = 'i'; }
			
			key = key.replaceAll('[', '\\[');
			key = key.replaceAll('.', '[.]');
			key = key.replaceAll(' ', '[ ]');
			key = "([^A-Za-z])" + key + "([^A-Za-z])";
			
			var key_re = new RegExp(key, mod + "g");

			r(key_re, kv[1]);
		}
		
		for (const [key, value] of Object.entries(prot)) {
			txt = txt.replace('@@' + key + '@@', value);
		}
		
		for(i = 0, N = self.repl.length; i < N; i++) {
			var kv = self.repl[i].split(':');
			var key = kv[0];
			var value = kv[1];
			
			if (value) {
				if (value.startsWith('-')) {
					txt = txt.replaceAll('R[' + value + ']', key);
				}
			}
		}
		
		return txt;
	};

	self.parseRemedy = function(_remedy, url, dom_el) {
		
		var remedy = self.getRemedy(_remedy); 

		var textOut = function(el) {
			var txt = self.fetcher.parseText(el);
			if (txt.startsWith(':')) { txt = txt.substr(1); }
			if (txt.toLowerCase().trim() == 'main') {
				return "";
			} else {
				return txt;
			}
		};
		
		var mmwiki = '';
		var elements = $(dom_el).find('p');
		var i, N;
		for(i = 0, N = elements.length; i < N; i++) {
			var el = elements[i];
			
			var html = $(el).html();
			html = html.replaceAll(/[<]i[>]/g, 'I[');
			html = html.replaceAll(/[<][/]i[>]/g, ']');
			$(el).html(html);

			var center = $(el).attr("align");
			if (!center) { center = ''; }
			
			var par = '';
			var start_of_line = true;
			
			var add = function(t) {
				if (start_of_line) {
					t = t.replace(/^\s+/, '');
				}
				par += t;
				start_of_line = false;
			};
			
			if (center.toLowerCase() != 'center') {
				var par_els = $(el).find('font');
	
				if (par_els.length > 0) {
					var j, M;
					for(j = 0, M = par_els.length; j < M; j++) {
						var f_el = par_els[j];
						var size = $(f_el).attr('size');
						var col = $(f_el).attr('color');
						if (size) {
							if (size == 2 && col == '#ff0000') {
								add('2[' + textOut(f_el) + ']');
							} else {
								add(textOut(f_el));
							}
						} else if (col == '#ff0000') { // rubric
							par += '\n\n:rubric=' + textOut(f_el).trim() + '\n\n';
							start_of_line = true;
						} else {
							var txt = textOut(f_el);
							/*if (txt.indexOf(']') >=0) {
								txt = txt.replace(']', '');
								txt = ']B[' + txt + ']';
							} else {
								txt = 'B[' + txt + ']';
							}*/
							add(txt);
						}
					}
				} else {
					add(textOut(el));
				}
				
				mmwiki += par + '\n\n';
			}
		}
		
		mmwiki = self.fetcher.cleanup(mmwiki);
		
		mmwiki = mmwiki.replaceAll(/([A-Z][A-Z][A-Z]+)/g, '3[$1]');
		
		mmwiki = self.replaceRemedies(mmwiki);

		self.fetcher.setPage(remedy.toLowerCase(), mmwiki, function() { }, function() { console.log('err'); });
	};
	
	return self;
}