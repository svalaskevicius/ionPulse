%{


#include "phpParser/ionParserLib.h"
#include "phpparser.h"

#define YYERROR_VERBOSE
#define YYSTYPE pASTNode
#define ion_php_scanner context->__scanner


#define yyparse ion_php_parse
#define yylex   context->__lex
#define yyerror context->__error
#define yylval  ion_php_lval
#define yychar  ion_php_char
#define yydebug ion_php_debug
#define yynerrs ion_php_nerrs

%}

%pure_parser
%glr-parser
%expect 2
%parse-param { IonPhp::phpParser* context }
%lex-param   { void* ion_php_scanner  }

%left T_INCLUDE T_INCLUDE_ONCE T_EVAL T_REQUIRE T_REQUIRE_ONCE
%left ','
%left T_LOGICAL_OR
%left T_LOGICAL_XOR
%left T_LOGICAL_AND
%right T_PRINT
%left '=' T_PLUS_EQUAL T_MINUS_EQUAL T_MUL_EQUAL T_DIV_EQUAL T_CONCAT_EQUAL T_MOD_EQUAL T_AND_EQUAL T_OR_EQUAL T_XOR_EQUAL T_SL_EQUAL T_SR_EQUAL
%left '?' ':'
%left T_BOOLEAN_OR
%left T_BOOLEAN_AND
%left '|'
%left '^'
%left '&'
%nonassoc T_IS_EQUAL T_IS_NOT_EQUAL T_IS_IDENTICAL T_IS_NOT_IDENTICAL
%nonassoc '<' T_IS_SMALLER_OR_EQUAL '>' T_IS_GREATER_OR_EQUAL
%left T_SL T_SR
%left '+' '-' '.'
%left '*' '/' '%'
%right '!'
%nonassoc T_INSTANCEOF
%right '~' T_INC T_DEC T_INT_CAST T_DOUBLE_CAST T_STRING_CAST T_ARRAY_CAST T_OBJECT_CAST T_BOOL_CAST T_UNSET_CAST '@'
%right '['
%nonassoc T_NEW T_CLONE
%token T_EXIT
%token T_IF
%left T_ELSEIF
%left T_ELSE
%left T_ENDIF
%token T_LNUMBER
%token T_DNUMBER
%token T_STRING
%token T_STRING_VARNAME
%token T_VARIABLE
%token T_NUM_STRING
%token T_INLINE_HTML
%token T_CHARACTER
%token T_BAD_CHARACTER
%token T_ENCAPSED_AND_WHITESPACE
%token T_CONSTANT_ENCAPSED_STRING
%token T_ECHO
%token T_DO
%token T_WHILE
%token T_ENDWHILE
%token T_FOR
%token T_ENDFOR
%token T_FOREACH
%token T_ENDFOREACH
%token T_DECLARE
%token T_ENDDECLARE
%token T_AS
%token T_SWITCH
%token T_ENDSWITCH
%token T_CASE
%token T_DEFAULT
%token T_BREAK
%token T_CONTINUE
%token T_GOTO
%token T_FUNCTION
%token T_CONST
%token T_RETURN
%token T_TRY
%token T_CATCH
%token T_THROW
%token T_USE
%token T_GLOBAL
%right T_STATIC T_ABSTRACT T_FINAL T_PRIVATE T_PROTECTED T_PUBLIC
%token T_VAR
%token T_UNSET
%token T_ISSET
%token T_EMPTY
%token T_HALT_COMPILER
%token T_CLASS
%token T_INTERFACE
%token T_EXTENDS
%token T_IMPLEMENTS
%token T_OBJECT_OPERATOR
%token T_DOUBLE_ARROW
%token T_LIST
%token T_ARRAY
%token T_CLASS_C
%token T_METHOD_C
%token T_FUNC_C
%token T_LINE
%token T_FILE
%token T_COMMENT
%token T_DOC_COMMENT
%token T_OPEN_TAG
%token T_OPEN_TAG_WITH_ECHO
%token T_CLOSE_TAG
%token T_WHITESPACE
%token T_START_HEREDOC
%token T_DOLLAR_OPEN_CURLY_BRACES
%token T_CURLY_OPEN
%token T_DOUBLE_COLON
%token T_NAMESPACE
%token T_NS_C
%token T_DIR
%token T_NS_SEPARATOR

%% /* Rules */

%start start;

start: top_statement_list {context->__result = $1;};

top_statement_list:
                top_statement_list top_statement {if ($2) { $1->addChild($2);}}
        |    /* empty */ {$$ = ASTNode::create("top_statement_list");}
;

namespace_name:
                T_STRING { $$ = ASTNode::create("namespace_name")->addChild($1); }
        |    namespace_name T_NS_SEPARATOR T_STRING { $1->addChild($3); }
;

top_statement:
                statement { $$ = $1; }
        |    function_declaration_statement { $$ = $1; }
        |    class_declaration_statement { $$ = $1; }
        |    T_HALT_COMPILER '(' ')' ';'      { $$ = $1; YYACCEPT; }
        |    T_NAMESPACE namespace_name ';'   { $$ = ASTNode::create("namespace")->addChild($2); }
        |    T_NAMESPACE namespace_name '{'
                top_statement_list '}'        { $$ = ASTNode::create("namespace")->addChild($2)->addChild($4); }
        |    T_NAMESPACE '{'
                top_statement_list '}'        { $$ = ASTNode::create("namespace")->addChild($3); }
        |    T_USE use_declarations ';'       { $$ = ASTNode::create("use")->addChild($2); }
        |    constant_declaration ';'  { $$ = $1; }
