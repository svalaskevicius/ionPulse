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
    var _self, _text, _pos;

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
        return function(){
            re.lastIndex = _pos;
            var m = re.exec(_text);
            return m ? m.index + (atStart?0:m[0].length) : -1;
        };
    };

    var charFormatting = {
        "html" :                 _createCharFormat(Qt.lightGray,              QFont.Normal, false),
        "php" :                  _createCharFormat(Qt.white,                  QFont.Normal, false),
        "php/constructs" :       _createCharFormat(new QColor(255, 255, 127), QFont.Bold,   true),
        "php/number" :           _createCharFormat(Qt.red,                    QFont.Normal, false),
        "php/whitespace" :       _createCharFormat(new QColor(128, 100, 96),  QFont.Light,  false),
        "php/keyword" :          _createCharFormat(new QColor(191, 127, 255), QFont.Bold,   false),
        "php/compileConstants" : _createCharFormat(new QColor(255, 255, 127), QFont.Bold,   true),
        "php/variable" :         _createCharFormat(new QColor(164, 158, 64),  QFont.Normal, false),
        "php/property" :         _createCharFormat(new QColor(158, 140, 44),  QFont.Normal, false),
        "php/function" :         _createCharFormat(new QColor(164, 158, 96),  QFont.Normal, false),
        "php/separator" :        _createCharFormat(new QColor(191, 255, 127), QFont.Black,  false),
        "php/comment" :          _createCharFormat(new QColor(100, 100, 100), QFont.Normal, true),
        "php/string_dq" :        _createCharFormat(new QColor(127, 225, 127), QFont.Normal, false),
        "php/string_sq" :        _createCharFormat(new QColor(127, 191, 127), QFont.Normal, false),
    };
    var states = ["html", "php", "php/comment", "php/string_dq", "php/string_sq"];
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
             "php" : _regexTransition(/(\\\\)*([^\\]|^)"/g, false),
        },
        "php/string_sq" : {
             "php" : _regexTransition(/(\\\\)*([^\\]|^)'/g, false),
        }
    };
    var highlightRules = {
         "php" : {
             "constructs" : new RegExp("class\\s+[a-z0-9_]+((\\s+extends\\s+[a-z0-9_]+)?(\\s+implements+[a-z0-9_]+)?)*"
                                       +"|die|echo|empty|exit|eval|include|include_once|isset|list|require|require_once"
                                       +"|return|print|unset", "ig"),
             "number" :     /-?([0-9]+)?\.?[0-9]+/g,
             "whitespace" : /\s+/g,
             "keyword" :    new RegExp("<\\?php|\\?>|return|class|function|protected|private|public"
                                       +"|abstract|extends|interface|implements|abstract|and|array"
                                       +"|as|break|case|catch|clone|const|continue|declare"
                                       +"|default|do|else|elseif|enddeclare|endfor"
                                       +"|endforeach|endif|endswitch|endwhile|extends"
                                       +"|final|for|foreach|function|global|goto"
                                       +"|if|instanceof|namespace"
                                       +"|new|or|static"
                                       +"|switch|throw|try|use|var|while|xor", "g"),
             "compileConstants" : /__CLASS__|__DIR__|__FILE__|__LINE__|__FUNCTION__|__METHOD__|__NAMESPACE__/g,
             "variable" :   /\$[a-z_][a-z0-9_]*/ig,
             "property" :   /->[a-z_][a-z0-9_]*/ig,
             "function" :   /[a-z_][a-z0-9_]*\s*\(/ig,
             "separator" :  /->|;|\+|-|\*|\/|=|\(|\)|\||&/g,
             "comment" :  /\/\/.*/g,
         }
    };

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
         var idx = -1;
         for (var newState in transitions[state]) {
             idx = transitions[state][newState]();
             if (idx >= 0) {
                 _hightlightState(state, _pos, idx);
                 _pos = idx;
                 return newState;
             }
         }
         idx = _text.length;
         _hightlightState(state, _pos, idx);
         _pos = idx;
         return state;
    }

    return function(self, text){
        _self = self; _text = text; _pos = 0;
        var stateId = self.previousBlockState;
        if (stateId < 0) {
            stateId = 0;
        }
        var state = states[stateId];
        var end = text.length;
        while (_pos < end) {
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

registerJsFileHighlighter("text",textHighlighter);
