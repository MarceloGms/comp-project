/* Marcelo Gomes 2021222994 */
/* Pedro Brites 2021226319 */

%{
#include <ctype.h>

int col = 0;
int line = 1;
int l = 0;
int com_line = 0;
int com_col = 0;

char * uppercase(char * s) {
    for (char *p = s; *p; p++) *p = toupper(*p);
    return s;
  }
%}

%X COMMENT

digit   [0-9]
decimal (({digit}*"."{digit}+([eE][+-]?{digit}+)?)|({digit}*"."([eE][+-]?{digit}+))|({digit}+([eE][+-]?{digit}+))|{digit}+".")
reserved int|double|if|else|char|while|short|return|void
RESERVED "["|"]"|"++"|"--"|"auto"|"break"|"case"|"const"|"continue"|"default"|"do"|"enum"|"extern"|"float"|"for"|"goto"|"inline"|"long"|"register"|"restrict"|"signed"|"sizeof"|"static"|"struct"|"switch"|"typedef"|"union"|"unsigned"|"volatile"|"_Bool"|"_Complex"|"_Imaginary"

%%
{RESERVED}                                            { col += yyleng; if(l) printf("RESERVED(%s)\n", yytext); }
{reserved}                                            { col += yyleng; if(l) printf("%s\n", uppercase(yytext)); }

{digit}+                                              { col += yyleng; if(l) printf("NATURAL(%s)\n", yytext); }
{decimal}                                             { col += yyleng; if(l) printf("DECIMAL(%s)\n", yytext); }
[_a-zA-Z][_a-zA-Z0-9]*                                { col += yyleng; if(l) printf("IDENTIFIER(%s)\n", yytext); }
'([^'\n]|(\\n|\\t|\\\"|\\\\|\\\')|\\[0-7]{1,3})'      { col += yyleng; if(l) printf("CHRLIT(%s)\n", yytext); }
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
"&"                                                   { col += yyleng; if(l) printf("BITWISEAND\n"); }
"|"                                                   { col += yyleng; if(l) printf("BITWISEOR\n"); }
"^"                                                   { col += yyleng; if(l) printf("BITWISEXOR\n"); }
"&&"                                                  { col += yyleng; if(l) printf("AND\n"); }
"=="                                                  { col += yyleng; if(l) printf("EQ\n"); }
">="                                                  { col += yyleng; if(l) printf("GE\n"); }
">"                                                   { col += yyleng; if(l) printf("GT\n"); }
"<="                                                  { col += yyleng; if(l) printf("LE\n"); }
"<"                                                   { col += yyleng; if(l) printf("LT\n"); }
"{"                                                   { col += yyleng; if(l) printf("LBRACE\n"); }
"}"                                                   { col += yyleng; if(l) printf("RBRACE\n"); }
"%"                                                   { col += yyleng; if(l) printf("MOD\n"); }
"!="                                                  { col += yyleng; if(l) printf("NE\n"); }
"!"                                                   { col += yyleng; if(l) printf("NOT\n"); }
"||"                                                  { col += yyleng; if(l) printf("OR\n"); }
";"                                                   { col += yyleng; if(l) printf("SEMI\n"); }
"("                                                   { col += yyleng; if(l) printf("LPAR\n"); }
")"                                                   { col += yyleng; if(l) printf("RPAR\n"); }
"-"                                                   { col += yyleng; if(l) printf("MINUS\n"); }
"+"                                                   { col += yyleng; if(l) printf("PLUS\n"); }
"="                                                   { col += yyleng; if(l) printf("ASSIGN\n"); }
"*"                                                   { col += yyleng; if(l) printf("MUL\n"); }
","                                                   { col += yyleng; if(l) printf("COMMA\n"); }
"/"                                                   { col += yyleng; if(l) printf("DIV\n"); }

.                                                     { col += yyleng; printf("Line %d, column %d: unrecognized character (%s)\n", line, col, yytext); }
%%
extern int yylex();
int main(int argc, char **argv) {
    if (argc > 1 && strcmp(argv[1], "-l") == 0){
        l = 1;
    }
    yylex();    /* run the lexical analysis automaton */
    return 0;
}
int yywrap() {  /* called on EOF, return 1 to terminate */
    return 1;
}