;

use_declarations:
                use_declarations ',' use_declaration  { $$ = $1; }
        |    use_declaration { $$ = $1; }
;

use_declaration:
                namespace_name { $$ = $1; }
        |    namespace_name T_AS T_STRING                { $$ = ASTNode::create("as")->addChild($1)->addChild($3); }
        |    T_NS_SEPARATOR namespace_name               { $$ = ASTNode::create("namespaceroot")->addChild($2); }
        |    T_NS_SEPARATOR namespace_name T_AS T_STRING { $$ = ASTNode::create("as")->addChild(ASTNode::create("namespaceroot")->addChild($2))->addChild($4); }
;

constant_declaration:
                constant_declaration ',' T_STRING '=' static_scalar { $$ = $1; }
        |    T_CONST T_STRING '=' static_scalar { $$ = $1; }
;

inner_statement_list:
                inner_statement_list  inner_statement  { $1->addChild($2); }
        |    /* empty */ { $$ = ASTNode::create("inner_statement_list"); }
;


inner_statement:
                statement { $$ = $1; }
        |    function_declaration_statement { $$ = $1; }
        |    class_declaration_statement { $$ = $1; }
;


statement:
                unticked_statement { $$ = $1; }
        |    T_STRING ':' { $$ = $1; }
;

unticked_statement:
                '{' inner_statement_list '}'           {$$=$2;}
        |    T_IF '(' expr ')'  statement  elseif_list else_single { $$ = $1; }
        |    T_IF '(' expr ')' ':'  inner_statement_list  new_elseif_list new_else_single T_ENDIF ';' { $$ = $1; }
        |    T_WHILE '('  expr  ')' while_statement { $$ = $1; }
        |    T_DO  statement T_WHILE '(' expr ')' ';' { $$ = $1; }
        |    T_FOR
                        '('
                                for_expr
                        ';'
                                for_expr
                        ';'
                                for_expr
                        ')'
                        for_statement
                { $$ = $1; }
        |    T_SWITCH '(' expr ')'     switch_case_list { $$ = $1; }
        |    T_BREAK ';' { $$ = $1; }
        |    T_BREAK expr ';' { $$ = $1; }
        |    T_CONTINUE ';' { $$ = $1; }
        |    T_CONTINUE expr ';' { $$ = $1; }
        |    T_RETURN ';' { $$ = $1; }
        |    T_RETURN expr_without_variable ';' { $$ = $1; }
        |    T_RETURN variable ';'          { $$ = ASTNode::create("return")->addChild($2);}
        |    T_GLOBAL global_var_list ';' { $$ = $1; }
        |    T_STATIC static_var_list ';' { $$ = $1; }
        |    T_ECHO echo_expr_list ';'      { $$ = ASTNode::create("echo")->addChild($2);}
        |    T_INLINE_HTML { $$ = $1; }
        |    expr ';' { $$ = $1; }
        |    T_UNSET '(' unset_variables ')' ';' { $$ = $1; }
        |    T_FOREACH '(' variable T_AS

                foreach_variable foreach_optional_arg ')'
                foreach_statement
            { $$ = $1; }
        |    T_FOREACH '(' expr_without_variable T_AS

                variable foreach_optional_arg ')'
                foreach_statement
             { $$ = $1; }
        |    T_DECLARE  '(' declare_list ')' declare_statement { $$ = $1; }
        |    ';'    	/* empty statement */ { $$ = $1; }
        |    T_TRY  '{' inner_statement_list '}'
                T_CATCH '('
                fully_qualified_class_name
                T_VARIABLE ')'
                '{' inner_statement_list '}'
                additional_catches
             { $$ = $1; }
        |    T_THROW expr ';' { $$ = $1; }
        |    T_GOTO T_STRING ';' { $$ = $1; }
;


additional_catches:
                non_empty_additional_catches { $$ = $1; }
        |    /* empty */ {}
;

non_empty_additional_catches:
                additional_catch { $$ = $1; }
        |    non_empty_additional_catches additional_catch { $$ = $2; }
;


additional_catch:
        T_CATCH '(' fully_qualified_class_name  T_VARIABLE ')'  '{' inner_statement_list '}' { $$ = $1; }
;


unset_variables:
                unset_variable { $$ = $1; }
        |    unset_variables ',' unset_variable { $$ = $1; }
;

unset_variable:
                variable { $$ = $1; }
;

function_declaration_statement:
                T_FUNCTION is_reference T_STRING
                        '(' parameter_list ')' '{' inner_statement_list '}'
                {
                    $$ = ASTNode::create("function_declaration")
                        ->addChild($2)
                        ->addChild($3)
                        ->addChild($5)
                        ->addChild($8)
                    ;
                }
;

class_declaration_statement:
                class_entry_type T_STRING extends_from

                        implements_list
                        '{'
                                class_statement_list
                        '}'
                {
                   $$ = ASTNode::create("class_declaration")
                       ->addChild($1)
                       ->addChild($2)
                       ->addChild($3)
                       ->addChild($4)
                       ->addChild($6)
                   ;
                }
        |    interface_entry T_STRING

                        interface_extends_list
                        '{'
                                class_statement_list
                        '}'
                {
                   $$ = ASTNode::create("interface_declaration")
                       ->addChild($1)
                       ->addChild($2)
                       ->addChild($3)
                       ->addChild($5)
                   ;
                }
;


