//Todo: Labels and goto have been disabled, need to test on 5.3

grammar ionPhp;

options {
    backtrack = true;
    memoize = true;
    k=2;
    output = AST;
    ASTLabelType = pANTLR3_COMMON_TREE;
    language = C;
}

tokens{
    SemiColon = ';';
    Comma = ',';
    OpenBrace = '(';
    CloseBrace = ')';
    OpenSquareBrace = '[';
    CloseSquareBrace = ']';
    OpenCurlyBrace = '{';
    CloseCurlyBrace = '}';
    ArrayAssign = '=>';
    LogicalOr = '||';
    LogicalAnd = '&&';
    ClassMember = '::';
    InstanceMember = '->';
    SuppressWarnings = '@';
    QuestionMark = '?';
    Dollar = '$';
    Colon = ':';
    Dot = '.';
    Ampersand = '&';
    Pipe = '|';
    Bang = '!';
    Plus = '+';
    Minus = '-';
    Asterisk = '*';
    Percent = '%';
    Forwardslash = '/';
    Tilde = '~';
    Equals = '=';
    New = 'new';
    Clone = 'clone';
    Echo = 'echo';
    If = 'if';
    Else = 'else';
    ElseIf = 'elseif';
    For = 'for';
    Foreach = 'foreach';
    While = 'while';
    Do = 'do';
    Switch = 'switch';
    Case = 'case';
    Default = 'default';
    Function = 'function';
    Break = 'break';
    Continue = 'continue';
    Goto = 'goto';
    Return = 'return';
    Global = 'global';
    Static = 'static';
    And = 'and';
    Or = 'or';
    Xor = 'xor';
    Instanceof = 'instanceof';

    Class = 'class';
    Interface = 'interface';
    Extends = 'extends';
    Implements = 'implements';
    Abstract = 'abstract';
    Var = 'var';
    Const = 'const';
   /* Modifiers;
    ClassDefinition;

    Block;
    Params;
    Apply;
    Member;
    Reference;
    Empty;
    Prefix;
    Postfix;
    IfExpression;
    Label;
    Cast;
    ForInit;
    ForCondition;
    ForUpdate;
    Field;
    Method;*/
}
@lexer::header {
#include "../ionParserLib.h"
}
@parser::header {
#include "../ionParserLib.h"
}

prog : statement* EOF;

statement
    : simpleStatement
//    | '{' statement '}' -> statement
//    | bracketedBlock
    //| UnquotedString Colon statement -> ^(Label UnquotedString statement)
//    | classDefinition
//    | interfaceDefinition
//    | complexStatement
//    | simpleStatement ';'!
    ;
/*
bracketedBlock
    : '{' stmts=statement* '}' -> ^(Block $stmts)
    ;

interfaceDefinition
    : Interface interfaceName=UnquotedString interfaceExtends?
        OpenCurlyBrace
        interfaceMember*
        CloseCurlyBrace
        -> ^(Interface $interfaceName interfaceExtends? interfaceMember*)
    ;

interfaceExtends
    : Extends^ UnquotedString (Comma! UnquotedString)*
    ;
interfaceMember
    : Const UnquotedString (Equals atom)? ';'
        -> ^(Const UnquotedString atom?)
    | fieldModifier* Function UnquotedString parametersDefinition ';'
        -> ^(Method ^(Modifiers fieldModifier*) UnquotedString parametersDefinition)
    ;

classDefinition
    :   classModifier?
        Class className=UnquotedString
        (Extends extendsclass=UnquotedString)?
        classImplements?
        OpenCurlyBrace
        classMember*
        CloseCurlyBrace
        -> ^(Class ^(Modifiers classModifier?) $className ^(Extends $extendsclass)? classImplements?
            classMember*
        )
    ;

classImplements
    :  Implements^ (UnquotedString (Comma! UnquotedString)*)
    ;

classMember
    : fieldModifier* Function UnquotedString parametersDefinition
        (bracketedBlock | ';')
        -> ^(Method ^(Modifiers fieldModifier*) UnquotedString parametersDefinition bracketedBlock?)
    | Var Dollar UnquotedString (Equals atom)? ';'
        -> ^(Var ^(Dollar UnquotedString) atom?)
    | Const UnquotedString (Equals atom)? ';'
        -> ^(Const UnquotedString atom?)
    | fieldModifier* (Dollar UnquotedString) (Equals atom)? ';'
        -> ^(Field ^(Modifiers fieldModifier*) ^(Dollar UnquotedString) atom?)
    ;

fieldDefinition
    : Dollar UnquotedString (Equals atom)? ';'-> ^(Field ^(Dollar UnquotedString) atom?)
    ;

classModifier
    : 'abstract';

fieldModifier
    : AccessModifier | 'abstract' | 'static'
    ;


complexStatement
    : If '(' ifCondition=expression ')' ifTrue=statement conditional?
        -> ^('if' expression $ifTrue conditional?)
    | For '(' forInit forCondition forUpdate ')' statement -> ^(For forInit forCondition forUpdate statement)
    | Foreach '(' variable 'as' arrayEntry ')' statement -> ^(Foreach variable arrayEntry statement)
    | While '(' whileCondition=expression? ')' statement -> ^(While $whileCondition statement)
    | Do statement While '(' doCondition=expression ')' ';' -> ^(Do statement $doCondition)
    | Switch '(' expression ')' '{'cases'}' -> ^(Switch expression cases)
    | functionDefinition
    ;
*/
simpleStatement
  //  :// Echo^ commaList
