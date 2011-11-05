%option bison-bridge
%option reentrant
%option noyywrap
%option stack
%option never-interactive
%option prefix="_impl_ionPhp_"

%{
#include "phpparser.h"
#include "phpParser/ionParserLib.h"
#include "gen_php_parser.hpp"

using namespace IonPhp;


/* Gets input and stuffs it into "buf".  number of characters read, or YY_NULL,
 * is returned in "result".
 */
#define YY_INPUT(buf,result,max_size) \
                result = 0;
#define YY_USER_ACTION \
        {  \
            yyextra->__line = yyextra->__posLine;\
            yyextra->__col = yyextra->__posCol;\
            for(char *c=yytext; *c; c++) {\
                switch (*c) { \
                    case '\n': \
                        yyextra->__posLine++;\
                        yyextra->__posCol = 0; \
                        break; \
                    default:\
                        yyextra->__posCol++;\
                }\
            }\
        }
#define yylval_param astNode
//#define YY_DECL int ion_php_lex(pASTNode *astNode, yyscan_t yyscanner)
#define YY_EXTRA_TYPE IonPhp::phpParser*
#define YY_FATAL_ERROR(msg) throw std::logic_error(msg)
#define ECHO yyextra->__echo(yytext, yyleng)

#define CREATE_AST_NODE(node) ASTNode::create(node)->setPosition(yyextra->__line, yyextra->__col)

char ionInput(yyscan_t yyscanner);
void ionUnput(char c, yyscan_t yyscanner);

char lookupChar(yyscan_t yyscanner);
QString lookupChars(int numberOfChars, yyscan_t yyscanner);
bool matchesRegexp(QString input, QString regex);
QString retrieveChars(int numberOfChars, yyscan_t yyscanner);
void putCharsBack(QString chars, yyscan_t yyscanner);
bool isStartOfLabel(yyscan_t yyscanner);
bool scanForHereDocEnd(yyscan_t yyscanner);
%}
LNUM	[0-9]+
DNUM	([0-9]*"."[0-9]+)|([0-9]+"."[0-9]*)
EXPONENT_DNUM	(({LNUM}|{DNUM})[eE][+-]?{LNUM})
HNUM	"0x"[0-9a-fA-F]+
LABEL	[a-zA-Z_\x7f-\xff][a-zA-Z0-9_\x7f-\xff]*
WHITESPACE [ \n\r\t]+
TABS_AND_SPACES [ \t]*
TOKENS [;:,.\[\]()|^&+/=*%!~$<>?@-]
NEWLINE ("\r"|"\n"|"\r\n")

%x ST_IN_SCRIPTING
%x ST_LOOKING_FOR_PROPERTY
%x ST_LOOKING_FOR_VARNAME
%s ST_DOUBLE_QUOTES
%s ST_BACKQUOTE
%s ST_HEREDOC
%s ST_NOWDOC
%s ST_VAR_OFFSET
%x ST_END_HEREDOC

%%

<ST_IN_SCRIPTING>"exit" {
        return T_EXIT;
}

<ST_IN_SCRIPTING>"die" {
        return T_EXIT;
}

<ST_IN_SCRIPTING>"function" {
        return T_FUNCTION;
}

<ST_IN_SCRIPTING>"const" {
        return T_CONST;
}

<ST_IN_SCRIPTING>"return" {
        return T_RETURN;
}

<ST_IN_SCRIPTING>"try" {
        return T_TRY;
}

<ST_IN_SCRIPTING>"catch" {
        return T_CATCH;
}

<ST_IN_SCRIPTING>"throw" {
        return T_THROW;
}

<ST_IN_SCRIPTING>"if" {
        return T_IF;
}

<ST_IN_SCRIPTING>"elseif" {
        return T_ELSEIF;
}

<ST_IN_SCRIPTING>"endif" {
        return T_ENDIF;
}

<ST_IN_SCRIPTING>"else" {
        return T_ELSE;
}

<ST_IN_SCRIPTING>"while" {
        return T_WHILE;
}

<ST_IN_SCRIPTING>"endwhile" {
        return T_ENDWHILE;
}

<ST_IN_SCRIPTING>"do" {
        return T_DO;
}

<ST_IN_SCRIPTING>"for" {
        return T_FOR;
}

<ST_IN_SCRIPTING>"endfor" {
        return T_ENDFOR;
}

<ST_IN_SCRIPTING>"foreach" {
        return T_FOREACH;
}

