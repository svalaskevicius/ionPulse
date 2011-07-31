program: ".(?!<\?php)*<\?php" statements ;

statements: statement*;
statements_expr: statements expression?;
statement:   namespace_definition
           | class_definition
           | function_definition
           | expression ';'
           | '{' statements_expr '}';

namespace_definition: "NAMESPACE";

class_definition: ('abstract' | 'final')? 'class' identifier class_specification? '{' class_contents '}';
class_specification: ':' (('extends' identifier) | ('implements' identifier_list))+ ;
class_contents: ;

function_definition:
    ('public' | 'protected' | 'private' | 'static' | 'final' | 'abstract')*
    'function' identifier '(' function_variable_def (',' function_variable_def)* ')' '{' statements_expr '}'
    ;

function_variable_def: variable ('=' (constant | string | array))? ;

array:;

expression
  : expr
  | expr ',' expr $left 6700
  /* labels */
  | identifier ':' expression $right 6600
  | 'case' expression ':' expression $right 6500
  | 'default' ':' expression $right 6500
  /* conditionals */
  | 'if' '(' statements_expr ')' expression $right 6000
  | 'if' '(' statements_expr ')' statement 'else' expression $right 6100
  | 'switch' '(' statements_expr ')' expression $right 6200
  /* loops */
  | 'while' '(' statements_expr ')' expression $right 6300
  | 'do' statement 'while' expression $right 6400
  | 'for' '(' expression (';' expression (';' expression)?)? ')'
          expression $right 6500
  /* jump */
  | 'goto' expression?
  | 'continue' expression?
  | 'break' expression?
  | 'return' expression?
  | expression juxiposition expression
  ;

juxiposition: $binary_op_left 5000;

expr
  : identifier
  | constant
  | strings
  | '(' statements_expr')'
  | '[' statements_expr ']'
  | '{' statements_expr '}'
  | expr '?' expression ':' expr $right 8600
  /* post operators */
  | expr '--' $left 9900
  | expr '++' $left 9900
  | expr '(' statements_expr ')' $left 9900
  | expr '[' statements_expr ']' $left 9900
  | expr '{' statements_expr '}' $left 9900
  /* pre operators */
  | 'sizeof' expression $right 9900
  | '-' expr $right 9800
  | '+' expr $right 9800
  | '~' expr $right 9800
  | '!' expr $right 9800
  | '*' expr $right 9800
  | '&' expr $right 9800
  | '--' expr $right 9800
  | '++' expr $right 9800
  | '(' identifier ')' expr $right 9800
  /* binary operators */
  | expr '->' expr $left 9900
  | expr '.' expr $left 9900
  | expr '*' expr $left 9600
  | expr '/' expr $left 9600
  | expr '%' expr $left 9600
  | expr '+' expr $left 9500
  | expr '-' expr $left 9500
  | expr '<<' expr $left 9400
  | expr '>>' expr $left 9400
  | expr '<' expr $left 9300
  | expr '<=' expr $left 9300
  | expr '>' expr $left 9300
  | expr '>=' expr $left 9300
  | expr '==' expr $left 9200
  | expr '!=' expr $left 9200
  | expr '&' expr $left 9100
  | expr '^' expr $left 9000
  | expr '|' expr $left 8900
  | expr '&&' expr $left 8800
  | expr '||' expr $left 8700
  | expr '=' expr $left 8500
  | expr '*=' expr $left 8500
  | expr '/=' expr $left 8500
  | expr '%=' expr $left 8500
  | expr '+=' expr $left 8500
  | expr '-=' expr $left 8500
  | expr '<<=' expr $left 8500
  | expr '>>=' expr $left 8500
  | expr '&=' expr $left 8500
  | expr '|=' expr $left 8500
  | expr '^=' expr $left 8500
  | expr application expr
  ;

application: $binary_op_left 7000;

strings: string | strings string $left 10000;
constant : decimalint | hexint | octalint | character | float1 | float2;
character: "'[^']*'";
string: "\"[^\"]*\"";
decimalint: "[1-9][0-9]*[uUlL]?" $term -1;
hexint: "(0x|0X)[0-9a-fA-F]+[uUlL]?" $term -2;
octalint: "0[0-7]*[uUlL]?" $term -3;
float1: "([0-9]+.[0-9]*|[0-9]*.[0-9]+)([eE][\-\+]?[0-9]+)?[fFlL]?" $term -4;
float2: "[0-9]+[eE][\-\+]?[0-9]+[fFlL]?" $term -5;
identifier: "[a-zA-Z_][a-zA-Z0-9_]*" $term -6;
variable: "\$[a-zA-Z_][a-zA-Z0-9_]*"  $term -5;

identifier_list: identifier (',' identifier)*;