//    | Global^ name (','! name)*
//    | Static^ variable Equals! atom
:'.'
  //  | Break^ Integer?
    //| Continue^ Integer?
    //| Goto^ UnquotedString
//    | Return^ expression?
//    | RequireOperator^ expression
//    | expression
    ;
/*

conditional
    : ElseIf '(' ifCondition=expression ')' ifTrue=statement conditional? -> ^(If $ifCondition $ifTrue conditional?)
    | Else statement -> statement
    ;

forInit
    : commaList? ';' -> ^(ForInit commaList?)
    ;

forCondition
    : commaList? ';' -> ^(ForCondition commaList?)
    ;

forUpdate
    : commaList? -> ^(ForUpdate commaList?)
    ;

cases
    : casestatement*  defaultcase
    ;

casestatement
    : Case^ expression ':'! statement*
    ;

defaultcase
    : (Default^ ':'! statement*)
    ;

functionDefinition
    : Function UnquotedString parametersDefinition bracketedBlock ->
        ^(Function UnquotedString parametersDefinition bracketedBlock)
    ;

parametersDefinition
    : OpenBrace (paramDef (Comma paramDef)*)? CloseBrace -> ^(Params paramDef*)
    ;

paramDef
    : paramName (Equals^ atom)?
    ;

paramName
    : Dollar^ UnquotedString
    | Ampersand Dollar UnquotedString -> ^(Ampersand ^(Dollar UnquotedString))
    ;

commaList
    : expression (','! expression)*
    ;

expression
    : weakLogicalOr
    ;

weakLogicalOr
    : weakLogicalXor (Or^ weakLogicalXor)*
    ;

weakLogicalXor
    : weakLogicalAnd (Xor^ weakLogicalAnd)*
    ;

weakLogicalAnd
    : assignment (And^ assignment)*
    ;

assignment
    : name ((Equals | AsignmentOperator)^ assignment)
    | ternary
    ;

ternary
    : logicalOr QuestionMark expression Colon expression -> ^(IfExpression logicalOr expression*)
    | logicalOr
    ;

logicalOr
    : logicalAnd (LogicalOr^ logicalAnd)*
    ;

logicalAnd
    : bitwiseOr (LogicalAnd^ bitwiseOr)*
    ;

bitwiseOr
    : bitWiseAnd (Pipe^ bitWiseAnd)*
    ;

bitWiseAnd
    : equalityCheck (Ampersand^ equalityCheck)*
    ;

equalityCheck
    : comparisionCheck (EqualityOperator^ comparisionCheck)?
    ;

comparisionCheck
    : bitWiseShift (ComparisionOperator^ bitWiseShift)?
    ;

bitWiseShift
    : addition (ShiftOperator^ addition)*
    ;

addition
    : multiplication ((Plus | Minus | Dot)^ multiplication)*
    ;

multiplication
    : logicalNot ((Asterisk | Forwardslash | Percent)^ logicalNot)*
    ;

logicalNot
    : Bang^ logicalNot
    | instanceOf
    ;

instanceOf
    : negateOrCast (Instanceof^ negateOrCast)?
    ;

negateOrCast
    : (Tilde | Minus | SuppressWarnings)^ increment
    | OpenBrace PrimitiveType CloseBrace increment -> ^(Cast PrimitiveType increment)
    | OpenBrace! weakLogicalAnd CloseBrace!
    | increment
    ;

increment
    : IncrementOperator name -> ^(Prefix IncrementOperator name)
    | name IncrementOperator -> ^(Postfix IncrementOperator name)
    | newOrClone
    ;

newOrClone
    : New^ nameOrFunctionCall
    | Clone^ name
    | atomOrReference
    ;

atomOrReference
    : atom
    | reference
    ;

arrayDeclaration
    : Array OpenBrace (arrayEntry (Comma arrayEntry)*)? CloseBrace -> ^(Array arrayEntry*)
    ;

arrayEntry
    : (keyValuePair | expression)
    ;

keyValuePair
    : (expression ArrayAssign expression) -> ^(ArrayAssign expression+)
    ;

atom: SingleQuotedString | DoubleQuotedString | HereDoc | Integer | Real | Boolean | arrayDeclaration
    ;

//Need to be smarter with references, they have their own tower of application.
reference
    : Ampersand^ nameOrFunctionCall
    | nameOrFunctionCall
    ;

nameOrFunctionCall
    : name OpenBrace (expression (Comma expression)*)? CloseBrace -> ^(Apply name expression*)
    | name
    ;

name: staticMemberAccess
    | memberAccess
    | variable
    ;

staticMemberAccess
    : UnquotedString '::'^ variable
    ;

memberAccess
    : variable
        ( OpenSquareBrace^ expression CloseSquareBrace!
        | '->'^ UnquotedString)*
    ;

variable
    : Dollar^ variable
    | UnquotedString
    ;

BodyString
    : '?>' (('<' ~ '?')=> '<' | ~'<' )* ('<?' ('php'?))?
    ;
*/