<ST_IN_SCRIPTING>"endforeach" {
        return T_ENDFOREACH;
}

<ST_IN_SCRIPTING>"declare" {
        return T_DECLARE;
}

<ST_IN_SCRIPTING>"enddeclare" {
        return T_ENDDECLARE;
}

<ST_IN_SCRIPTING>"instanceof" {
        return T_INSTANCEOF;
}

<ST_IN_SCRIPTING>"as" {
        return T_AS;
}

<ST_IN_SCRIPTING>"switch" {
        return T_SWITCH;
}

<ST_IN_SCRIPTING>"endswitch" {
        return T_ENDSWITCH;
}

<ST_IN_SCRIPTING>"case" {
        return T_CASE;
}

<ST_IN_SCRIPTING>"default" {
        return T_DEFAULT;
}

<ST_IN_SCRIPTING>"break" {
        return T_BREAK;
}

<ST_IN_SCRIPTING>"continue" {
        return T_CONTINUE;
}

<ST_IN_SCRIPTING>"goto" {
        return T_GOTO;
}

<ST_IN_SCRIPTING>"echo" {
        return T_ECHO;
}

<ST_IN_SCRIPTING>"print" {
        return T_PRINT;
}

<ST_IN_SCRIPTING>"class" {
        *astNode = CREATE_AST_NODE("class");
        return T_CLASS;
}

<ST_IN_SCRIPTING>"interface" {
        *astNode = CREATE_AST_NODE("interface");
        return T_INTERFACE;
}

<ST_IN_SCRIPTING>"extends" {
        *astNode = CREATE_AST_NODE("extends");
        return T_EXTENDS;
}

<ST_IN_SCRIPTING>"implements" {
        *astNode = CREATE_AST_NODE("implements");
        return T_IMPLEMENTS;
}

<ST_IN_SCRIPTING>"->" {
        yy_push_state(ST_LOOKING_FOR_PROPERTY, yyscanner);
        return T_OBJECT_OPERATOR;
}

<ST_IN_SCRIPTING,ST_LOOKING_FOR_PROPERTY>{WHITESPACE}+ {
        // *astNode = CREATE_AST_NODE("T_WHITESPACE")->setData("text", yytext);
        return T_WHITESPACE;
}

<ST_LOOKING_FOR_PROPERTY>"->" {
        return T_OBJECT_OPERATOR;
}

<ST_LOOKING_FOR_PROPERTY>{LABEL} {
        yy_pop_state(yyscanner);
        *astNode = CREATE_AST_NODE("T_STRING")->setData("text", yytext);
        return T_STRING;
}
<ST_LOOKING_FOR_PROPERTY>.|[\n\r] {
        yyless(0);
        yy_pop_state(yyscanner);
}
<ST_IN_SCRIPTING>"::" {
        return T_DOUBLE_COLON;
}

<ST_IN_SCRIPTING>"\\" {
        return T_NS_SEPARATOR;
}

<ST_IN_SCRIPTING>"new" {
        return T_NEW;
}

<ST_IN_SCRIPTING>"clone" {
        return T_CLONE;
}

<ST_IN_SCRIPTING>"var" {
        return T_VAR;
}

<ST_IN_SCRIPTING>"("{TABS_AND_SPACES}("int"|"integer"){TABS_AND_SPACES}")" {
        return T_INT_CAST;
}

<ST_IN_SCRIPTING>"("{TABS_AND_SPACES}("real"|"double"|"float"){TABS_AND_SPACES}")" {
        return T_DOUBLE_CAST;
}

<ST_IN_SCRIPTING>"("{TABS_AND_SPACES}"string"{TABS_AND_SPACES}")" {
        return T_STRING_CAST;
}

<ST_IN_SCRIPTING>"("{TABS_AND_SPACES}"binary"{TABS_AND_SPACES}")" {
        return T_STRING_CAST;
}

<ST_IN_SCRIPTING>"("{TABS_AND_SPACES}"array"{TABS_AND_SPACES}")" {
        return T_ARRAY_CAST;
}

<ST_IN_SCRIPTING>"("{TABS_AND_SPACES}"object"{TABS_AND_SPACES}")" {
        return T_OBJECT_CAST;
}

<ST_IN_SCRIPTING>"("{TABS_AND_SPACES}("bool"|"boolean"){TABS_AND_SPACES}")" {
        return T_BOOL_CAST;
}

