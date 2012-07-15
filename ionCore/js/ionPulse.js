/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

qs.script.include("lib/jsDump.js");
qs.script.include("_core.js");
qs.script.include("_console.js");

console = new JsConsoleWidget(window);
console.hide();
layoutManager.add("central_container/footer", console);

qs.script.include("_suggestions.js");
qs.script.include("_highlighter.js");
qs.script.include("_php.js");


console.log("ionPulse.js initialised");
console.log(layoutManager.getSubZoneNames("left"));
layoutManager.getZoneWidgets(
    "left",
    {"name":"project_tree"}
).each(function(widget) {
    widget.filterInputField.show();
    widget.filterInputField.text = "Test";
    widget.treeView.contextMenuPolicy = Qt.CustomContextMenu;
    widget.treeView.customContextMenuRequested.connect(function(point){
        console.log(point);
        try {
            var actions = [];
            actions.push(new QAction("", "test", 0));
            actions.push(new QAction("", "test1", 0));
            var index = widget.treeView.indexAt(point);
            if (index.isValid()) {
                actions.push(new QAction("", index.data().toString(), 0));
            }
            QMenu.exec(actions, widget.treeView.mapToGlobal(point));
        }catch(e) {
            console.error(e);
        }
    });
});

//editorPlugin.focusedEditor.focusOnLine(7)
editorPlugin.editorOpened.connect(
    this,
    function (editor) {
        try {
            editor.tabStopWidth = 30;
            editor.textOptionFlags |= 1; // | 2;// | 4;
            var suggestions = new Suggestions(editor);
            var shortcut_fixText = qs.system.installAppShortcut(Qt.Key_F, Qt.ControlModifier | Qt.ShiftModifier, editor);
            shortcut_fixText.setParent(editor);
            shortcut_fixText.callback.connect(editor, function(){
                this.selectAll();
                this.insertPlainText(
                    this.document().toPlainText()
                         .replace(/^[ \t]+/gm, function($0){ return $0.replace(/\t/g,"    "); })
                         .replace(/[ \t]+$/gm, "")
                         .replace("\r\n", "\n")
                );
            });

        } catch(e) {
            console.error(e);
        }
    }
);


registerFileHighlighter("text", phpHighlighter);
registerFileHighlighter("text/php", phpHighlighter);




//a = dbxml.getStorage().query('for $a in collection()/top_statement_list return dbxml:metadata("dbxml:name", $a)')
//b = dbxml.getStorage().query('for $a in doc("dbxml:/files/%47Volumes%47Disk%20Image%47Disclosure_Varnish%47tests%47features%47bootstrap%47FeatureContext.php")/top_statement_list return dbxml:metadata("dbxml:name", $a)')
//    dbxml.getStorage().getTimeStamp("/Volumes/Disk Image/Disclosure_Varnish/tests/features/bootstrap/FeatureContext.php")
