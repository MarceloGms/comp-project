/* definitions */
%{
#include <stdio.h>
%}

%token NATURAL
%token IDENTIFIER
%token NUMBER 123

%% 
/* rules */ 
FunctionsAndDeclarations : (FunctionDefinition | FunctionDeclaration | Declaration) {FunctionDefinition | FunctionDeclaration | Declaration}
FunctionDefinition −→ TypeSpec FunctionDeclarator FunctionBody
FunctionBody −→ LBRACE [DeclarationsAndStatements] RBRACE
DeclarationsAndStatements −→ Statement DeclarationsAndStatements | Declaration Declarati-
onsAndStatements | Statement | Declaration
FunctionDeclaration −→ TypeSpec FunctionDeclarator SEMI
FunctionDeclarator −→ IDENTIFIER LPAR ParameterList RPAR
ParameterList −→ ParameterDeclaration {COMMA ParameterDeclaration}
ParameterDeclaration −→ TypeSpec [IDENTIFIER]
Declaration −→ TypeSpec Declarator {COMMA Declarator} SEMI
TypeSpec −→ CHAR | INT | VOID | SHORT | DOUBLE
Declarator −→ IDENTIFIER [ASSIGN Expr]
Statement −→ [Expr] SEMI
Statement −→ LBRACE {Statement} RBRACE
Statement −→ IF LPAR Expr RPAR Statement [ELSE Statement]
Statement −→ WHILE LPAR Expr RPAR Statement
Statement −→ RETURN [Expr] SEMI
Expr −→ Expr (ASSIGN | COMMA) Expr
Expr −→ Expr (PLUS | MINUS | MUL | DIV | MOD) Expr
Expr −→ Expr (OR | AND | BITWISEAND | BITWISEOR | BITWISEXOR) Expr
Expr −→ Expr (EQ | NE | LE | GE | LT | GT) Expr
Expr −→ (PLUS | MINUS | NOT) Expr
Expr −→ IDENTIFIER LPAR [Expr {COMMA Expr}] RPAR
Expr −→ IDENTIFIER | NATURAL | CHRLIT | DECIMAL | LPAR Expr RPAR
hello: IDENTIFIER NUMBER { printf("hello\n"); }
%% 

/* auxiliary routines */