<ST_IN_SCRIPTING>"("{TABS_AND_SPACES}("unset"){TABS_AND_SPACES}")" {
        return T_UNSET_CAST;
}

<ST_IN_SCRIPTING>"eval" {
        return T_EVAL;
}

<ST_IN_SCRIPTING>"include" {
        return T_INCLUDE;
}

<ST_IN_SCRIPTING>"include_once" {
        return T_INCLUDE_ONCE;
}

<ST_IN_SCRIPTING>"require" {
        return T_REQUIRE;
}

<ST_IN_SCRIPTING>"require_once" {
        return T_REQUIRE_ONCE;
}

<ST_IN_SCRIPTING>"namespace" {
        return T_NAMESPACE;
}

<ST_IN_SCRIPTING>"use" {
        return T_USE;
}

<ST_IN_SCRIPTING>"global" {
        return T_GLOBAL;
}

<ST_IN_SCRIPTING>"isset" {
        return T_ISSET;
}

<ST_IN_SCRIPTING>"empty" {
        return T_EMPTY;
}

<ST_IN_SCRIPTING>"__halt_compiler" {
        return T_HALT_COMPILER;
}

<ST_IN_SCRIPTING>"static" {
        return T_STATIC;
}

<ST_IN_SCRIPTING>"abstract" {
        return T_ABSTRACT;
}

<ST_IN_SCRIPTING>"final" {
        return T_FINAL;
}

<ST_IN_SCRIPTING>"private" {
        return T_PRIVATE;
}

<ST_IN_SCRIPTING>"protected" {
        return T_PROTECTED;
}

<ST_IN_SCRIPTING>"public" {
        return T_PUBLIC;
}

<ST_IN_SCRIPTING>"=>" {
        return T_DOUBLE_ARROW;
}

<ST_IN_SCRIPTING>"list" {
        return T_LIST;
}

<ST_IN_SCRIPTING>"array" {
        return T_ARRAY;
}

<ST_IN_SCRIPTING>"++" {
        return T_INC;
}

<ST_IN_SCRIPTING>"--" {
        return T_DEC;
}

<ST_IN_SCRIPTING>"===" {
        return T_IS_IDENTICAL;
}

<ST_IN_SCRIPTING>"!==" {
        return T_IS_NOT_IDENTICAL;
}

<ST_IN_SCRIPTING>"==" {
        return T_IS_EQUAL;
}

<ST_IN_SCRIPTING>"!="|"<>" {
        return T_IS_NOT_EQUAL;
}

<ST_IN_SCRIPTING>"<=" {
        return T_IS_SMALLER_OR_EQUAL;
}

<ST_IN_SCRIPTING>">=" {
        return T_IS_GREATER_OR_EQUAL;
}

<ST_IN_SCRIPTING>"+=" {
        return T_PLUS_EQUAL;
}

<ST_IN_SCRIPTING>"-=" {
        return T_MINUS_EQUAL;
}

<ST_IN_SCRIPTING>"*=" {
        return T_MUL_EQUAL;
}

<ST_IN_SCRIPTING>"/=" {
        return T_DIV_EQUAL;
}

<ST_IN_SCRIPTING>".=" {
        return T_CONCAT_EQUAL;
}

<ST_IN_SCRIPTING>"%=" {
        return T_MOD_EQUAL;
}

<ST_IN_SCRIPTING>"<<=" {
        return T_SL_EQUAL;
}

<ST_IN_SCRIPTING>">>=" {
        return T_SR_EQUAL;
}

<ST_IN_SCRIPTING>"&=" {
        return T_AND_EQUAL;
}

<ST_IN_SCRIPTING>"|=" {
        return T_OR_EQUAL;
}

<ST_IN_SCRIPTING>"^=" {
        return T_XOR_EQUAL;
}

<ST_IN_SCRIPTING>"||" {
        return T_BOOLEAN_OR;
}

<ST_IN_SCRIPTING>"&&" {
        return T_BOOLEAN_AND;
}

<ST_IN_SCRIPTING>"OR" {
        return T_LOGICAL_OR;
}

<ST_IN_SCRIPTING>"AND" {
        return T_LOGICAL_AND;
}

<ST_IN_SCRIPTING>"XOR" {
        return T_LOGICAL_XOR;
}

<ST_IN_SCRIPTING>"<<" {
        return T_SL;
}

<ST_IN_SCRIPTING>">>" {
        return T_SR;
}

<ST_IN_SCRIPTING>{TOKENS} {
        return yytext[0];
}