is_reference:
                /* empty */ { $$ = ASTNode::create("is_reference")->setData("is_reference", "0"); }
        |    '&'            { $$ = ASTNode::create("is_reference")->setData("is_reference", "1"); }
;


class_entry_type:
                T_CLASS { $$ = $1; }
        |    T_ABSTRACT T_CLASS { $$ = $2; $$->setData("type", "abstract"); }
        |    T_FINAL T_CLASS    { $$ = $2; $$->setData("type", "final"); }
;

extends_from:
                /* empty */ { $$ = ASTNode::create("extends"); }
        |    T_EXTENDS fully_qualified_class_name { $$ = ASTNode::create("extends")->addChild($2); }
;

interface_entry:
        T_INTERFACE { $$ = $1; }
;

interface_extends_list:
                /* empty */ { $$ = ASTNode::create("extends"); }
        |    T_EXTENDS interface_list { $$ = ASTNode::create("extends")->addChild($2); }
;

implements_list:
                /* empty */  { $$ = ASTNode::create("implements"); }
        |    T_IMPLEMENTS interface_list { $$ = ASTNode::create("implements")->addChild($2); }
;

interface_list:
                fully_qualified_class_name { $$ = ASTNode::create("interfaceList")->addChild($1); }
        |    interface_list ',' fully_qualified_class_name { $1->addChild($3); }
;

foreach_optional_arg:
                /* empty */ {}
        |    T_DOUBLE_ARROW foreach_variable    { $$ = $2; }
;


foreach_variable:
                variable { $$ = $1; }
        |    '&' variable { $$ = $1; }
;

for_statement:
                statement { $$ = $1; }
        |    ':' inner_statement_list T_ENDFOR ';' { $$ = $1; }
;


foreach_statement:
                statement { $$ = $1; }
        |    ':' inner_statement_list T_ENDFOREACH ';' { $$ = $1; }
;


declare_statement:
                statement { $$ = $1; }
        |    ':' inner_statement_list T_ENDDECLARE ';' { $$ = $1; }
;


declare_list:
                T_STRING '=' static_scalar { $$ = $1; }
        |    declare_list ',' T_STRING '=' static_scalar { $$ = $1; }
;


switch_case_list:
                '{' case_list '}'    	    	    { $$ = $2; }
        |    '{' ';' case_list '}'    	    	{ $$ = $3; }
        |    ':' case_list T_ENDSWITCH ';'    	{ $$ = $2; }
        |    ':' ';' case_list T_ENDSWITCH ';'    { $$ = $3; }
;


case_list:
                /* empty */ {}
        |    case_list T_CASE expr case_separator  inner_statement_list { $$ = $1; }
        |    case_list T_DEFAULT case_separator  inner_statement_list { $$ = $1; }
;


case_separator:
                ':' { $$ = $1; }
        |    ';' { $$ = $1; }
;


while_statement:
                statement { $$ = $1; }
        |    ':' inner_statement_list T_ENDWHILE ';' { $$ = $1; }
;



elseif_list:
                /* empty */ { }
        |    elseif_list T_ELSEIF '(' expr ')'  statement { $$ = $1; }
;


new_elseif_list:
                /* empty */ {}
        |    new_elseif_list T_ELSEIF '(' expr ')' ':'  inner_statement_list { $$ = $1; }
;


else_single:
                /* empty */ {}
        |    T_ELSE statement { $$ = $1; }
;


new_else_single:
                /* empty */ {}
        |    T_ELSE ':' inner_statement_list { $$ = $1; }
;


parameter_list:
                non_empty_parameter_list { $$ = $1; }
        |    /* empty */ { $$ = ASTNode::create("parameter_list"); }
;


non_empty_parameter_list:
             optional_class_type T_VARIABLE
             {
                 $$ = ASTNode::create("parameter_list")
                     ->addChild(
                         ASTNode::create("parameter")
                             ->addChild($1)
                             ->addChild($2)
                     )
                 ;
             }
        |    optional_class_type '&' T_VARIABLE
             {
                  $$ = ASTNode::create("parameter_list")
                      ->addChild(
                          ASTNode::create("parameter")
                              ->addChild($1)
                              ->addChild($3)
                              ->setData("is_reference", "1")
                      )
                  ;
             }
        |    optional_class_type '&' T_VARIABLE '=' static_scalar
            {
                 $$ = ASTNode::create("parameter_list")
                     ->addChild(
                         ASTNode::create("parameter")
                             ->addChild($1)
                             ->addChild($3)
                             ->addChild($5)
                             ->setData("is_reference", "1")
                     )
                 ;
            }
        |    optional_class_type T_VARIABLE '=' static_scalar
            {
                 $$ = ASTNode::create("parameter_list")
                     ->addChild(
                         ASTNode::create("parameter")
                             ->addChild($1)
                             ->addChild($2)
                             ->addChild($4)
                     )
                 ;
            }
        |    non_empty_parameter_list ',' optional_class_type T_VARIABLE
            {
                 $1->addChild(
                     ASTNode::create("parameter")
                         ->addChild($3)
                         ->addChild($4)
                 );
            }
        |    non_empty_parameter_list ',' optional_class_type '&' T_VARIABLE
            {
                 $1->addChild(
                     ASTNode::create("parameter")
                         ->addChild($3)
                         ->addChild($4)
                         ->setData("is_reference", "1")
                 );
            }
        |    non_empty_parameter_list ',' optional_class_type '&' T_VARIABLE     '=' static_scalar
            {
                 $1->addChild(
                     ASTNode::create("parameter")
                         ->addChild($3)
                         ->addChild($5)
                         ->addChild($7)
                         ->setData("is_reference", "1")
                 );
            }
        |    non_empty_parameter_list ',' optional_class_type T_VARIABLE '=' static_scalar
            {
                 $1->addChild(
                     ASTNode::create("parameter")
                         ->addChild($3)
                         ->addChild($4)
                         ->addChild($6)
                 );
            }
