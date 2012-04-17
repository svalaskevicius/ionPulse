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



// other hacks - testing place

//editorPlugin.focusedEditor.focusOnLine(7)
editorPlugin.editorOpened.connect(
    this,
    function(editor) {
        editor.tabStopWidth = 30;
        editor.textOptionFlags |= 1;// | 2;// | 4;
    }
);


phpHighlighter =
(function() {
    var _self, _text, _stateMatcher;

    var _createCharFormat = function (color, weight, italic){
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
    };

    var _regexTransition = function(re, atStart) {
        if (/([^\[]|^)\^/.test(re.source)) {
            return function() {
                var textToMatch;
                if (0 === _stateMatcher.next) {
                    textToMatch = _text;
                } else {
                    textToMatch = _text.substring(_stateMatcher.next);
                }
                re.lastIndex = 0;
                var m = re.exec(textToMatch);
                if (m) {
                    return {
                        'start': _stateMatcher.next + m.index + (atStart?0:m[0].length),
                        'next': _stateMatcher.next + m.index + m[0].length,
                    };
                }
                return false;
            };
        } else {
            return function(){
                re.lastIndex = _stateMatcher.next;
                var m = re.exec(_text);
                if (m) {
                    return {
                        'start': m.index + (atStart?0:m[0].length),
                        'next': m.index + m[0].length,
                    };
                }
                return false;
            };
        }
    };

    var charFormatting = {
        "html" :                 _createCharFormat(Qt.lightGray,              QFont.Normal, false),
        "php" :                  _createCharFormat(new QColor(220, 220, 180), QFont.Normal, false),
        "php/constructs" :       _createCharFormat(new QColor(255, 255, 127), QFont.Bold,   true),
        "php/number" :           _createCharFormat(Qt.red,                    QFont.Normal, false),
        "php/whitespace" :       _createCharFormat(new QColor(128, 100, 96),  QFont.Light,  false),
        "php/keyword" :          _createCharFormat(new QColor(191, 127, 255), QFont.Bold,   false),
        "php/compileConstants" : _createCharFormat(new QColor(255, 255, 127), QFont.Bold,   true),
        "php/variable" :         _createCharFormat(new QColor(194, 158, 64),  QFont.Normal, false),
        "php/property" :         _createCharFormat(new QColor(198, 120, 44),  QFont.Normal, false),
        "php/function" :         _createCharFormat(new QColor(164, 158, 96),  QFont.Normal, false),
        "php/separator" :        _createCharFormat(new QColor(191, 255, 127), QFont.Black,  false),
        "php/comment" :          _createCharFormat(new QColor(100, 100, 100), QFont.Normal, true),
        "php/string_dq" :        _createCharFormat(new QColor(127, 225, 127), QFont.Normal, false),
        "php/string_sq" :        _createCharFormat(new QColor(127, 191, 127), QFont.Normal, false),
    };
    var transitions = {
        "html" : {
            "php" : _regexTransition(/<\?php\b/g, true)
        },
        "php" : {
            "html" : _regexTransition(/\?>/g, false),
            "php/comment" : _regexTransition(/\/\*/g, true),
            "php/string_dq" : _regexTransition(/"/g, true),
            "php/string_sq" : _regexTransition(/'/g, true),
        },
        "php/comment" : {
            "php" : _regexTransition(/\*\//g, false),
        },
        "php/string_dq" : {
             "php" : _regexTransition(/([^\\"]|\\.|^)"/g, false),
        },
        "php/string_sq" : {
             "php" : _regexTransition(/([^\\']|\\.|^)'/g, false),
        }
    };
    var highlightRules = {
         "php" : {
             "number" :     /-?([0-9]+)?\.?[0-9]+/g,
             "whitespace" : /\s+/g,
             "function" :   /[a-z_][a-z0-9_]*\s*\(/ig,
             "constructs" : new RegExp("class\\s+[a-z0-9_]+((\\s+extends\\s+[a-z0-9_]+)?(\\s+implements+[a-z0-9_]+)?)*"
                                       +"|\\b(die|echo|empty|exit|eval|include|include_once|isset|list|require|require_once"
                                       +"|return|print|unset)\\b", "ig"),
             "keyword" :    new RegExp("<\\?php|\\?>|\\b(class|function|protected|private|public"
                                       +"|abstract|extends|interface|implements|abstract|and|array"
                                       +"|as|break|case|catch|clone|const|continue|declare"
                                       +"|default|do|else|elseif|enddeclare|endfor"
                                       +"|endforeach|endif|endswitch|endwhile|extends"
                                       +"|final|for|foreach|function|global|goto"
                                       +"|if|instanceof|namespace"
                                       +"|new|or|static"
                                       +"|switch|throw|try|use|var|while|xor)\\b", "g"),
             "compileConstants" : /__CLASS__|__DIR__|__FILE__|__LINE__|__FUNCTION__|__METHOD__|__NAMESPACE__/g,
             "variable" :   /\$[a-z_][a-z0-9_]*/ig,
             "property" :   /->[a-z_][a-z0-9_]*/ig,
             "separator" :  /->|;|\+|-|\*|\/|=|\(|\)|\||&/g,
             "comment" :  /\/\/.*/g,
         }
    };

    var states = [];
    for (var state in transitions) {
        states.push(state);
    }

    var _hightlightState = function(state, from, to) {
         _self.setFormat(from, to-from, charFormatting[state]);
         for (var rule in highlightRules[state]) {
             var re = highlightRules[state][rule];
             re.lastIndex = from;
             var idx = -1;
             do {
                 var match = re.exec(_text);
                 if (match && match.index < to) {
                     idx = match.index;
                     _self.setFormat(idx, match[0].length, charFormatting[state+"/"+rule]);
                 } else {
                     idx = -1;
                 }
             } while (idx >= 0);
         }
    };

    var _processState = function(state) {
         var minPos = false;
         var minState = false;
         for (var newState in transitions[state]) {
             var statePos = transitions[state][newState]();
             if (statePos) {
                 if (!minPos || (minPos.start > statePos.start)) {
                     minPos = statePos;
                     minState = newState;
                 }
             }
         }
         if (minPos) {
             _hightlightState(state, _stateMatcher.start, minPos.start);
             _stateMatcher = minPos;
             return minState;
         } else {
             _hightlightState(state, _stateMatcher.start, _text.length);
             _stateMatcher = false;
             return state;
         }
    }

    return function(self, text){
         _self = self; _text = text; _stateMatcher = {'start':0,'next':0};
        var stateId = self.previousBlockState;
        if (stateId < 0) {
            stateId = 0;
        }
        var state = states[stateId];
        while (_stateMatcher) {
            state = _processState(state);
        }
        stateId = states.indexOf(state);
        if (self.currentBlockState != stateId) {
            self.currentBlockState = stateId;
        }
    };
})();

textHighlighter = function(self, text)
{
    phpHighlighter(self, text);
    /*
    var myClassFormat = new QTextCharFormat();
    myClassFormat.setFontWeight(QFont.Bold);
    var brush = new QBrush();
    brush.setColor(Qt.darkGray);
    brush.setStyle(Qt.SolidPattern);

    self.currentBlockState = self.previousBlockState + 1;
    myClassFormat.setForeground(brush);
    self.setFormat(0, 10, myClassFormat);*/
 }

registerFileHighlighter("text", textHighlighter);
registerFileHighlighter("text/php", textHighlighter);