<ST_IN_SCRIPTING>"{" {
        yy_push_state(ST_IN_SCRIPTING, yyscanner);
        return '{';
}


<ST_DOUBLE_QUOTES,ST_BACKQUOTE,ST_HEREDOC>"${" {
        yy_push_state(ST_LOOKING_FOR_VARNAME, yyscanner);
        return T_DOLLAR_OPEN_CURLY_BRACES;
}


<ST_IN_SCRIPTING>"}" {
        // was pushed in all occurences of '{'
        yy_pop_state(yyscanner);
        return '}';
}


<ST_LOOKING_FOR_VARNAME>{LABEL} {
        *astNode = CREATE_AST_NODE("T_STRING_VARNAME")->setData("text", yytext);
        yy_pop_state(yyscanner);
        yy_push_state(ST_IN_SCRIPTING, yyscanner);
        return T_STRING_VARNAME;
}


<ST_LOOKING_FOR_VARNAME>.|[\n\r] {
        yyless(0);
        yy_pop_state(yyscanner);
        yy_push_state(ST_IN_SCRIPTING, yyscanner);
}


<ST_IN_SCRIPTING>{LNUM}|{HNUM} {
        *astNode = CREATE_AST_NODE("T_LNUMBER")->setData("text", yytext);
        return T_LNUMBER;
}

<ST_VAR_OFFSET>[0]|([1-9][0-9]*) { /* Offset could be treated as a long */
        *astNode = CREATE_AST_NODE("T_NUM_STRING")->setData("text", yytext);
        return T_NUM_STRING;
}

<ST_VAR_OFFSET>{LNUM}|{HNUM} { /* Offset must be treated as a string */
        *astNode = CREATE_AST_NODE("T_NUM_STRING")->setData("text", yytext);
        return T_NUM_STRING;
}

<ST_IN_SCRIPTING>{DNUM}|{EXPONENT_DNUM} {
        *astNode = CREATE_AST_NODE("T_DNUMBER")->setData("text", yytext);
        return T_DNUMBER;
}

<ST_IN_SCRIPTING>"__CLASS__" {
        *astNode = CREATE_AST_NODE("__CLASS__");
        return T_CLASS_C;
}

<ST_IN_SCRIPTING>"__FUNCTION__" {
        *astNode = CREATE_AST_NODE("__FUNCTION__");
        return T_FUNC_C;
}

<ST_IN_SCRIPTING>"__METHOD__" {
        *astNode = CREATE_AST_NODE("__METHOD__");
        return T_METHOD_C;
}

<ST_IN_SCRIPTING>"__LINE__" {
        *astNode = CREATE_AST_NODE("__LINE__");
        return T_LINE;
}

<ST_IN_SCRIPTING>"__FILE__" {
        *astNode = CREATE_AST_NODE("__FILE__");
        return T_FILE;
}

<ST_IN_SCRIPTING>"__DIR__" {
        *astNode = CREATE_AST_NODE("__DIR__");
        return T_DIR;
}

<ST_IN_SCRIPTING>"__NAMESPACE__" {
        *astNode = CREATE_AST_NODE("__NAMESPACE__");
        return T_NS_C;
}

<INITIAL>"<script"{WHITESPACE}+"language"{WHITESPACE}*"="{WHITESPACE}*("php"|"\"php\""|"'php'"){WHITESPACE}*">" {
        BEGIN(ST_IN_SCRIPTING);
        return T_OPEN_TAG;
}


<INITIAL>"<%=" {
        // OPTION: IF ASP TAGS?
        BEGIN(ST_IN_SCRIPTING);
        return T_OPEN_TAG_WITH_ECHO;
}


<INITIAL>"<?=" {
        // OPTIONS: SHORT TAGS
        BEGIN(ST_IN_SCRIPTING);
        return T_OPEN_TAG_WITH_ECHO;

        /*} else {
                goto inline_char_handler;
        }*/
}


<INITIAL>"<%" {
        // OPTION: IF ASP TAGS?
                BEGIN(ST_IN_SCRIPTING);
                return T_OPEN_TAG;

}


<INITIAL>"<?php"([ \t]|{NEWLINE}) {
        BEGIN(ST_IN_SCRIPTING);
        return T_OPEN_TAG;
}

        /*
<INITIAL>"<?" {
        // OPTIONS: SHORT TAGS
                BEGIN(ST_IN_SCRIPTING);
                return T_OPEN_TAG;

}
    */