;


optional_class_type:
                /* empty */ { $$ = ASTNode::create("__ANY_CLASS_TYPE__"); }
        |    fully_qualified_class_name { $$ = $1; }
        |    T_ARRAY { $$ = ASTNode::create("T_ARRAY"); }
;


function_call_parameter_list:
                non_empty_function_call_parameter_list { $$ = $1; }
        |    /* empty */ {$$=ASTNode::create("function_call_parameter_list");}
;


non_empty_function_call_parameter_list:
                expr_without_variable {$$=ASTNode::create("function_call_parameter_list")->addChild($1);}
        |    variable               {$$=ASTNode::create("function_call_parameter_list")->addChild($1);}
        |    '&' w_variable         {$$=ASTNode::create("function_call_parameter_list")->addChild($2);$2->setData("reference", "1");}
        |    non_empty_function_call_parameter_list ',' expr_without_variable {$1->addChild($3);}
        |    non_empty_function_call_parameter_list ',' variable              {$1->addChild($3);}
        |    non_empty_function_call_parameter_list ',' '&' w_variable        {$1->addChild($4);$4->setData("reference", "1");}
;

global_var_list:
                global_var_list ',' global_var { $$ = $1; }
        |    global_var { $$ = $1; }
;


global_var:
                T_VARIABLE    	    { $$ = $1; }
        |    '$' r_variable    	{ $$ = $2; }
        |    '$' '{' expr '}'    { $$ = $3; }
;


static_var_list:
                static_var_list ',' T_VARIABLE { $$ = $1; }
        |    static_var_list ',' T_VARIABLE '=' static_scalar { $$ = $1; }
        |    T_VARIABLE { $$ = $1; }
        |    T_VARIABLE '=' static_scalar { $$ = $1; }

;


class_statement_list:
                class_statement_list class_statement { $1->addChild($2); }
        |    /* empty */  {$$=ASTNode::create("class_statement_list");}
;


class_statement:
                variable_modifiers  class_variable_declaration ';' { $$ = $1; }
        |    class_constant_declaration ';' { $$ = $1; }
        |    method_modifiers T_FUNCTION is_reference T_STRING  '('
                        parameter_list ')' method_body { $$ = $1; }
;


method_body:
                ';' /* abstract method */ { $$ = $1; }
        |    '{' inner_statement_list '}' { $$ = $1; }
;

variable_modifiers:
                non_empty_member_modifiers { $$ = $1; }
        |    T_VAR { $$ = $1; }
;

method_modifiers:
                /* empty */ {}
        |    non_empty_member_modifiers { $$ = $1; }
;

non_empty_member_modifiers:
                member_modifier { $$ = $1; }
        |    non_empty_member_modifiers member_modifier { $$ = $1; }
;

member_modifier:
                T_PUBLIC { $$ = $1; }
        |    T_PROTECTED { $$ = $1; }
        |    T_PRIVATE { $$ = $1; }
        |    T_STATIC { $$ = $1; }
        |    T_ABSTRACT { $$ = $1; }
        |    T_FINAL { $$ = $1; }
;

class_variable_declaration:
                class_variable_declaration ',' T_VARIABLE { $$ = $1; }
        |    class_variable_declaration ',' T_VARIABLE '=' static_scalar { $$ = $1; }
        |    T_VARIABLE { $$ = $1; }
        |    T_VARIABLE '=' static_scalar { $$ = $1; }
;

class_constant_declaration:
                class_constant_declaration ',' T_STRING '=' static_scalar { $$ = $1; }
        |    T_CONST T_STRING '=' static_scalar { $$ = $1; }
;

echo_expr_list:
                echo_expr_list ',' expr {$1->addChild($2);}
        |    expr {$$ = ASTNode::create("echo_expr_list")->addChild($1);}
;


for_expr:
                /* empty */
        |    non_empty_for_expr { $$ = $1; }
;

non_empty_for_expr:
                non_empty_for_expr ','     expr { $$ = $1; }
        |    expr { $$ = $1; }
;

