/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

qs.script.include("lib/underscore.js");
qs.script.include("lib/jsDump.js");
qs.script.include("_core.js");
qs.script.include("_console.js");

qs.script.include("_suggestions.js");
qs.script.include("_highlighter.js");
qs.script.include("_php.js");


console.log("ionPulse.js initialised");

_.each(
    layoutManager.getZoneWidgets( "left", {"name":"project_tree"} ),
    function(widget) {
        widget.filterInputField.show();
        widget.treeView.contextMenuPolicy = Qt.CustomContextMenu;
        widget.treeView.customContextMenuRequested.connect(function(point){
            try {
                var actions = [];
                var item = widget.treeView.treeItemAt(point);
                if (item) {
                    var action = new QAction("", "Refresh "+item.data(0).toString(), 0);
                    action.triggered.connect(function() {
                        console.log(item.getPath());
                        widget.treeView.updateProjectNode(widget.treeView.indexAt(point));
                    });
                    actions.push(action);
                }
                QMenu.exec(actions, widget.treeView.mapToGlobal(point));
            }catch(e) {
                console.error(e);
            }
        });
    }
);

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


var user_js_path = QDir.homePath() + "/.ionPulse/init.js";
if (QFile.exists(user_js_path)) {
    console.log("Using user init script at: " + user_js_path);
    qs.script.include(user_js_path);
} else {
    console.log("User init script not found, looked at: " + user_js_path);
}

textHighlighter.initialize();
registerFileHighlighter(
    "text",
    function (cppApi, text) {
        try {
            textHighlighter.highlight(cppApi, text);
        } catch (e) {
            debug(e);
        }
    }
);



