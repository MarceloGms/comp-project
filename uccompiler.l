%{
#include <ctype.h>
#include <string.h>
#include "y.tab.h"
%}

%X COMMENT

digit   [0-9]
decimal (({digit}*"."{digit}+([eE][+-]?{digit}+)?)|({digit}*"."([eE][+-]?{digit}+))|({digit}+([eE][+-]?{digit}+))|{digit}+".")
reserved int|double|if|else|char|while|short|return|void
RESERVED "["|"]"|"++"|"--"|"auto"|"break"|"case"|"const"|"continue"|"default"|"do"|"enum"|"extern"|"float"|"for"|"goto"|"inline"|"long"|"register"|"restrict"|"signed"|"sizeof"|"static"|"struct"|"switch"|"typedef"|"union"|"unsigned"|"volatile"|"_Bool"|"_Complex"|"_Imaginary"

%%
{RESERVED}                                            { col += yyleng; if(l) printf("RESERVED(%s)\n", yytext); return reserved; }
{reserved}                                            { col += yyleng; if(l) printf("%s\n", uppercase(yytext)); return reserved; }

{digit}+                                              { col += yyleng; if(l) printf("NATURAL(%s)\n", yytext); yylval.str = strdup(yytext); return NATURAL; }
{decimal}                                             { col += yyleng; if(l) printf("DECIMAL(%s)\n", yytext); yylval.str = strdup(yytext); return DECIMAL; }
[_a-zA-Z][_a-zA-Z0-9]*                                { col += yyleng; if(l) printf("IDENTIFIER(%s)\n", yytext); yylval.str = strdup(yytext); return IDENTIFIER; }
'([^'\n]|(\\n|\\t|\\\"|\\\\|\\\')|\\[0-7]{1,3})'      { col += yyleng; if(l) printf("CHRLIT(%s)\n", yytext); yylval.str = strdup(yytext); return CHRLIT; }
''                                                    { col += yyleng; printf("Line %d, column %d: invalid char constant (%s)\n", line, col, yytext); }
'([^'\n]|(\\n|\\t|\\\"|\\\\|\\\')|\\[0-7]{1,3})       { col += yyleng; printf("Line %d, column %d: unterminated char constant\n", line, col); }

"/*"                                                  { col += yyleng; BEGIN(COMMENT); com_line = line; com_col = col - 1; }
<COMMENT>.                                            { col += yyleng; }
<COMMENT>\n                                           { line++; col = 0; }
<COMMENT>"*/"                                         { col += yyleng; BEGIN(INITIAL); }
<COMMENT><<EOF>>                                      { col += yyleng; printf("Line %d, column %d: unterminated comment\n", com_line, com_col); BEGIN(INITIAL);}
\/\/.*                                                { col += yyleng; }

\n|\r\n|\r                                            { line++; col = 0;}
\t                                                    { col += 1; }
[ ]                                                   { col += 1; }
"&"                                                   { col += yyleng; if(l) printf("BITWISEAND\n"); return BITWISEAND; }
"|"                                                   { col += yyleng; if(l) printf("BITWISEOR\n"); return BITWISEOR; }
"^"                                                   { col += yyleng; if(l) printf("BITWISEXOR\n"); return BITWISEXOR; }
"&&"                                                  { col += yyleng; if(l) printf("AND\n"); return AND; }
"=="                                                  { col += yyleng; if(l) printf("EQ\n"); return EQ; }
">="                                                  { col += yyleng; if(l) printf("GE\n"); return GE; }
">"                                                   { col += yyleng; if(l) printf("GT\n"); return GT; }
"<="                                                  { col += yyleng; if(l) printf("LE\n"); return LE; }
"<"                                                   { col += yyleng; if(l) printf("LT\n"); return LT; }
"{"                                                   { col += yyleng; if(l) printf("LBRACE\n"); return LBRACE; }
"}"                                                   { col += yyleng; if(l) printf("RBRACE\n"); return RBRACE; }
"%"                                                   { col += yyleng; if(l) printf("MOD\n"); return MOD; }
"!="                                                  { col += yyleng; if(l) printf("NE\n"); return NE; }
"!"                                                   { col += yyleng; if(l) printf("NOT\n"); return NOT; }
"||"                                                  { col += yyleng; if(l) printf("OR\n"); return OR; }
";"                                                   { col += yyleng; if(l) printf("SEMI\n"); return SEMI; }
"("                                                   { col += yyleng; if(l) printf("LPAR\n"); return LPAR; }
")"                                                   { col += yyleng; if(l) printf("RPAR\n"); return RPAR; }
"-"                                                   { col += yyleng; if(l) printf("MINUS\n"); return MINUS; }
"+"                                                   { col += yyleng; if(l) printf("PLUS\n"); return PLUS; }
"="                                                   { col += yyleng; if(l) printf("ASSIGN\n"); return ASSIGN; }
"*"                                                   { col += yyleng; if(l) printf("MUL\n"); return MUL; }
","                                                   { col += yyleng; if(l) printf("COMMA\n"); return COMMA; }
"/"                                                   { col += yyleng; if(l) printf("DIV\n"); return DIV; }

.                                                     { col += yyleng; printf("Line %d, column %d: unrecognized character (%s)\n", line, col, yytext); }
%%
extern int yylex();
extern int yylval;
extern int yywrap();
int yylineno = 1;  // To keep track of line numbers

int main(int argc, char **argv) {
    if (argc > 1 && strcmp(argv[1], "-l") == 0){
        l = 1;
    }
    yyparse();   // Use the Yacc parser
    return 0;
}
int yywrap() {  // called on EOF, return 1 to terminate
    return 1;
}
