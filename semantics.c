#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "semantics.h"

int semantic_errors = 0;
int test = 0;

struct symbol_list *symbol_table;
struct symbol_list_list *scope_list = NULL, *first; 

// semantic analysis begins here, with the AST root node
int check_program(struct node *program) {
    symbol_table = (struct symbol_list *) malloc(sizeof(struct symbol_list));
    symbol_table->next = NULL;
    struct node_list *child = program->children;
    while((child = child->next) != NULL)
        check_all(child->node, symbol_table, &scope_list);
    free(symbol_table);
    return semantic_errors;
}

void check_all(struct node *temp, struct symbol_list *s_table, struct symbol_list_list **scope) {
    if (temp->category == Declaration){
        check_declaration(temp, s_table);
    }else if(temp->category == FuncDefinition){
        check_func_definition(temp, s_table, scope);
    }
}

void check_declaration(struct node *temp, struct symbol_list *aux_table) {
    struct node *id = getchild(temp, 1);
    check_expression(getchild(temp, 0), aux_table);
    if(search_symbol(aux_table, id->token) == NULL) {
        insert_symbol(aux_table, id->token, getchild(temp, 0)->type, "", temp);
    } else {
        printf("Identifier %s (%d:%d) already declared\n", id->token, id->token_line, id->token_column);
        semantic_errors++;
    }
    /* ToDo: scope should be free'd */
}

void reverse_scope(struct symbol_list_list **scope) {
    struct symbol_list_list *prev = NULL;
    struct symbol_list_list *current = *scope;
    struct symbol_list_list *next = NULL;

    while (current != NULL) {
        next = current->next;
        current->next = prev;
        prev = current;
        current = next;
    }

    *scope = prev;
}

void check_func_definition(struct node *temp, struct symbol_list *symbols_table, struct symbol_list_list **scope) {
    struct node *id = getchild(temp, 1);
    check_expression(getchild(temp, 0), symbol_table);

    struct symbol_list_list *scope_uni = (struct symbol_list_list *)malloc(sizeof(struct symbol_list_list));
    strcpy(scope_uni->func_name, id->token);
    scope_uni->next = *scope;
    *scope = scope_uni;

    // Allocate memory for aux_table
    struct symbol_list *aux_table = (struct symbol_list *)malloc(sizeof(struct symbol_list));
    aux_table->next = scope_uni->symbol_list;  // Set next to NULL initially
    scope_uni->symbol_list = aux_table;

    if(search_symbol(symbol_table, id->token) == NULL) {
        insert_symbol(scope_uni->symbol_list, "return", getchild(temp, 0)->type, "", temp);

        check_parameters(getchild(temp, 2), scope_uni->symbol_list);

        char *para = (char *) malloc(sizeof(char) * 100);
        strcpy(para, "(");
        struct symbol_list *symbol;
        for(symbol = scope_uni->symbol_list->next->next; symbol != NULL; symbol = symbol->next){
            strcat (para, type_name(symbol->type));
            if (symbol->next == NULL){
                strcat (para, ")");
            }else{
                strcat (para, ",");
            }
        }

        insert_symbol(symbol_table, id->token,  getchild(temp, 0)->type, para, temp);
    } else {
        printf("Identifier %s (%d:%d) already declared\n", id->token, id->token_line, id->token_column);
        semantic_errors++;
    }

    check_func_body(getchild(temp, 3), scope_uni->symbol_list, &scope);
}

void check_parameters(struct node *parameters, struct symbol_list *scope) {
    struct node_list *parameter = parameters->children;
    struct node *id;
    while((parameter = parameter->next) != NULL) {
        check_expression(getchild(parameter->node, 0), scope);
        id = getchild(parameter->node, 1);
        if(search_symbol(scope, id->token) == NULL) {
            insert_symbol(scope, id->token, getchild(parameter->node, 0)->type, "\tparam", parameter->node);
        } else {
            printf("Identifier %s (%d:%d) already declared\n", id->token, id->token_line, id->token_column);
            semantic_errors++;
        }
    }
}

void check_func_body(struct node *func_body, struct symbol_list *table, struct symbol_list_list **scope){
    struct node_list *body_exp = func_body->children;
    while((body_exp = body_exp->next) != NULL) {
        check_all(body_exp->node, table, &scope);
    }
    
}

