/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

qs.script.include("_core.js");
qs.script.include("_console.js");
qs.script.include("_highlighter.js");
qs.script.include("_php.js");

console = new JsConsoleWidget(window);
console.hide();
layoutManager.add("central/central_footer", console);

console.log("ionPulse.js initialised");



//editorPlugin.focusedEditor.focusOnLine(7)
editorPlugin.editorOpened.connect(
    this,
    function (editor) {
        if (/\.slides$/.test(editor.path)) {
            editor.setStyleSheet("background-color: #fab976");
            editor.tabStopWidth = 90;
        } else {
            editor.textOptionFlags |= 1; // | 2;// | 4;
            editor.tabStopWidth = 30;
        }
    }
);


registerFileHighlighter("text", phpHighlighter);
registerFileHighlighter("text/php", phpHighlighter);




slideHighlighter = (function () {
    var _Highlighter = (function () {}).inheritsFrom(TextHighlighterFactory);
    _Highlighter.prototype.initialize = function () {
        this.charFormatting = {
            "text":                 this.createCharFormat(new QColor(100, 80, 60), QFont.Normal, false, 64),
            "header":               this.createCharFormat(new QColor(120, 60, 40), QFont.Normal, true, 96),
            "text/black":           this.createCharFormat(new QColor(0, 0, 0), QFont.Normal, true, 64),
            "text/emphasis":        this.createCharFormat(new QColor(100, 80, 60), QFont.Normal, true, 64),
            "space":                this.createCharFormat(new QColor(190, 150, 100), QFont.Normal, true, 500),
        };
        this.transitions = {
            "text": {
                "header": this.regexTransition(/^\./g, true),
                "space": this.regexTransition(/^___$/g, true),
            },
            "header": {
                "text": this.regexTransition(/$/g, false),
            },
            "space": {
                "text": this.regexTransition(/$/g, false),
            },
        };
        this.highlightRules = {
            "text" : {
                "black": /\*[^*]+\*/g,
                "emphasis": /\/[^\/]+\//g,
            }
        };
        this.parent.initialize.call(this);
    }
    var _highlighter = new _Highlighter();
    _highlighter.initialize();
    return function (cppApi, text) {
        _highlighter.highlight(cppApi, text);
    }
})();

registerFileType("slides", "text/slide");
registerFileHighlighter("text/slide", slideHighlighter);
