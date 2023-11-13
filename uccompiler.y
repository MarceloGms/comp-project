%{
#include <stdio.h>
%}

%token CHAR INT VOID SHORT DOUBLE
%token IDENTIFIER NATURAL CHRLIT DECIMAL
%token LPAR RPAR LBRACE RBRACE SEMI COMMA ASSIGN PLUS MINUS MUL DIV MOD
%token OR AND BITWISEAND BITWISEOR BITWISEXOR EQ NE LE GE LT GT NOT IF ELSE WHILE RETURN

%start FunctionsAndDeclarations

%%

FunctionsAndDeclarations:
    | FunctionDefinition FunctionsAndDeclarations
    | FunctionDeclaration FunctionsAndDeclarations
    | Declaration FunctionsAndDeclarations
    ;

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

FunctionDeclarator:
    IDENTIFIER LPAR ParameterList RPAR
    ;

ParameterList:
    ParameterDeclaration {COMMA ParameterDeclaration}
    ;

ParameterDeclaration:
    TypeSpec [IDENTIFIER]
    ;

Declaration:
    TypeSpec Declarator {COMMA Declarator} SEMI
    ;

TypeSpec:
    CHAR | INT | VOID | SHORT | DOUBLE
    ;

Declarator:
    IDENTIFIER ASSIGN Expr
    ;

Statement:
    Expr SEMI
    | LBRACE {Statement} RBRACE
    | IF LPAR Expr RPAR Statement ELSE Statement
    | WHILE LPAR Expr RPAR Statement
    | RETURN [Expr] SEMI
    ;

Expr:
    Expr ASSIGN | COMMA Expr
    | Expr PLUS | MINUS | MUL | DIV | MOD Expr
    | Expr OR | AND | BITWISEAND | BITWISEOR | BITWISEXOR Expr
    | Expr EQ | NE | LE | GE | LT | GT Expr
    | PLUS | MINUS | NOT Expr
    | IDENTIFIER LPAR Expr {COMMA Expr} RPAR
    | IDENTIFIER | NATURAL | CHRLIT | DECIMAL
    | LPAR Expr RPAR
    ;



%%
 
void yyerror(char *error) {
    printf("%s '%s'\n", error, yytext);
} 