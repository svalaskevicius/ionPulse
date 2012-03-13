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
    text = "<html><pre>"+text.replace(/&/g, '&amp;').replace(/>/g, '&gt;').replace(/</g, '&lt;');
    QMessageBox.information(this, "ionPulse - alert", text);
}


function JsConsoleWidget(parent) {
    QWidget.call(this, parent);

    this.textEdit = new QTextEdit(this);
    this.textEdit.readOnly = true;
    this.htmlPrefix = "<html><head>        \
        <STYLE type=\"text/css\">          \
            .prefix {color: #aaee77;}      \
        </STYLE>                           \
    </head><body>";
    this.htmlSuffix = "</body></html>";
    this.htmlContent = "";

    this.lineInput = new QLineEdit(this);
    this.lineInput.editingFinished.connect(
        this,
        function() {
            eval(this.lineInput.text);
            this.lineInput.text = "";
        }
    );

    var layout = new QVBoxLayout();
    layout.addWidget(this.textEdit, 0, 0);
    layout.addWidget(this.lineInput, 0, 0);
    this.setLayout(layout);

    this.setWindowTitle("Javascript console");
    qs.system.installAppShortcut(Qt.Key_F12).callback.connect(
        this,
        function() {
            if (this.visible) {
                this.hide();
            } else {
                this.show();
                this.lineInput.setFocus();
            }
        }
    );
}


JsConsoleWidget.prototype = new QWidget();
JsConsoleWidget.prototype.log = function (text)
{
    this.htmlContent += "<div class='line'><span class='prefix'>&gt; </span>"+text+"</div>";
    this.textEdit.html = this.htmlPrefix + this.htmlContent + this.htmlSuffix;
}


console = new JsConsoleWidget(window);
console.hide();
layoutManager.add("central/central_footer", console);

console.log("ionPulse.js initialised");
