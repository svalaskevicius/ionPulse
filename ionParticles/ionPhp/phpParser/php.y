%{


#include "ionParserLib.h"
#include "../phpparser.h"

#define YYDEBUG 1
#define YYSTACKEXPANDABLE 1
#define YYERROR_VERBOSE
#define ion_php_scanner context->__scanner

#define yyparse ion_php_parse
#define yylex   context->__lex
#define yyerror context->__error
#define yychar  ion_php_char
#define yydebug ion_php_debug
#define yynerrs ion_php_nerrs

#define CREATE_AST_NODE(node) ASTNode::create(node) \
            ->setStartPosition(yylloc.first_line, yylloc.first_column) \
            ->setEndPosition(yylloc.last_line, yylloc.last_column)

using namespace IonPhp::Private;

%}

%pure_parser
%glr-parser
%expect 0
%locations
%parse-param { IonPhp::Private::PhpParser* context }
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
%nonassoc T_IF
%left T_ELSEIF
%left T_ELSE
%nonassoc T_ENDIF
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

start: top_statement_list {context->__result->setRoot($1);};

top_statement_list:
        /* empty */ { $$ = CREATE_AST_NODE("top_statement_list");}
        | top_statement_list top_statement { $1->addChild($2); }
        | top_statement_list error { DEBUG_MSG("top error"); $1->addChild(CREATE_AST_NODE("__PARSE_ERROR")); yyclearin; yyerrok; }
;

namespace_name:
                T_STRING { $$ = CREATE_AST_NODE("namespace_name")->addChild($1); }
        |    namespace_name T_NS_SEPARATOR T_STRING { $1->addChild($3); }
;

top_statement:
                statement { $$ = $1; }
        |    function_declaration_statement { $$ = $1; }
        |    class_declaration_statement { $$ = $1; }
        |    T_HALT_COMPILER '(' ')' ';'      { $$ = $1; YYACCEPT; }
        |    T_NAMESPACE namespace_name ';'   { $$ = CREATE_AST_NODE("namespace")->addChild($2); }
        |    T_NAMESPACE namespace_name '{'
                top_statement_list '}'        { $$ = CREATE_AST_NODE("namespace")->addChild($2)->addChild($4); }
        |    T_NAMESPACE '{'
                top_statement_list '}'        { $$ = CREATE_AST_NODE("namespace")->addChild($3); }
        |    T_USE use_declarations ';'       { $$ = CREATE_AST_NODE("use")->addChild($2); }
        |    constant_declaration ';'  { $$ = $1; }
;

use_declarations:
                use_declarations ',' use_declaration  { $$ = $1; }
        |    use_declaration { $$ = $1; }
;

use_declaration:
                namespace_name { $$ = $1; }
        |    namespace_name T_AS T_STRING                { $$ = CREATE_AST_NODE("as")->addChild($1)->addChild($3); }
        |    T_NS_SEPARATOR namespace_name               { $$ = CREATE_AST_NODE("namespaceroot")->addChild($2); }
        |    T_NS_SEPARATOR namespace_name T_AS T_STRING { $$ = CREATE_AST_NODE("as")->addChild(CREATE_AST_NODE("namespaceroot")->addChild($2))->addChild($4); }
;

constant_declaration:
                constant_declaration ',' T_STRING '=' static_scalar { $$ = $1; }
        |    T_CONST T_STRING '=' static_scalar { $$ = $1; }
;

inner_statement_list:
            /* empty */ { $$ = CREATE_AST_NODE("inner_statement_list"); }
        |   inner_statement_list  inner_statement  { $1->addChild($2); $$=$1; }
        |   inner_statement_list  error { DEBUG_MSG("inner error"); $1->addChild(CREATE_AST_NODE("__PARSE_ERROR")); yyclearin; yyerrok; }
;


inner_statement:
             statement
        |    function_declaration_statement
        |    class_declaration_statement
;


statement:
             unticked_statement { $$ = $1; }
        |    T_STRING ':' { $$ = $1; }
;


elseif_list_non_empty:
             T_ELSEIF '(' expr ')'  statement {  $$ = CREATE_AST_NODE("elseif_list")->addChild(CREATE_AST_NODE("elseif")->addChild($3)->addChild($5)); }
        |    elseif_list_non_empty T_ELSEIF '(' expr ')'  statement { $1->addChild(CREATE_AST_NODE("elseif")->addChild($4)->addChild($6)); }
;


classical_if:
      T_IF '(' expr ')'  statement  elseif_list_non_empty T_ELSE statement { $$ = CREATE_AST_NODE("if")->addChild($3)->addChild($5)->addChild($6)->addChild(CREATE_AST_NODE("else")->addChild($8)); }
    | T_IF '(' expr ')'  statement  elseif_list_non_empty %prec T_ELSEIF { $$ = CREATE_AST_NODE("if")->addChild($3)->addChild($5)->addChild($6)->addChild(CREATE_AST_NODE("else")); }
    | T_IF '(' expr ')'  statement  T_ELSE statement { $$ = CREATE_AST_NODE("if")->addChild($3)->addChild($5)->addChild(CREATE_AST_NODE("elseif_list"))->addChild(CREATE_AST_NODE("else")->addChild($7)); }
    | T_IF '(' expr ')'  statement  %prec T_IF { $$ = CREATE_AST_NODE("if")->addChild($3)->addChild($5)->addChild(CREATE_AST_NODE("elseif_list"))->addChild(CREATE_AST_NODE("else")); }
