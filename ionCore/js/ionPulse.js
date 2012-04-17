/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

qs.script.include("_core.js");
qs.script.include("_console.js");

console = new JsConsoleWidget(window);
console.hide();
layoutManager.add("central/central_footer", console);

console.log("ionPulse.js initialised");



//editorPlugin.focusedEditor.focusOnLine(7)
editorPlugin.editorOpened.connect(
    this,
    function (editor) {
        editor.tabStopWidth = 30;
        editor.textOptionFlags |= 1; // | 2;// | 4;
    }
);


TextHighlighterFactory = function () {};
TextHighlighterFactory.prototype = {
    _cppApi: null,
    _text: null,
    _stateMatcher: null,

    createCharFormat: function (color, weight, italic) {
        var format = new QTextCharFormat();
        var brush = new QBrush();
        var font = new QFont("Monaco", 14, weight, italic);
        if (!font.exactMatch()) {
            font = new QFont("Courier New", 14, weight, italic);
        }
        brush.setColor(color);
        brush.setStyle(Qt.SolidPattern);
        format.setForeground(brush);
        format.setFont(font);
        return format;
    },

    regexTransition: function (re, atStart) {
        var self = this;
        if (/([^\[]|^)\^/.test(re.source)) {
            return function () {
                var textToMatch;
                if (0 === self._stateMatcher.next) {
                    textToMatch = self._text;
                } else {
                    textToMatch = self._text.substring(self._stateMatcher.next);
                }
                re.lastIndex = 0;
                var m = re.exec(textToMatch);
                if (m) {
                    return {
                        'start': self._stateMatcher.next + m.index + (atStart ? 0 : m[0].length),
                        'next': self._stateMatcher.next + m.index + m[0].length,
                    };
                }
                return false;
            };
        } else {
            return function () {
                re.lastIndex = self._stateMatcher.next;
                var m = re.exec(self._text);
                if (m) {
                    return {
                        'start': m.index + (atStart ? 0 : m[0].length),
                        'next': m.index + m[0].length,
                    };
                }
                return false;
            };
        }
    },

    charFormatting: {},
    transitions: {},
    highlightRules: {},
    states: [],

    _hightlightState: function (state, from, to) {
        this._cppApi.setFormat(from, to - from, this.charFormatting[state]);
        for (var rule in this.highlightRules[state]) {
            var re = this.highlightRules[state][rule];
            re.lastIndex = from;
            var idx = -1;
            do {
                var match = re.exec(this._text);
                if (match && match.index < to) {
                    idx = match.index;
                    this._cppApi.setFormat(idx, match[0].length, this.charFormatting[state + "/" + rule]);
                } else {
                    idx = -1;
                }
            } while (idx >= 0);
        }
    },

    _processState: function (state) {
        var minPos = false;
        var minState = false;
        for (var newState in this.transitions[state]) {
            var statePos = this.transitions[state][newState]();
            if (statePos) {
                if (!minPos || (minPos.start > statePos.start)) {
                    minPos = statePos;
                    minState = newState;
                }
            }
        }
        if (minPos) {
            this._hightlightState(state, this._stateMatcher.start, minPos.start);
            this._stateMatcher = minPos;
            return minState;
        } else {
            this._hightlightState(state, this._stateMatcher.start, this._text.length);
            this._stateMatcher = false;
            return state;
        }
    },

    initialize: function () {
        for (var state in this.transitions) {
            this.states.push(state);
        }
    },
    highlight: function (cppApi, text) {
        this._cppApi = cppApi;
        this._text = text;
        this._stateMatcher = {
            'start': 0,
            'next': 0
        };
        var stateId = cppApi.previousBlockState;
        if (stateId < 0) {
            stateId = 0;
        }
        var state = this.states[stateId];
        while (this._stateMatcher) {
            state = this._processState(state);
        }
        stateId = this.states.indexOf(state);
        if (cppApi.currentBlockState != stateId) {
            cppApi.currentBlockState = stateId;
        }
    }
};


phpHighlighter = (function () {
    var PhpHighlighter = (function () {}).inheritsFrom(TextHighlighterFactory);
    PhpHighlighter.prototype.initialize = function () {
        this.charFormatting = {
            "html":                 this.createCharFormat(Qt.lightGray, QFont.Normal, false),
            "php":                  this.createCharFormat(new QColor(220, 220, 180), QFont.Normal, false),
            "php/constructs":       this.createCharFormat(new QColor(255, 255, 127), QFont.Bold, true),
            "php/number":           this.createCharFormat(Qt.red, QFont.Normal, false),
            "php/whitespace":       this.createCharFormat(new QColor(128, 100, 96), QFont.Light, false),
            "php/keyword":          this.createCharFormat(new QColor(191, 127, 255), QFont.Bold, false),
            "php/compileConstants": this.createCharFormat(new QColor(255, 255, 127), QFont.Bold, true),
            "php/variable":         this.createCharFormat(new QColor(194, 158, 64), QFont.Normal, false),
            "php/property":         this.createCharFormat(new QColor(198, 120, 44), QFont.Normal, false),
            "php/function":         this.createCharFormat(new QColor(164, 158, 96), QFont.Normal, false),
            "php/separator":        this.createCharFormat(new QColor(191, 255, 127), QFont.Black, false),
            "php/comment":          this.createCharFormat(new QColor(100, 100, 100), QFont.Normal, true),
            "php/string_dq":        this.createCharFormat(new QColor(127, 225, 127), QFont.Normal, false),
            "php/string_sq":        this.createCharFormat(new QColor(127, 191, 127), QFont.Normal, false),
        };
        this.transitions = {
            "html": {
                "php": this.regexTransition(/<\?php\b/g, true)
            },
            "php": {
                "html": this.regexTransition(/\?>/g, false),
                "php/comment": this.regexTransition(/\/\*/g, true),
                "php/string_dq": this.regexTransition(/"/g, true),
                "php/string_sq": this.regexTransition(/'/g, true),
            },
            "php/comment": {
                "php": this.regexTransition(/\*\//g, false),
            },
            "php/string_dq": {
                "php": this.regexTransition(/([^\\"]|\\.|^)"/g, false),
            },
            "php/string_sq": {
                "php": this.regexTransition(/([^\\']|\\.|^)'/g, false),
            }
        };
        this.highlightRules = {
            "php": {
                "number":           /-?([0-9]+)?\.?[0-9]+/g,
                "whitespace":       /\s+/g,
                "function":         /[a-z_][a-z0-9_]*\s*\(/ig,
                "constructs":       new RegExp("class\\s+[a-z0-9_]+((\\s+extends\\s+[a-z0-9_]+)?(\\s+implements+[a-z0-9_]+)?)*"
                                               + "|\\b(die|echo|empty|exit|eval|include|include_once|isset|list|require|require_once"
                                               + "|return|print|unset)\\b",
                                               "ig"
                                    ),
                "keyword":          new RegExp("<\\?php|\\?>|\\b(class|function|protected|private|public"
                                               + "|abstract|extends|interface|implements|abstract|and|array"
                                               + "|as|break|case|catch|clone|const|continue|declare"
                                               + "|default|do|else|elseif|enddeclare|endfor"
                                               + "|endforeach|endif|endswitch|endwhile|extends"
                                               + "|final|for|foreach|function|global|goto"
                                               + "|if|instanceof|namespace|new|or|static"
                                               + "|switch|throw|try|use|var|while|xor)\\b",
                                               "g"
                                    ),
                "compileConstants": /__CLASS__|__DIR__|__FILE__|__LINE__|__FUNCTION__|__METHOD__|__NAMESPACE__/g,
                "variable":         /\$[a-z_][a-z0-9_]*/ig,
                "property":         /->[a-z_][a-z0-9_]*/ig,
                "separator":        /->|;|\+|-|\*|\/|=|\(|\)|\||&/g,
                "comment":          /\/\/.*/g,
            }
        };

        this.parent.initialize.call(this);
    }
    php = new PhpHighlighter();
    php.initialize();
    return function (cppApi, text) {
        php.highlight(cppApi, text);
    }
})();

registerFileHighlighter("text", phpHighlighter);
registerFileHighlighter("text/php", phpHighlighter);
