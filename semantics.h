#ifndef _SEMANTICS_H
#define _SEMANTICS_H

#include "ast.h"

int check_program(struct node *program);

struct symbol_list {
    char *identifier;
    enum type type;
    struct node *node;
    char *func_parameters;
    struct symbol_list *next;
};

struct symbol_list_list {
    char func_name[50];
    struct symbol_list *symbol_list;
      struct symbol_list_list *next;
};

struct symbol_list *insert_symbol(struct symbol_list *symbol_table, char *identifier, enum type type, char *func_parameters, struct node *node);
struct symbol_list *search_symbol(struct symbol_list *symbol_table, char *identifier);
void show_symbol_table();

#endif