<INITIAL>.|[\n\r] {
inline_char_handler:
        // will eat at least one char to avoid endless wandering in this loop
        QString result = "";
        result += (char)yytext[0];
        char c = ionInput(yyscanner);
        while (c) {
            if ('<' == c) {
                switch (lookupChar(yyscanner)) {
                    case 0:
                        c = 0;
                        break;
                    case '?':
                            if (0) {
                                // allow shorttags?
                                ionUnput(c, yyscanner);
                                break;
                            }
                            {
                                if ( matchesRegexp(lookupChars(5, yyscanner), "^\\?php\\s$") ) {
                                    ionUnput(c, yyscanner);
                                    break;
                                }
                                result += c;
                                c = ionInput(yyscanner);
                                continue;
                            }
                        case '%':
                                if (0) {
                                //  allow asp tags?
                                    ionUnput(c, yyscanner);
                                        break;
                                }
                                result += c;
                                c = ionInput(yyscanner);
                                continue;
                        case 's':
                        case 'S':
                                /* Probably NOT an opening PHP <script> tag, so don't end the HTML chunk yet
                                 * If it is, the PHP <script> tag rule checks for any HTML scanned before it */
                                ionUnput(c, yyscanner);
                                break;
                        default:
                                result += c;
                                c = ionInput(yyscanner);
                                continue;
                }
                break;
            } else {
                result += c;
                c = ionInput(yyscanner);
            }
        }
        if (result.length()) {
            *astNode = CREATE_AST_NODE("T_INLINE_HTML")->setData("text", result);
        }
        return T_INLINE_HTML;
}

<ST_DOUBLE_QUOTES,ST_HEREDOC,ST_BACKQUOTE>"$"{LABEL}"->"[a-zA-Z_\x7f-\xff] {
            /* Make sure a label character follows "->", otherwise there is no property
             * and "->" will be taken literally
             */
        yyless(yyleng - 3);
        yy_push_state(ST_LOOKING_FOR_PROPERTY, yyscanner);
        *astNode = CREATE_AST_NODE("T_VARIABLE")->setData("text", yytext);
        return T_VARIABLE;
}

<ST_DOUBLE_QUOTES,ST_HEREDOC,ST_BACKQUOTE>"$"{LABEL}"[" {
        /* A [ always designates a variable offset, regardless of what follows
         */
        yyless(yyleng - 1);
        yy_push_state(ST_VAR_OFFSET, yyscanner);
        *astNode = CREATE_AST_NODE("T_VARIABLE")->setData("text", yytext);
        return T_VARIABLE;
}

<ST_IN_SCRIPTING,ST_DOUBLE_QUOTES,ST_HEREDOC,ST_BACKQUOTE,ST_VAR_OFFSET>"$"{LABEL} {
        *astNode = CREATE_AST_NODE("T_VARIABLE")->setData("text", yytext);
        return T_VARIABLE;
}

<ST_VAR_OFFSET>"]" {
        yy_pop_state(yyscanner);
        return ']';
}

<ST_VAR_OFFSET>{TOKENS}|[{}"`] {
        /* Only '[' can be valid, but returning other tokens will allow a more explicit parse error */
        return yytext[0];
}

<ST_VAR_OFFSET>[ \n\r\t\\'#] {
        /* Invalid rule to return a more explicit parse error with proper line number */
        yyless(0);
        yy_pop_state(yyscanner);
        return T_ENCAPSED_AND_WHITESPACE;
}

<ST_IN_SCRIPTING,ST_VAR_OFFSET>{LABEL} {
        *astNode = CREATE_AST_NODE("T_STRING")->setData("text", yytext);
        return T_STRING;
}


<ST_IN_SCRIPTING>("#"|"//") {

    bool stop = false;
    char c;
    while(!stop && ((c = ionInput(yyscanner)) != 0)) {
        switch (c) {
            case '\r':
                    c = ionInput(yyscanner);
                    if (c != '\n') {
                        ionUnput(c, yyscanner);
                    }
                    /* fall through */
            case '\n':
                    stop = true;
                    break;
            case '%':
                    /*if (!CG(asp_tags)) {
                            continue;
                    }*/
                    /* fall through */
            case '?':
                    {
                        if (lookupChar(yyscanner) == '>') {
                                ionUnput(c, yyscanner);
                                stop = true;
                                break;
                        }
                    }
                    /* fall through */
            default:
                    continue;
        }
    }

    *astNode = CREATE_AST_NODE("T_COMMENT")->setData("text", yytext);
    return T_COMMENT;
}

