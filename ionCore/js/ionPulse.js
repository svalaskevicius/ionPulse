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



textHighlighter = function(self, text)
{
    var myClassFormat = new QTextCharFormat();
    myClassFormat.setFontWeight(QFont.Bold);
    var brush = new QBrush();
    brush.setColor(Qt.darkGray);
    brush.setStyle(Qt.SolidPattern);

    self.currentBlockState = self.previousBlockState + 1;
    myClassFormat.setForeground(brush);
    self.setFormat(0, 10, myClassFormat);
 }

registerJsFileHighlighter("text",textHighlighter);
