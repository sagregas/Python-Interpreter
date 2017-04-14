/*
 * The MIT License (MIT)
 * 
 * Copyright (c) 2016 Brian A. Malloy and James F. Power
 * 
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * 
 */
%{
/* This scanner is written by James Power, Chris Malloy, and Brian Malloy
 * During Fall, 2016.  
 * Most of the definitions and specifications were taken from:
 * https://docs.python.org/2.7/reference/index.html
 */

#include "ast.h"
#include "symTable.h"
#include "parse.tab.hpp"
#include <stack>

  /* The scanner state: */
  static std::stack<int> indentations;
  static int curr_indent = 0;
  static int encmatching = 0;
  static bool given_endmarker = false;

  static void display_error(const char *msg);
  static int iced(const int);
  static int calc_indent_change(const int);
  static int handle_indent(const int);
  static void left_enclose(void);
  static void right_enclose(void);

  /* Some counters, just for information: */
  static int keywords = 0;
  static int identifiers = 0;
  static int numbers = 0;

%}

%option yylineno
%option noyywrap



imagnumber     {imaghead}[jJ]
imaghead       {floatnumber}|{intpart}

floatnumber    {pointfloat}|{expfloat}
expfloat       {expfloat_1}{exponent}
expfloat_1     {expfloat_2}|{pointfloat}
expfloat_2     {nonzero}{digit}*
pointfloat     {pointfloat_1}|{pointfloat_2}
pointfloat_1   {intpart}?{fraction}
pointfloat_2   {intpart}\.
intpart        {nzintpart}|0
nzintpart      {nonzero}{digit}*
fraction       \.{digit}+
exponent       [eE][+-]?{digit}+

longinteger    {integer}[lL]
integer        {decinteger}|{octinteger}|{hexinteger}|{bininteger}
decinteger     {nzointeger}|0
nzointeger     {nonzero}{digit}*
hexinteger     0[xX]{hexdigit}+
octinteger     0[oO]{octdigit}+|0{octdigit}+
bininteger     0[bB]{bindigit}+

digit          [0-9]
nonzero        [1-9]
octdigit       [0-7]
bindigit       [01]
hexdigit       [0-9a-fA-F]

string        {stringprefix}?{stringbody}
stringbody    {shortstring}

stringprefix  {ruprefix}|{rbprefix}
ruprefix      [rRuU][rRuU]?
rbprefix      [rRbB][rRbB]?

shortstring       {shortstring1tck}|{shortstring2tck}
shortstring2tck   \"{shortitem2tck}*\"
shortstring1tck   \'{shortitem1tck}*\'
longstring        {longstring1tck}|{longstring2tck}
longstring2tck    \"\"\"{longitem}*\"\"\"
longstring1tck    \'\'\'{longitem}*\'\'\'