<ST_IN_SCRIPTING>"/*"|"/**"{WHITESPACE} {
        bool doc_com;

        if (yyleng > 2) {
                doc_com = true;
        } else {
                doc_com = false;
        }

        char c;

        while ( (c = ionInput(yyscanner)) ) {
            if (c == '*') {
                char nc = ionInput(yyscanner);
                if (nc == '/') {
                    break;
                } else {
                    ionUnput(nc, yyscanner);
                }
            }
        }

        if (doc_com) {
            *astNode = CREATE_AST_NODE("T_DOC_COMMENT")->setData("text", yytext);
            return T_DOC_COMMENT;
        }

        *astNode = CREATE_AST_NODE("T_COMMENT")->setData("text", yytext);
        return T_COMMENT;
}

<ST_IN_SCRIPTING>("?>"|"</script"{WHITESPACE}*">"){NEWLINE}? {
        BEGIN(INITIAL);
        return T_CLOSE_TAG;
}


<ST_IN_SCRIPTING>"%>"{NEWLINE}? {
        if (false /*CG(asp_tags)*/) {
                BEGIN(INITIAL);
                return T_CLOSE_TAG;
        } else {
                yyless(1);
                return yytext[0];
        }
}


<ST_IN_SCRIPTING>"'" {
        char c;
        QString result = "";
        while ( (c = ionInput(yyscanner)) )  {
            if (c == '\'') {
                break;
            } else {
                result += c;
                if (c == '\\') {
                    result += ionInput(yyscanner); // consume one more char
                }
            }
        }
        *astNode = CREATE_AST_NODE("T_CONSTANT_ENCAPSED_STRING")->setData("text", result);
        return T_CONSTANT_ENCAPSED_STRING;
}


<ST_IN_SCRIPTING>"\"" {
        char c;
        QString result;
        while ( (c = ionInput(yyscanner)) )  {
            switch (c) {
                case '"':
                    *astNode = CREATE_AST_NODE("T_CONSTANT_ENCAPSED_STRING")->setData("text", result);
                    return T_CONSTANT_ENCAPSED_STRING;
                case '\\':
                    result += c;
                    c = ionInput(yyscanner); // consume one more char
                    result += c;
                    continue;
                case '$':
                    {
                        if ( isStartOfLabel(yyscanner) ) {
                            ionUnput(c, yyscanner);
                            break;
                        }
                        result += c;
                        continue;
                    }
                case '{':
                    {
                        if ( lookupChar(yyscanner) == '$') {
                            ionUnput(c, yyscanner);
                            break;
                        }
                    }
                    /* fall through */
                default:
                    result += c;
                    continue;
            }
            break;
        }

        // rewind scanned data - it will be scanned properly in string scanner
        for (int i = result.length()-1; i>=0; i--) {
            ionUnput(result[i].toAscii(), yyscanner);
        }
        BEGIN(ST_DOUBLE_QUOTES);
        return '"';
}

<ST_IN_SCRIPTING>"<<<"{TABS_AND_SPACES}({LABEL}|([']{LABEL}['])|(["]{LABEL}["])){NEWLINE} {
        char *s = yytext + 3;
        while ((*s == ' ') || (*s == '\t')) {
            s++;
        }

        if (*s == '\'') {
            s++;
            BEGIN(ST_NOWDOC);
        } else {
            if (*s == '"') {
                s++;
            }
            BEGIN(ST_HEREDOC);
        }


        yyextra->currentHeredocLabel = "";
        int limit = 255;
        do {
            switch(*s) {
                case '"':
                    // fall through
                case '\'':
                    s++;
                    // fall through
                case '\r':
                    {
                        if (*(s+1) == '\n') {
                            s++;
                        }
                    }
                case '\n':
                    // fall through
                case '\t':
                    // fall through
                case ' ':
                    break;
                default:
                    yyextra->currentHeredocLabel += *s;
                    s++;
                    continue;
            }
            break;
        } while (--limit);
        return T_START_HEREDOC;
}

<ST_IN_SCRIPTING>[`] {
        BEGIN(ST_BACKQUOTE);
        return '`';
}

<ST_DOUBLE_QUOTES,ST_BACKQUOTE,ST_HEREDOC>"{$" {
        yy_push_state(ST_IN_SCRIPTING, yyscanner);
        yyless(1);
        return T_CURLY_OPEN;
}