expr_without_variable:
            T_LIST '('  assignment_list ')' '=' expr
            {
                $$=ASTNode::create("assignment")->addChild($3)->addChild($6);
            }
        |    variable '=' expr {$$=ASTNode::create("assignment")->addChild($1)->addChild($3);}
        |    variable '=' '&' variable {$$=ASTNode::create("assignment")->addChild($1)->addChild($4)->setData("is_reference", "1");}
        |    variable '=' '&' T_NEW class_name_reference    { $$ = ASTNode::create("assignment")->addChild($1)->addChild(ASTNode::create("T_NEW")->addChild($5))->setData("is_reference", "1");}
        |    T_NEW class_name_reference  ctor_arguments     { $$ = ASTNode::create("T_NEW")->addChild($2)->addChild($3);}
        |    T_CLONE expr {$$=ASTNode::create("clone")->addChild($2);}
        |    variable T_PLUS_EQUAL expr    {$$=ASTNode::create("T_PLUS_EQUAL")->addChild($1)->addChild($3);}
        |    variable T_MINUS_EQUAL expr   {$$=ASTNode::create("T_MINUS_EQUAL")->addChild($1)->addChild($3);}
        |    variable T_MUL_EQUAL expr     {$$=ASTNode::create("T_MUL_EQUAL")->addChild($1)->addChild($3);}
        |    variable T_DIV_EQUAL expr     {$$=ASTNode::create("T_DIV_EQUAL")->addChild($1)->addChild($3);}
        |    variable T_CONCAT_EQUAL expr  {$$=ASTNode::create("T_CONCAT_EQUAL")->addChild($1)->addChild($3);}
        |    variable T_MOD_EQUAL expr     {$$=ASTNode::create("T_MOD_EQUAL")->addChild($1)->addChild($3);}
        |    variable T_AND_EQUAL expr     {$$=ASTNode::create("T_AND_EQUAL")->addChild($1)->addChild($3);}
        |    variable T_OR_EQUAL expr      {$$=ASTNode::create("T_OR_EQUAL")->addChild($1)->addChild($3);}
        |    variable T_XOR_EQUAL expr     {$$=ASTNode::create("T_XOR_EQUAL")->addChild($1)->addChild($3);}
        |    variable T_SL_EQUAL expr      {$$=ASTNode::create("T_SL_EQUAL")->addChild($1)->addChild($3);}
        |    variable T_SR_EQUAL expr      {$$=ASTNode::create("T_SR_EQUAL")->addChild($1)->addChild($3);}
        |    rw_variable T_INC             {$$=ASTNode::create("POSTINC")->addChild($1);}
        |    T_INC rw_variable             {$$=ASTNode::create("PREINC")->addChild($2);}
        |    rw_variable T_DEC             {$$=ASTNode::create("POSTDEC")->addChild($1);}
        |    T_DEC rw_variable             {$$=ASTNode::create("PREDEC")->addChild($2);}
        |    expr T_BOOLEAN_OR  expr       {$$=ASTNode::create("T_BOOLEAN_OR")->addChild($1)->addChild($3);}
        |    expr T_BOOLEAN_AND  expr      {$$=ASTNode::create("T_BOOLEAN_AND")->addChild($1)->addChild($3);}
        |    expr T_LOGICAL_OR  expr       {$$=ASTNode::create("T_LOGICAL_OR")->addChild($1)->addChild($3);}
        |    expr T_LOGICAL_AND  expr      {$$=ASTNode::create("T_LOGICAL_AND")->addChild($1)->addChild($3);}
        |    expr T_LOGICAL_XOR expr       {$$=ASTNode::create("T_LOGICAL_XOR")->addChild($1)->addChild($3);}
        |    expr '|' expr                 {$$=ASTNode::create("T_BINARY_OR")->addChild($1)->addChild($3);}
        |    expr '&' expr                 {$$=ASTNode::create("T_BINARY_AND")->addChild($1)->addChild($3);}
        |    expr '^' expr                 {$$=ASTNode::create("T_BINARY_XOR")->addChild($1)->addChild($3);}
        |    expr '.' expr                 {$$=ASTNode::create("T_CONCAT")->addChild($1)->addChild($3);}
        |    expr '+' expr                 {$$=ASTNode::create("T_PLUS")->addChild($1)->addChild($3);}
        |    expr '-' expr                 {$$=ASTNode::create("T_MINUS")->addChild($1)->addChild($3);}
        |    expr '*' expr                 {$$=ASTNode::create("T_MUL")->addChild($1)->addChild($3);}
        |    expr '/' expr                 {$$=ASTNode::create("T_DIV")->addChild($1)->addChild($3);}
        |    expr '%' expr                 {$$=ASTNode::create("T_MOD")->addChild($1)->addChild($3);}
        |    expr T_SL expr                {$$=ASTNode::create("T_SHIFT_LEFT")->addChild($1)->addChild($3);}
        |    expr T_SR expr                {$$=ASTNode::create("T_SHIFT_RIGHT")->addChild($1)->addChild($3);}
        |    '+' expr %prec T_INC { $$ = $1; }
        |    '-' expr %prec T_INC { $$ = $1; }
        |    '!' expr                      {$$=ASTNode::create("T_NEGATE")->addChild($2);}
        |    '~' expr                      {$$=ASTNode::create("T_INVERSE")->addChild($2);}
        |    expr T_IS_IDENTICAL expr      {$$=ASTNode::create("T_IS_IDENTICAL")->addChild($1)->addChild($3);}
        |    expr T_IS_NOT_IDENTICAL expr  {$$=ASTNode::create("T_IS_NOT_IDENTICAL")->addChild($1)->addChild($3);}
        |    expr T_IS_EQUAL expr          {$$=ASTNode::create("T_IS_EQUAL")->addChild($1)->addChild($3);}
        |    expr T_IS_NOT_EQUAL expr      {$$=ASTNode::create("T_IS_NOT_EQUAL")->addChild($1)->addChild($3);}
        |    expr '<' expr                 {$$=ASTNode::create("T_LESSTHAN")->addChild($1)->addChild($3);}
        |    expr T_IS_SMALLER_OR_EQUAL expr  {$$=ASTNode::create("T_LESSTHAN_EQ")->addChild($1)->addChild($3);}
        |    expr '>' expr                 {$$=ASTNode::create("T_GREATERTHAN")->addChild($1)->addChild($3);}
        |    expr T_IS_GREATER_OR_EQUAL expr  {$$=ASTNode::create("T_GREATERTHAN_EQ")->addChild($1)->addChild($3);}
        |    expr T_INSTANCEOF class_name_reference  {$$=ASTNode::create("T_INSTANCEOF")->addChild($1)->addChild($3);}
        |    '(' expr ')'                  { $$ = $2; }
        |    expr '?'
                expr ':'
                expr                       {$$=ASTNode::create("TERNARYOP")->addChild($1)->addChild($3)->addChild($5);}
        |    expr '?' ':'
                expr                       {$$=ASTNode::create("TERNARYOP")->addChild($1)->addChild(ASTNode::create("VOID"))->addChild($4);}
        |    internal_functions
        |    T_INT_CAST expr               {$$=ASTNode::create("T_INT_CAST")->addChild($2);}
        |    T_DOUBLE_CAST expr            {$$=ASTNode::create("T_DOUBLE_CAST")->addChild($2);}
        |    T_STRING_CAST expr            {$$=ASTNode::create("T_STRING_CAST")->addChild($2);}
        |    T_ARRAY_CAST expr             {$$=ASTNode::create("T_ARRAY_CAST")->addChild($2);}
        |    T_OBJECT_CAST expr            {$$=ASTNode::create("T_OBJECT_CAST")->addChild($2);}
        |    T_BOOL_CAST expr              {$$=ASTNode::create("T_BOOL_CAST")->addChild($2);}
        |    T_UNSET_CAST expr             {$$=ASTNode::create("T_UNSET_CAST")->addChild($2);}
        |    T_EXIT exit_expr              {$$=ASTNode::create("T_EXIT")->addChild($2);}
        |    '@'  expr                     {$$=ASTNode::create("SILENCE")->addChild($2);}
        |    scalar { $$ = $1; }
        |    T_ARRAY '(' array_pair_list ')' { $$ = ASTNode::create("T_ARRAY")->addChild($3); }
        |    '`' backticks_expr '`'        { $$ = ASTNode::create("BACKTICKS")->addChild($2); }
        |    T_PRINT expr                  {$$=ASTNode::create("T_PRINT")->addChild($2);}
        |    T_FUNCTION is_reference '('
                        parameter_list ')' lexical_vars '{' inner_statement_list '}'
             {
                $$=ASTNode::create("LAMBDA_FUNCTION")
                    ->addChild($2)
                    ->addChild($4)
                    ->addChild($6)
                    ->addChild($8)
                ;
             }