;

unticked_statement:
                '{' inner_statement_list '}'           {$$=$2;}
        |    classical_if {$$=$1;}
        |    T_IF '(' expr ')' ':'  inner_statement_list  new_elseif_list new_else_single T_ENDIF ';' { $$ = CREATE_AST_NODE("if")->addChild($3)->addChild($6)->addChild($7)->addChild($8); }
        |    T_WHILE '('  expr  ')' while_statement  { $$ = CREATE_AST_NODE("while")->addChild($3)->addChild($5); }
        |    T_DO  statement T_WHILE '(' expr ')' ';' { $$ = CREATE_AST_NODE("dowhile")->addChild($2)->addChild($5); }
        |    T_FOR
                        '('
                                for_expr
                        ';'
                                for_expr
                        ';'
                                for_expr
                        ')'
                        for_statement
                { $$ = CREATE_AST_NODE("for")->addChild($3)->addChild($5)->addChild($7)->addChild($9); }
        |    T_SWITCH '(' expr ')' switch_case_list { $$ = CREATE_AST_NODE("switch")->addChild($3)->addChild($5); }
        |    T_BREAK ';' { $$ = CREATE_AST_NODE("break"); }
        |    T_BREAK expr ';' { $$ = CREATE_AST_NODE("break")->addChild($2); }
        |    T_CONTINUE ';' { $$ = CREATE_AST_NODE("continue"); }
        |    T_CONTINUE expr ';' { $$ = CREATE_AST_NODE("continue")->addChild($2); }
        |    T_RETURN ';' { $$ = CREATE_AST_NODE("return"); }
        |    T_RETURN expr_without_variable ';' { $$ = CREATE_AST_NODE("return")->addChild($2); }
        |    T_RETURN variable ';'          { $$ = CREATE_AST_NODE("return")->addChild($2);}
        |    T_GLOBAL global_var_list ';' { $$ = CREATE_AST_NODE("global")->addChild($2);}
        |    T_STATIC static_var_list ';' { $$ = CREATE_AST_NODE("static")->addChild($2); }
        |    T_ECHO echo_expr_list ';'    { $$ = CREATE_AST_NODE("echo")->addChild($2);}
        |    T_INLINE_HTML { $$ = $1; }
        |    expr ';' { $$ = $1; }
        |    T_FOREACH '(' variable T_AS
                foreach_variable foreach_optional_arg ')'
                foreach_statement
            { $$ = CREATE_AST_NODE("foreach")->addChild($3)->addChild($5)->addChild($6)->addChild($8);}
        |    T_FOREACH '(' expr_without_variable T_AS
                variable foreach_optional_arg ')'
                foreach_statement
             {  $$ = CREATE_AST_NODE("foreach")->addChild($3)->addChild($5)->addChild($6)->addChild($8); }
        |    T_DECLARE  '(' declare_list ')' declare_statement { $$ = $1; }
        |    ';'    	/* empty statement */ { $$ = CREATE_AST_NODE("empty statement"); }
        |    T_TRY  '{' inner_statement_list '}'
                T_CATCH '('
                fully_qualified_class_name
                T_VARIABLE ')'
                '{' inner_statement_list '}'
                additional_catches
             { $$ = CREATE_AST_NODE("try")->addChild($3)->addChild( CREATE_AST_NODE("catch")->addChild($7)->addChild($8)->addChild($11) )->addChild($13); }
        |    T_THROW expr ';' { $$ = CREATE_AST_NODE("throw")->addChild($2); }
        |    T_GOTO T_STRING ';' { $$ = $1; }
;


additional_catches:
                non_empty_additional_catches { $$ = $1; }
        |    /* empty */ { $$ = CREATE_AST_NODE("additional_catches"); }
;

non_empty_additional_catches:
                additional_catch { $$ = CREATE_AST_NODE("additional_catches")->addChild($1); }
        |    non_empty_additional_catches additional_catch { $1->addChild($2); $$ = $1; }
;


additional_catch:
        T_CATCH '(' fully_qualified_class_name  T_VARIABLE ')'  '{' inner_statement_list '}'
        { $$ = CREATE_AST_NODE("catch")->addChild($3)->addChild($4)->addChild($7); }
;


function_declaration_statement:
                T_FUNCTION is_reference T_STRING
                        '(' parameter_list ')' '{' inner_statement_list '}'
                {
                    $$ = CREATE_AST_NODE("function_declaration")
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
                   $$ = CREATE_AST_NODE("class_declaration")
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
                   $$ = CREATE_AST_NODE("interface_declaration")
                       ->addChild($1)
                       ->addChild($2)
                       ->addChild($3)
                       ->addChild($5)
                   ;
                }
