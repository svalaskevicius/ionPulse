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

function is_string(input)
{
    return typeof(input)=='string';
}

function to_string(input)
{
    if (is_string(input)) {
        return input;
    }
    if (null === input) {
        return "null";
    }
    return input.toString();
}

function escape(text)
{
    return to_string(text).replace(/&/g, '&amp;').replace(/>/g, '&gt;').replace(/</g, '&lt;');
}

function trim(str) {
    str = to_string(str).replace(/^\s\s*/, '')
    var ws = /\s/, i = str.length;
    while ((i>0) && ws.test(str.charAt(--i)));
    return str.slice(0, i + 1);
}

function alert(text)
{
    QMessageBox.information(this, "ionPulse - alert", "<html><pre>"+escape(text));
}


function JsConsoleWidget(parent) {
    QWidget.call(this, parent);

    this.textEdit = new QTextEdit(this);
    this.textEdit.readOnly = true;
    this.htmlPrefix = "<html><head>        \
        <STYLE type=\"text/css\">          \
                .log {color: #aaee77;}     \
                .error {color: #ee8855;}   \
        </STYLE>                           \
    </head><body>";
    this.htmlSuffix = "</body></html>";
    this.htmlContent = "";

    this.lineInput = new QLineEdit(this);
    this.lineInput.returnPressed.connect(
        this,
        function() {
            var line = trim(this.lineInput.text);
            this.lineInput.text = "";
            if (line) {
                // wrap to catch thrown exceptions
                line = "try {ret = "+line+";console.log(typeof(ret)+ret?ret:\"\");} catch (err) {"
                     + "console.error('An error has occurred: '+err.message);"
                     + "}"
                try {
                    eval(line);
                } catch (err) {
                    // catches parse errors
                    console.error('An error has occurred: '+err.message);
                }
            }
            var scrollBar = this.textEdit.verticalScrollBar();
            scrollBar.value = scrollBar.maximum;
        }
    );

    var layout = new QVBoxLayout();
    layout.setSpacing(1);
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
    this.htmlContent += "<div class='line'><span class='log'>&gt; </span>"+escape(text)+"</div>";
    this.textEdit.html = this.htmlPrefix + this.htmlContent + this.htmlSuffix;
}
JsConsoleWidget.prototype.error = function (text)
{
    this.htmlContent += "<div class='line'><span class='error'>&gt; </span>"+escape(text)+"</div>";
    this.textEdit.html = this.htmlPrefix + this.htmlContent + this.htmlSuffix;
}


console = new JsConsoleWidget(window);
console.hide();
layoutManager.add("central/central_footer", console);

console.log("ionPulse.js initialised");
