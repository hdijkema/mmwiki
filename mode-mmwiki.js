/* ***** BEGIN LICENSE BLOCK *****
 * Distributed under the BSD license:
 *
 * Copyright (c) 2010, Ajax.org B.V.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Ajax.org B.V. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL AJAX.ORG B.V. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * ***** END LICENSE BLOCK ***** */

ace.define("ace/mode/mmwiki_highlight_rules",["require","exports","module","ace/lib/oop","ace/mode/text_highlight_rules"],
function(require, exports, module) {
"use strict";

var oop = require("../lib/oop");
var TextHighlightRules = require("./text_highlight_rules").TextHighlightRules;

var MMWikiHighlightRules = function() {

    // regexp must not have capturing parentheses. Use (?:) instead.
    // regexps are ordered -> the first match is used
    this.$rules = {
        "start" : [
			{ 
				token : "keyword",
				regex : /^\s*:begin\[[^\]]+\]\s*$/,
				next  : "content"
			}, 
            {
                token : "keyword", 
                regex : /^\s*:[a-zA-Z0-9-]+\s*[=]/,
                next  : "value"
            },
			{
				token : "keyword",
                regex : /^\s*:[a-zA-Z0-9-]+\s*$/
			}
		],
		"content": [
			{
				token : "keyword",
                regex : /^\s*:[a-zA-Z0-9-]+\s*[=]/,
                next  : "value1"
			},
			{
				token : "keyword",
                regex : /^\s*:[a-zA-Z0-9-]+\s*/
			},
			{
				token : "support.constant.color",	
                regex : /[R]\[[^\]]+\]/	
			},
			{
                token : "variable", 
                regex : /[SCGZR]([A-Z]*)[{]/,
            }, 
			{   
				token : "variable",
				regex : /[}]/
			},
			{
				token : "markup.heading",
				regex : /[234PQHNMITBRCLt^!+-]\[|R[234]\[/
			}, 
			{
				token: "markup.heading",
				regex : /\]/
			},
        ],
        "value" : [
            {
                token : "string",
                regex : /.*$/,
                next  : "start"
            }
        ],
		"value1" : [
            {
                token : "string",
                regex : /.*$/,
                next  : "content"
            }
		]
    };
    
};

oop.inherits(MMWikiHighlightRules, TextHighlightRules);

exports.MMWikiHighlightRules = MMWikiHighlightRules;
});


ace.define("ace/mode/mmwiki",["require","exports","module","ace/lib/oop","ace/mode/text","ace/mode/mmwiki_highlight_rules","ace/mode/matching_brace_outdent","ace/mode/behaviour/cstyle","ace/mode/folding/cstyle"], 
function(require, exports, module) {
	
"use strict";

var oop = require("../lib/oop");
var TextMode = require("./text").Mode;
var HighlightRules = require("./mmwiki_highlight_rules").MMWikiHighlightRules;
//var MatchingBraceOutdent = require("./matching_brace_outdent").MatchingBraceOutdent;
//var CstyleBehaviour = require("./behaviour/cstyle").CstyleBehaviour;
//var CStyleFoldMode = require("./folding/cstyle").FoldMode;

var Mode = function() {
    this.HighlightRules = HighlightRules;
    //this.$outdent = new MatchingBraceOutdent();
    //this.$behaviour = new CstyleBehaviour();
    //this.foldingRules = new CStyleFoldMode();
};
oop.inherits(Mode, TextMode);

(function() {
    //this.lineCommentStart = "//";
    //this.blockComment = {start: "/*", end: "*/"};

    //this.checkOutdent = function(state, line, input) {
    //    return this.$outdent.checkOutdent(line, input);
    //};

    //this.autoOutdent = function(state, doc, row) {
    //    this.$outdent.autoOutdent(doc, row);
    //};

    this.$id = "ace/mode/mmwiki";
}).call(Mode.prototype);

exports.Mode = Mode;
});                (function() {
                    ace.require(["ace/mode/mmwiki"], function(m) {
                        if (typeof module == "object" && typeof exports == "object" && module) {
                            module.exports = m;
                        }
                    });
                })();
				
				