;


is_reference:
                /* empty */ { $$ = CREATE_AST_NODE("is_reference")->setData("is_reference", "0"); }
        |    '&'            { $$ = CREATE_AST_NODE("is_reference")->setData("is_reference", "1"); }
;


class_entry_type:
                T_CLASS { $$ = $1; }
        |    T_ABSTRACT T_CLASS { $$ = $2; $$->setData("type", "abstract"); }
        |    T_FINAL T_CLASS    { $$ = $2; $$->setData("type", "final"); }
;

extends_from:
                /* empty */ { $$ = CREATE_AST_NODE("extends"); }
        |    T_EXTENDS fully_qualified_class_name { $$ = CREATE_AST_NODE("extends")->addChild($2); }
;

interface_entry:
        T_INTERFACE { $$ = $1; }
;

interface_extends_list:
                /* empty */ { $$ = CREATE_AST_NODE("extends"); }
        |    T_EXTENDS interface_list { $$ = CREATE_AST_NODE("extends")->addChild($2); }
;

implements_list:
                /* empty */  { $$ = CREATE_AST_NODE("implements"); }
        |    T_IMPLEMENTS interface_list { $$ = CREATE_AST_NODE("implements")->addChild($2); }
;

interface_list:
                fully_qualified_class_name { $$ = CREATE_AST_NODE("interfaceList")->addChild($1); }
        |    interface_list ',' fully_qualified_class_name { $1->addChild($3); }
;

foreach_optional_arg:
                /* empty */ { $$ = CREATE_AST_NODE("foreach_optional_arg"); }
        |    T_DOUBLE_ARROW foreach_variable    { $$ = CREATE_AST_NODE("foreach_optional_arg")->addChild($2);  }
;


foreach_variable:
                variable { $$ = $1; }
        |    '&' variable { $$ = $2; $2->setData("is_reference", "1"); }
;

for_statement:
                statement { $$ = $1; }
        |    ':' inner_statement_list T_ENDFOR ';' { $$ = $1; }
;


foreach_statement:
                statement { $$ = $1; }
        |    ':' inner_statement_list T_ENDFOREACH ';' { $$ = $2; }
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
                /* empty */ { $$ = CREATE_AST_NODE("case_list"); }
        |    case_list T_CASE expr case_separator  inner_statement_list { $1->addChild(CREATE_AST_NODE("case")->addChild($3)->addChild($5)); $$=$1; }
        |    case_list T_DEFAULT case_separator  inner_statement_list   { $1->addChild(CREATE_AST_NODE("default_case")->addChild($4)); $$=$1; }
;


case_separator:
                ':' { $$ = $1; }
        |    ';' { $$ = $1; }
;


while_statement:
                statement { $$ = $1; }
        |    ':' inner_statement_list T_ENDWHILE ';' { $$ = $1; }
;



new_elseif_list:
                /* empty */ {  $$ = CREATE_AST_NODE("elseif_list"); }
        |    new_elseif_list T_ELSEIF '(' expr ')' ':'  inner_statement_list { $1->addChild(CREATE_AST_NODE("elseif")->addChild($4)->addChild($7)); $$ = $1; }
;


new_else_single:
                /* empty */ { $$ = CREATE_AST_NODE("else"); }
        |    T_ELSE ':' inner_statement_list { $$ = CREATE_AST_NODE("else")->addChild($3); }
;


parameter_list:
                non_empty_parameter_list { $$ = $1; }
        |    /* empty */ { $$ = CREATE_AST_NODE("parameter_list"); }
;


non_empty_parameter_list:
             optional_class_type T_VARIABLE
             {
                 $$ = CREATE_AST_NODE("parameter_list")
                     ->addChild(
                         CREATE_AST_NODE("parameter")
                             ->addChild($1)
                             ->addChild($2)
                     )
                 ;
             }
        |    optional_class_type '&' T_VARIABLE
             {
                  $$ = CREATE_AST_NODE("parameter_list")
                      ->addChild(
                          CREATE_AST_NODE("parameter")
                              ->addChild($1)
                              ->addChild($3)
                              ->setData("is_reference", "1")
                      )
                  ;
             }
        |    optional_class_type '&' T_VARIABLE '=' static_scalar
            {
                 $$ = CREATE_AST_NODE("parameter_list")
                     ->addChild(
                         CREATE_AST_NODE("parameter")
                             ->addChild($1)
                             ->addChild($3)
                             ->addChild($5)
                             ->setData("is_reference", "1")
                     )
                 ;
            }
        |    optional_class_type T_VARIABLE '=' static_scalar
            {
                 $$ = CREATE_AST_NODE("parameter_list")
                     ->addChild(
                         CREATE_AST_NODE("parameter")
                             ->addChild($1)
                             ->addChild($2)
                             ->addChild($4)
                     )
                 ;
            }
        |    non_empty_parameter_list ',' optional_class_type T_VARIABLE
            {
                 $1->addChild(
                     CREATE_AST_NODE("parameter")
                         ->addChild($3)
                         ->addChild($4)
                 );
            }
        |    non_empty_parameter_list ',' optional_class_type '&' T_VARIABLE
            {
                 $1->addChild(
                     CREATE_AST_NODE("parameter")
                         ->addChild($3)
                         ->addChild($5)
                         ->setData("is_reference", "1")
                 );
            }
        |    non_empty_parameter_list ',' optional_class_type '&' T_VARIABLE '=' static_scalar
            {
                 $1->addChild(
                     CREATE_AST_NODE("parameter")
                         ->addChild($3)
                         ->addChild($5)
                         ->addChild($7)
                         ->setData("is_reference", "1")
                 );
            }
        |    non_empty_parameter_list ',' optional_class_type T_VARIABLE '=' static_scalar
            {
                 $1->addChild(
                     CREATE_AST_NODE("parameter")
                         ->addChild($3)
                         ->addChild($4)
                         ->addChild($6)
                 );
            }
