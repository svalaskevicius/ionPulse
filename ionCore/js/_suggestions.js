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
function getPrecedingContextForTheCurrentWord(cursor, currentWord) {
    var positionInBlock = cursor.positionInBlock() - currentWord.length - 1;
    var block = cursor.block();
    var text = block.text();
    var goBackOneChar = function() {
        positionInBlock--;
        while (positionInBlock < 0) {
            block = block.previous();
            if (!block.isValid()) {
                return false;
            }
            text = block.text();
            positionInBlock = text.length - 1;
        }
        return true;
    }
    var skimThroughWhiteSpace = function() {
        while (_.contains([' ', "\t"], text[positionInBlock]) || positionInBlock < 0) {
            if (!goBackOneChar()) {
                return false;
            }
        }
        return true;
    }

    if (!skimThroughWhiteSpace()) {
        return false;
    }
    var separator = text.substring(positionInBlock-1, positionInBlock+1);
    if (!_.contains(["->", "::"], separator)) {
        return false;
    }
    if (!goBackOneChar() || !goBackOneChar() || !skimThroughWhiteSpace()) {
        return false;
    }
    var m = text
        .substring(0, positionInBlock+1)
        .match(/[a-z\$_][a-z0-9_]*$/i);

    return (m && (0 in m)) ? {separator : separator, context : m[0]} : false;
}

function isAstNodeWrappingPosition(node, line, col) {
    var startLine = node.getStartLine();
    if (startLine > line) {
        return false;
    }
    if ((startLine === line) && (node.getStartCol() > col)) {
        return false;
    }

    var endLine = node.getEndLine();
    if (endLine < line) {
        return false;
    }
    if ((endLine === line) && (node.getEndCol() < col)) {
        return false;
    }

    return true;
}
function findAstChildWrappingPosition(parent, line, col) {
    var nodeIterator = parent.getChildrenIterator();
    while (nodeIterator.isValid()) {
        if (isAstNodeWrappingPosition(nodeIterator.value(), line, col)) {
            return nodeIterator.value();
        }
        nodeIterator.next();
    }
    return null;
}
function getAstPathForLocation(node, line, col) {
    var path = [];
    do {
        path = path.concat(node);
        node = findAstChildWrappingPosition(node, line, col);
    } while (node);
    return path;
}
function recursiveAstChildrenFilter(node, filter) {
    var ret = [];
    var parents = [node];
    while (parents.length > 0) {
        var nodeIterator = parents.pop().getChildrenIterator();
        while (nodeIterator.isValid()) {
            if (filter(nodeIterator.value())) {
                ret.push(nodeIterator.value());
            }
            parents.push(nodeIterator.value());
            nodeIterator.next();
        }
    }
    return ret;
}

Suggestions = function(editor)
{
    QListWidget.call(this, editor);
    this.editor = editor;
    this.setProperty('type', 'editor-suggestions');
    this.shortcut = qs.system.installAppShortcut(Qt.Key_Space, Qt.MetaModifier, editor);
    this.shortcut.callback.connect(
        this,
        this.showSuggestions
    );

}

Suggestions.prototype = new QListWidget();

Suggestions.prototype.getCurrentContext = function(ast) {
    var lineNr = this.editor.textCursor().blockNumber() + 1;
    var colNr = this.editor.textCursor().positionInBlock();

    return _.find(
        getAstPathForLocation(ast.getRoot(), lineNr, colNr).reverse(),
        function(el){
            return _.contains(['METHOD', 'function_declaration'], el.getName());
        }
    );
}

Suggestions.prototype.retrieveClassName = function(variable, context) {
    if ('$this' === variable) {
        return context.className;
    }
    // else consult parsed data index
}

Suggestions.prototype.retrieveVariables = function(context) {
    var currentWord = this.currentWord;

    var ret = recursiveAstChildrenFilter(
        context,
        function(node){
            return (node.getName() === 'variable') && (node.getText().indexOf(currentWord) === 0);
        }
    ).map(
        function(node){
            return node.getText();
        }
    );
    if (('METHOD' === context.getName()) && ("$this".indexOf(currentWord) === 0)) {
        ret.push("$this");
    }

    ret.sort();

    return _.uniq(ret, true);
}

Suggestions.prototype.retrieveSuggestions = function() {
    var precedingContext = getPrecedingContextForTheCurrentWord(this.editor.textCursor(), this.currentWord);
    if (/^\$/.test(this.currentWord)) {
        if (!precedingContext) {
            var ast = phpPlugin.createParser().parseString(this.editor.plainText);
            return this.retrieveVariables(this.getCurrentContext(ast));
        } else if (precedingContext.separator === "::") {
            //static property or method, check this.classHierarchy(this.retrieveClassName(precedingContext.context)) contents
            console.log("not implemented yet: static properties for autocomplete");
        }
    } else {
        if (!precedingContext) {
            //this might be a class name
            console.log("not implemented yet: class names for autocomplete");
        } else if (precedingContext.separator === "->") {
            //property or method, check this.classHierarchy(this.retrieveClassName(precedingContext.context)) contents
            console.log("not implemented yet: object properties for autocomplete");
        }
    }
}

Suggestions.prototype.showSuggestions = function() {
    try {
        this.currentWord = getWordBeforeCursor(this.editor.textCursor());

        var results = this.retrieveSuggestions();
        if (!results || !results.length) {
            return;
        }

        this.clear();
        this.addItems(results);
        if (this.count) {
            this.currentRow = 0;
        }

        if (!this.shortcut_close) {
            this.shortcut_close = qs.system.installAppShortcut(Qt.Key_Escape, Qt.NoModifier, this.editor);
            this.shortcut_close.callback.connect(this, this.onCloseRequested);
        }
        if (!this.shortcut_down) {
            this.shortcut_down = qs.system.installAppShortcut(Qt.Key_Down, Qt.NoModifier, this.editor);
            this.shortcut_down.callback.connect(this, this.onKeyDown);
        }
        if (!this.shortcut_up) {
            this.shortcut_up = qs.system.installAppShortcut(Qt.Key_Up, Qt.NoModifier, this.editor);
            this.shortcut_up.callback.connect(this, this.onKeyUp);
        }
        if (!this.shortcut_enter) {
            this.shortcut_enter = qs.system.installAppShortcut(Qt.Key_Enter, Qt.NoModifier, this.editor);
            this.shortcut_enter.callback.connect(this, this.onSubmit);
        }
        if (!this.shortcut_return) {
            this.shortcut_return = qs.system.installAppShortcut(Qt.Key_Return, Qt.NoModifier, this.editor);
            this.shortcut_return.callback.connect(this, this.onSubmit);
        }

        var origin = this.editor.cursorRect(this.editor.textCursor()).bottomLeft();
        this.move(origin.x(), origin.y());

        this.show();
     } catch(e) {
         console.error(e);
     }
}

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
            this.editor.textCursor().movePosition(QTextCursor.PreviousCharacter, QTextCursor.KeepAnchor, this.currentWord.length);
            this.editor.textCursor().removeSelectedText();
            this.editor.textCursor().insertText(this.item(this.currentRow).text())
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