void check_expression(struct node *expression, struct symbol_list *scope) {
    switch(expression->category) {
        case Int:
            expression->type = int_type;
            break;
        case Double:
            expression->type = double_type;
            break;
        case Short:
            expression->type = short_type;
            break;
        case Char:
            expression->type = char_type;
            break;
        case Identifier:
            if(search_symbol(scope, expression->token) == NULL) {
                printf("Variable %s (%d:%d) undeclared\n", expression->token, expression->token_line, expression->token_column);
                semantic_errors++;
            } else {
                expression->type = search_symbol(scope, expression->token)->type;
            }
            break;
        case Natural:
            expression->type = int_type;
            break;
        case Decimal:
            expression->type = double_type;
            break;
        case Call:
            /*if(search_symbol(symbol_table, getchild(expression, 0)->token) == NULL || search_symbol(symbol_table, getchild(expression, 0)->token)->node->category != Function) {
                printf("Function %s (%d:%d) undeclared\n", getchild(expression, 0)->token, getchild(expression, 0)->token_line, getchild(expression, 0)->token_column);
                semantic_errors++;
            } else {
                struct node *arguments = getchild(expression, 1);
                struct node *parameters = getchild(search_symbol(symbol_table, getchild(expression, 0)->token)->node, 1);
                if(parameters != NULL && countchildren(arguments) != countchildren(parameters)) {
                    printf("Calling %s (%d:%d) with incorrect arguments\n", getchild(expression, 0)->token, getchild(expression, 0)->token_line, getchild(expression, 0)->token_column);
                    semantic_errors++;
                } else {
                    struct node_list *argument = arguments->children;
                    while((argument = argument->next) != NULL)
                        check_expression(argument->node, scope);
                }
            }*/
            break;
        case If:
            check_expression(getchild(expression, 0), scope);
            check_expression(getchild(expression, 1), scope);
            check_expression(getchild(expression, 2), scope);
            break;
        case Add:
        case Sub:
        case Mul:
        case Div:
            check_expression(getchild(expression, 0), scope);
            check_expression(getchild(expression, 1), scope);
            break;
        default:
            break;
    }
}
//para a lista ligada de listas ligadas
/*int count_symbol_list(struct symbol_list_list *table_list) {
    int count = 0;
    struct symbol_list_list *aux_list;
    for(aux_list = table_list; aux_list != NULL; aux_list = aux_list->next)
        if(aux_list->next == NULL)
            break;
        else{
            count++;
        }
    return count;
}*/

// insert a new symbol in the list, unless it is already there
struct symbol_list *insert_symbol(struct symbol_list *table, char *identifier, enum type type, char *func_parameters, struct node *node) {
    struct symbol_list *new = (struct symbol_list *) malloc(sizeof(struct symbol_list));
    new->identifier = strdup(identifier);
    new->type = type;
    new->node = node;
    new->func_parameters = func_parameters;
    new->next = NULL;
    struct symbol_list *symbol = table;
    while(symbol != NULL) {
        if(symbol->next == NULL) {
            symbol->next = new;    /* insert new symbol at the tail of the list */
            break;
        } else if(strcmp(symbol->next->identifier, identifier) == 0) {
            free(new);
            return NULL;           /* return NULL if symbol is already inserted */
        }
        symbol = symbol->next;
    }
    return new;
}

// look up a symbol by its identifier
struct symbol_list *search_symbol(struct symbol_list *table, char *identifier) {
    struct symbol_list *symbol;
    for(symbol = table->next; symbol != NULL; symbol = symbol->next)
        if(strcmp(symbol->identifier, identifier) == 0)
            return symbol;
    return NULL;
}

void show_symbol_table() {
    struct symbol_list *symbol;

    printf("===== Global Symbol Table =====\nputchar\tint(int)\ngetchar\tint(void)\n");
    for (symbol = symbol_table->next; symbol != NULL; symbol = symbol->next) {
        printf("%s\t%s%s\n", symbol->identifier, type_name(symbol->type), symbol->func_parameters);
    }

    struct symbol_list_list *currentScope = scope_list;
    reverse_scope(&currentScope);
    while (currentScope != NULL) {
        printf("\n===== Function %s Symbol Table =====\n", currentScope->func_name);
        for (symbol = currentScope->symbol_list->next; symbol != NULL; symbol = symbol->next) {
            printf("%s\t%s%s\n", symbol->identifier, type_name(symbol->type), symbol->func_parameters);
        }
        currentScope = currentScope->next;
    }
}