;


optional_class_type:
                /* empty */ { $$ = CREATE_AST_NODE("__ANY_CLASS_TYPE__"); }
        |    fully_qualified_class_name { $$ = $1; }
        |    T_ARRAY { $$ = CREATE_AST_NODE("array"); }
;


function_call_parameter_list:
                non_empty_function_call_parameter_list { $$ = $1; }
        |    /* empty */ {$$=CREATE_AST_NODE("function_call_parameter_list");}
;


non_empty_function_call_parameter_list:
                expr_without_variable {$$=CREATE_AST_NODE("function_call_parameter_list")->addChild($1);}
        |    variable               {$$=CREATE_AST_NODE("function_call_parameter_list")->addChild($1);}
        |    '&' w_variable         {$$=CREATE_AST_NODE("function_call_parameter_list")->addChild($2);$2->setData("reference", "1");}
        |    non_empty_function_call_parameter_list ',' expr_without_variable {$1->addChild($3);}
        |    non_empty_function_call_parameter_list ',' variable              {$1->addChild($3);}
        |    non_empty_function_call_parameter_list ',' '&' w_variable        {$1->addChild($4);$4->setData("reference", "1");}
;

global_var_list:
                global_var_list ',' global_var { $1->addChild($3); }
        |    global_var { $$ = CREATE_AST_NODE("global_var_list")->addChild($1);}
;


global_var:
                T_VARIABLE    	    { $$ = $1; }
        |    '$' r_variable    	{ $$ = $2; }
        |    '$' '{' expr '}'    { $$ = $3; }
;


static_var_list:
                static_var_list ',' T_VARIABLE { $1->addChild($3); }
        |    static_var_list ',' T_VARIABLE '=' static_scalar { $1->addChild($3->addChild($5)); }
        |    T_VARIABLE { $$ = CREATE_AST_NODE("static_var_list")->addChild($1); }
        |    T_VARIABLE '=' static_scalar { $$ = CREATE_AST_NODE("static_var_list")->addChild($1->addChild($3)); }

;


class_statement_list:
                class_statement_list class_statement { $1->addChild($2); }
        |    /* empty */  {$$=CREATE_AST_NODE("class_statement_list");}
;


class_statement:
                variable_modifiers  class_variable_declaration ';'
            {
                $$=CREATE_AST_NODE("PROPERTY")
                    ->addChild($1)
                    ->addChild($2)
                ;
            }
        |    class_constant_declaration ';' { $$ = $1; }
        |    method_modifiers T_FUNCTION is_reference T_STRING  '('
                        parameter_list ')' method_body
             {
                 $$=CREATE_AST_NODE("METHOD")
                     ->addChild($1)
                     ->addChild($3)
                     ->addChild($4)
                     ->addChild($6)
                     ->addChild($8)
                 ;
             }
;


method_body:
                ';' /* abstract method */ {$$=CREATE_AST_NODE("METHOD_BODY");}
        |    '{' inner_statement_list '}' { $$=CREATE_AST_NODE("METHOD_BODY")->addChild($2); }
;

variable_modifiers:
                non_empty_member_modifiers { $$ = $1; }
        |    T_VAR { $$=CREATE_AST_NODE("MODIFIERS")->addChild(CREATE_AST_NODE("VAR")); }
;

method_modifiers:
                /* empty */ {$$=CREATE_AST_NODE("MODIFIERS");}
        |    non_empty_member_modifiers { $$ = $1; }
;

non_empty_member_modifiers:
                member_modifier { $$=CREATE_AST_NODE("MODIFIERS")->addChild($1); }
        |    non_empty_member_modifiers member_modifier { $$ = $1; $$->addChild($2); }
;

member_modifier:
                T_PUBLIC { $$=CREATE_AST_NODE("public"); }
        |    T_PROTECTED { $$ = CREATE_AST_NODE("protected"); }
        |    T_PRIVATE { $$ = CREATE_AST_NODE("private"); }
        |    T_STATIC { $$ = CREATE_AST_NODE("static"); }
        |    T_ABSTRACT { $$ = CREATE_AST_NODE("abstract"); }
        |    T_FINAL { $$ = CREATE_AST_NODE("final"); }
