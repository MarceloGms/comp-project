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
%type<node> FunctionsAndDeclarations FunctionDefinition FunctionBody DeclarationsAndStatements FunctionDeclaration FunctionDeclarator ParameterList ParameterDeclaration Declaration DeclarationList TypeSpec Declarator Statement StatementList Expr ExprList StatementError IfStatement


%nonassoc LOW
%nonassoc ELSE
%left COMMA
%right ASSIGN
%left OR
%left AND
%left BITWISEOR
%left BITWISEXOR
%left BITWISEAND
%left EQ NE
%left LE GE LT GT
%left PLUS MINUS
%left MUL DIV MOD
%left HIGH
%right NOT


%locations
%{
#define LOCATE(node, line, column) { node->token_line = line; node->token_column = column; }
%}

%%
//CERTO 99%(FRED CONFIRM)
FunctionsAndDeclarations: Declaration                                   { $$ = program = newnode(Program, NULL);
                                                                        inheritChilds(program, $1); }
                        | FunctionsAndDeclarations Declaration          {inheritChilds(program, $2); } 
                        | FunctionDefinition                            { $$ = program = newnode(Program, NULL);
                                                                        addchild(program, $1); }
                        | FunctionsAndDeclarations FunctionDefinition   { addchild(program, $2); }
                        | FunctionDeclaration                           { $$ = program = newnode(Program, NULL);
                                                                        addchild(program, $1); } 
                        | FunctionsAndDeclarations FunctionDeclaration  { addchild(program, $2); }
                       ;

FunctionDefinition: TypeSpec FunctionDeclarator FunctionBody           { $$ = newnode(FuncDefinition, NULL);
                                                                           addchild($$, $1);
                                                                           inheritChilds($$, $2);
                                                                           addchild($$, $3); }
                    ;

FunctionBody: LBRACE DeclarationsAndStatements RBRACE                  { $$ = newnode(FuncBody, NULL);
                                                                       inheritChilds($$, $2);}
            | LBRACE RBRACE                                            { $$ = newnode(FuncBody, NULL); }
            ;

DeclarationsAndStatements: DeclarationsAndStatements Statement         { $$ = $1;
                                                                       addChildNull($$, $2, 0);}
                         | DeclarationsAndStatements Declaration       { $$ = $1;
                                                                       inheritChilds($$, $2); }
                         | Statement                                   { $$ = newnode(Temp, NULL); 
                                                                       addChildNull($$, $1, 0); }
                         | Declaration                                 { $$ = newnode(Temp, NULL); 
                                                                       inheritChilds($$, $1);}
                         ;

FunctionDeclaration:
    TypeSpec FunctionDeclarator SEMI                                    {$$ = newnode(FuncDeclaration, NULL);
                                                                        addchild($$, $1);
                                                                        inheritChilds($$, $2);}
    ;

FunctionDeclarator: IDENTIFIER LPAR ParameterList RPAR                  {$$ = newnode(Temp, NULL);
                                                                        addchild($$, newnode(Identifier, $1));
                                                                        addchild($$, $3);}
                    ;

ParameterList:
    ParameterDeclaration                                                {$$ = newnode(ParamList, NULL);
                                                                        addchild($$, $1);}
    | ParameterList COMMA ParameterDeclaration                          { addchild($$, $3); }
    ;

ParameterDeclaration:TypeSpec IDENTIFIER                                {$$ = newnode(ParamDeclaration, NULL);
                                                                        addchild($$, $1);
                                                                        addchild($$, newnode(Identifier, $2));}
                    | TypeSpec                                          {$$ = newnode(ParamDeclaration, NULL);
                                                                        addchild($$, $1);}
                

Declaration: TypeSpec DeclarationList SEMI                              {$$ = newnode(Temp, NULL); 
                                                                        addTypeToDeclarators($2, $1);
                                                                        inheritChilds($$, $2);}
    | error SEMI                                                        { $$ = newnode(Error, NULL); }
    ;

DeclarationList:
    Declarator                                                          {$$ = newnode(Temp, NULL);
                                                                        addchild($$, $1);}                                                 
    | DeclarationList COMMA Declarator                                  {addchild($$, $3);}
    ;

