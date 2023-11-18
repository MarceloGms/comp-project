%{
#include <stdio.h>
#include "ast.h"
extern int yylex(void);
void yyerror(char *);
struct node *program;
%}

%union{
    char *str;
    struct node *node;
}

%token CHAR INT VOID SHORT DOUBLE
%token<str> IDENTIFIER NATURAL CHRLIT DECIMAL
%token LPAR RPAR LBRACE RBRACE SEMI COMMA ASSIGN PLUS MINUS MUL DIV MOD
%token OR AND BITWISEAND BITWISEOR BITWISEXOR EQ NE LE GE LT GT NOT IF ELSE WHILE RETURN
%type<node> FunctionsAndDeclarations FunctionDefinition FunctionBody DeclarationsAndStatements FunctionDeclaration FunctionDeclarator ParameterList ParameterDeclaration Declaration DeclarationList TypeSpec Declarator Statement StatementList Expr ExprList


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

%locations
%{
#define LOCATE(node, line, column) { node->token_line = line; node->token_column = column; }
%}

%%

FunctionsAndDeclarations: FunctionsAndDeclarations FunctionDeclaration   { $$ = program = newnode(Program, NULL); 
                                                                         addchild(program, $1); }
                       | FunctionsAndDeclarations FunctionDefinition
                       | FunctionsAndDeclarations Declaration
                       | FunctionDeclaration
                       | FunctionDefinition
                       | Declaration
                       ;

FunctionDefinition: TypeSpec FunctionDeclarator FunctionBody           { $$ = $1;
                                                                       struct node *function = newnode(FuncDefinition, NULL);
                                                                       addchild(function, $2);
                                                                       addchild(function, $3);
                                                                       addchild($$, function); printf("b\n"); }
                    ;

FunctionBody: LBRACE DeclarationsAndStatements RBRACE                  { $$ = newnode(FuncBody, NULL);
                                                                       addchild($$, $2); }
            | LBRACE RBRACE                                            { $$ = newnode(FuncBody, NULL); }
            ;

DeclarationsAndStatements: Statement                                   { addchild($$, $1); }
                         | Declaration                                 { addchild($$, $1); }
                         | Statement DeclarationsAndStatements         { $$ = $2;}
                         | Declaration DeclarationsAndStatements       { $$ = $2;}
                         ;

FunctionDeclaration:
    TypeSpec FunctionDeclarator SEMI                                    {$$ = newnode(FuncDeclaration, NULL);
                                                                        addchild($$, $1);
                                                                        addchild($$, $2);}
    ;

FunctionDeclarator: IDENTIFIER LPAR ParameterList RPAR                  {$$ = newnode(ParamList, NULL);
                                                                        /*addchild($$, newnode(Identifier, $1)*/;
                                                                        addchild($$, $3);}
                    ;

ParameterList:
    ParameterDeclaration                                                {$$ = newnode(ParamDeclaration, NULL);
                                                                        addchild($$, $1); printf("f\n");}
    | ParameterList COMMA ParameterDeclaration                          {$$ = $1;
                                                                        addchild($$, $3);}
    ;

ParameterDeclaration:TypeSpec IDENTIFIER                                {$$ = newnode(ParamDeclaration, NULL); printf("d\n");
                                                                        addchild($$, $1);
                                                                        /*addchild($$, newnode(Identifier, $2));*/}
                    | TypeSpec                                          {$$ = newnode(ParamDeclaration, NULL);
                                                                        addchild($$, $1);}
                    ;

Declaration: TypeSpec DeclarationList SEMI                              {$$ = newnode(Declaration, NULL);
                                                                        addchild($$, $1);
                                                                        addchild($$, $2); printf("a\n");}
    | error SEMI                                                        { ; }
    ;

DeclarationList:
    Declarator                                                          
    | DeclarationList COMMA Declarator {}
    ;

TypeSpec:
    CHAR                            { $$ = newnode(Char, NULL); } 
    | INT                           { $$ = newnode(Void, NULL); printf("c\n");}
    | VOID                          { $$ = newnode(Int, NULL); }
    | SHORT                         { $$ = newnode(Short, NULL); }
    | DOUBLE                        { $$ = newnode(Double, NULL); }
    ;

Declarator: IDENTIFIER                                                  {$$ = newnode(Declaration, NULL);
                                                                        /*addchild($1, newnode(Identifier, $1));*/}    
          | IDENTIFIER ASSIGN Expr                                      {$$ = newnode(Declaration, NULL);
                                                                        /*addchild($1, newnode(Identifier, $1));*/
                                                                        addchild($$, $3);}
          ;