;

class_variable_declaration:
                class_variable_declaration ',' T_VARIABLE { $$ = $1->addChild($3); }
        |    class_variable_declaration ',' T_VARIABLE '=' static_scalar { $$ = $1->addChild($3->addChild($5)); }
        |    T_VARIABLE { $$ = CREATE_AST_NODE("class_properties")->addChild($1); }
        |    T_VARIABLE '=' static_scalar { $$ = CREATE_AST_NODE("class_properties")->addChild($1->addChild($3)); }
;

class_constant_declaration:
                class_constant_declaration ',' T_STRING '=' static_scalar { $$ = $1->addChild($3->addChild($5)); }
        |    T_CONST T_STRING '=' static_scalar { $$ = CREATE_AST_NODE("class_constants")->addChild($2->addChild($4)); }
;

echo_expr_list:
                echo_expr_list ',' expr {$1->addChild($2);}
        |    expr {$$ = CREATE_AST_NODE("echo_expr_list")->addChild($1);}
;


for_expr:
                /* empty */ {$$ = CREATE_AST_NODE("VOID");}
        |    non_empty_for_expr { $$ = $1; }
;

non_empty_for_expr:
                non_empty_for_expr ','     expr { $$ = $1; }
        |    expr { $$ = $1; }
;

expr_without_variable:
            T_LIST '('  assignment_list ')' '=' expr
            {
                $$=CREATE_AST_NODE("assignment")->addChild($3)->addChild($6);
            }
        |    variable '=' expr {$$=CREATE_AST_NODE("assignment")->addChild($1)->addChild($3);}
        |    variable '=' '&' variable {$$=CREATE_AST_NODE("assignment")->addChild($1)->addChild($4)->setData("is_reference", "1");}
        |    variable '=' '&' T_NEW class_name_reference ctor_arguments   { $$ = CREATE_AST_NODE("assignment")->addChild($1)->addChild(CREATE_AST_NODE("new")->addChild($5)->addChild($6))->setData("is_reference", "1");}
        |    T_NEW class_name_reference  ctor_arguments     { $$ = CREATE_AST_NODE("new")->addChild($2)->addChild($3);}
        |    T_CLONE expr {$$=CREATE_AST_NODE("clone")->addChild($2);}
        |    variable T_PLUS_EQUAL expr    {$$=CREATE_AST_NODE("plus_equal")->addChild($1)->addChild($3);}
        |    variable T_MINUS_EQUAL expr   {$$=CREATE_AST_NODE("minus_equal")->addChild($1)->addChild($3);}
        |    variable T_MUL_EQUAL expr     {$$=CREATE_AST_NODE("mul_equal")->addChild($1)->addChild($3);}
        |    variable T_DIV_EQUAL expr     {$$=CREATE_AST_NODE("div_equal")->addChild($1)->addChild($3);}
        |    variable T_CONCAT_EQUAL expr  {$$=CREATE_AST_NODE("concat_equal")->addChild($1)->addChild($3);}
        |    variable T_MOD_EQUAL expr     {$$=CREATE_AST_NODE("mod_equal")->addChild($1)->addChild($3);}
        |    variable T_AND_EQUAL expr     {$$=CREATE_AST_NODE("and_equal")->addChild($1)->addChild($3);}
        |    variable T_OR_EQUAL expr      {$$=CREATE_AST_NODE("or_equal")->addChild($1)->addChild($3);}
        |    variable T_XOR_EQUAL expr     {$$=CREATE_AST_NODE("xor_equal")->addChild($1)->addChild($3);}
        |    variable T_SL_EQUAL expr      {$$=CREATE_AST_NODE("sl_equal")->addChild($1)->addChild($3);}
        |    variable T_SR_EQUAL expr      {$$=CREATE_AST_NODE("sr_equal")->addChild($1)->addChild($3);}
        |    rw_variable T_INC             {$$=CREATE_AST_NODE("postinc")->addChild($1);}
        |    T_INC rw_variable             {$$=CREATE_AST_NODE("preinc")->addChild($2);}
        |    rw_variable T_DEC             {$$=CREATE_AST_NODE("postdec")->addChild($1);}
        |    T_DEC rw_variable             {$$=CREATE_AST_NODE("predec")->addChild($2);}
        |    expr T_BOOLEAN_OR  expr       {$$=CREATE_AST_NODE("boolean_or")->addChild($1)->addChild($3);}
        |    expr T_BOOLEAN_AND  expr      {$$=CREATE_AST_NODE("boolean_and")->addChild($1)->addChild($3);}
        |    expr T_LOGICAL_OR  expr       {$$=CREATE_AST_NODE("logical_or")->addChild($1)->addChild($3);}
        |    expr T_LOGICAL_AND  expr      {$$=CREATE_AST_NODE("logical_and")->addChild($1)->addChild($3);}
        |    expr T_LOGICAL_XOR expr       {$$=CREATE_AST_NODE("logical_xor")->addChild($1)->addChild($3);}
        |    expr '|' expr                 {$$=CREATE_AST_NODE("binary_or")->addChild($1)->addChild($3);}
        |    expr '&' expr                 {$$=CREATE_AST_NODE("binary_and")->addChild($1)->addChild($3);}
        |    expr '^' expr                 {$$=CREATE_AST_NODE("binary_xor")->addChild($1)->addChild($3);}
        |    expr '.' expr                 {$$=CREATE_AST_NODE("concat")->addChild($1)->addChild($3);}
        |    expr '+' expr                 {$$=CREATE_AST_NODE("plus")->addChild($1)->addChild($3);}
        |    expr '-' expr                 {$$=CREATE_AST_NODE("minus")->addChild($1)->addChild($3);}
        |    expr '*' expr                 {$$=CREATE_AST_NODE("mul")->addChild($1)->addChild($3);}
        |    expr '/' expr                 {$$=CREATE_AST_NODE("div")->addChild($1)->addChild($3);}
        |    expr '%' expr                 {$$=CREATE_AST_NODE("mod")->addChild($1)->addChild($3);}
        |    expr T_SL expr                {$$=CREATE_AST_NODE("shift_left")->addChild($1)->addChild($3);}
        |    expr T_SR expr                {$$=CREATE_AST_NODE("shift_right")->addChild($1)->addChild($3);}
        |    '+' expr %prec T_INC { $$=CREATE_AST_NODE("positive")->addChild($2); }
        |    '-' expr %prec T_DEC { $$=CREATE_AST_NODE("negative")->addChild($2); }
        |    '!' expr                      {$$=CREATE_AST_NODE("logical_negate")->addChild($2);}
        |    '~' expr                      {$$=CREATE_AST_NODE("binary_inverse")->addChild($2);}
        |    expr T_IS_IDENTICAL expr      {$$=CREATE_AST_NODE("is_identical")->addChild($1)->addChild($3);}
        |    expr T_IS_NOT_IDENTICAL expr  {$$=CREATE_AST_NODE("is_not_identical")->addChild($1)->addChild($3);}
        |    expr T_IS_EQUAL expr          {$$=CREATE_AST_NODE("is_equal")->addChild($1)->addChild($3);}
        |    expr T_IS_NOT_EQUAL expr      {$$=CREATE_AST_NODE("is_not_equal")->addChild($1)->addChild($3);}
        |    expr '<' expr                 {$$=CREATE_AST_NODE("lessthan")->addChild($1)->addChild($3);}
        |    expr T_IS_SMALLER_OR_EQUAL expr  {$$=CREATE_AST_NODE("lessthan_equal")->addChild($1)->addChild($3);}
        |    expr '>' expr                 {$$=CREATE_AST_NODE("greatherthan")->addChild($1)->addChild($3);}
        |    expr T_IS_GREATER_OR_EQUAL expr  {$$=CREATE_AST_NODE("greatherthan_equal")->addChild($1)->addChild($3);}
        |    expr T_INSTANCEOF class_name_reference  {$$=CREATE_AST_NODE("instanceof")->addChild($1)->addChild($3);}
        |    '(' expr ')'                  { $$ = $2; }
        |    expr '?'
                expr ':'
                expr                       {$$=CREATE_AST_NODE("ternaryop")->addChild($1)->addChild($3)->addChild($5);}
        |    expr '?' ':'
                expr                       {$$=CREATE_AST_NODE("ternaryop")->addChild($1)->addChild(CREATE_AST_NODE("VOID"))->addChild($4);}
        |    internal_functions
        |    T_INT_CAST expr               {$$=CREATE_AST_NODE("int_cast")->addChild($2);}
        |    T_DOUBLE_CAST expr            {$$=CREATE_AST_NODE("double_cast")->addChild($2);}
        |    T_STRING_CAST expr            {$$=CREATE_AST_NODE("string_cast")->addChild($2);}
        |    T_ARRAY_CAST expr             {$$=CREATE_AST_NODE("array_cast")->addChild($2);}
        |    T_OBJECT_CAST expr            {$$=CREATE_AST_NODE("object_cast")->addChild($2);}
        |    T_BOOL_CAST expr              {$$=CREATE_AST_NODE("bool_cast")->addChild($2);}
        |    T_UNSET_CAST expr             {$$=CREATE_AST_NODE("unset_cast")->addChild($2);}
        |    T_EXIT exit_expr              {$$=CREATE_AST_NODE("exit")->addChild($2);}
        |    '@'  expr                     {$$=CREATE_AST_NODE("silence")->addChild($2);}
        |    scalar { $$ = $1; }
        |    T_ARRAY '(' array_pair_list ')' { $$ = CREATE_AST_NODE("array")->addChild($3); }
        |    '`' backticks_expr '`'        { $$ = CREATE_AST_NODE("backticks")->addChild($2); }
        |    T_PRINT expr                  {$$=CREATE_AST_NODE("print")->addChild($2);}
        |    T_FUNCTION is_reference '('
                        parameter_list ')' lexical_vars '{' inner_statement_list '}'
             {
                $$=CREATE_AST_NODE("LAMBDA_FUNCTION")
                    ->addChild($2)
                    ->addChild($4)
                    ->addChild($6)
                    ->addChild($8)
                ;
             }
