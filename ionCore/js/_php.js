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
            "php/comment":          this.createCharFormat(new QColor(100, 100, 100), QFont.Normal, true),
            "php/string_dq":        this.createCharFormat(new QColor(127, 225, 127), QFont.Normal, false),
            "php/string_sq":        this.createCharFormat(new QColor(127, 191, 127), QFont.Normal, false),
        };
        this.transitions = {
            "html": {
                "php": this.regexTransition(/<\?php\b/g, true)
            },
            "php": {
                "html": this.regexTransition(/\?>/g, false),
                "php/comment": this.regexTransition(/\/\*/g, true),
                "php/string_dq": this.regexTransition(/"/g, true),
                "php/string_sq": this.regexTransition(/'/g, true),
            },
            "php/comment": {
                "php": this.regexTransition(/\*\//g, false),
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
                "number":           /-?([0-9]+)?\.?[0-9]+/g,
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
                "comment":          /\/\/.*/g,
            }
        };

        this.parent.initialize.call(this);
    }
    php = new PhpHighlighter();
    php.initialize();
    return function (cppApi, text) {
        php.highlight(cppApi, text);
    }
})();
