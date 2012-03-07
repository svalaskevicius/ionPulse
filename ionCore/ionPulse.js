/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

Array.prototype.each = function(callback) {
    for (var i=0; i<this.length;i++) {
        callback(this[i]);
    }
};

["qt.core", "qt.gui", "qt.xml", "qt.svg", "qt.network",
 "qt.sql", "qt.opengl", "qt.webkit", "qt.xmlpatterns",
 "qt.uitools"].each(qs.script.importExtension);

//debugger
function alert(text)
{
    QMessageBox.information(this, "ionPulse - alert", text);
}
//layoutManager.add("central/central_footer", new QLabel("asd"))



function JsConsoleWidget(parent) {
    QWidget.call(this, parent);

    this.textEdit = new QTextEdit(this);

    var layout = new QVBoxLayout();
    layout.addWidget(this.textEdit, 0, 0);
    this.setLayout(layout);

    this.setWindowTitle("Javascript console");
    qs.system.installAppShortcut(Qt.Key_F12).callback.connect(
        this,
        function() {
            if (this.visible) {
                this.hide();
            } else {
                this.show();
            }
        }
    );
}


JsConsoleWidget.prototype = new QWidget();
console = new JsConsoleWidget(window);
console.hide();
layoutManager.add("central/central_footer", console);