;


lexical_vars:
                /* empty */                 { $$ = CREATE_AST_NODE("use"); }
        |    T_USE '(' lexical_var_list ')' { $$ = CREATE_AST_NODE("use")->addChild($3); }
;

lexical_var_list:
                lexical_var_list ',' T_VARIABLE   { $1->addChild($3); }
        |    lexical_var_list ',' '&' T_VARIABLE  { $1->addChild($4); $4->setData("is_reference", "1");}
        |    T_VARIABLE                     { $$ = CREATE_AST_NODE("lexical_var_list")->addChild($1); }
        |    '&' T_VARIABLE                 { $$ = CREATE_AST_NODE("lexical_var_list")->addChild($2); $2->setData("is_reference", "1"); }
;

function_call:
                namespace_name '('
                                function_call_parameter_list
                                ')'
               {$$=CREATE_AST_NODE("function_call")->addChild($1)->addChild($3);}
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
                /* empty */  { $$ = CREATE_AST_NODE("VOID"); }
        |    '(' ')'         { $$ = CREATE_AST_NODE("VOID"); }
        |    '(' expr ')'    { $$ = $2; }
;

backticks_expr:
                /* empty */ { $$ = CREATE_AST_NODE("VOID"); }
        |    T_ENCAPSED_AND_WHITESPACE    { $$ = $1; }
        |    encaps_list    { $$ = $1; }
