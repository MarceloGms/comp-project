%{
#include <stdio.h>
#include "ast.h"
extern int yylex(void);
void yyerror(char *s);
struct node *program;
%}

%union{
    char *str;
    struct node *node;
}

%token RESERVED
%token CHAR INT VOID SHORT DOUBLE
%token<str> IDENTIFIER NATURAL CHRLIT DECIMAL
%token LPAR RPAR LBRACE RBRACE SEMI COMMA ASSIGN PLUS MINUS MUL DIV MOD
%token OR AND BITWISEAND BITWISEOR BITWISEXOR EQ NE LE GE LT GT NOT IF ELSE WHILE RETURN
%type<node> FunctionsAndDeclarations FunctionDefinition FunctionBody DeclarationsAndStatements FunctionDeclaration FunctionDeclarator ParameterList ParameterDeclaration Declaration DeclarationList TypeSpec Declarator Statement StatementList Expr ExprList StatementError


%nonassoc LOW
%nonassoc ELSE
%left COMMA
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
%right NOT HIGH

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
                       | Declaration                                        { $$ = program = newnode(Program, NULL);
                                                                        addchild($$, $1); }
                       ;

FunctionDefinition: TypeSpec FunctionDeclarator FunctionBody           { $$ = newnode(FuncDefinition, NULL);
                                                                           addchild($$, $1);
                                                                           addchild($$, $2->children->next->node);
                                                                           addchild($$, $2->children->next->next->node);
                                                                           addchild($$, $3); }
                    ;

FunctionBody: LBRACE DeclarationsAndStatements RBRACE                  { $$ = newnode(FuncBody, NULL);
                                                                       addchild($$, $2); }
            | LBRACE RBRACE                                            { $$ = newnode(FuncBody, NULL); }
            ;

DeclarationsAndStatements: Statement                                   { $$ = $1; }
                         | Declaration                                 { $$ = $1; }
                         | Statement DeclarationsAndStatements         { $$ = $2;}
                         | Declaration DeclarationsAndStatements       { $$ = $2;}
                         ;

FunctionDeclaration:
    TypeSpec FunctionDeclarator SEMI                                    {$$ = newnode(FuncDeclaration, NULL);
                                                                        addchild($$, $1);
                                                                        addchild($$, $2->children->next->node);
                                                                        addchild($$, $2->children->next->next->node);}
    ;

FunctionDeclarator: IDENTIFIER LPAR ParameterList RPAR                  {struct node *temp = newnode(Call, NULL);
                                                                        addchild(temp, newnode(Identifier, $1));
                                                                        addchild(temp, $3);
                                                                        $$ = temp;}
                    ;

ParameterList:
    ParameterDeclaration                                                {$$ = newnode(ParamDeclaration, NULL);
                                                                        addchild($$, $1);}
    | ParameterList COMMA ParameterDeclaration                          { ; }
    ;

ParameterDeclaration:TypeSpec IDENTIFIER                                {$$ = newnode(ParamDeclaration, NULL);
                                                                        addchild($$, $1);
                                                                        addchild($$, newnode(Identifier, $2));}
                    | TypeSpec                                          {$$ = newnode(ParamDeclaration, NULL);
                                                                        addchild($$, $1);}
                

Declaration: TypeSpec DeclarationList SEMI                              {$$ = newnode(Declaration, NULL);
                                                                        addchild($$, $1);
                                                                        addchild($$, $2);}
    | error SEMI                                                        { $$ = newnode(Error, NULL); }
    ;

DeclarationList:
    Declarator                                                          
    | DeclarationList COMMA Declarator {}
    ;

TypeSpec:
    CHAR                            { $$ = newnode(Char, NULL); }
    | INT                           { $$ = newnode(Void, NULL);}
    | VOID                          { $$ = newnode(Int, NULL); }
    | SHORT                         { $$ = newnode(Short, NULL); }
    | DOUBLE                        { $$ = newnode(Double, NULL); }
    ;

Declarator: IDENTIFIER                                                  {$$ = newnode(Declaration, NULL);
                                                                        addchild($$, newnode(Identifier, $1));}  
          | IDENTIFIER ASSIGN Expr                                      {$$ = newnode(Declaration, NULL);
                                                                        addchild($$, newnode(Identifier, $1));
                                                                        addchild($$, $3);}
          ;

Statement:
    Expr SEMI                                                           {$$ = newnode(StatList, NULL);
                                                                         addchild($$, $1);}
    | SEMI                                                              {$$ = newnode(StatList, NULL);}
    | LBRACE RBRACE                                                     {$$ = newnode(StatList, NULL);}    
    | LBRACE StatementList RBRACE                                       {$$ = newnode(StatList, NULL);
                                                                        addchild($$, $2);}
    | LBRACE error RBRACE                                               { $$ = newnode(Error, NULL); }
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

StatementList: StatementError  
             | StatementList StatementError  
             ;

StatementError: error SEMI                                              { $$ = newnode(Error, NULL); }
              | Statement
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
    | Expr LE Expr                    { $$ = newnode(Le, NULL);
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
    | IDENTIFIER LPAR error RPAR      { $$ = newnode(Error, NULL); }
    | LPAR error RPAR                 { $$ = newnode(Error, NULL); }        
    | IDENTIFIER                      { $$ = newnode(Identifier, NULL);}                             
    | NATURAL                         { $$ = newnode(Natural, NULL); }                              
    | CHRLIT                          { $$ = newnode(ChrLit, NULL); }                             
    | DECIMAL                         { $$ = newnode(Decimal, NULL); }                            
    | LPAR Expr RPAR                  { $$ = $2; }                             
    ;

ExprList: Expr %prec HIGH {}
        | ExprList COMMA Expr
        ;

%%