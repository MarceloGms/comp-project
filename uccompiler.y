%{
#include <stdio.h>
#include "ast.h"
extern int yylex(void);
void yyerror(char *);
struct node *program;
%}

%token CHAR INT VOID SHORT DOUBLE
%token IDENTIFIER NATURAL CHRLIT DECIMAL
%token LPAR RPAR LBRACE RBRACE SEMI COMMA ASSIGN PLUS MINUS MUL DIV MOD
%token OR AND BITWISEAND BITWISEOR BITWISEXOR EQ NE LE GE LT GT NOT IF ELSE WHILE RETURN

%nonassoc LOW
%nonassoc ELSE
%nonassoc COMMA
%left ASSIGN
%left OR
%left AND
%left BITWISEOR
%left BITWISEXOR
%left BITWISEAND
%left EQ NE
%left LE GE LT GT
%left PLUS MINUS
%left MUL DIV MOD
%right NOT

%union {char *str;}

%start FunctionsAndDeclarations

%%

FunctionsAndDeclarations: FunctionsAndDeclarations FunctionDeclaration
                        | FunctionsAndDeclarations FunctionDefinition
                        | FunctionsAndDeclarations Declaration
                        | FunctionDeclaration
                        | FunctionDefinition
                        | Declaration
                        ;

FunctionDefinition: TypeSpec FunctionDeclarator FunctionBody
                  ;

FunctionBody: LBRACE RBRACE
            | LBRACE DeclarationsAndStatements RBRACE
            ;

DeclarationsAndStatements: Statement DeclarationsAndStatements
                         | Declaration DeclarationsAndStatements
                         | Statement
                         | Declaration
                         ;

FunctionDeclaration: TypeSpec FunctionDeclarator SEMI
                   ;

FunctionDeclarator: IDENTIFIER LPAR ParameterList RPAR
                  ;

//*
ParameterList: ParameterDeclaration   {}
             | ParameterList COMMA ParameterDeclaration {}
             ;

ParameterDeclaration: TypeSpec IDENTIFIER
                    | TypeSpec
                    ;

Declaration: TypeSpec DeclarationList SEMI  {}
           ;

//*
DeclarationList: Declarator {}
               | DeclarationList COMMA Declarator {}
               ;

TypeSpec: CHAR 
        | INT 
        | VOID 
        | SHORT 
        | DOUBLE
        ;

Declarator: IDENTIFIER
          | IDENTIFIER ASSIGN Expr
          ;

Statement: Expr SEMI  {}
         | SEMI {}
         | LBRACE RBRACE
         | LBRACE StatementList RBRACE  {}
         | IfStatement 
         | WHILE LPAR Expr RPAR Statement   {}
         | RETURN Expr SEMI {}
         | RETURN SEMI  {}
         ;

//
IfStatement: IF LPAR Expr RPAR Statement %prec LOW
           | IF LPAR Expr RPAR Statement ELSE Statement
           ;

//*
StatementList: Statement  {}
             | StatementList Statement  {}
             ;

Expr: Expr ASSIGN Expr                                              { ; }
    | Expr COMMA Expr                                               { ; }
    | Expr PLUS Expr                                                { ; }
    | Expr MINUS Expr                                               { ; }
    | Expr MUL Expr                                                 { ; }
    | Expr DIV Expr                                                 { ; }
    | Expr MOD Expr                                                 { ; }
    | Expr OR Expr                                                  { ; }
    | Expr AND Expr                                                 { ; }
    | Expr BITWISEAND Expr                                          { ; }
    | Expr BITWISEOR Expr                                           { ; }
    | Expr BITWISEXOR Expr                                          { ; }
    | Expr EQ Expr                                                  { ; }
    | Expr NE Expr                                                  { ; }
    | Expr LE Expr                                                  { ; }
    | Expr GE Expr                                                  { ; }
    | Expr LT Expr                                                  { ; }
    | Expr GT Expr                                                  { ; }
    | PLUS Expr                                                     { ; }
    | MINUS Expr                                                    { ; }
    | NOT Expr                                                      { ; }
    | IDENTIFIER LPAR ExprList RPAR                                 { ; }
    | IDENTIFIER LPAR RPAR                                          { ; }
    | IDENTIFIER                                                    { ; }
    | NATURAL                                                       { ; }
    | CHRLIT                                                        { ; }
    | DECIMAL                                                       { ; }
    | LPAR Expr RPAR                                                { ; }
    ;

//*
ExprList: Expr %prec COMMA {}
        | ExprList COMMA Expr %prec COMMA {}
        ;

%%
