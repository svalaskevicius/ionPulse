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
        editor.tabStopWidth = 30;
        editor.textOptionFlags |= 1; // | 2;// | 4;
    }
);


registerFileHighlighter("text", phpHighlighter);
registerFileHighlighter("text/php", phpHighlighter);




//dbxml.getStorage().query('for $a in collection()/top_statement_list return dbxml:metadata("dbxml:name", $a)')
