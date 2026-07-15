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
(?i:"fillpreserve")			{ return FILLPRESERVE; }
(?i:"rotate")			{ return ROTATE; }
(?i:"translate")			{ return TRANSLATE; }
(?i:"if")			{ return IF; }
(?i:"else")			{ return ELSE; }
(?i:"for")			{ return FORLOOP; }

[0-9]+(\.[0-9]+)? {
    yylval.number = atof(yytext);
    return NUMBER;
}

[a-zA-Z_][a-zA-Z0-9_]* {
    yylval.string = strdup(yytext);
    return IDENTIFIER;
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