;


ctor_arguments:
                /* empty */ { $$ = CREATE_AST_NODE("VOID"); }
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
        |    T_NAMESPACE T_NS_SEPARATOR namespace_name { $$ = $3; }
        |    T_NS_SEPARATOR namespace_name { $$ = $1; }
        |    '+' static_scalar { $$=CREATE_AST_NODE("positive")->addChild($2); }
        |    '-' static_scalar { $$=CREATE_AST_NODE("negative")->addChild($2); }
        |    T_ARRAY '(' array_pair_list ')' { $$=CREATE_AST_NODE("array")->addChild($3); }
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
        |    '"' encaps_list '"'     { $$ = CREATE_AST_NODE("doubleQuotes")->addChild($2); }
        |    T_START_HEREDOC encaps_list { $$ = CREATE_AST_NODE("hereDoc")->addChild($2); }
;


possible_comma:
                /* empty */
        |    ',' { $$ = $1; }
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
                { $$ = CREATE_AST_NODE("object_operator")->addChild($1)->addChild($3)->addChild($4)->addChild($5); }
        |    base_variable_with_function_calls { $$ = $1; }
;

variable_properties:
                variable_properties variable_property {$1->addChild($2);}
        |    /* empty */ { $$ = CREATE_AST_NODE("variable_properties"); }
;


variable_property:
                T_OBJECT_OPERATOR object_property  method_or_not
                {$$ = CREATE_AST_NODE("object_operator")->addChild($2)->addChild($3);}
;

method_or_not:
                '('
                                function_call_parameter_list ')'
                         {$$ = CREATE_AST_NODE("method_or_not")->addChild($2);}
        |    /* empty */ {$$ = CREATE_AST_NODE("method_or_not");}
;

variable_without_objects:
                reference_variable { $$ = $1; }
        |    simple_indirect_reference reference_variable { $1->addChild($2); }
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
        |    simple_indirect_reference reference_variable { $1->addChild($2); }
        |    static_member { $$ = $1; }
;

reference_variable:
                reference_variable '[' dim_offset ']' { $$ = CREATE_AST_NODE("offset")->addChild($1)->addChild($3); }
        |    reference_variable '{' expr '}'          { $$ = CREATE_AST_NODE("str_offset")->addChild($1)->addChild($3); }
        |    compound_variable { $$ = $1; }
;


compound_variable:
                T_VARIABLE { $$ = $1; }
        |    '$' '{' expr '}'    { $$ = $3; }
;

dim_offset:
                /* empty */ { $$ = CREATE_AST_NODE("empty_offset"); }
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
                '$' { $$ = CREATE_AST_NODE("simple_indirect_reference"); }
        |    simple_indirect_reference '$' { $$ = $1; }
;

assignment_list:
                assignment_list ',' assignment_list_element { $1->addChild($3); }
        |    assignment_list_element { $$ = CREATE_AST_NODE("assignment_list")->addChild($1); }
;


assignment_list_element:
                variable { $$ = $1; }
        |    T_LIST '('  assignment_list ')' { $$ = $3; }
        |    /* empty */ {$$ = CREATE_AST_NODE("VOID");}
;


array_pair_list:
                /* empty */ {$$ = CREATE_AST_NODE("array_pair_list");}
        |    non_empty_array_pair_list possible_comma { $$ = $1; }
;

