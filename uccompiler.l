/* Marcelo Gomes 2021222994 */
/* Pedro Brites 2021226319 */

%{
#include <ctype.h>
#include <string.h>
#include "y.tab.h"
#include "ast.h"

int col_lex = 0;
int line_lex = 1;
int l = 0;
int com_line = 0;
int com_col = 0;
int col_syn = -1;
int line_syn = -1;
int errors = 0;

extern struct node *program;
%}

%x COMMENT

digit   [0-9]
decimal (({digit}*"."{digit}+([eE][+-]?{digit}+)?)|({digit}*"."([eE][+-]?{digit}+))|({digit}+([eE][+-]?{digit}+))|{digit}+".")
RESERVED "["|"]"|"++"|"--"|"auto"|"break"|"case"|"const"|"continue"|"default"|"do"|"enum"|"extern"|"float"|"for"|"goto"|"inline"|"long"|"register"|"restrict"|"signed"|"sizeof"|"static"|"struct"|"switch"|"typedef"|"union"|"unsigned"|"volatile"|"_Bool"|"_Complex"|"_Imaginary"

%%
{RESERVED}                                            { col_lex += yyleng; if(l) printf("RESERVED(%s)\n", yytext); }
"int"                                                 { col_lex += yyleng; if(l) printf("(%s)\n", yytext); return INT; }
"double"                                              { col_lex += yyleng; if(l) printf("(%s)\n", yytext); return DOUBLE; }
"if"                                                  { col_lex += yyleng; if(l) printf("(%s)\n", yytext); return IF; }
"else"                                                { col_lex += yyleng; if(l) printf("(%s)\n", yytext); return ELSE; }
"char"                                                { col_lex += yyleng; if(l) printf("(%s)\n", yytext); return CHAR; }
"while"                                               { col_lex += yyleng; if(l) printf("(%s)\n", yytext); return WHILE; }
"short"                                               { col_lex += yyleng; if(l) printf("(%s)\n", yytext); return SHORT; }
"return"                                              { col_lex += yyleng; if(l) printf("(%s)\n", yytext); return RETURN; }
"void"                                                { col_lex += yyleng; if(l) printf("(%s)\n", yytext); return VOID; }

{digit}+                                              { col_lex += yyleng; if(l) printf("NATURAL(%s)\n", yytext); yylval.str = strdup(yytext); return NATURAL; }
{decimal}                                             { col_lex += yyleng; if(l) printf("DECIMAL(%s)\n", yytext); yylval.str = strdup(yytext); return DECIMAL; }
[_a-zA-Z][_a-zA-Z0-9]*                                { col_lex += yyleng; if(l) printf("IDENTIFIER(%s)\n", yytext); yylval.str = strdup(yytext); return IDENTIFIER; }
'([^'\n]|(\\n|\\t|\\\"|\\\\|\\\')|\\[0-7]{1,3})'      { col_lex += yyleng; if(l) printf("CHRLIT(%s)\n", yytext); yylval.str = strdup(yytext); return CHRLIT; }
'(([^'\n]|(\\n|\\t|\\\"|\\\\|\\\')|\\[0-7]{1,3})|\\[^\n'])*'                                                    { col_lex += yyleng; printf("Line %d, column %d: invalid char constant (%s)\n", line_lex, col_lex, yytext); }
'([^'\n]|(\\n|\\t|\\\"|\\\\|\\\')|\\[0-7]{1,3})       { col_lex += yyleng; printf("Line %d, column %d: unterminated char constant\n", line_lex, col_lex); }

"/*"                                                  { col_lex += yyleng; BEGIN(COMMENT); com_line = line_lex; com_col = col_lex - 1; }
<COMMENT>.                                            { col_lex += yyleng; }
<COMMENT>\n                                           { line_lex++; col_lex = 0; }
<COMMENT>"*/"                                         { col_lex += yyleng; BEGIN(INITIAL); }
<COMMENT><<EOF>>                                      { col_lex += yyleng; printf("Line %d, column %d: unterminated comment\n", com_line, com_col); BEGIN(INITIAL);}
\/\/.*                                                { col_lex += yyleng; }

\n|\r\n|\r                                            { line_lex++; col_lex = 0;}
\t                                                    { col_lex += 1; }
[ ]                                                   { col_lex += 1; }
"&"                                                   { col_lex += yyleng; if(l) printf("BITWISEAND\n"); return BITWISEAND; }
"|"                                                   { col_lex += yyleng; if(l) printf("BITWISEOR\n"); return BITWISEOR; }
"^"                                                   { col_lex += yyleng; if(l) printf("BITWISEXOR\n"); return BITWISEXOR; }
"&&"                                                  { col_lex += yyleng; if(l) printf("AND\n"); return AND; }
"=="                                                  { col_lex += yyleng; if(l) printf("EQ\n"); return EQ; }
">="                                                  { col_lex += yyleng; if(l) printf("GE\n"); return GE; }
">"                                                   { col_lex += yyleng; if(l) printf("GT\n"); return GT; }
"<="                                                  { col_lex += yyleng; if(l) printf("LE\n"); return LE; }
"<"                                                   { col_lex += yyleng; if(l) printf("LT\n"); return LT; }
"{"                                                   { col_lex += yyleng; if(l) printf("LBRACE\n"); return LBRACE; }
"}"                                                   { col_lex += yyleng; if(l) printf("RBRACE\n"); return RBRACE; }
"%"                                                   { col_lex += yyleng; if(l) printf("MOD\n"); return MOD; }
"!="                                                  { col_lex += yyleng; if(l) printf("NE\n"); return NE; }
"!"                                                   { col_lex += yyleng; if(l) printf("NOT\n"); return NOT; }
"||"                                                  { col_lex += yyleng; if(l) printf("OR\n"); return OR; }
";"                                                   { col_lex += yyleng; if(l) printf("SEMI\n"); return SEMI; }
"("                                                   { col_lex += yyleng; if(l) printf("LPAR\n"); return LPAR; }
")"                                                   { col_lex += yyleng; if(l) printf("RPAR\n"); return RPAR; }
"-"                                                   { col_lex += yyleng; if(l) printf("MINUS\n"); return MINUS; }
"+"                                                   { col_lex += yyleng; if(l) printf("PLUS\n"); return PLUS; }
"="                                                   { col_lex += yyleng; if(l) printf("ASSIGN\n"); return ASSIGN; }
"*"                                                   { col_lex += yyleng; if(l) printf("MUL\n"); return MUL; }
","                                                   { col_lex += yyleng; if(l) printf("COMMA\n"); return COMMA; }
"/"                                                   { col_lex += yyleng; if(l) printf("DIV\n"); return DIV; }

.                                                     { col_lex += yyleng; printf("Line %d, column %d: unrecognized character (%s)\n", line_lex, col_lex, yytext); }
%%

int main(int argc, char **argv) {
    if (argc > 1 && strcmp(argv[1], "-l") == 0){
        l = 1;
    }
    yyparse();
    if(argc > 1 && strcmp(argv[1], "-t") == 0 && errors == 0) {
        show(program, 0);
    }
    deallocate(program);
    yylex_destroy();  
    return 0;
}

void yyerror(char *s) {
    errors++;
    printf ( "Line %d , column %d : %s : %s \n " , line_syn , col_syn , s , yytext );
} 