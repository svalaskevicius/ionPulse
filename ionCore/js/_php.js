/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

phpHighlighter = (function () {
    var PhpHighlighter = (function () {}).inheritsFrom(TextHighlighterFactory);
    PhpHighlighter.prototype.initialize = function () {
        this.charFormatting = {
            "html":                 this.createCharFormat(Qt.lightGray, QFont.Normal, false),
            "php":                  this.createCharFormat(new QColor(220, 220, 180), QFont.Normal, false),
            "php/constructs":       this.createCharFormat(new QColor(255, 255, 127), QFont.Bold, true),
            "php/number":           this.createCharFormat(Qt.red, QFont.Normal, false),
            "php/whitespace":       this.createCharFormat(new QColor(128, 100, 96), QFont.Light, false),
            "php/keyword":          this.createCharFormat(new QColor(191, 127, 255), QFont.Bold, false),
            "php/compileConstants": this.createCharFormat(new QColor(255, 255, 127), QFont.Bold, true),
            "php/variable":         this.createCharFormat(new QColor(194, 158, 64), QFont.Normal, false),
            "php/property":         this.createCharFormat(new QColor(198, 120, 44), QFont.Normal, false),
            "php/function":         this.createCharFormat(new QColor(164, 158, 96), QFont.Normal, false),
            "php/separator":        this.createCharFormat(new QColor(191, 255, 127), QFont.Black, false),
            "php/comment_sl":          this.createCharFormat(new QColor(100, 100, 100), QFont.Normal, true),
            "php/comment_ml":          this.createCharFormat(new QColor(100, 100, 100), QFont.Normal, true),
            "php/string_dq":        this.createCharFormat(new QColor(127, 225, 127), QFont.Normal, false),
            "php/string_sq":        this.createCharFormat(new QColor(127, 191, 127), QFont.Normal, false),
        };
        this.transitions = {
            "html": {
                "php": this.regexTransition(/<\?php\b/g, true)
            },
            "php": {
                "html": this.regexTransition(/\?>/g, false),
                "php/comment_ml": this.regexTransition(/\/\*/g, true),
                "php/string_dq": this.regexTransition(/"/g, true),
                "php/string_sq": this.regexTransition(/'/g, true),
                "php/comment_sl": this.regexTransition(/\/\//g, true),
            },
            "php/comment_ml": {
                "php": this.regexTransition(/\*\//g, false),
            },
            "php/comment_sl": {
                "html": this.regexTransition(/\?>/g, false),
                "php": this.regexTransition(/$/g, false),
            },
            "php/string_dq": {
                "php": this.regexTransition(/([^\\"]|\\.|^)"/g, false),
            },
            "php/string_sq": {
                "php": this.regexTransition(/([^\\']|\\.|^)'/g, false),
            }
        };
        this.highlightRules = {
            "php": {
                "number":           /\b([0-9]+)?\.?[0-9]+\b/g,
                "whitespace":       /\s+/g,
                "function":         /[a-z_][a-z0-9_]*\s*\(/ig,
                "constructs":       new RegExp("class\\s+[a-z0-9_]+((\\s+extends\\s+[a-z0-9_]+)?(\\s+implements+[a-z0-9_]+)?)*"
                                               + "|\\b(die|echo|empty|exit|eval|include|include_once|isset|list|require|require_once"
                                               + "|return|print|unset)\\b",
                                               "ig"
                                    ),
                "keyword":          new RegExp("<\\?php|\\?>|\\b(class|function|protected|private|public"
                                               + "|abstract|extends|interface|implements|abstract|and|array"
                                               + "|as|break|case|catch|clone|const|continue|declare"
                                               + "|default|do|else|elseif|enddeclare|endfor"
                                               + "|endforeach|endif|endswitch|endwhile|extends"
                                               + "|final|for|foreach|function|global|goto"
                                               + "|if|instanceof|namespace|new|or|static"
                                               + "|switch|throw|try|use|var|while|xor)\\b",
                                               "g"
                                    ),
                "compileConstants": /__CLASS__|__DIR__|__FILE__|__LINE__|__FUNCTION__|__METHOD__|__NAMESPACE__/g,
                "variable":         /\$[a-z_][a-z0-9_]*/ig,
                "property":         /->[a-z_][a-z0-9_]*/ig,
                "separator":        /->|;|\+|-|\*|\/|=|\(|\)|\||&/g,
            }
        };

        this.parent.initialize.call(this);
    };

    PhpHighlighter.prototype.addParensInfo = function(from, to) {
        var blockInfo = [];
        var re = /[\(\)\{\}\[\]]/g;
        re.lastIndex = from;
        var match;
        do {
            match = re.exec(this._text);
            if (match) {
                if (match.index < to) {
                    blockInfo.push({
                        pos : match.index,
                        char : match[0],
                    });
                } else {
                    match = false;
                }
            }
        } while (match);

        this._cppApi.setCurrentBlockUserData(blockInfo);
    };

    PhpHighlighter.prototype._hightlightState = function (state, from, to) {
        this.parent._hightlightState.call(this, state, from, to);
        if ("php" === state) {
            this.addParensInfo(from, to);
        }
    };

    var php = new PhpHighlighter();
    php.initialize();
    return function (cppApi, text) {
        php.highlight(cppApi, text);
    }
})();



var matchingParensHighlighter = (function(){

    var createSelectionForPos = function(editor, pos) {
        var selection = new QTextEdit_ExtraSelection();
        var format = selection.format();
        var brush = new QBrush();
        brush.setColor(Qt.black);
        brush.setStyle(Qt.SolidPattern);
        //format.setForeground(brush);
        format.setBackground(brush);
        selection.setFormat(format);
        var cursor = editor.textCursor();
        cursor.setPosition(pos);
        cursor.movePosition(QTextCursor.NextCharacter, QTextCursor.KeepAnchor);
        qs.system.setTextEditExtraSelectionCursor(selection, cursor);
        return selection;
    }

     matchLeftParenthesis = function(currentBlock, i, numLeftParentheses)
     {
         var infos = currentBlock.userData();

         docPos = currentBlock.position();
         for (; i < infos.length; ++i) {

             if (infos[i].char === '(') {
                 ++numLeftParentheses;
                 continue;
             }

             if (infos[i].char === ')' && numLeftParentheses === 0) {
                 return docPos + infos[i].pos;
             } else {
                 --numLeftParentheses;
             }
         }

         currentBlock = currentBlock.next();
         if (currentBlock.isValid()) {
             return matchLeftParenthesis(currentBlock, 0, numLeftParentheses);
         }

         return false;
     }

     matchRightParenthesis = function(currentBlock, i, numRightParentheses)
     {
         var infos = currentBlock.userData();

         docPos = currentBlock.position();
         for (; i > -1 && infos.length > 0; --i) {
             if (infos[i].char === ')') {
                 ++numRightParentheses;
                 continue;
             }
             if (infos[i].char === '(' && numRightParentheses === 0) {
                 return docPos + infos[i].pos;
             } else {
                 --numRightParentheses;
             }
         }

         currentBlock = currentBlock.next();
         if (currentBlock.isValid()) {
             return matchRightParenthesis(currentBlock, 0, numRightParentheses);
         }

         return false;
     }

    return function(editor) {
        try{
            var selections = editor.extraSelections();
            var textCursor = editor.textCursor();
            var userData = textCursor.block().userData();
            var curPos = textCursor.position() - textCursor.block().position();
            if (userData) {
                for (var i = userData.length-1; i>=0; --i) {
                    if ((userData[i].pos == (curPos - 1)) && (userData[i].char == '(')) {
                        var matchedLeft = matchLeftParenthesis(textCursor.block(), i + 1, 0);
                        if (matchedLeft !== false) {
                            selections.push(createSelectionForPos(editor, matchedLeft));
                            selections.push(createSelectionForPos(editor, textCursor.block().position() + userData[i].pos));
                        }
                    } else if (userData[i].pos == curPos - 1 && userData[i].char == ')') {
                        var matchedRight = matchRightParenthesis(textCursor.block(), i - 1, 0);
                        if (matchedRight !== false) {
                            selections.push(createSelectionForPos(editor, matchedRight));
                            selections.push(createSelectionForPos(editor, textCursor.block().position() + userData[i].pos));
                        }
                    }

                }
            }
            editor.setExtraSelections(selections);
        } catch(e) {
            console.error(e);
        }
    }
})();


editorPlugin.editorOpened.connect(
    this,
    function (editor) {
        editor.cursorPositionChanged.connect(this, function(){
            matchingParensHighlighter(editor);
        });
    }
);
