/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

var toColor = function(hex) {
    return new QColor(parseInt(hex, 16));
};
var solarizedColor = function(code) {
    var solarizedPallete = {
        'base03' : {'sRGB': "002b36", 'genRGB': "042028", 'c256': "1c1c1c", 'c16': "brightblack"  , 'c8': 'black'},
        'base02' : {'sRGB': "073642", 'genRGB': "0a2832", 'c256': "262626", 'c16': "black"        , 'c8': 'black'},
        'base01' : {'sRGB': "586e75", 'genRGB': "465a61", 'c256': "585858", 'c16': "brightgreen"  , 'c8': 'green'},
        'base00' : {'sRGB': "657b83", 'genRGB': "52676f", 'c256': "626262", 'c16': "brightyellow" , 'c8': 'yellow'},
        'base0'  : {'sRGB': "839496", 'genRGB': "708183", 'c256': "808080", 'c16': "brightblue"   , 'c8': 'blue'},
        'base1'  : {'sRGB': "93a1a1", 'genRGB': "81908f", 'c256': "8a8a8a", 'c16': "brightcyan"   , 'c8': 'cyan'},
        'base2'  : {'sRGB': "eee8d5", 'genRGB': "e9e2cb", 'c256': "e4e4e4", 'c16': "white"        , 'c8': 'white'},
        'base3'  : {'sRGB': "fdf6e3", 'genRGB': "fcf4dc", 'c256': "ffffd7", 'c16': "brightwhite"  , 'c8': 'white'},
        'yellow' : {'sRGB': "b58900", 'genRGB': "a57705", 'c256': "af8700", 'c16': "yellow"       , 'c8': 'yellow'},
        'orange' : {'sRGB': "cb4b16", 'genRGB': "bd3612", 'c256': "d75f00", 'c16': "brightred"    , 'c8': 'red'},
        'red'    : {'sRGB': "dc322f", 'genRGB': "c60007", 'c256': "d70000", 'c16': "red"          , 'c8': 'red'},
        'magenta': {'sRGB': "d33682", 'genRGB': "c61b6e", 'c256': "af005f", 'c16': "magenta"      , 'c8': 'magenta'},
        'violet' : {'sRGB': "6c71c4", 'genRGB': "5859b7", 'c256': "5f5faf", 'c16': "brightmagenta", 'c8': 'magenta'},
        'blue'   : {'sRGB': "268bd2", 'genRGB': "2075c7", 'c256': "0087ff", 'c16': "blue"         , 'c8': 'blue'},
        'cyan'   : {'sRGB': "2aa198", 'genRGB': "259185", 'c256': "00afaf", 'c16': "cyan"         , 'c8': 'cyan'},
        'green'  : {'sRGB': "859900", 'genRGB': "728a05", 'c256': "5f8700", 'c16': "green"        , 'c8': 'green'},
    };
    if (typeof solarizedPallete[code] === 'undefined') {
        console.error("cannot find the requested solarized color: "+code);
        throw "cannot find the requested solarized color: "+code;
    }
    return toColor(solarizedPallete[code]['sRGB']);
};


phpHighlighter = (function () {
    var PhpHighlighter = (function () {}).inheritsFrom(TextHighlighterFactory);
    PhpHighlighter.prototype.initialize = function () {

        this.charFormatting = {
            "html":                         this.createCharFormat(solarizedColor("base0"), QFont.Normal, false),
            "html/whitespace":              this.createCharFormat(solarizedColor("base02"), QFont.Light, false),
            "php":                          this.createCharFormat(solarizedColor("base0"), QFont.Normal, false),
            "php/constructs":               this.createCharFormat(solarizedColor("orange"), QFont.Bold, true),
            "php/number":                   this.createCharFormat(solarizedColor("red"), QFont.Normal, false),
            "php/whitespace":               this.createCharFormat(solarizedColor("base02"), QFont.Light, false),
            "php/keyword":                  this.createCharFormat(solarizedColor("orange"), QFont.Bold, false),
            "php/compileConstants":         this.createCharFormat(solarizedColor("yellow"), QFont.Bold, true),
            "php/variable":                 this.createCharFormat(solarizedColor("base3"), QFont.Normal, false),
            "php/property":                 this.createCharFormat(solarizedColor("base2"), QFont.Normal, false),
            "php/function":                 this.createCharFormat(solarizedColor("blue"), QFont.Normal, false),
            "php/separator":                this.createCharFormat(solarizedColor("violet"), QFont.Black, false),
            "php/comment_sl":               this.createCharFormat(solarizedColor("base01"), QFont.Normal, true),
            "php/comment_ml":               this.createCharFormat(solarizedColor("base01"), QFont.Normal, true),
            "php/comment_ml/whitespace":    this.createCharFormat(solarizedColor("base02"), QFont.Normal, true),
            "php/string_dq":                this.createCharFormat(solarizedColor("green"), QFont.Normal, false),
            "php/string_dq/variable":       this.createCharFormat(solarizedColor("base2"), QFont.Normal, false),
            "php/string_sq":                this.createCharFormat(solarizedColor("cyan"), QFont.Normal, false),
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
                "php/comment_sl":   this.regexTransition(/(\/\/|#)/g, true),
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
                "property":         /->[a-z_][a-z0-9_]*/ig,
                "function":         /[a-z_][a-z0-9_]*\s*\(/ig,
                "variable":         /\$[a-z_][a-z0-9_]*/ig,
                "separator":        /->|;|\+|-|\*|\/|=|\(|\)|\||&/g,
                "whitespace":       /\s+/g,
            },
            "php/comment_ml": {
                "whitespace":       /\s+/g,
            },
            "php/string_dq": {
                "variable":         /\$[a-z_][a-z0-9_]*|\{\$.*?\}/ig,
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

            var fgbrush = new QBrush();
            fgbrush.setColor(solarizedColor("red"));
            fgbrush.setStyle(Qt.SolidPattern);
            format.setForeground(fgbrush);

            var bgbrush = new QBrush();
            bgbrush.setColor(Qt.black);
            bgbrush.setStyle(Qt.SolidPattern);
            format.setBackground(bgbrush);

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
