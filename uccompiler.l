%{
// Marcelo Gomes 2021222994
// Pedro Brites 2021226319

#include <ctype.h>

int col = 0;
int line = 1;

char * uppercase(char * s) {
    for (char *p = s; *p; p++) *p = toupper(*p);
    return s;
  }
%}

digit   [0-9]
reserved int|double|if|else|char|while|short|return|void
RESERVED "["|"]"|"++"|"--"|"auto"|"break"|"case"|"const"|"continue"|"default"|"do"|"enum"|"extern"|"float"|"for"|"goto"|"inline"|"long"|"register"|"restrict"|"signed"|"sizeof"|"static"|"struct"|"switch"|"typedef"|"union"|"unsigned"|"volatile"|"_Bool"|"_Complex"|"_Imaginary"

%%
{RESERVED}                                            { printf("RESERVED(%s)\n", yytext); col += yyleng; }
{reserved}                                            { printf("%s\n", uppercase(yytext)); col += yyleng; }

"&"                                                   { printf("BITWISEAND\n"); col += yyleng; }
"|"                                                   { printf("BITWISEOR\n"); col += yyleng; }
"^"                                                   { printf("BITWISEXOR\n"); col += yyleng; }
"&&"                                                  { printf("AND\n"); col += yyleng; }
"=="                                                  { printf("EQ\n"); col += yyleng; }
">="                                                  { printf("GE\n"); col += yyleng; }
">"                                                   { printf("GT\n"); col += yyleng; }
"<="                                                  { printf("LE\n"); col += yyleng; }
"<"                                                   { printf("LT\n"); col += yyleng; }
"{"                                                   { printf("LBRACE\n"); col += yyleng; }
"}"                                                   { printf("RBRACE\n"); col += yyleng; }
"%"                                                   { printf("MOD\n"); col += yyleng; }
"!="                                                  { printf("NE\n"); col += yyleng; }
"!"                                                   { printf("NOT\n"); col += yyleng; }
"||"                                                  { printf("OR\n"); col += yyleng; }
";"                                                   { printf("SEMI\n"); col += yyleng; }
"("                                                   { printf("LPAR\n"); col += yyleng; }
")"                                                   { printf("RPAR\n"); col += yyleng; }
"-"                                                   { printf("MINUS\n"); col += yyleng; }
"+"                                                   { printf("PLUS\n"); col += yyleng; }
"="                                                   { printf("ASSIGN\n"); col += yyleng; }
"*"                                                   { printf("MUL\n"); col += yyleng;}
","                                                   { printf("COMMA\n"); col += yyleng;}
"/"                                                   { printf("DIV\n"); col += yyleng;}

{digit}+                                              { col += yyleng; printf("NATURAL(%s)\n", yytext); }
{digit}*"."{digit}*[[eE][+-]{digit}+]*                { col += yyleng; printf("DECIMAL(%s)\n", yytext); }
[_a-zA-Z][_a-zA-Z0-9]*                                { col += yyleng; printf("IDENTIFIER(%s)\n", yytext); }
'([^'^\n]|(\\n|\\t|\"|\\)|\\[0-7]{1,3})'              { col += yyleng; printf("CHRLIT(%s)\n", yytext); }

\n			                                          { line++; col = 0;}
\t	         	                                      { col += 1; }
[ ]			                                          { col += 1; }
.			                                          { col += yyleng; printf("Unrecognized character '%s' (line %d, column %d)\n", yytext, line, col); }
%%
extern int yylex();
int main(int argc, char **argv) {
    if (argc > 1 && strcmp(argv[1], "-l") == 0){
        yylex();    /* run the lexical analysis automaton */
    }
    return 0;
}
int yywrap() {  /* called on EOF, return 1 to terminate */
    return 1;
}