;


lexical_vars:
                /* empty */                 { $$ = ASTNode::create("T_USE"); }
        |    T_USE '(' lexical_var_list ')' { $$ = ASTNode::create("T_USE")->addChild($3); }
;

lexical_var_list:
                lexical_var_list ',' T_VARIABLE   { $1->addChild($3); }
        |    lexical_var_list ',' '&' T_VARIABLE  { $1->addChild($4); $4->setData("is_reference", "1");}
        |    T_VARIABLE                     { $$ = ASTNode::create("lexical_var_list")->addChild($1); }
        |    '&' T_VARIABLE                 { $$ = ASTNode::create("lexical_var_list")->addChild($2); $2->setData("is_reference", "1"); }
;

function_call:
                namespace_name '('
                                function_call_parameter_list
                                ')'
               {$$=ASTNode::create("function_call")->addChild($1)->addChild($3);}
        |    T_NAMESPACE T_NS_SEPARATOR namespace_name '('
                                function_call_parameter_list
                                ')'
                { $$ = $1; }
        |    T_NS_SEPARATOR namespace_name '('
                                function_call_parameter_list
                                ')'
                 { $$ = $1; }
        |    class_name T_DOUBLE_COLON T_STRING '('
                        function_call_parameter_list
                        ')'
                 { $$ = $1; }
        |    class_name T_DOUBLE_COLON variable_without_objects '('
                        function_call_parameter_list
                        ')'
                 { $$ = $1; }
        |    variable_class_name T_DOUBLE_COLON T_STRING '('
                        function_call_parameter_list
                        ')'
                 { $$ = $1; }
        |    variable_class_name T_DOUBLE_COLON variable_without_objects '('
                        function_call_parameter_list
                        ')'
                 { $$ = $1; }
        |    variable_without_objects  '('
                        function_call_parameter_list ')'
                 { $$ = $1; }

;

class_name:
                T_STATIC { $$ = $1; }
        |    namespace_name { $$ = $1; }
        |    T_NAMESPACE T_NS_SEPARATOR namespace_name { $$ = $1; }
        |    T_NS_SEPARATOR namespace_name { $$ = $1; }
;

fully_qualified_class_name:
                namespace_name { $$ = $1; }
        |    T_NAMESPACE T_NS_SEPARATOR namespace_name { $$ = $1; }
        |    T_NS_SEPARATOR namespace_name { $$ = $1; }
;



class_name_reference:
                class_name { $$ = $1; }
        |    dynamic_class_name_reference { $$ = $1; }
;


dynamic_class_name_reference:
                base_variable T_OBJECT_OPERATOR
                        object_property  dynamic_class_name_variable_properties { $$ = $1; }
        |    base_variable { $$ = $1; }
;


dynamic_class_name_variable_properties:
                dynamic_class_name_variable_properties dynamic_class_name_variable_property { $$ = $1; }
        |    /* empty */
;


dynamic_class_name_variable_property:
                T_OBJECT_OPERATOR object_property { $$ = $1; }
;

exit_expr:
                /* empty */
        |    '(' ')'         { $$ = ASTNode::create("VOID"); }
        |    '(' expr ')'    { $$ = $2; }
;