shortitem2tck     {shortchar2tck}|{escapeseq}
shortitem1tck     {shortchar1tck}|{escapeseq}
longitem          {longchar}|{escapeseq}
shortchar2tck     [^\n\r\f\"\\]
shortchar1tck     [^\n\r\f\'\\]
longchar          [^\\]
escapeseq         \\.|\\\n

name      [_a-zA-Z][_a-zA-Z0-9]*

ws        [ \t\f]
spaces    {ws}+
stringQQ  [ru]?\"([^\\\"]|\\.)*\"
stringQ   [ru]?\'([^\\\']|\\.)*\'

newline   [\r]?[\n]|[\r]
comment   #.*


/***** Start states for multi-line strings and indentation *****/
%x LONG_STRING
%x LONG_STRING2
%x INDENTING
%x FINISHING

%%

<INITIAL,INDENTING>^{ws}*{comment}{newline}  { BEGIN(INDENTING); }
"^{ws}*"{newline}           { BEGIN(INDENTING); }

{comment}                   { ; }

{newline}                   { BEGIN(INDENTING); if(encmatching <= 0) return NEWLINE;  }

<INDENTING>^{spaces}        { int t = handle_indent(yyleng); if( t > 0 ) return t; }
<INDENTING>^.               { int t = handle_indent(0);  if( t > 0 ) return t; }
<INDENTING>^{ws}*{newline}  { /* Entirely blank line: do nothing */; }
<INDENTING>{newline}        { /* Entirely blank line: do nothing */; }

{spaces}                    { ; }
"\\"{newline}               {  /* Explicit line joining: throw it away */ ;  }


{stringprefix}?"'''"        { BEGIN(LONG_STRING); }
<LONG_STRING>"'''"          { BEGIN(INITIAL); return STRING; }

{stringprefix}?"\"\"\""     { BEGIN(LONG_STRING2); }
<LONG_STRING2>"\"\"\""      { BEGIN(INITIAL); return STRING; }

<LONG_STRING,LONG_STRING2>{newline}    { ; }
<LONG_STRING,LONG_STRING2>{escapeseq}  { ; }
<LONG_STRING,LONG_STRING2>.            { ; }

{string}   { return STRING; }


"("        { left_enclose();  return LPAR; }
")"        { right_enclose(); return RPAR; }
"["        { left_enclose();  return LSQB;  }
"]"        { right_enclose(); return RSQB; }
"{"        { left_enclose();  return LBRACE; }
"}"        { right_enclose(); return RBRACE; }

":"        { return COLON; }
","        { return COMMA; }
";"        { return SEMI; }

"+"        { return PLUS; }
"-"        { return MINUS; }
"*"        { return STAR; }
"/"        { return SLASH; }

"<"        { return LESS; }
">"        { return GREATER; }
"="        { return EQUAL; }

"."        { return DOT; }
"%"        { return PERCENT; }
"`"        { return BACKQUOTE; }

"=="       { return EQEQUAL; }
"!="       { return NOTEQUAL; }
"<="       { return LESSEQUAL; }
">="       { return GREATEREQUAL; }
"<>"       { return GRLT; }

"~"        { return TILDE; }
"^"        { return CIRCUMFLEX; }
"<<"       { return LEFTSHIFT; }
">>"       { return RIGHTSHIFT; }
"**"       { return DOUBLESTAR; }

"+="       { return PLUSEQUAL; }
"-="       { return MINEQUAL; }
"*="       { return STAREQUAL; }
"/="       { return SLASHEQUAL; }
"%="       { return PERCENTEQUAL; }
"&="       { return AMPEREQUAL; }
"|"        { return BAR; }
"|="       { return VBAREQUAL; }
"^="       { return CIRCUMFLEXEQUAL; }
"<<="      { return LEFTSHIFTEQUAL; }
">>="      { return RIGHTSHIFTEQUAL; }
"**="      { return DOUBLESTAREQUAL; }
"//"       { return DOUBLESLASH; }
"//="      { return DOUBLESLASHEQUAL; }

"@"        { return AT; }

"and"      { return AND; }
"&"        { return AMPERSAND; }
"not"      { return NOT; }
"or"       { return OR; }
"as"       { return AS; }
"assert"   { return ASSERT; }
"break"    { return BREAK; }
"class"    { return CLASS; }
"continue" { return CONTINUE; }
"def"      { return DEF; }
"del"      { return DEL; }
"elif"     { return ELIF; }
"else"     { return ELSE; }
"except"   { return EXCEPT; }
"exec"     { return EXEC; }
"finally"  { return FINALLY; }
"for"      { return FOR; }
"from"     { return FROM; }
"global"   { return GLOBAL; }
"if"       { return IF; }
"import"   { return IMPORT; }
"in"       { return IN; }
"is"       { return IS; }
"lambda"   { return LAMBDA; }
"pass"     { return PASS; }
"print"    { return PRINT; }
"raise"    { return RAISE; }
"return"   { return RETURN; }
"try"      { return TRY; }
"while"    { return WHILE; }
"with"     { return WITH; }
"yield"    { return YIELD; }

{integer}      { 
                 ++numbers; 
                 yylval.ast = new AstNumber('K',atoi(yytext), 'i'); 
                 return NUMBER; 
               }
{floatnumber}  {
                 ++numbers;
                 yylval.ast =new AstNumber('K',atof(yytext), 'f');
                 return NUMBER;   
               }
{name}         { 
                 ++identifiers;
                 yylval.ast = new IdNode('I', 't', yytext); 
                 return NAME; 
               }

<INDENTING,INITIAL><<EOF>>   {
             int t = calc_indent_change(0);
             BEGIN(FINISHING);
             unput('\n');
             if(t<0) return DEDENT;
           }
<FINISHING>\n {
             int t = calc_indent_change(0);
             unput('\n');
             if(t<0) return DEDENT;

             if( !given_endmarker ) { 
               given_endmarker = true; 
               return ENDMARKER; 
             }

             yyterminate();
           }


<LONG_STRING,LONG_STRING2><<EOF>> 
           { 
             display_error("unterminated long string at EOF"); 
           }
<*>.       { display_error("unexpected character"); }


%%

void 
init_scanner(FILE *input_file)
{
  yyin = input_file;
}

static void display_error(const char *msg) 
{ 
    fprintf(stderr, "%d: lexical error with [%s]: %s\n", 
            yylineno-1, yytext, msg);
    exit(EXIT_FAILURE);
}

/*                         { Z+ | iff indenting
 * calc_indent_change(thisIndent) -> { 0  | iff no indentation
 *                         { Z- | iff dedenting
 */
static int calc_indent_change(const int thisIndent) 
{ 
    int delta  = thisIndent-curr_indent;
    if (delta == 0 || encmatching > 0) {
        return 0; /* No indent */
    }
    if (delta >= 0) { /* Positive indent */
        curr_indent = thisIndent;
        indentations.push(delta);
        return delta;
    }
    /* Else, dedent. Possibly many */
    if ( indentations.empty() ) { /* Indentation mismatch */
        display_error("(internal) indentation stack underflow");
    }
    int prev_indent = indentations.top(); 
    indentations.pop();
    if ( prev_indent > -delta ) { /* Indentation mismatch */
        display_error("dedent is less than corresponding indent");
    }
    curr_indent -= prev_indent;
    return -prev_indent;
}

static int iced(const int t) 
{ 
    return t<0 ? DEDENT : INDENT; 
}

static int handle_indent(const int num_spaces)
{
    int t = calc_indent_change(num_spaces);
    if (t>=0) {
        BEGIN(INITIAL);
    } 
    if (t<0 || num_spaces==0) { /* undigest the token and go again */
        yyless(0);
        unput('\n'); 
        yyinput();
    }
    return t!=0 ? iced(t) : -1; 
}


static void left_enclose(void)
{
    ++encmatching;
}

static void right_enclose(void)
{
    --encmatching;
}

