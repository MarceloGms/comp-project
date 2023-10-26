%{
// Marcelo Gomes 2021222994
// Pedro Brites 2021226319

#include <ctype.h>

int col = 0;
int line = 1;
int print_tokens = 0;

char * uppercase(char * s) {
    for (char *p = s; *p; p++) *p = toupper(*p);
    return s;
  }
%}
%X COMMENT

digit   [0-9]
reserved int|double|if|else|char|while|short|return|void
RESERVED "["|"]"|"++"|"--"|"auto"|"break"|"case"|"const"|"continue"|"default"|"do"|"enum"|"extern"|"float"|"for"|"goto"|"inline"|"long"|"register"|"restrict"|"signed"|"sizeof"|"static"|"struct"|"switch"|"typedef"|"union"|"unsigned"|"volatile"|"_Bool"|"_Complex"|"_Imaginary"

%%
{RESERVED}                                            { if(print_tokens) printf("RESERVED(%s)\n", yytext); col += yyleng; }
{reserved}                                            { if(print_tokens) printf("%s\n", uppercase(yytext)); col += yyleng; }

{digit}+                                              { col += yyleng; printf("NATURAL(%s)\n", yytext); }
(({digit}*"."{digit}+([eE][+-]?{digit}+)?)|({digit}*"."([eE][+-]?{digit}+))|({digit}+([eE][+-]?{digit}+)))             { col += yyleng; if(print_tokens) printf("DECIMAL(%s)\n", yytext); }
[_a-zA-Z][_a-zA-Z0-9]*                                { col += yyleng; if(print_tokens) printf("IDENTIFIER(%s)\n", yytext); }
'([^'\n]|(\\n|\\t|\"|\\)|\\[0-7]{1,3})'               { col += yyleng; if(print_tokens) printf("CHRLIT(%s)\n", yytext); }
"/*"                                                  { col += yyleng; BEGIN(COMMENT); }
<COMMENT>.                                            { col += yyleng; }
<COMMENT>\n                                           { line++; col = 0; }
<COMMENT>"*/"                                         { col += yyleng; BEGIN(INITIAL); }
<COMMENT>                                             { col += yyleng; printf("Line %d, column %d: unterminated comment\n", line, col); }
\/\/.*                                                { col += yyleng; }
\n			                                          { line++; col = 0;}
\t	         	                                      { col += 1; }
[ ]			                                          { col += 1; }

"&"                                                   { if(print_tokens) printf("BITWISEAND\n"); col += yyleng; }
"|"                                                   { if(print_tokens) printf("BITWISEOR\n"); col += yyleng; }
"^"                                                   { if(print_tokens) printf("BITWISEXOR\n"); col += yyleng; }
"&&"                                                  { if(print_tokens) printf("AND\n"); col += yyleng; }
"=="                                                  { if(print_tokens) printf("EQ\n"); col += yyleng; }
">="                                                  { if(print_tokens) printf("GE\n"); col += yyleng; }
">"                                                   { if(print_tokens) printf("GT\n"); col += yyleng; }
"<="                                                  { if(print_tokens) printf("LE\n"); col += yyleng; }
"<"                                                   { if(print_tokens) printf("LT\n"); col += yyleng; }
"{"                                                   { if(print_tokens) printf("LBRACE\n"); col += yyleng; }
"}"                                                   { if(print_tokens) printf("RBRACE\n"); col += yyleng; }
"%"                                                   { if(print_tokens) printf("MOD\n"); col += yyleng; }
"!="                                                  { if(print_tokens) printf("NE\n"); col += yyleng; }
"!"                                                   { if(print_tokens) printf("NOT\n"); col += yyleng; }
"||"                                                  { if(print_tokens) printf("OR\n"); col += yyleng; }
";"                                                   { if(print_tokens) printf("SEMI\n"); col += yyleng; }
"("                                                   { if(print_tokens) printf("LPAR\n"); col += yyleng; }
")"                                                   { if(print_tokens) printf("RPAR\n"); col += yyleng; }
"-"                                                   { if(print_tokens) printf("MINUS\n"); col += yyleng; }
"+"                                                   { if(print_tokens) printf("PLUS\n"); col += yyleng; }
"="                                                   { if(print_tokens) printf("ASSIGN\n"); col += yyleng; }
"*"                                                   { if(print_tokens) printf("MUL\n"); col += yyleng; }
","                                                   { if(print_tokens) printf("COMMA\n"); col += yyleng; }
"/"                                                   { if(print_tokens) printf("DIV\n"); col += yyleng; }

.			                                          { col += yyleng; printf("Line %d, column %d: unrecognized character(%s)\n", line, col, yytext); }
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