backticks_expr:
                /* empty */
        |    T_ENCAPSED_AND_WHITESPACE    { $$ = $1; }
        |    encaps_list    { $$ = $1; }
;


ctor_arguments:
                /* empty */
        |    '(' function_call_parameter_list ')'    { $$ = $2; }
;


common_scalar:
                T_LNUMBER { $$ = $1; }
        |    T_DNUMBER { $$ = $1; }
        |    T_CONSTANT_ENCAPSED_STRING { $$ = $1; }
        |    T_LINE { $$ = $1; }
        |    T_FILE { $$ = $1; }
        |    T_DIR { $$ = $1; }
        |    T_CLASS_C { $$ = $1; }
        |    T_METHOD_C { $$ = $1; }
        |    T_FUNC_C { $$ = $1; }
        |    T_NS_C { $$ = $1; }
        |    T_START_HEREDOC T_ENCAPSED_AND_WHITESPACE { $$ = $2; }
        //|    T_START_HEREDOC T_END_HEREDOC
;


static_scalar: /* compile-time evaluated scalars */
                common_scalar    	{ $$ = $1; }
        |    namespace_name { $$ = $1; }
        |    T_NAMESPACE T_NS_SEPARATOR namespace_name { $$ = $1; }
        |    T_NS_SEPARATOR namespace_name { $$ = $1; }
        |    '+' static_scalar { $$ = $1; }
        |    '-' static_scalar { $$ = $1; }
        |    T_ARRAY '(' static_array_pair_list ')' { $$ = $1; }
        |    static_class_constant { $$ = $1; }
;

static_class_constant:
                class_name T_DOUBLE_COLON T_STRING { $$ = $1; }
;

scalar:
                T_STRING_VARNAME    	{ $$ = $1; }
        |    class_constant    	{ $$ = $1; }
        |    namespace_name { $$ = $1; }
        |    T_NAMESPACE T_NS_SEPARATOR namespace_name { $$ = $1; }
        |    T_NS_SEPARATOR namespace_name { $$ = $1; }
        |    common_scalar    	    { $$ = $1; }
        |    '"' encaps_list '"'     { $$ = ASTNode::create("doubleQuotes")->addChild($2); }
        |    T_START_HEREDOC encaps_list { $$ = ASTNode::create("hereDoc")->addChild($2); }
;


static_array_pair_list:
                /* empty */
        |    non_empty_static_array_pair_list possible_comma    { $$ = $1; }
;

possible_comma:
                /* empty */
        |    ',' { $$ = $1; }
;

non_empty_static_array_pair_list:
                non_empty_static_array_pair_list ',' static_scalar T_DOUBLE_ARROW static_scalar { $$ = $1; }
        |    non_empty_static_array_pair_list ',' static_scalar { $$ = $1; }
        |    static_scalar T_DOUBLE_ARROW static_scalar { $$ = $1; }
        |    static_scalar { $$ = $1; }
;

expr:
                r_variable { $$ = $1; }
        |    expr_without_variable { $$ = $1; }
;


r_variable:
        variable { $$ = $1; }
;


w_variable:
        variable { $$ = $1; }
;

rw_variable:
        variable { $$ = $1; }
;

variable:
                base_variable_with_function_calls T_OBJECT_OPERATOR
                        object_property  method_or_not variable_properties
                { $$ = ASTNode::create("T_OBJECT_OPERATOR")->addChild($1)->addChild($3)->addChild($4)->addChild($5); }
        |    base_variable_with_function_calls { $$ = $1; }
;

variable_properties:
                variable_properties variable_property {$1->addChild($2);}
        |    /* empty */ { $$ = ASTNode::create("variable_properties"); }
;


variable_property:
                T_OBJECT_OPERATOR object_property  method_or_not
                {$$ = ASTNode::create("T_OBJECT_OPERATOR")->addChild($2)->addChild($3);}
;

method_or_not:
                '('
                                function_call_parameter_list ')'
                         {$$ = ASTNode::create("method_or_not")->addChild($2);}
        |    /* empty */ {$$ = ASTNode::create("method_or_not");}
;

variable_without_objects:
                reference_variable { $$ = $1; }
        |    simple_indirect_reference reference_variable { $$ = $1; }
;

static_member:
                class_name T_DOUBLE_COLON variable_without_objects { $$ = $1; }
        |    variable_class_name T_DOUBLE_COLON variable_without_objects { $$ = $1; }

;

variable_class_name:
                reference_variable { $$ = $1; }
;

base_variable_with_function_calls:
                base_variable { $$ = $1; }
        |    function_call { $$ = $1; }
;


base_variable:
                reference_variable { $$ = $1; }
        |    simple_indirect_reference reference_variable { $$ = $1; }
        |    static_member { $$ = $1; }
;

reference_variable:
                reference_variable '[' dim_offset ']' { $$ = ASTNode::create("offset")->addChild($1)->addChild($3); }
        |    reference_variable '{' expr '}'          { $$ = ASTNode::create("str_offset")->addChild($1)->addChild($3); }
        |    compound_variable { $$ = $1; }
;


compound_variable:
                T_VARIABLE { $$ = $1; }
        |    '$' '{' expr '}'    { $$ = $3; }
;

dim_offset:
                /* empty */
        |    expr { $$ = $1; }
;


object_property:
                object_dim_list { $$ = $1; }
        |    variable_without_objects { $$ = $1; }
;

object_dim_list:
                object_dim_list '[' dim_offset ']' { $$ = $1; }
        |    object_dim_list '{' expr '}' { $$ = $1; }
        |    variable_name { $$ = $1; }
