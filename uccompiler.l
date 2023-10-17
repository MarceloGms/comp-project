digit   [0-9]
%{
int col = 0;
int line = 1;
%}
%%
{digit}+                { col += yyleng; printf("NATURAL(%s)\n", yytext); }
{digit}*"."{digit}*[[eE][+-]{digit}+]*    { col += yyleng; printf("DECIMAL(%s)\n", yytext); }
"integer"		{ col += yyleng; printf("INTEGER\n"); }
"double"		{ col += yyleng; printf("DOUBLE\n"); }
"if"			{ col += yyleng; printf("IF\n"); }
"then"			{ col += yyleng; printf("THEN\n"); }
"else"			{ col += yyleng; printf("ELSE\n"); }
[_a-zA-Z][_a-zA-Z0-9]*   { col += yyleng; printf("IDENTIFIER(%s)\n", yytext); }

\n			{ line++; col = 0;}
\t	         	{ col += 1; }
[ ]			{ col += 1; }
[()=,*/+-]		{ col += yyleng; printf("%s\n", yytext); }
.			{ col += yyleng; printf("Unrecognized character '%s' (line %d, column %d)\n", yytext, line, col); }
%%
extern int yylex();
int main() {
    yylex();    /* run the lexical analysis automaton */
    return 0;
}
int yywrap() {  /* called on EOF, return 1 to terminate */
    return 1;
}