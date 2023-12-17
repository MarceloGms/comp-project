#ifndef _SEMANTICS_H
#define _SEMANTICS_H

#include "ast.h"

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

void check_program(struct node *program);
void check_all(struct node *temp, struct symbol_list *s_table, struct symbol_list_list **scope, int flag_func);
void check_declaration(struct node *temp, struct symbol_list *aux_table, int flag);
void check_statements(struct node *temp, struct symbol_list *aux_table);
void check_ifstatement(struct node *temp, struct symbol_list *aux_table);
void check_while(struct node *temp, struct symbol_list *aux_table);
void check_func_declaration(struct node *temp);
void check_func_definition(struct node *temp, struct symbol_list_list **scope);
void get_parameters(struct node *parameters, struct symbol_list *scope);
int check_parameters_func_decla(struct node *parameters);
int check_parameters_func_defin(struct node *parameters);
void check_func_body(struct node *func_body, struct symbol_list *table, struct symbol_list_list **scope);
void check_return(struct node *temp, struct symbol_list *table);
void check_expression(struct node *expression, struct symbol_list *scope);
int find_declaration(char *id);
int find_func(char *id);
void remove_declaration(int pos);
void reverse_scope(struct symbol_list_list **scope);
void check_scope(struct symbol_list_list **scope);
struct symbol_list *insert_symbol(struct symbol_list *symbol_table, char *identifier, enum type type, char *func_parameters, struct node *node);
struct symbol_list *search_symbol(struct symbol_list *symbol_table, char *identifier);

void show_symbol_table();

#endif