non_empty_array_pair_list:
                non_empty_array_pair_list ',' expr T_DOUBLE_ARROW expr
            {
                $1->addChild(
                    CREATE_AST_NODE("array_pair")
                    ->addChild(CREATE_AST_NODE("array_key")->addChild($3))
                    ->addChild(CREATE_AST_NODE("array_value")->addChild($5))
                );
            }
        |    non_empty_array_pair_list ',' expr
            {
                $1->addChild(
                    CREATE_AST_NODE("array_pair")
                    ->addChild(CREATE_AST_NODE("array_key"))
                    ->addChild(CREATE_AST_NODE("array_value")->addChild($3))
                );
            }
        |    expr T_DOUBLE_ARROW expr
            {
                $$ = CREATE_AST_NODE("array_pair_list")
                ->addChild(
                    CREATE_AST_NODE("array_pair")
                    ->addChild(CREATE_AST_NODE("array_key")->addChild($1))
                    ->addChild(CREATE_AST_NODE("array_value")->addChild($3))
                );
            }
        |    expr
            {
                $$ = CREATE_AST_NODE("array_pair_list")
                ->addChild(
                    CREATE_AST_NODE("array_pair")
                    ->addChild(CREATE_AST_NODE("array_key"))
                    ->addChild(CREATE_AST_NODE("array_value")->addChild($1))
                );
            }
        |    non_empty_array_pair_list ',' expr T_DOUBLE_ARROW '&' w_variable
            {
                $1->addChild(
                    CREATE_AST_NODE("array_pair")
                    ->addChild(CREATE_AST_NODE("array_key")->addChild($3))
                    ->addChild(CREATE_AST_NODE("array_value")->addChild($6)->setData("is_reference", "1"))
                );
            }
        |    non_empty_array_pair_list ',' '&' w_variable
        {
            $1->addChild(
                CREATE_AST_NODE("array_pair")
                ->addChild(CREATE_AST_NODE("array_key"))
                ->addChild(CREATE_AST_NODE("array_value")->addChild($4)->setData("is_reference", "1"))
            );
        }
        |    expr T_DOUBLE_ARROW '&' w_variable
        {
            $$ = CREATE_AST_NODE("array_pair_list")
            ->addChild(
                CREATE_AST_NODE("array_pair")
                ->addChild(CREATE_AST_NODE("array_key")->addChild($1))
                ->addChild(CREATE_AST_NODE("array_value")->addChild($4)->setData("is_reference", "1"))
            );
        }
        |    '&' w_variable
        {
            $$ = CREATE_AST_NODE("array_pair_list")
            ->addChild(
                CREATE_AST_NODE("array_pair")
                ->addChild(CREATE_AST_NODE("array_key"))
                ->addChild(CREATE_AST_NODE("array_value")->addChild($2)->setData("is_reference", "1"))
            );
        }
;

encaps_list:
                encaps_list encaps_var {$1->addChild($2);}
        |    encaps_list T_ENCAPSED_AND_WHITESPACE {$1->addChild($2);}
        |    encaps_var  {$$ = CREATE_AST_NODE("encaps_list")->addChild($1);}
        |    T_ENCAPSED_AND_WHITESPACE encaps_var {$$ = CREATE_AST_NODE("encaps_list")->addChild($1)->addChild($2);}
;



encaps_var:
                T_VARIABLE { $$ = $1; }
        |    T_VARIABLE '['  encaps_var_offset ']' {$$ = CREATE_AST_NODE("offset")->addChild($1)->addChild($3);}
        |    T_VARIABLE T_OBJECT_OPERATOR T_STRING {$$ = CREATE_AST_NODE("object_operator")->addChild($1)->addChild($3);}
        |    T_DOLLAR_OPEN_CURLY_BRACES expr '}'   {$$ = $2;}
        |    T_DOLLAR_OPEN_CURLY_BRACES T_STRING_VARNAME '[' expr ']' '}' {$$ = CREATE_AST_NODE("offset")->addChild($2)->addChild($4);}
        |    T_CURLY_OPEN variable '}'             { $$ = $2; }
;


encaps_var_offset:
                T_STRING    	{ $$ = $1; }
        |    T_NUM_STRING    { $$ = $1; }
        |    T_VARIABLE { $$ = $1; }
;


internal_functions:
                T_ISSET '(' isset_variables ')' {$$ = CREATE_AST_NODE("isset")->addChild($3);}
        |    T_EMPTY '(' variable ')' {$$ = CREATE_AST_NODE("empty")->addChild($3);}
        |    T_INCLUDE expr {$$ = CREATE_AST_NODE("include")->addChild($2);}
        |    T_INCLUDE_ONCE expr {$$ = CREATE_AST_NODE("include_once")->addChild($2);}
        |    T_EVAL '(' expr ')' {$$ = CREATE_AST_NODE("eval")->addChild($3);}
        |    T_REQUIRE expr {$$ = CREATE_AST_NODE("require")->addChild($2);}
        |    T_REQUIRE_ONCE expr {$$ = CREATE_AST_NODE("require_once")->addChild($2);}
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