<ST_DOUBLE_QUOTES>"\"" {
        BEGIN(ST_IN_SCRIPTING);
        return '"';
}

<ST_DOUBLE_QUOTES>[^"] {
        QString result;
        result += yytext[0];
        if (yytext[0] == '\\') {
            result += ionInput(yyscanner);
        }

        char c;
        while ( (c = ionInput(yyscanner)) ) {
            switch (c) {
                case '"':
                    ionUnput(c, yyscanner);
                    break;
                case '\\':
                    result += '\\';
                    result += ionInput(yyscanner); // consume one more char
                    continue;
                case '$':
                    {
                        if ( isStartOfLabel(yyscanner) ) {
                            ionUnput(c, yyscanner);
                            break;
                        }
                        result += c;
                        continue;
                    }
                case '{':
                    {
                        if ( (lookupChar(yyscanner) == '$') ) {
                            ionUnput(c, yyscanner);
                            break;
                        }
                    }
                    /* fall through */
                default:
                    result += c;
                    continue;
            }
            break;
        }

        *astNode = CREATE_AST_NODE("T_ENCAPSED_AND_WHITESPACE")->setData("text", result);
        return T_ENCAPSED_AND_WHITESPACE;
}

<ST_BACKQUOTE>` {
    BEGIN(ST_IN_SCRIPTING);
    return '`';
}

<ST_BACKQUOTE>[^`] {
            QString result;
            result += yytext[0];
            if (yytext[0] == '\\') {
                result += ionInput(yyscanner);
            }

            char c;
            while ( (c = ionInput(yyscanner)) ) {
                switch (c) {
                    case '`':
                        ionUnput(c, yyscanner);
                        break;
                    case '\\':
                        result += '\\';
                        result += ionInput(yyscanner); // consume one more char
                        continue;
                    case '$':
                        {
                            if ( isStartOfLabel(yyscanner) ) {
                                ionUnput(c, yyscanner);
                                break;
                            }
                            result += c;
                            continue;
                        }
                    case '{':
                        {
                            if ( lookupChar(yyscanner) == '$')  {
                                ionUnput(c, yyscanner);
                                break;
                            }
                        }
                        /* fall through */
                    default:
                        result += c;
                        continue;
                }
                break;
            }

            *astNode = CREATE_AST_NODE("T_ENCAPSED_AND_WHITESPACE")->setData("text", result);
            return T_ENCAPSED_AND_WHITESPACE;
}

<ST_HEREDOC>.|[\n\r] {
        int newline = 0;
        QString result = "";

        char c = yytext[0];
        while ( c ) {
                switch (c) {
                        case '\r':
                                {
                                    char nc = ionInput(yyscanner);
                                    if (nc != '\n') {
                                        ionUnput(nc, yyscanner);
                                    }
                                    if (scanForHereDocEnd(yyscanner)) {
                                        BEGIN(ST_IN_SCRIPTING);
                                        break;
                                    }
                                    result += c;
                                    if (nc == '\n') {
                                        result += nc;
                                    }
                                    c = ionInput(yyscanner);
                                }
                                continue;
                        case '\n':
                                if (scanForHereDocEnd(yyscanner)) {
                                    BEGIN(ST_IN_SCRIPTING);
                                    break;
                                }
                                result += c;
                                c = ionInput(yyscanner);
                                continue;
                        case '$':
                            {
                                if ( isStartOfLabel(yyscanner) ) {
                                    ionUnput(c, yyscanner);
                                    break;
                                }
                                result += c;
                                c = ionInput(yyscanner);
                                continue;
                            }
                        case '{':
                            {
                                if ( lookupChar(yyscanner) == '$') {
                                    ionUnput(c, yyscanner);
                                    break;
                                }
                            }
                            /* fall through */
                        default:
                            result += c;
                            c = ionInput(yyscanner);
                            continue;
                }
                break;
        }
        *astNode = CREATE_AST_NODE("T_ENCAPSED_AND_WHITESPACE")->setData("text", result);
        return T_ENCAPSED_AND_WHITESPACE;
}

