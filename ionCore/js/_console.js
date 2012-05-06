/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

JsConsoleWidget = function(parent)
{
    QWidget.call(this, parent);

    this.history = [];
    this.historyIdx = null;
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
                this.history.push(line);
                this.historyIdx = null;
                // wrap to catch thrown exceptions
                line = "try {"+line+"} catch (err) {"
                     + "console.error('An error has occurred: '+err.message);"
                     + "}"
                try {
                    var ret = eval(line);
                    if ("undefined" === typeof(ret)) {
                        console.log("undefined");
                    } else {
                        console.log(typeof(ret)+": "+to_string(ret));
                    }
                } catch (err) {
                    // catches parse errors
                    console.error('An error has occurred: '+err.message);
                }
            }
        }
    );

    var layout = new QVBoxLayout();
    layout.setSpacing(1);
    layout.addWidget(this.textEdit, 0, 0);
    layout.addWidget(this.lineInput, 0, 0);
    this.setLayout(layout);

    this.setWindowTitle("Javascript console");


    this._appShortcutF12 = qs.system.installAppShortcut(Qt.Key_F12);
    this._appShortcutF12.callback.connect(
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
    this._lineInpShortcutUp = qs.system.installAppShortcut(Qt.Key_Up, Qt.NoModifier, this.lineInput);
    this._lineInpShortcutUp.callback.connect(
        this,
        function() {
            if (null === this.historyIdx) {
                this.historyIdx = this.history.length;
            }
            if (this.historyIdx > 0) {
                this.lineInput.text = this.history[--this.historyIdx];
            }
        }
    );
    this._lineInpShortcutDown = qs.system.installAppShortcut(Qt.Key_Down, Qt.NoModifier, this.lineInput);
    this._lineInpShortcutDown.callback.connect(
        this,
        function() {
            if (null === this.historyIdx) {
                return;
            }
            if (this.historyIdx < this.history.length) {
                this.lineInput.text = this.history[++this.historyIdx];
            }
        }
    );

}


JsConsoleWidget.prototype = new QWidget();
JsConsoleWidget.prototype.log = function (text)
{
    this.htmlContent += "<div class='line'><span class='log'>&gt; </span>"+escape(text)+"</div>";
    this.updateHtml();
}
JsConsoleWidget.prototype.error = function (text)
{
    this.htmlContent += "<div class='line'><span class='error'>&gt; </span>"+escape(text)+"</div>";
    this.updateHtml();
}
JsConsoleWidget.prototype.updateHtml = function()
{
    this.textEdit.html = this.htmlPrefix + this.htmlContent + this.htmlSuffix;
    var scrollBar = this.textEdit.verticalScrollBar();
    scrollBar.value = scrollBar.maximum;
}
