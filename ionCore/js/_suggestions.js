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
    return (m && (0 in m)) ? m[0] : '';
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
function findAstChild(parent, criteria) {
    var nodeIterator = parent.getChildrenIterator();
    while (nodeIterator.isValid()) {
        if (criteria(nodeIterator.value())) {
            return nodeIterator.value();
        }
        nodeIterator.next();
    }
    return null;
}
function findAstChildWrappingPosition(parent, line, col) {
   return findAstChild(parent, function(node){return isAstNodeWrappingPosition(node, line, col);});
}
function findAstChildByName(parent, name) {
    return findAstChild(parent, function(node){return node.getName() === name;});
}
function getAstPathForLocation(node, line, col) {
    var path = [];
    do {
        path.push(node);
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
function findAstParent(node, filter) {
    do {
        if (filter(node)) {
            return node;
        }
        node = node.getParent();
    } while (node);
    return null;
}
function iterateDbResults(query, callback)
{
    var results = dbxml.getStorage().query(query);
    while (results.next()) {
        callback(results.value());
    }
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
    if (!context) {
        return null;
    }
    if (('$this' === variable) || ('self' === variable)) {
        var classNode = findAstParent(context, function(node){
            return node.getName() === 'class_declaration';
        });
        if (classNode) {
            var classNameNode = findAstChildByName(classNode, 'string');
            if (classNameNode) {
                return classNameNode.getText();
            }
        }
        return null;
    }
    // else consult parsed data index
    console.log("not implemented yet: retrieveClassName for autocomplete");
}

Suggestions.prototype.retrieveVariables = function(context) {
    if (!context) {
        return [];
    }
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

Suggestions.prototype.retrievePropertiesAndMethodsForClass = function(className) {
    var currentWord = this.currentWord;
    var ret = [];
    console.log("not implemented yet: class hierarchy for autocomplete");

    iterateDbResults(
        'collection("dbxml:/files")//class_declaration[string/text()="'+className+'"]/class_statement_list/PROPERTY/class_properties/variable/text()',
        function(node) {
            var candidate = node.toString().substring(1);
            if (candidate.indexOf(currentWord) === 0) {
                ret.push(candidate);
            }
        }
    );
    iterateDbResults(
        'collection("dbxml:/files")//class_declaration[string/text()="'+className+'"]/class_statement_list/METHOD/string/text()',
        function(node) {
            var candidate = node.toString();
            if (candidate.indexOf(currentWord) === 0) {
                ret.push(candidate);
            }
        }
    );

    ret.sort();

    return _.uniq(ret, true);
}

Suggestions.prototype.retrieveSuggestions = function() {
    var precedingContext = getPrecedingContextForTheCurrentWord(this.editor.textCursor(), this.currentWord);
    var ast = null;
    if (/^\$/.test(this.currentWord)) {
        if (!precedingContext) {
            ast = phpPlugin.createParser().parseString(this.editor.plainText);
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
            ast = phpPlugin.createParser().parseString(this.editor.plainText);
            var className = this.retrieveClassName(precedingContext.context, this.getCurrentContext(ast));
            console.log(className);
            if (className) {
                return this.retrievePropertiesAndMethodsForClass(className);
            }
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