<ST_NOWDOC>.|[\n\r] {
        int newline = 0;
        QString result = "";

        char c = yytext[0];
        while ( c ) {
                switch (c) {
                        case '\r':
                                {
                                    char nc = ionInput(yyscanner);
                                    if (nc != '\n') {
                                        ionUnput(nc, yyscanner);
                                    }
                                    if (scanForHereDocEnd(yyscanner)) {
                                        BEGIN(ST_IN_SCRIPTING);
                                        break;
                                    }
                                    result += c;
                                    if (nc == '\n') {
                                        result += nc;
                                    }
                                    c = ionInput(yyscanner);
                                }
                                continue;
                        case '\n':
                                if (scanForHereDocEnd(yyscanner)) {
                                    BEGIN(ST_IN_SCRIPTING);
                                    break;
                                }
                                result += c;
                                c = ionInput(yyscanner);
                                continue;
                        default:
                            result += c;
                            c = ionInput(yyscanner);
                            continue;
                }
                break;
        }
        *astNode = CREATE_AST_NODE("T_ENCAPSED_AND_WHITESPACE")->setData("text", result);
        return T_ENCAPSED_AND_WHITESPACE;
}

%%
#include <QRegExp>

char ionInput(yyscan_t yyscanner)
{
    struct yyguts_t * yyg = (struct yyguts_t*)yyscanner;
    char c = yyinput(yyscanner);
    switch (c) {
        case '\n':
            yyextra->__posLine++;
            yyextra->__posColHistory.append(yyextra->__posCol);
            yyextra->__posCol = 0;
            break;
        default:
            yyextra->__posCol++;
    }
    return c;
}

void ionUnput(char c, yyscan_t yyscanner)
{
    struct yyguts_t * yyg = (struct yyguts_t*)yyscanner;
    unput(c);
    switch (c) {
        case '\n':
            if (yyextra->__posLine > 0) {
                yyextra->__posLine--;
                if (yyextra->__posColHistory.size()) {
                    yyextra->__posCol = yyextra->__posColHistory.back();
                    yyextra->__posColHistory.pop_back();
                }
            }
            break;
        default:
            if (yyextra->__posCol > 0) {
                yyextra->__posCol--;
            }
    }
}

QString retrieveChars(int numberOfChars, yyscan_t yyscanner)
{
    struct yyguts_t * yyg = (struct yyguts_t*)yyscanner;
    QString result;
    char c;
    while (numberOfChars-- && (c = ionInput(yyscanner))) {
        result += c;
    }
    return result;

}
void putCharsBack(QString chars, yyscan_t yyscanner)
{
    struct yyguts_t * yyg = (struct yyguts_t*)yyscanner;
    int i = chars.length();
    while (i--) {
        ionUnput(chars[i].toAscii(), yyscanner);
    }
}
QString lookupChars(int numberOfChars, yyscan_t yyscanner)
{
    QString result = retrieveChars(numberOfChars, yyscanner);
    putCharsBack(result, yyscanner);
    return result;
}

char lookupChar(yyscan_t yyscanner)
{
    struct yyguts_t * yyg = (struct yyguts_t*)yyscanner;
    char c = ionInput(yyscanner);
    ionUnput(c, yyscanner);
    return c;
}
bool matchesRegexp(QString input, QString regex)
{
    QRegExp r(regex);
    return r.exactMatch(input);
}
bool isStartOfLabel(yyscan_t yyscanner)
{
    static QRegExp r("^[a-zA-Z\\{_]$");
    return r.exactMatch(lookupChars(1, yyscanner));
}
bool scanForHereDocEnd(yyscan_t yyscanner)
{
    struct yyguts_t * yyg = (struct yyguts_t*)yyscanner;
    QString nextText = retrieveChars(yyextra->currentHeredocLabel.length(), yyscanner);
    if (nextText == yyextra->currentHeredocLabel) {
        switch (lookupChar(yyscanner)) {
            case ';':
            case '\n':
                return true;
            case '\r':
                if ("\r\n" == lookupChars(2, yyscanner)) {
                    return true;
                }
        }
    }
    putCharsBack(nextText, yyscanner);
    return false;
}

namespace IonPhp {
        void phpParser::init_scanner()
        {
           yylex_init(&__scanner);
           yyset_extra(this, __scanner);
        }

        void phpParser::destroy_scanner()
        {
           yylex_destroy(__scanner);
        }

        void *phpParser::setBuf(const char* data)
        {
            YY_BUFFER_STATE b = _impl_ionPhp__scan_string(data, __scanner);
            b->yy_fill_buffer = 0;
            this->__col = this->__line = this->__posCol = this->__posLine = 0;
            return b;
        }
        void phpParser::delBuf(void *buf)
        {
            _impl_ionPhp__delete_buffer((yy_buffer_state *)buf, __scanner);
        }

}
