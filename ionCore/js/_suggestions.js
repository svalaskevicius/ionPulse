/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

function getWordBeforeCursor(cursor) {
    var m = cursor
            .block()
            .text()
            .substring(0, cursor.positionInBlock())
            .match(/[a-z\$_][a-z0-9_]*$/i);
    return (m && (0 in m)) ? m[0] : null;
}

Suggestions = function(editor)
{
    QListWidget.call(this, editor);
    this.setProperty('type', 'editor-suggestions');
    this.shortcut = qs.system.installAppShortcut(Qt.Key_Space, Qt.MetaModifier, editor);
    this.shortcut.callback.connect(
        this,
        function() {
            try {
                var uri = dbxml.getStorage().pathToDocumentUri(editor.path);
                var currentWord = getWordBeforeCursor(editor.textCursor());
                var results = dbxml.getStorage().query('distinct-values(doc("dbxml:/files/'+uri+'")//variable[starts-with(.,"'+currentWord+'")]/text())');
                if (!results.hasNext()) {
                    return;
                }
                this.clear();
                while (results.next()) {
                    this.addItem(results.value().toString());
                }

                if (!this.shortcut_close) {
                    this.shortcut_close = qs.system.installAppShortcut(Qt.Key_Escape, Qt.NoModifier, editor);
                    this.shortcut_close.callback.connect(this, this.onCloseRequested);
                }
                if (!this.shortcut_down) {
                    this.shortcut_down = qs.system.installAppShortcut(Qt.Key_Down, Qt.NoModifier, editor);
                    this.shortcut_down.callback.connect(this, this.onKeyDown);
                }
                if (!this.shortcut_up) {
                    this.shortcut_up = qs.system.installAppShortcut(Qt.Key_Up, Qt.NoModifier, editor);
                    this.shortcut_up.callback.connect(this, this.onKeyUp);
                }
                if (!this.shortcut_enter) {
                    this.shortcut_enter = qs.system.installAppShortcut(Qt.Key_Enter, Qt.NoModifier, editor);
                    this.shortcut_enter.callback.connect(this, this.onSubmit);
                }
                if (!this.shortcut_return) {
                    this.shortcut_return = qs.system.installAppShortcut(Qt.Key_Return, Qt.NoModifier, editor);
                    this.shortcut_return.callback.connect(this, this.onSubmit);
                }

                var origin = editor.cursorRect(editor.textCursor()).bottomLeft();
                this.move(origin.x(), origin.y());

                this.show();
             } catch(e) {
                 console.error(e);
             }
        }
    );

}

Suggestions.prototype = new QListWidget();

Suggestions.prototype.onCloseRequested = function() {
    try {
        this._hide();
     } catch(e) {
         console.error(e);
     }
}
Suggestions.prototype.onKeyDown = function() {
    try {
        if (this.currentRow < this.count-1) {
            this.currentRow++;
        }
     } catch(e) {
         console.error(e);
     }
}
Suggestions.prototype.onKeyUp = function() {
    try {
        if (this.currentRow > 0) {
            this.currentRow--;
        }
     } catch(e) {
         console.error(e);
     }
}
Suggestions.prototype.onSubmit = function() {
    try {
        if (this.currentRow >= 0) {
            console.log(this.item(this.currentRow).text());
        }
        this._hide();
     } catch(e) {
         console.error(e);
     }
}
Suggestions.prototype._hide = function() {
    if (this.shortcut_close) {
        this.shortcut_close.disable();
        this.shortcut_close = null;
    }
    if (this.shortcut_down) {
        this.shortcut_down.disable();
        this.shortcut_down = null;
    }
    if (this.shortcut_up) {
        this.shortcut_up.disable();
        this.shortcut_up = null;
    }
    if (this.shortcut_enter) {
        this.shortcut_enter.disable();
        this.shortcut_enter = null;
    }
    if (this.shortcut_return) {
        this.shortcut_return.disable();
        this.shortcut_return = null;
    }
    this.hide();
}
