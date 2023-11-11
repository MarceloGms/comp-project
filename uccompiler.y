/* definitions */
%{
#include <stdio.h>
%}

%token NATURAL
%token IDENTIFIER
%token NUMBER 123

%% 
/* rules */ 

hello: IDENTIFIER NUMBER { printf("hello\n"); }
%% 

/* auxiliary routines */