Statement:
    Expr SEMI                                                           {$$ = newnode(StatList, NULL);
                                                                         addchild($$, $1);}
    | SEMI                                                              {$$ = newnode(StatList, NULL);}
    | LBRACE RBRACE                                                     {$$ = newnode(StatList, NULL);}    
    | LBRACE StatementList RBRACE                                       {$$ = newnode(StatList, NULL);
                                                                        addchild($$, $2);}
    | IfStatement                                                       {$$ = newnode(If, NULL);} 
    | WHILE LPAR Expr RPAR Statement                                    {$$ = newnode(While, NULL);
                                                                        addchild($$, $3);
                                                                        addchild($$, $5);}
    | RETURN Expr SEMI                                                  {$$ = newnode(Return, NULL);
                                                                        addchild($$, $2);}
    | RETURN SEMI                                                       {$$ = newnode(Return, NULL);}
    ;

IfStatement: IF LPAR Expr RPAR Statement %prec LOW
           | IF LPAR Expr RPAR Statement ELSE Statement
           ;

StatementList:
    Statement  {printf("h\n");}
    | StatementList Statement  {printf("i\n");}
    ;

Expr: Expr ASSIGN Expr                
    | Expr COMMA Expr                 { $$ = newnode(Comma, NULL);
                                      addchild($$, $1);
                                      addchild($$, $3); }                             
    | Expr PLUS Expr                  { $$ = newnode(Add, NULL);
                                      addchild($$, $1);
                                      addchild($$, $3); }                            
    | Expr MINUS Expr                 { $$ = newnode(Sub, NULL);
                                      addchild($$, $1);
                                      addchild($$, $3); }                              
    | Expr MUL Expr                   { $$ = newnode(Mul, NULL);
                                      addchild($$, $1);
                                      addchild($$, $3); }                              
    | Expr DIV Expr                   { $$ = newnode(Div, NULL);
                                      addchild($$, $1);
                                      addchild($$, $3); }                              
    | Expr MOD Expr                   { $$ = newnode(Mod, NULL);
                                      addchild($$, $1);
                                      addchild($$, $3); }
    | Expr OR Expr                    { $$ = newnode(Or, NULL);
                                      addchild($$, $1);
                                      addchild($$, $3); }                              
    | Expr AND Expr                   { $$ = newnode(And, NULL);
                                      addchild($$, $1);
                                      addchild($$, $3); }                              
    | Expr BITWISEAND Expr            { $$ = newnode(BitWiseAnd, NULL);
                                      addchild($$, $1);
                                      addchild($$, $3); }                              
    | Expr BITWISEOR Expr             { $$ = newnode(BitWiseOr, NULL);
                                      addchild($$, $1);
                                      addchild($$, $3); }                              
    | Expr BITWISEXOR Expr            { $$ = newnode(BitWiseXor, NULL);
                                      addchild($$, $1);
                                      addchild($$, $3); }                              
    | Expr EQ Expr                    { $$ = newnode(Eq, NULL);
                                      addchild($$, $1);
                                      addchild($$, $3); }                              
    | Expr NE Expr                    { $$ = newnode(Ne, NULL);
                                      addchild($$, $1);
                                      addchild($$, $3); }                              
    | Expr LE Expr                    { $$ = newnode(Le, NULL); printf("g\n");
                                      addchild($$, $1);
                                      addchild($$, $3); }                              
    | Expr GE Expr                    { $$ = newnode(Ge, NULL);
                                      addchild($$, $1);
                                      addchild($$, $3); }                              
    | Expr LT Expr                    { $$ = newnode(Lt, NULL);
                                      addchild($$, $1);
                                      addchild($$, $3); }                              
    | Expr GT Expr                    { $$ = newnode(Gt, NULL);
                                      addchild($$, $1);
                                      addchild($$, $3); }                              
    | PLUS Expr                       { $$ = newnode(Plus, NULL);
                                      addchild($$, $2); }                             
    | MINUS Expr                      { $$ = newnode(Minus, NULL);
                                      addchild($$, $2); }                              
    | NOT Expr                        { $$ = newnode(Not, NULL);
                                      addchild($$, $2); }                              
    | IDENTIFIER LPAR ExprList RPAR   { $$ = $3;}                            
    | IDENTIFIER LPAR RPAR            { $$ = NULL;}                            
    | IDENTIFIER                      { $$ = newnode(Identifier, NULL); printf("e\n");}                             
    | NATURAL                         { $$ = newnode(Natural, NULL); }                              
    | CHRLIT                          { $$ = newnode(ChrLit, NULL); }                             
    | DECIMAL                         { $$ = newnode(Decimal, NULL); }                            
    | LPAR Expr RPAR                  { $$ = $2; }                             
    ;

ExprList: Expr %prec COMMA {}
        | ExprList COMMA Expr %prec COMMA {}
        ;

%%

