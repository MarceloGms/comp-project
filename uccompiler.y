%{
#include <stdio.h>
extern int yylex(void);
void yyerror(char *);
extern char *yytext;
%}

%token CHAR INT VOID SHORT DOUBLE
%token IDENTIFIER NATURAL CHRLIT DECIMAL
%token LPAR RPAR LBRACE RBRACE SEMI COMMA ASSIGN PLUS MINUS MUL DIV MOD
%token OR AND BITWISEAND BITWISEOR BITWISEXOR EQ NE LE GE LT GT NOT IF ELSE WHILE RETURN

%union {char *str;}

%start FunctionsAndDeclarations

%%

FunctionsAndDeclarations: FunctionsAndDeclarations FunctionDeclaration
                      | FunctionsAndDeclarations FunctionDefinition
                      | FunctionsAndDeclarations Declaration
                      | FunctionDeclaration
                      | FunctionDefinition
                      | Declaration

FunctionDefinition:
    TypeSpec FunctionDeclarator FunctionBody
    ;

FunctionBody:
    LBRACE DeclarationsAndStatements RBRACE
    ;

DeclarationsAndStatements:
    Statement DeclarationsAndStatements
    | Declaration DeclarationsAndStatements
    | Statement
    | Declaration
    ;

FunctionDeclaration:
    TypeSpec FunctionDeclarator SEMI
    ;

FunctionDeclarator: IDENTIFIER LPAR ParameterList RPAR
    ;

ParameterList:
    ParameterDeclaration;
    ;

ParameterDeclaration: COMMA ParameterDeclaration
                    | TypeSpec IDENTIFIER
                    | TypeSpec
                    ;

Declaration:
    TypeSpec Declarator SEMI
    ;

TypeSpec:
    CHAR | INT | VOID | SHORT | DOUBLE
    ;

Declarator: COMMA Declarator
            | IDENTIFIER ASSIGN Expr
            | IDENTIFIER
    ;

Statement: Statement
        | Expr SEMI
        | SEMI
        | LBRACE Statement RBRACE
        | IF LPAR Expr RPAR Statement ELSE Statement
        | IF LPAR Expr RPAR Statement
        | WHILE LPAR Expr RPAR Statement
        | RETURN Expr SEMI
        | RETURN SEMI
        ;

Expr: 
    Expr ASSIGN Expr
    | Expr COMMA Expr
    | Expr PLUS Expr
    | Expr MINUS Expr
    | Expr MUL Expr
    | Expr DIV Expr
    | Expr MOD Expr
    | Expr OR Expr
    | Expr AND Expr
    | Expr BITWISEAND Expr
    | Expr BITWISEOR Expr
    | Expr BITWISEXOR Expr
    | Expr EQ Expr
    | Expr NE Expr
    | Expr LE Expr
    | Expr GE Expr
    | Expr LT Expr
    | Expr GT Expr
    | PLUS Expr
    | MINUS Expr
    | NOT Expr
    | IDENTIFIER LPAR RPAR
    | IDENTIFIER LPAR Expr RPAR
    | IDENTIFIER LPAR Expr COMMA Expr RPAR
    | IDENTIFIER 
    | NATURAL 
    | CHRLIT
    | DECIMAL
    | LPAR Expr RPAR
    ;

%%
 

