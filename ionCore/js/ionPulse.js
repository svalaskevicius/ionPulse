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
        //editor.textOptionFlags |= 1 | 2;// | 4;
    }
);


phpHighlighter =
(function() {
    var _self, _text, _pos;
    var _createColoredCharFormat = function (color){
        var format = new QTextCharFormat();
        var brush = new QBrush();
        brush.setColor(color);
        brush.setStyle(Qt.SolidPattern);
        format.setForeground(brush);
        return format;
    };

    var _regexTransition = function(re) {
        return function(){
            re.lastIndex = _pos;
            var m = re.exec(_text);
            return m ? m.index : -1;
        };
    };

    var charFormatting = {
        "html" : _createColoredCharFormat(Qt.white),
        "php" : _createColoredCharFormat(Qt.darkGray),
        "php/number" : _createColoredCharFormat(Qt.red)
    };
    var states = ["html", "php"];
    var transitions = {
        "html" : {
            "php" : _regexTransition(/<\?php\b/g)
        },
        "php" : {
            "html" : _regexTransition(/\?>/g)
        }
    };
    var highlightRules = {
         "php" : {
             "number" : /-?([0-9]+)?\.?[0-9]+/g
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