;

variable_name:
                T_STRING { $$ = $1; }
        |    '{' expr '}'    { $$ = $2; }
;

simple_indirect_reference:
                '$' { $$ = $1; }
        |    simple_indirect_reference '$' { $$ = $1; }
;

assignment_list:
                assignment_list ',' assignment_list_element { $1->addChild($3); }
        |    assignment_list_element { $$ = ASTNode::create("assignment_list")->addChild($1); }
;


assignment_list_element:
                variable { $$ = $1; }
        |    T_LIST '('  assignment_list ')' { $$ = $1; }
        |    /* empty */
;


array_pair_list:
                /* empty */ {$$ = ASTNode::create("array_pair_list");}
        |    non_empty_array_pair_list possible_comma { $$ = $1; }
;

non_empty_array_pair_list:
                non_empty_array_pair_list ',' expr T_DOUBLE_ARROW expr
            {
                $1->addChild(
                    ASTNode::create("array_pair")
                    ->addChild(ASTNode::create("array_key")->addChild($3))
                    ->addChild(ASTNode::create("array_value")->addChild($5))
                );
            }
        |    non_empty_array_pair_list ',' expr
            {
                $1->addChild(
                    ASTNode::create("array_pair")
                    ->addChild(ASTNode::create("array_key"))
                    ->addChild(ASTNode::create("array_value")->addChild($3))
                );
            }
        |    expr T_DOUBLE_ARROW expr
            {
                $$ = ASTNode::create("array_pair_list")
                ->addChild(
                    ASTNode::create("array_pair")
                    ->addChild(ASTNode::create("array_key")->addChild($1))
                    ->addChild(ASTNode::create("array_value")->addChild($3))
                );
            }
        |    expr
            {
                $$ = ASTNode::create("array_pair_list")
                ->addChild(
                    ASTNode::create("array_pair")
                    ->addChild(ASTNode::create("array_key"))
                    ->addChild(ASTNode::create("array_value")->addChild($1))
                );
            }
        |    non_empty_array_pair_list ',' expr T_DOUBLE_ARROW '&' w_variable
            {
                $1->addChild(
                    ASTNode::create("array_pair")
                    ->addChild(ASTNode::create("array_key")->addChild($3))
                    ->addChild(ASTNode::create("array_value")->addChild($6)->setData("is_reference", "1"))
                );
            }
        |    non_empty_array_pair_list ',' '&' w_variable
        {
            $1->addChild(
                ASTNode::create("array_pair")
                ->addChild(ASTNode::create("array_key"))
                ->addChild(ASTNode::create("array_value")->addChild($4)->setData("is_reference", "1"))
            );
        }
        |    expr T_DOUBLE_ARROW '&' w_variable
        {
            $$ = ASTNode::create("array_pair_list")
            ->addChild(
                ASTNode::create("array_pair")
                ->addChild(ASTNode::create("array_key")->addChild($1))
                ->addChild(ASTNode::create("array_value")->addChild($4)->setData("is_reference", "1"))
            );
        }
        |    '&' w_variable
        {
            $$ = ASTNode::create("array_pair_list")
            ->addChild(
                ASTNode::create("array_pair")
                ->addChild(ASTNode::create("array_key"))
                ->addChild(ASTNode::create("array_value")->addChild($2)->setData("is_reference", "1"))
            );
        }
;

encaps_list:
                encaps_list encaps_var {$1->addChild($2);}
        |    encaps_list T_ENCAPSED_AND_WHITESPACE {$1->addChild($2);}
        |    encaps_var  {$$ = ASTNode::create("encaps_list")->addChild($1);}
        |    T_ENCAPSED_AND_WHITESPACE encaps_var {$$ = ASTNode::create("encaps_list")->addChild($1)->addChild($2);}
;



encaps_var:
                T_VARIABLE { $$ = $1; }
        |    T_VARIABLE '['  encaps_var_offset ']' {$$ = ASTNode::create("offset")->addChild($1)->addChild($3);}
        |    T_VARIABLE T_OBJECT_OPERATOR T_STRING {$$ = ASTNode::create("T_OBJECT_OPERATOR")->addChild($1)->addChild($3);}
        |    T_DOLLAR_OPEN_CURLY_BRACES expr '}'   {$$ = $2;}
        |    T_DOLLAR_OPEN_CURLY_BRACES T_STRING_VARNAME '[' expr ']' '}' {$$ = ASTNode::create("offset")->addChild($2)->addChild($4);}
        |    T_CURLY_OPEN variable '}'             { $$ = $2; }
;


encaps_var_offset:
                T_STRING    	{ $$ = $1; }
        |    T_NUM_STRING    { $$ = $1; }
        |    T_VARIABLE { $$ = $1; }
;


internal_functions:
                T_ISSET '(' isset_variables ')' { $$ = $3; }
        |    T_EMPTY '(' variable ')' { $$ = $1; }
        |    T_INCLUDE expr { $$ = $1; }
        |    T_INCLUDE_ONCE expr { $$ = $1; }
        |    T_EVAL '(' expr ')' { $$ = $1; }
        |    T_REQUIRE expr { $$ = $1; }
        |    T_REQUIRE_ONCE expr { $$ = $1; }
;

isset_variables:
                variable { $$ = $1; }
        |    isset_variables ','  variable { $$ = $1; }
;

class_constant:
                class_name T_DOUBLE_COLON T_STRING { $$ = $1; }
        |    variable_class_name T_DOUBLE_COLON T_STRING { $$ = $1; }
;

%%

