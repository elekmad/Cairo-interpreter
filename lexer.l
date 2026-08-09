%option header-file="lexer.h"
%option noyywrap
%option yylineno
%{
#include "parser.tab.h"
#include <stdlib.h>
#include <string.h>

int yycolumn = 1;

#define YY_USER_ACTION                     \
    yylloc.first_line = yylineno;         \
    yylloc.last_line  = yylineno;         \
    yylloc.first_column = yycolumn;       \
    yylloc.last_column  = yycolumn + yyleng - 1; \
    yycolumn += yyleng;
%}

/* %option debug */
%%

\/\/[^\n]*				;
 
\n {
    yycolumn = 1;
}

[ \t\r\n]+          ;

(?i:"rectangle")	{ return RECTANGLE; }
(?i:"setbgcolor")	{ return SETBGCOLOR; }
(?i:"setautostrokecolor")	{ return SETAUTOSTROKECOLOR; }
(?i:"setautofillcolor")	{ return SETAUTOFILLCOLOR; }
(?i:"setautofill")	{ return SETAUTOFILL; }
(?i:"setautostroke")	{ return SETAUTOSTROKE; }
(?i:"setautofillstroke")	{ return SETAUTOFILLSTROKE; }
(?i:"setautostrokefill")	{ return SETAUTOSTROKEFILL; }
(?i:"arc")			{ return ARC; }
(?i:"arcneg")		{ return ARCNEG; }
(?i:"curve")		{ return CURVE; }
(?i:"stroke")		{ return STROKE; }
(?i:"strokepreserve")		{ return STROKEPRESERVE; }
(?i:"color")		{ return COLOR; }
(?i:"setlinewidth")	{ return SETLINEWIDTH; }
(?i:"getlinewidth")	{ return GETLINEWIDTH; }
(?i:"fill")			{ return FILL; }
(?i:"fillpreserve")	{ return FILLPRESERVE; }
(?i:"rotate")		{ return ROTATE; }
(?i:"translate")	{ return TRANSLATE; }
(?i:"moveto")		{ return MOVETO; }
(?i:"scale")		{ return SCALE; }
(?i:"lineto")		{ return LINETO; }
(?i:"if")			{ return IF; }
(?i:"else")			{ return ELSE; }
(?i:"for")			{ return FORLOOP; }
(?i:"foreach")		{ return FOREACH; }
(?i:"in")			{ return IN; }
(?i:"while")		{ return WHILETRUE; }
(?i:"setfontsize")	{ return FONTSIZE; }
(?i:"setfont")		{ return FONT; }
(?i:"text")			{ return TEXT; }
(?i:"textpath")		{ return TEXTPATH; }
(?i:"textextents")	{ return TEXTEXTENTS; }
(?i:"fontextents")	{ return FONTEXTENTS; }
(?i:"getoutputsize")	{ return GETOUTPUTSIZE; }
(?i:"getlength")		{ return GETLEN; }
(?i:"getlen")		{ return GETLEN; }
(?i:"getsize")		{ return GETSIZE; }
(?i:"getfirst")		{ return GETFIRST; }
(?i:"getlast")		{ return GETLAST; }
(?i:"concat")		{ return CONCAT; }
(?i:"strtod")		{ return STRTOD; }
(?i:"dtostr")		{ return DTOSTR; }
(?i:"pi")			{ return PI; }
(?i:"phi")			{ return PHI; }
(?i:"red")			{ return RED; }
(?i:"green")		{ return GREEN; }
(?i:"blue")			{ return BLUE; }
(?i:"black")		{ return BLACK; }
(?i:"white")		{ return WHITE; }
(?i:"radians")		{ return RADIANS; }
(?i:"degrees")		{ return DEGREES; }
(?i:"cos")			{ return COS; }
(?i:"acos")			{ return ACOS; }
(?i:"sin")			{ return SIN; }
(?i:"asin")			{ return ASIN; }
(?i:"tan")			{ return TAN; }
(?i:"atan")			{ return ATAN; }
(?i:"floor")		{ return FLOOR; }
(?i:"ceil")			{ return CEIL; }
(?i:"power")		{ return POWER; }
(?i:"sqrt")			{ return SQRT; }
(?i:"message")		{ return MESSAGE; } 
^(?i:"#initmessage")	{ return PREMESSAGE; }
^(?i:"#setoutputsize")	{ return SETOUTPUTSIZE; } 
^(?i:"#setpngoutput")	{ return SETPNGOUTPUT; }
^(?i:"#setsvgoutput")	{ return SETSVGOUTPUT; } 

[0-9]+(\.[0-9]+)? {
    yylval.number = atof(yytext);
    return NUMBER;
}

[a-zA-Z_][a-zA-Z0-9_]* {
    yylval.string = strdup(yytext);
    return IDENTIFIER;
}

\"[^\"]*\"  {
    yylval.string = strndup(yytext+1, strlen(yytext)-2);
    return TEXTCONTENT;
}

"<<"    return APPEND;
"<="    return LEQ;
">="    return GEQ;
"=="    return EQ;
"!="    return NEQ;
"<"     return '<';
">"     return '>';
"!"     return '!';

[+\-*/=;,(){}\[\]:] {
    return yytext[0];
}

. {
    fprintf(stderr,
            "Caractere inconnu '%c'\n",
            yytext[0]);
}

%%