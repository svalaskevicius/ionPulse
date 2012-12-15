/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

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

var solarizedColorTheme = {
    "html":                        [ solarizedColor("base0"),  QFont.Normal, false, null, solarizedColor("base03") ],
    "html/whitespace":             [ solarizedColor("base02"), QFont.Light,  false, null, solarizedColor("base03") ],
    "php":                         [ solarizedColor("base0"),  QFont.Normal, false, null, solarizedColor("base03") ],
    "php/constructs":              [ solarizedColor("orange"), QFont.Bold,   true , null, solarizedColor("base03") ],
    "php/number":                  [ solarizedColor("red"),    QFont.Normal, false, null, solarizedColor("base03") ],
    "php/whitespace":              [ solarizedColor("base02"), QFont.Light,  false, null, solarizedColor("base03") ],
    "php/keyword":                 [ solarizedColor("orange"), QFont.Bold,   false, null, solarizedColor("base03") ],
    "php/compileConstants":        [ solarizedColor("yellow"), QFont.Bold,   true , null, solarizedColor("base03") ],
    "php/variable":                [ solarizedColor("base3"),  QFont.Normal, false, null, solarizedColor("base03") ],
    "php/property":                [ solarizedColor("base2"),  QFont.Normal, false, null, solarizedColor("base03") ],
    "php/function":                [ solarizedColor("blue"),   QFont.Normal, false, null, solarizedColor("base03") ],
    "php/method":                  [ solarizedColor("blue"),   QFont.Normal, false, null, solarizedColor("base03") ],
    "php/separator":               [ solarizedColor("violet"), QFont.Black,  false, null, solarizedColor("base03") ],
    "php/comment_sl":              [ solarizedColor("base01"), QFont.Normal, true , null, solarizedColor("base03") ],
    "php/comment_ml":              [ solarizedColor("base01"), QFont.Normal, true , null, solarizedColor("base03") ],
    "php/comment_ml/whitespace":   [ solarizedColor("base02"), QFont.Normal, true , null, solarizedColor("base03") ],
    "php/string_dq":               [ solarizedColor("green"),  QFont.Normal, false, null, solarizedColor("base03") ],
    "php/string_dq/variable":      [ solarizedColor("base2"),  QFont.Normal, false, null, solarizedColor("base03") ],
    "php/string_sq":               [ solarizedColor("cyan"),   QFont.Normal, false, null, solarizedColor("base03") ],
};

// from http://studiostyl.es/schemes/node-js-db-drivers
var nodeJsDbTheme = {
    "html":                        [ toColor("ccccb0"), QFont.Normal, false, 1, null ],
    "html/whitespace":             [ toColor("4F4F4F"), QFont.Light,  false, 1, null ],
    "php":                         [ toColor("e6e1dc"), QFont.Normal, false, 1, null ],
    "php/constructs":              [ toColor("ffc66d"), QFont.Bold,   true , 1, null ],
    "php/number":                  [ toColor("7a9eb8"), QFont.Normal, false, 1, null ],
    "php/whitespace":              [ toColor("4F4F4F"), QFont.Light,  false, 1, null ],
    "php/keyword":                 [ toColor("bd4f3e"), QFont.Bold,   false, 1, null ],
    "php/compileConstants":        [ toColor("ffc66d"), QFont.Bold,   true , 1, null ],
    "php/variable":                [ toColor("f5f0b9"), QFont.Normal, false, 1, null ],
    "php/property":                [ toColor("efcb93"), QFont.Normal, false, 1, null ],
    "php/function":                [ toColor("cc7833"), QFont.Normal, false, 1, null ],
    "php/method":                  [ toColor("cc7833"), QFont.Normal, false, 1, null ],
    "php/separator":               [ toColor("ffc66d"), QFont.Black,  false, 1, null ],
    "php/comment_sl":              [ toColor("987c4e"), QFont.Normal, true , 1, null ],
    "php/comment_ml":              [ toColor("987c4e"), QFont.Normal, true , .9, null ],
    "php/comment_ml/whitespace":   [ toColor("4F4F4F"), QFont.Normal, true , .9, null ],
    "php/string_dq":               [ toColor("a5c261"), QFont.Normal, false, 1, null ],
    "php/string_dq/variable":      [ toColor("7a9eb8"), QFont.Normal, false, 1, null ],
    "php/string_sq":               [ toColor("bdf43e"), QFont.Normal, false, 1, null ],
};