PhpEnd 	:
	{PHP == ionPhpParserOptions.state}?=>'?>'{ionPhpParserOptions.state = TEXT;}
	{ $channel = HIDDEN; } ;
PhpStart:
	{TEXT == ionPhpParserOptions.state}?=>'<?' 'php'? {ionPhpParserOptions.state = PHP;}
	{ $channel = HIDDEN; } ;
TextContent
	:
	{TEXT == ionPhpParserOptions.state}?=>(
			  ({LA(2)!='?'}?=>.)
			| ({LA(2)=='?'}?=>~'<')
		)*
	{ $channel = HIDDEN; } ;

//FirstBodyString
  //  : (('<' ~ '?')=> '<' | ~'<' )* '<?' ('php'?)
    //;
/*
MultilineComment
    : '/*' (('*' ~ '/')=>'*' | ~ '*')* '* /' {$channel=HIDDEN;}
    ;

SinglelineComment
    : '//'  (('?' ~'>')=>'?' | ~('\n'|'?'))* {$channel=HIDDEN;}
    ;

UnixComment
    : '#' (('?' ~'>')=>'?' | ~('\n'|'?'))* {$channel=HIDDEN;}
    ;

Array
    : ('a'|'A')('r'|'R')('r'|'R')('a'|'A')('y'|'Y')
    ;

RequireOperator
    : 'require' | 'require_once' | 'include' | 'include_once'
    ;

PrimitiveType
    : 'int'|'float'|'string'|'array'|'object'|'bool'
    ;

AccessModifier
    : 'public' | 'private' | 'protected'
    ;

fragment
Decimal
        :('1'..'9' ('0'..'9')*)|'0'
        ;
fragment
Hexadecimal
        : '0'('x'|'X')('0'..'9'|'a'..'f'|'A'..'F')+
        ;

fragment
Octal
        : '0'('0'..'7')+
        ;
Integer
        :Octal|Decimal|Hexadecimal
        ;

fragment
Digits
        : '0'..'9'+
        ;

fragment
DNum
        :(('.' Digits)=>('.' Digits)|(Digits '.' Digits?))
        ;

fragment
Exponent_DNum
        :((Digits|DNum)('e'|'E')('+''-')?Digits)
        ;

Real
    : DNum|Exponent_DNum
    ;

Boolean
    : 'true' | 'false'
    ;

SingleQuotedString
    : '\'' (('\\' '\'')=>'\\' '\''
    |         ('\\' '\\')=>'\\' '\\'
    |         '\\' | ~ ('\'' | '\\'))*
      '\''
    ;

fragment
EscapeCharector
    : 'n' | 'r' | 't' | '\\' | '$' | '"' | Digits | 'x'
    ;

DoubleQuotedString
    : '"'  ( ('\\' EscapeCharector)=> '\\' EscapeCharector
    | '\\'
    | ~('\\'|'"') )*
      '"'
    ;

HereDoc
    : '<<<' HereDocContents
    ;

//Todo handle '\x7f' - '\xff'
UnquotedString
   : ('a'..'z' | 'A'..'Z' | '_')  ('a'..'z' | 'A'..'Z' | '0'..'9' | '_')*
   ;

//TODO: add error handling
fragment
HereDocContents
    : 'HEREDOC'
    ;

AsignmentOperator
    : '+='|'-='|'*='|'/='|'.='|'%='|'&='|'|='|'^='|'<<='|'>>='
    ;

EqualityOperator
    : '==' | '!=' | '===' | '!=='
    ;

ComparisionOperator
    : '<' | '<=' | '>' | '>=' | '<>'
    ;

ShiftOperator
    : '<<' | '>>'
    ;

IncrementOperator
    : '--'|'++'
    ;


fragment
Eol : '\n'
    ;

*/
WS : (' '| '\t'| '\n'|'\r')+ { $channel = HIDDEN; } ;

