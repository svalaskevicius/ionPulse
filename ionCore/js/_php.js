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
            "html/whitespace":       this.createCharFormat(new QColor(128, 100, 96), QFont.Light, false),
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
            "php/comment_sl":       this.createCharFormat(new QColor(100, 100, 100), QFont.Normal, true),
            "php/comment_ml":       this.createCharFormat(new QColor(100, 100, 100), QFont.Normal, true),
            "php/string_dq":        this.createCharFormat(new QColor(127, 225, 127), QFont.Normal, false),
            "php/string_sq":        this.createCharFormat(new QColor(127, 191, 127), QFont.Normal, false),
        };
        this.transitions = {
            "html": {
                "php":              this.regexTransition(/<\?php\b/g, true)
            },
            "php": {
                "html":             this.regexTransition(/\?>/g, false),
                "php/comment_ml":   this.regexTransition(/\/\*/g, true),
                "php/string_dq":    this.regexTransition(/"/g, true),
                "php/string_sq":    this.regexTransition(/'/g, true),
                "php/comment_sl":   this.regexTransition(/\/\//g, true),
            },
            "php/comment_ml": {
                "php":              this.regexTransition(/\*\//g, false),
            },
            "php/comment_sl": {
                "html":             this.regexTransition(/\?>/g, false),
                "php":              this.regexTransition(/$/g, false),
            },
            "php/string_dq": {
                "php":              this.regexTransition(/([^\\"]|\\.|^)"/g, false),
            },
            "php/string_sq": {
                "php":              this.regexTransition(/([^\\']|\\.|^)'/g, false),
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
            },
            "html": {
                "whitespace":       /\s+/g,
            }

        };

        this.parent.initialize.call(this);
    };

    PhpHighlighter.prototype.addBracketsInfo = function (from, to) {
        var re = /[\(\)\{\}\[\]]/g;
        re.lastIndex = from;
        var match;
        do {
            match = re.exec(this._text);
            if (match) {
                if (match.index < to) {
                    this.blockInfo.push({
                        pos: match.index,
                        char: match[0],
                    });
                } else {
                    match = false;
                }
            }
        } while (match);
    };

    PhpHighlighter.prototype._hightlightState = function (state, from, to) {
        this.parent._hightlightState.call(this, state, from, to);
        if ("php" === state) {
            this.addBracketsInfo(from, to);
        }
    };


    PhpHighlighter.prototype.highlight = function (cppApi, text) {
        this.blockInfo = [];

        this.parent.highlight.call(this, cppApi, text);

        this._cppApi.setCurrentBlockUserData(this.blockInfo);
    }


    var php = new PhpHighlighter();
    php.initialize();

    return function (cppApi, text) {
        php.highlight(cppApi, text);
    }
})();



var matchingBracketsHighlighter = (function () {

    var createSelectionForPos = function (editor, pos) {
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
        };

    var matchingRightBracket = function (character) {
            switch (character) {
                case '(': return ')';
                case '{': return '}';
                case '[': return ']';
            }
            return false;
        };

    var matchingLeftBracket = function (character) {
            switch (character) {
                case ')': return '(';
                case '}': return '{';
                case ']': return '[';
            }
            return false;
        };

    var matchLeftBracket = function (currentBlock, i, numLeftBrackets, bracketPair) {
            var infos = currentBlock.userData();
            docPos = currentBlock.position();
            for (; infos && i < infos.length; ++i) {
                if (infos[i].char === bracketPair.left) {
                    ++numLeftBrackets;
                } else if (infos[i].char === bracketPair.right) {
                    if (numLeftBrackets === 0) {
                        return docPos + infos[i].pos;
                    } else {
                        --numLeftBrackets;
                    }
                }
            }

            currentBlock = currentBlock.next();
            if (currentBlock.isValid()) {
                return matchLeftBracket(currentBlock, 0, numLeftBrackets, bracketPair);
            }

            return false;
        }

    var matchRightBracket = function (currentBlock, i, numRightBrackets, bracketPair) {
            var infos = currentBlock.userData();

            if (i === null && infos) {
                i = infos.length - 1;
            }
            docPos = currentBlock.position();
            for (; infos && i > -1 && infos.length > 0; --i) {
                if (infos[i].char === bracketPair.right) {
                    ++numRightBrackets;
                } else if (infos[i].char === bracketPair.left) {
                    if (numRightBrackets === 0) {
                        return docPos + infos[i].pos;
                    }--numRightBrackets;
                }
            }

            currentBlock = currentBlock.previous();
            if (currentBlock.isValid()) {
                return matchRightBracket(currentBlock, null, numRightBrackets, bracketPair);
            }

            return false;
        }

    var makeBracketPairFromLeft = function (leftChar) {
            var rightChar = matchingRightBracket(leftChar);
            if (rightChar) {
                return {
                    left: leftChar,
                    right: rightChar
                };
            }
            return null;
        };

    var makeBracketPairFromRight = function (rightChar) {
            var leftChar = matchingLeftBracket(rightChar);
            if (leftChar) {
                return {
                    left: leftChar,
                    right: rightChar
                };
            }
            return null;
        };

    var findMatchingBracket = function(currentChar, startingPos, block) {
        var bracketPair = makeBracketPairFromLeft(currentChar);
        if (bracketPair) {
            var matchedLeft = matchLeftBracket(block, startingPos + 1, 0, bracketPair);
            if (matchedLeft !== false) {
                return matchedLeft;
            }
        } else {
            bracketPair = makeBracketPairFromRight(currentChar);
            if (bracketPair) {
                var matchedRight = matchRightBracket(block, startingPos - 1, 0, bracketPair);
                if (matchedRight !== false) {
                    return matchedRight;
                }
           }
        }
        return false;
    }

    return function (editor) {
        try {
            var selections = editor.extraSelections();
            var textCursor = editor.textCursor();
            var userData = textCursor.block().userData();
            var curPos = textCursor.position() - textCursor.block().position();
            if (userData) {
                for (var i = userData.length - 1; i >= 0; --i) {
                    if (userData[i].pos == (curPos - 1)) {
                        var matchedPos = findMatchingBracket(userData[i].char, i, textCursor.block());
                        if (false !== matchedPos) {
                            selections.push(createSelectionForPos(editor, matchedPos));
                            selections.push(createSelectionForPos(editor, textCursor.block().position() + userData[i].pos));
                        }
                    }
                }
            }
            editor.setExtraSelections(selections);
        } catch (e) {
            console.error(e);
        }
    }
})();


editorPlugin.editorOpened.connect(
this, function (editor) {
    editor.cursorPositionChanged.connect(this, function () {
        matchingBracketsHighlighter(editor);
    });
});