textHighlighter.addTextFormatting(nodeJsDbTheme);

textHighlighter.addTransitions({
    "html": {
        "php":              textHighlighter.regexTransition(/<\?php\b/g, true)
    },
    "php": {
        "html":             textHighlighter.regexTransition(/\?>/g, false),
        "php/comment_ml":   textHighlighter.regexTransition(/\/\*/g, true),
        "php/string_dq":    textHighlighter.regexTransition(/"/g, true),
        "php/string_sq":    textHighlighter.regexTransition(/'/g, true),
        "php/comment_sl":   textHighlighter.regexTransition(/(\/\/|#)/g, true),
    },
    "php/comment_ml": {
        "php":              textHighlighter.regexTransition(/\*\//g, false),
    },
    "php/comment_sl": {
        "html":             textHighlighter.regexTransition(/\?>/g, false),
        "php":              textHighlighter.regexTransition(/$/g, false),
    },
    "php/string_dq": {
        "php":              textHighlighter.regexTransition(/([^\\"]|\\.|^)"/g, false),
    },
    "php/string_sq": {
        "php":              textHighlighter.regexTransition(/([^\\']|\\.|^)'/g, false),
    }
});
textHighlighter.addHighlightRules({
    "php": {
        "number":           /\b([0-9]+)?\.?[0-9]+\b/g,
        "constructs":       new RegExp("class\\s+[a-z0-9_]+((\\s+extends\\s+[a-z0-9_]+)?(\\s+implements+[a-z0-9_]+)?)*"
                                       + "|\\b(die|echo|empty|exit|eval|include|include_once|isset|list|require|require_once"
                                       + "|return|print|unset)\\b",
                                       "ig"
                            ),
        "function":         /[a-z_][a-z0-9_]*\s*\(/ig,
        "keyword":          new RegExp("<\\?php|\\?>|\\b(class|function|protected|private|public"
                                       + "|abstract|extends|interface|implements|abstract|and|array"
                                       + "|as|break|case|catch|clone|const|continue|declare"
                                       + "|default|do|else|elseif|enddeclare|endfor"
                                       + "|endforeach|endif|endswitch|endwhile|extends"
                                       + "|final|for|foreach|function|global|goto"
                                       + "|if|instanceof|namespace|new|or|static"
                                       + "|switch|throw|try|use|var|while|xor|true|false|null|self)\\b",
                                       "g"
                            ),
        "compileConstants": /__CLASS__|__DIR__|__FILE__|__LINE__|__FUNCTION__|__METHOD__|__NAMESPACE__/g,
        "property":         /->[a-z_][a-z0-9_]*/ig,
        "method":           /->[a-z_][a-z0-9_]*\s*\(/ig,
        "variable":         /\$[a-z_][a-z0-9_]*/ig,
        "separator":        /->|;|\+|-|\*|\/|=|\(|\)|\||&|\{|\}|\[|\]/g,
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

});


textHighlighter.setBlockInfoHandler(
    'php',
    function (blockInfo, from, to, text) {
        if (blockInfo === undefined) {
            blockInfo = [];
        }
        var re = /[\(\)\{\}\[\]]/g;
        re.lastIndex = from;
        var match;
        do {
            match = re.exec(text);
            if (match) {
                if (match.index < to) {
                    blockInfo.push({
                        pos: match.index,
                        char: match[0],
                    });
                } else {
                    match = false;
                }
            }
        } while (match);
        return blockInfo;
    }
);

textHighlighter.setDefaultState('text/php', 'html');
textHighlighter.setDefaultState('text', 'html');

var matchingBracketsHighlighter = (function () {

    var getPhpUserData = function (currentBlock) {
            var userData = currentBlock.userData();
            if (userData && userData['php'] !== undefined) {
                return userData['php'];
            }
            return null;
        };

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
            var infos = getPhpUserData(currentBlock);
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
            var infos = getPhpUserData(currentBlock)

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
            var userData = getPhpUserData(textCursor.block());
            var curPos = textCursor.position() - textCursor.block().position();
            if (userData) {
                for (var i = userData.length - 1; i >= 0; --i) {
                    if (userData[i].pos === (curPos - 1)) {
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
    this,
    function (editor) {
        editor.cursorPositionChanged.connect(
            this,
            function () {
                matchingBracketsHighlighter(editor);
            }
        );
    }
);
