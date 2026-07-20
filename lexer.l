%{
#include "parser.tab.h"
#include <stdlib.h>
#include <string.h>
%}
%option yylineno
%option debug
%%

[ \t\r\n]+          ;

(?i:"rectangle")         { return RECTANGLE; }
(?i:"arc")            { return ARC; }
(?i:"stroke")		{ return STROKE; }
(?i:"strokepreserve")		{ return STROKEPRESERVE; }
(?i:"color")		{ return COLOR; }
(?i:"linewidth")	{ return LINEWIDTH; }
(?i:"fill")			{ return FILL; }
(?i:"fillpreserve")	{ return FILLPRESERVE; }
(?i:"rotate")		{ return ROTATE; }
(?i:"translate")	{ return TRANSLATE; }
(?i:"moveto")		{ return MOVETO; }
(?i:"if")			{ return IF; }
(?i:"else")			{ return ELSE; }
(?i:"for")			{ return FORLOOP; }
(?i:"while")		{ return WHILETRUE; }
(?i:"fontsize")		{ return FONTSIZE; }
(?i:"font")			{ return FONT; }
(?i:"text")			{ return TEXT; }
(?i:"textpath")		{ return TEXTPATH; }
(?i:"pi")			{ return PI; }
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