TypeSpec:
    CHAR                            { $$ = newnode(Char, NULL); }
    | INT                           { $$ = newnode(Int, NULL);}
    | VOID                          { $$ = newnode(Void, NULL); }
    | SHORT                         { $$ = newnode(Short, NULL); }
    | DOUBLE                        { $$ = newnode(Double, NULL); }
    ;

Declarator: IDENTIFIER                                                  {$$ = newnode(Declaration, NULL);
                                                                        addchild($$, newnode(Identifier, $1));}
           |IDENTIFIER ASSIGN Expr                                      {$$ = newnode(Declaration, NULL);
                                                                        addchild($$, newnode(Identifier, $1));
                                                                        addchild($$, $3);}
            ;  

Statement: SEMI                                                         { $$ = NULL; }
    | Expr SEMI                                                         { $$ = $1; }
    | LBRACE RBRACE                                                     { $$ = NULL; } 
    | IfStatement                                                       { $$ = $1;}    
    | LBRACE StatementList RBRACE                                       { $$ = $2; 
                                                                        if($$->children->next == NULL) $$ = NULL;
                                                                        else if(countChildren($$) <= 1) $$ = $$->children->next->node;}
    | LBRACE error RBRACE                                               { $$ = newnode(Error, NULL); }
    | WHILE LPAR Expr RPAR Statement                                    {$$ = newnode(While, NULL);
                                                                        addchild($$, $3);
                                                                        addChildNull($$, $5, 1);}
    | RETURN Expr SEMI                                                  {$$ = newnode(Return, NULL);
                                                                        addchild($$, $2);}
    | RETURN SEMI                                                       {$$ = newnode(Return, NULL);
                                                                        addchild($$, newnode(Null, NULL));}
    ;

IfStatement: IF LPAR Expr RPAR Statement %prec LOW                      { $$ = newnode(If, NULL);
                                                                        addchild($$, $3);
                                                                        addChildNull($$, $5, 1);
                                                                        addchild($$, newnode(Null, NULL));}
           | IF LPAR Expr RPAR Statement ELSE Statement                 { $$ = newnode(If, NULL);
                                                                        addchild($$, $3);
                                                                        addChildNull($$, $5, 1);
                                                                        addChildNull($$, $7, 1); }
           ;

StatementList: StatementError                                           {$$ = newnode(StatList, NULL);
                                                                        addChildNull($$, $1, 0);}
             | StatementList StatementError                             {addChildNull($$, $2, 0);}
             ;

StatementError: error SEMI                                              { $$ = newnode(Error, NULL); }
              | Statement                                               { $$ = $1; }
              ;

Expr: Expr ASSIGN Expr                { $$ = newnode(Store, NULL);
                                      addchild($$, $1);
                                      addchild($$, $3); }
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
    | PLUS Expr %prec HIGH            { $$ = newnode(Plus, NULL);
                                      addchild($$, $2); }                             
    | MINUS Expr %prec HIGH           { $$ = newnode(Minus, NULL);
                                      addchild($$, $2); }                            
    | NOT Expr                        { $$ = newnode(Not, NULL);
                                      addchild($$, $2); }                          
    | IDENTIFIER LPAR ExprList RPAR   { $$ = newnode(Call, NULL); 
                                        addchild($$, newnode(Identifier, $1)); 
                                        inheritChilds($$, $3); }                    
    | IDENTIFIER LPAR RPAR            { $$ = newnode(Call, NULL); 
                                        addchild($$, newnode(Identifier, $1)); 
                                      }
    | IDENTIFIER LPAR error RPAR      { $$ = newnode(Error, NULL); }
    | LPAR error RPAR                 { $$ = newnode(Error, NULL); }        
    | IDENTIFIER                      { $$ = newnode(Identifier, $1);}                             
    | NATURAL                         { $$ = newnode(Natural, $1); }                              
    | CHRLIT                          { $$ = newnode(ChrLit, $1); }                             
    | DECIMAL                         { $$ = newnode(Decimal, $1); }                            
    | LPAR Expr RPAR                  { $$ = $2; }                             
    ;

ExprList: Expr %prec HIGH             { $$ = newnode(Temp, NULL); 
                                      addchild($$, $1); }
        | ExprList COMMA Expr         { addchild($$, $3); }
        ;
%%