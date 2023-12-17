#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "semantics.h"

int test = 0;
int isBody = 0;
char **list_func_declarations = NULL, **list_func_decla_param = NULL, **list_func_decla_all = NULL, **list_func_decla_all_param = NULL;
int num_list_func_declarations = 0, num_list_func_decla_all = 0;

struct symbol_list *symbol_table;
struct symbol_list_list *scope_list = NULL, *first; 

// semantic analysis begins here, with the AST root node
void check_program(struct node *program) {
    symbol_table = (struct symbol_list *) malloc(sizeof(struct symbol_list));
    symbol_table->next = NULL;
    struct node_list *child = program->children;
    insert_symbol(symbol_table, "putchar", int_type, "(int)", NULL);
    insert_symbol(symbol_table, "getchar", int_type, "(void)", NULL);
    while((child = child->next) != NULL){
        check_all(child->node, symbol_table, &scope_list, 0);
    }
    /*for (int i = 0; i < num_list_func_declarations; i++) {
        free(list_func_declarations[i]);
    }
    free(list_func_declarations);
    for (int i = 0; i < num_list_func_declarations; i++) {
        free(list_func_decla_param[i]);
    }
    free(list_func_decla_param);
    //free(symbol_table);
    free(list_func_decla_all);*/
}

void check_all(struct node *temp, struct symbol_list *s_table, struct symbol_list_list **scope, int flag_func) {
    if (temp->category == Declaration){
        check_declaration(temp, s_table, flag_func);
    }else if(temp->category == FuncDeclaration){
        check_func_declaration(temp);
    }else if(temp->category == FuncDefinition){
        check_func_definition(temp, scope);
    }else if(temp->category == Return){
        check_return(temp, s_table);
    }else if (temp->category == StatList){
        check_statements(temp, s_table);
    }else if(temp->category == If){
        check_ifstatement(temp, s_table);
    }else if(temp->category == While){
        check_while(temp, s_table);
    }else{
        check_expression(temp, s_table);
    }
}

void check_declaration(struct node *temp, struct symbol_list *aux_table, int flag) {
    int decider = 0;
    struct node *id = getchild(temp, 1);
    struct node *value = getchild(temp, 2);
    if (isBody == 1){
        decider = 1;
        isBody = 0;
    }
    check_expression(getchild(temp, 0), aux_table);
    if (decider == 1){
        isBody = 1;
    }
    struct symbol_list *list = NULL;
    if (flag == 0){
        list = search_symbol(symbol_table, id->token);
    }else if (flag == 1){
        list = search_symbol(aux_table, id->token);
    }
    if (list != NULL){
        if (getchild(temp, 0)->type == void_type){
            if (value != NULL){
                check_expression(value, aux_table);
            }
            printf("Line %d, column %d: Invalid use of void type in declaration\n", id->token_line, id->token_column);
        }
    }else{
        if (getchild(temp, 0)->type == void_type){
            if (value != NULL){
                check_expression(value, aux_table);
            }
            printf("Line %d, column %d: Invalid use of void type in declaration\n", id->token_line, id->token_column);
        }else if (value == NULL){ 
            if (flag == 0){
                insert_symbol(symbol_table, id->token, getchild(temp, 0)->type, "", temp);
            }else{
                insert_symbol(aux_table, id->token, getchild(temp, 0)->type, "", temp);
            }
        }else if (value != NULL){
            check_expression(value, aux_table);
            if (flag == 0){
                insert_symbol(symbol_table, id->token, getchild(temp, 0)->type, "", temp);
                if (((value->category == Natural) || (value->category == Decimal) || (value->category == ChrLit)) && ((getchild(temp, 0)->type == int_type && (getchild(temp, 2)->type == void_type || getchild(temp, 2)->type == double_type || getchild(temp, 2)->type == no_type)) || (getchild(temp, 0)->type == short_type && (getchild(temp, 2)->type == void_type || getchild(temp, 2)->type == double_type || getchild(temp, 2)->type == no_type)) || (getchild(temp, 0)->type == char_type && (getchild(temp, 2)->type == void_type || getchild(temp, 2)->type == double_type || getchild(temp, 2)->type == no_type)) || (getchild(temp, 0)->type == double_type && (getchild(temp, 2)->type == void_type || getchild(temp, 2)->type == no_type)) || (getchild(temp, 0)->type == void_type) || (getchild(temp, 0)->type == no_type))){
                    printf("Line %d, column %d: Conflicting types (got %s, expected %s)\n", id->token_line, id->token_column, type_name(getchild(temp, 2)->type), type_name(getchild(temp, 0)->type));      
                }
            }else{
                insert_symbol(aux_table, id->token, getchild(temp, 0)->type, "", temp);
                if (((value->category == Natural) || (value->category == Decimal) || (value->category == ChrLit)) && ((getchild(temp, 0)->type == int_type && (getchild(temp, 2)->type == void_type || getchild(temp, 2)->type == double_type || getchild(temp, 2)->type == no_type)) || (getchild(temp, 0)->type == short_type && (getchild(temp, 2)->type == void_type || getchild(temp, 2)->type == double_type || getchild(temp, 2)->type == no_type)) || (getchild(temp, 0)->type == char_type && (getchild(temp, 2)->type == void_type || getchild(temp, 2)->type == double_type || getchild(temp, 2)->type == no_type)) || (getchild(temp, 0)->type == double_type && (getchild(temp, 2)->type == void_type || getchild(temp, 2)->type == no_type)) || (getchild(temp, 0)->type == void_type) || (getchild(temp, 0)->type == no_type))){
                    printf("Line %d, column %d: Conflicting types (got %s, expected %s)\n", id->token_line, id->token_column, type_name(getchild(temp, 2)->type), type_name(getchild(temp, 0)->type));      
                }
            }
            
        }
    }
}

void check_func_declaration(struct node *temp) {
    struct node *id = getchild(temp, 1);
    check_expression(getchild(temp, 0), symbol_table);
    if (check_parameters_func_decla(getchild(temp, 2)) == 1){

        char *para = (char *) malloc(sizeof(char) * 100);
        strcpy(para, "(");
        struct node_list *parameter = getchild(temp, 2)->children;
        while((parameter = parameter->next) != NULL) {
            check_expression(getchild(parameter->node, 0), NULL);
            strcat (para, type_name(getchild(parameter->node, 0)->type));
            if (parameter->next == NULL){
                strcat (para, ")");
            }else{
                strcat (para, ",");
            }
        }

        if(search_symbol(symbol_table, id->token) == NULL) {

            insert_symbol(symbol_table, id->token,  getchild(temp, 0)->type, para, temp);

            list_func_declarations = (char **) realloc(list_func_declarations, (num_list_func_declarations + 1) * sizeof(char *));
            list_func_declarations[num_list_func_declarations] = strdup(id->token);

            list_func_decla_param = (char **) realloc(list_func_decla_param, (num_list_func_declarations + 1) * sizeof(char *));
            list_func_decla_param[num_list_func_declarations] = strdup(para);
            num_list_func_declarations++;

            list_func_decla_all = (char **) realloc(list_func_decla_all, (num_list_func_decla_all + 1) * sizeof(char *));
            list_func_decla_all[num_list_func_decla_all] = strdup(id->token);

            list_func_decla_all_param = (char **) realloc(list_func_decla_all_param, (num_list_func_decla_all + 1) * sizeof(char *));
            list_func_decla_all_param[num_list_func_decla_all] = strdup(para);
            num_list_func_decla_all++;

        } else if(find_declaration(id->token) != -1 && strcmp(list_func_decla_param[find_declaration(id->token)], para) != 0){
            char exp[200], got[200];
            strcpy(exp, (type_name(getchild(temp, 1)->type)));
            strcat(exp, list_func_decla_param[find_declaration(id->token)]);
            strcpy(got, (type_name(getchild(temp, 1)->type)));
            strcat(got, para);
            printf("Line %d, column %d: Conflicting types (got %s, expected %s)\n", id->token_line, id->token_column, got, exp);   
        }
    }else{
        printf("Line %d, column %d: Invalid use of void type in declaration\n", id->token_line, id->token_column);
    }
}


void check_func_definition(struct node *temp, struct symbol_list_list **scope) {
    struct node *id = getchild(temp, 1);
    check_expression(getchild(temp, 0), symbol_table);
    int check = check_parameters_func_defin(getchild(temp, 2));
    if((search_symbol(symbol_table, id->token) == NULL && check == 1) || (find_declaration(id->token) != -1 && getchild(temp, 0)->type == search_symbol(symbol_table, id->token)->type)) {
        char *para = (char *) malloc(sizeof(char) * 100);
        strcpy(para, "(");
        struct node_list *parameter = getchild(temp, 2)->children;
        while((parameter = parameter->next) != NULL) {
            strcat (para, type_name(getchild(parameter->node, 0)->type));
            if (parameter->next == NULL){
                strcat (para, ")");
            }else{
                strcat (para, ",");
            }
        }

        if ((search_symbol(symbol_table, id->token) == NULL && check == 1) || strcmp(list_func_decla_param[find_declaration(id->token)], para) == 0){

            struct symbol_list_list *scope_uni = (struct symbol_list_list *)malloc(sizeof(struct symbol_list_list));
            strcpy(scope_uni->func_name, id->token);
            scope_uni->next = *scope;
            *scope = scope_uni;

            // Allocate memory for aux_table
            struct symbol_list *aux_table = (struct symbol_list *)malloc(sizeof(struct symbol_list));
            aux_table->next = scope_uni->symbol_list;
            scope_uni->symbol_list = aux_table;
            scope_uni->symbol_list->next = NULL;

            insert_symbol(scope_uni->symbol_list, "return", getchild(temp, 0)->type, "", temp);

            get_parameters(getchild(temp, 2), scope_uni->symbol_list);

            insert_symbol(symbol_table, id->token,  getchild(temp, 0)->type, para, temp);

            int pos = -1;
            for (int i = 0; i < num_list_func_declarations; i++) {
                if (list_func_declarations[i] != NULL && strcmp(list_func_declarations[i], id->token) == 0) {
                    pos = i;
                    break;
                }
            }
            if (pos != -1) {
                remove_declaration(pos);
            }else if (pos == -1){

                list_func_decla_all = (char **) realloc(list_func_decla_all, (num_list_func_decla_all + 1) * sizeof(char *));
                list_func_decla_all[num_list_func_decla_all] = strdup(id->token);
                
                list_func_decla_all_param = (char **) realloc(list_func_decla_all_param, (num_list_func_decla_all + 1) * sizeof(char *));
                list_func_decla_all_param[num_list_func_decla_all] = strdup(para);
                num_list_func_decla_all++;
            }
            check_func_body(getchild(temp, 3), scope_uni->symbol_list, scope);
        }else if (strcmp(list_func_decla_param[find_declaration(id->token)], para) != 0){
            char exp[200], got[200];
            strcpy(exp, (type_name(getchild(temp, 1)->type)));
            strcat(exp, list_func_decla_param[find_declaration(id->token)]);
            strcpy(got, (type_name(getchild(temp, 1)->type)));
            strcat(got, para);
            printf("Line %d, column %d: Conflicting types (got %s, expected %s)\n", id->token_line, id->token_column, got, exp);
        }

    /*}else if (find_declaration(id->token) == 1 && getchild(temp, 0)->type != search_symbol(symbol_table, id->token)->type){
        printf("Line %d, column %d: Conflicting types (got %s, expected %s)\n", id->token_line, id->token_column, strcat(strcat(strcat(type_name(getchild(temp, 0)->type), "("), type_name(getchild(getchild(getchild(temp, 2), 0), 0)->type)), ")"), type_name(search_symbol(symbol_table, id->token)->type));
        */
    }else if(check == 0){
        printf("Line %d, column %d: Invalid use of void type in declaration\n", id->token_line, id->token_column);
    }else if (search_symbol(symbol_table, id->token) != NULL && find_declaration(id->token) == -1){
        printf("Line %d, column %d: Symbol %s already defined\n", id->token_line, id->token_column, id->token);
    }
}

void check_statements(struct node *temp, struct symbol_list *aux_table){
    struct node_list *stat = temp->children;
    while((stat = stat->next) != NULL) {
        check_all(stat->node, aux_table, NULL, 0);
    }
}

void check_ifstatement(struct node *temp, struct symbol_list *aux_table){
    struct node_list *ifstat = temp->children;
    int first = 0;
    while((ifstat = ifstat->next) != NULL) {
        check_expression(ifstat->node, aux_table);
        if (ifstat->node->category == Identifier && first == 0){
            if (ifstat->node->type != int_type){
                printf("Line %d, column %d: Conflicting types (got %s, expected %s)\n", ifstat->node->token_line, ifstat->node->token_column, type_name(ifstat->node->type), type_name(int_type));
            }
        }else if ((ifstat->node->type != int_type) && (ifstat->node->type != short_type) && (ifstat->node->type != char_type) && (first == 0)){
            printf("Line %d, column %d: Conflicting types (got %s, expected %s)\n", ifstat->node->token_line, ifstat->node->token_column, type_name(ifstat->node->type), type_name(int_type));
        }
        first++;
    }
}

void check_while(struct node *temp, struct symbol_list *aux_table){
    struct node_list *whilestat = temp->children;
    int first = 0;
    while((whilestat = whilestat->next) != NULL) {
        check_expression(whilestat->node, aux_table);
        if (whilestat->node->category == Identifier && first == 0){
            if (whilestat->node->type != int_type){
                printf("Line %d, column %d: Conflicting types (got %s, expected %s)\n", whilestat->node->token_line, whilestat->node->token_column, type_name(whilestat->node->type), type_name(int_type));
            }
            first++;
        }
    }
}

void get_parameters(struct node *parameters, struct symbol_list *scope) {
    struct node_list *parameter = parameters->children;
    struct node *id;
    while((parameter = parameter->next) != NULL) {
        check_expression(getchild(parameter->node, 0), scope);
        if (getchild(parameter->node, 0)->type != void_type){
            id = getchild(parameter->node, 1);
            if(search_symbol(scope, id->token) == NULL) {
                insert_symbol(scope, id->token, getchild(parameter->node, 0)->type, "\tparam", parameter->node);
            }
        }
    }
}

int check_parameters_func_decla(struct node *parameters) {
    struct node_list *parameter = parameters->children;
    struct node *id, *type;
    char variables_param[50][100];
    int i = 0, v_check = 0, count = 0;
    while((parameter = parameter->next) != NULL) {
        int check = 0;
        check_expression(getchild(parameter->node, 0), NULL);
        type = getchild(parameter->node, 0);
        id = getchild(parameter->node, 1);
        if (count > 0){
            if (type->type == void_type){
                return 0;
            }
        }
        if (type == NULL){
            return 0;
        }
        if(type->type == void_type){
            if (id == NULL){
                if (v_check > 0){
                    return 0;
                }else{
                    v_check++;
                }
            }else if (id != NULL) {
                return 0;
            }
        }else if(id != NULL){
            for (int j = 0; j < i; j++) {
                if (strcmp(variables_param[j], id->token) == 0){
                    printf("Line %d, column %d: Symbol %s already defined\n", id->token_line, id->token_column, id->token);
                    check = 1;
                }
            }
        }
        if (check == 0 && id != NULL){
            strcpy(variables_param[i], id->token);
            i++;
        }
        count++;
    }
    return 1;
}

int check_parameters_func_defin(struct node *parameters) {
    struct node_list *parameter = parameters->children;
    struct node *id;
    int i = 0, count = 0;
    char variables_param[50][100];
    while((parameter = parameter->next) != NULL) {
        int check = 0;
        check_expression(getchild(parameter->node, 0), NULL);
        id = getchild(parameter->node, 1);
        if (count > 0){
            if (getchild(parameter->node, 0)->type == void_type){
                printf("Line %d, column %d: Invalid use of void type in declaration\n", getchild(parameters, 0)->token_line, getchild(parameters, 0)->token_column);
                return 0;
            }
        }
        for(int j = 0; j < i; j++) {
            if (strcmp(variables_param[j], id->token) == 0){
                printf("Line %d, column %d: Symbol %s already defined\n", id->token_line, id->token_column, id->token);
                check = 1;
            }
        }
        if (getchild(parameter->node, 0)->type == void_type){
            if (id != NULL){
                //printf("Line %d, column %d: Invalid use of void type in declaration\n", id->token_line, id->token_column);
                return 0;
            }
        }else{
            if (id == NULL){
                return 0;
            }
        }
        if(id != NULL && check == 0){
            strcpy(variables_param[i], id->token);
            i++;
        }
        count++;
    }
    return 1;
}

void check_func_body(struct node *func_body, struct symbol_list *table, struct symbol_list_list **scope){
    struct node_list *body_exp = func_body->children;
    isBody = 1;
    while((body_exp = body_exp->next) != NULL) {
        check_all(body_exp->node, table, scope, 1);
    }
    isBody = 0; 
}

void check_return(struct node *temp, struct symbol_list *table) {
    struct node *id = getchild(temp, 0);
    check_expression(id, table);
    if ((id->type != search_symbol(table, "return")->type) || (id->type == short_type && search_symbol(table, "return")->type == int_type) || (id->type == int_type && search_symbol(table, "return")->type == short_type)){
        printf("Line %d, column %d: Conflicting types (got %s, expected %s)\n", id->token_line, id->token_column, type_name(id->type), type_name(search_symbol(table, "return")->type));
    }
    
}

void check_expression(struct node *expression, struct symbol_list *scope) {
    int func_bool = 0;
    if (isBody != 0){
        expression->isExpr = 1;
    }
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
        case Void:  
            expression->type = void_type;
            break;
        case Natural:
            expression->type = int_type;
            expression->isExpr = 1;
            break;
        case Decimal:
            expression->type = double_type;
            expression->isExpr = 1;
            break;
        case ChrLit:
            expression->type = int_type;
            expression->isExpr = 1;
            break;
        case Add:
            check_expression(getchild(expression, 0), scope);
            check_expression(getchild(expression, 1), scope);

            if ((getchild(expression, 0)->type == int_type && getchild(expression, 1)->type == int_type) || (getchild(expression, 0)->type == char_type && getchild(expression, 1)->type == int_type) || (getchild(expression, 0)->type == int_type && getchild(expression, 1)->type == char_type) || (getchild(expression, 0)->type == short_type && getchild(expression, 1)->type == int_type) || (getchild(expression, 0)->type == int_type && getchild(expression, 1)->type == short_type)){
                expression->type = int_type;
            }else if ((getchild(expression, 0)->type == int_type && getchild(expression, 1)->type == double_type) || (getchild(expression, 0)->type == double_type && getchild(expression, 1)->type == int_type) || (getchild(expression, 0)->type == short_type && getchild(expression, 1)->type == double_type) || (getchild(expression, 1)->type == short_type && getchild(expression, 0)->type == double_type) || (getchild(expression, 0)->type == double_type && getchild(expression, 1)->type == double_type) || (getchild(expression, 0)->type == char_type && getchild(expression, 1)->type == double_type) || (getchild(expression, 0)->type == double_type && getchild(expression, 1)->type == char_type) ){
                expression->type = double_type;
            }else if (((getchild(expression, 0)->type == short_type && getchild(expression, 1)->type == short_type) || (getchild(expression, 0)->type == char_type && getchild(expression, 1)->type == short_type) || (getchild(expression, 0)->type == short_type && getchild(expression, 1)->type == char_type))){
                expression->type = short_type;
            }else if((getchild(expression, 0)->type == char_type && getchild(expression, 1)->type == char_type)){
                expression->type = char_type;
            }else{
                expression->type = no_type;
                printf("Line %d, column %d: Operator + cannot be applied to types %s, %s\n", getchild(expression, 1)->token_line, getchild(expression, 1)->token_column-3, type_name(getchild(expression, 0)->type), type_name(getchild(expression, 1)->type));
            }
            break;
        case Sub:
            check_expression(getchild(expression, 0), scope);
            check_expression(getchild(expression, 1), scope);

            if ((getchild(expression, 0)->type == int_type && getchild(expression, 1)->type == int_type) || (getchild(expression, 0)->type == char_type && getchild(expression, 1)->type == int_type) || (getchild(expression, 0)->type == int_type && getchild(expression, 1)->type == char_type) || (getchild(expression, 0)->type == short_type && getchild(expression, 1)->type == int_type) || (getchild(expression, 0)->type == int_type && getchild(expression, 1)->type == short_type)){
                expression->type = int_type;
            }else if ((getchild(expression, 0)->type == int_type && getchild(expression, 1)->type == double_type) || (getchild(expression, 0)->type == double_type && getchild(expression, 1)->type == int_type) || (getchild(expression, 0)->type == short_type && getchild(expression, 1)->type == double_type) || (getchild(expression, 1)->type == short_type && getchild(expression, 0)->type == double_type) || (getchild(expression, 0)->type == double_type && getchild(expression, 1)->type == double_type) || (getchild(expression, 0)->type == char_type && getchild(expression, 1)->type == double_type) || (getchild(expression, 0)->type == double_type && getchild(expression, 1)->type == char_type) ){
                expression->type = double_type;
            }else if (((getchild(expression, 0)->type == short_type && getchild(expression, 1)->type == short_type) || (getchild(expression, 0)->type == char_type && getchild(expression, 1)->type == short_type) || (getchild(expression, 0)->type == short_type && getchild(expression, 1)->type == char_type))){
                expression->type = short_type;
            }else if((getchild(expression, 0)->type == char_type && getchild(expression, 1)->type == char_type)){
                expression->type = char_type;
            }else{
                expression->type = no_type;
                printf("Line %d, column %d: Operator - cannot be applied to types %s, %s\n", getchild(expression, 1)->token_line, getchild(expression, 1)->token_column-2, type_name(getchild(expression, 0)->type), type_name(getchild(expression, 1)->type));
            }
            break;
        case Mul:
            check_expression(getchild(expression, 0), scope);
            check_expression(getchild(expression, 1), scope);

            if ((getchild(expression, 0)->type == int_type && getchild(expression, 1)->type == int_type) || (getchild(expression, 0)->type == char_type && getchild(expression, 1)->type == int_type) || (getchild(expression, 0)->type == int_type && getchild(expression, 1)->type == char_type) || (getchild(expression, 0)->type == short_type && getchild(expression, 1)->type == int_type) || (getchild(expression, 0)->type == int_type && getchild(expression, 1)->type == short_type)){
                expression->type = int_type;
            }else if ((getchild(expression, 0)->type == int_type && getchild(expression, 1)->type == double_type) || (getchild(expression, 0)->type == double_type && getchild(expression, 1)->type == int_type) || (getchild(expression, 0)->type == double_type && getchild(expression, 1)->type == double_type) || (getchild(expression, 0)->type == char_type && getchild(expression, 1)->type == double_type) || (getchild(expression, 0)->type == double_type && getchild(expression, 1)->type == char_type) || (getchild(expression, 0)->type == short_type && getchild(expression, 1)->type == double_type) || (getchild(expression, 0)->type == double_type && getchild(expression, 1)->type == short_type)){
                expression->type = double_type;
            }else if ((getchild(expression, 0)->type == short_type && getchild(expression, 1)->type == short_type) || (getchild(expression,0)->type == char_type && getchild(expression, 1)->type == short_type) || (getchild(expression, 0)->type == short_type && getchild(expression, 1)->type == char_type)){
                expression->type = short_type;
            }else if((getchild(expression, 0)->type == char_type && getchild(expression, 1)->type == char_type)){
                expression->type = char_type;
            }else{
                expression->type = no_type;
                printf("Line %d, column %d: Operator * cannot be applied to types %s, %s\n", getchild(expression, 1)->token_line, getchild(expression, 1)->token_column-1, type_name(getchild(expression, 0)->type), type_name(getchild(expression, 1)->type));
            }
            break;
        case Div:
            check_expression(getchild(expression, 0), scope);
            check_expression(getchild(expression, 1), scope);

            if ((getchild(expression, 0)->type == int_type && getchild(expression, 1)->type == int_type) || (getchild(expression, 0)->type == char_type && getchild(expression, 1)->type == int_type) || (getchild(expression, 0)->type == int_type && getchild(expression, 1)->type == char_type) || (getchild(expression, 0)->type == short_type && getchild(expression, 1)->type == int_type) || (getchild(expression, 0)->type == int_type && getchild(expression, 1)->type == short_type)){
                expression->type = int_type;
            }else if ((getchild(expression, 0)->type == int_type && getchild(expression, 1)->type == double_type) || (getchild(expression, 0)->type == double_type && getchild(expression, 1)->type == int_type) || (getchild(expression, 0)->type == double_type && getchild(expression, 1)->type == double_type) || (getchild(expression, 0)->type == char_type && getchild(expression, 1)->type == double_type) || (getchild(expression, 0)->type == double_type && getchild(expression, 1)->type == char_type) || (getchild(expression, 0)->type == short_type && getchild(expression, 1)->type == double_type) || (getchild(expression, 0)->type == double_type && getchild(expression, 1)->type == short_type)){
                expression->type = double_type;
            }else if ((getchild(expression, 0)->type == short_type && getchild(expression, 1)->type == short_type) || (getchild(expression,0)->type == char_type && getchild(expression, 1)->type == short_type) || (getchild(expression, 0)->type == short_type && getchild(expression, 1)->type == char_type)){
                expression->type = short_type;
            }else if((getchild(expression, 0)->type == char_type && getchild(expression, 1)->type == char_type)){
                expression->type = char_type;
            }else{
                expression->type = no_type;
                printf("Line %d, column %d: Operator / cannot be applied to types %s, %s\n", getchild(expression, 1)->token_line, getchild(expression, 1)->token_column-1, type_name(getchild(expression, 0)->type), type_name(getchild(expression, 1)->type));
            }
            break;
        case Mod:
            check_expression(getchild(expression, 0), scope);
            check_expression(getchild(expression, 1), scope);
            if (getchild(expression, 0)->type == double_type || getchild(expression, 1)->type == double_type || getchild(expression, 0)->type == no_type || getchild(expression, 1)->type == no_type || getchild(expression, 0)->type == void_type || getchild(expression, 1)->type == void_type){
                expression->type = int_type;
                printf("Line %d, column %d: Operator %% cannot be applied to types %s, %s\n", getchild(expression, 1)->token_line, getchild(expression, 1)->token_column-1, type_name(getchild(expression, 0)->type), type_name(getchild(expression, 1)->type));
            }else{
                expression->type = int_type;
            }
            break;
        case Comma:
            check_expression(getchild(expression, 0), scope);
            if(func_bool == 1){
                check_expression(getchild(expression, 1), scope);
                if (func_bool == 1){
                    expression->type = getchild(expression, 1)->type;
                }else{
                    expression->type = getchild(expression, 1)->type;
                }
            }else{
                check_expression(getchild(expression, 1), scope);
                if (func_bool == 1){
                    expression->type = getchild(expression, 1)->type;
                }else{
                    expression->type = getchild(expression, 1)->type;
                }
            }
            break;

        case Identifier:
            if ((find_func(expression->token)) != -1){
                expression->p = list_func_decla_all_param[find_func(expression->token)];
            }
            if (search_symbol(scope, expression->token) != NULL){
                expression->type = search_symbol(scope, expression->token)->type;
            }else if (search_symbol(symbol_table, expression->token) != NULL){
                if(find_declaration(expression->token) != -1){
                    func_bool = 1;
                }
                expression->type = search_symbol(symbol_table, expression->token)->type;
            }else{
                expression->type = no_type;
                printf("Line %d, column %d: Unknown symbol %s\n", expression->token_line, expression->token_column, expression->token);
            }
            break;
        case Null:
            expression->isExpr = 0;
            expression->type = void_type;
            break;
        case Call:
            
            if (search_symbol(symbol_table, getchild(expression, 0)->token) != NULL){
                
                int count = 1, i = 0;
                expression->type = search_symbol(symbol_table, getchild(expression, 0)->token)->type;
                if (strcmp((search_symbol(symbol_table, getchild(expression, 0)->token)->func_parameters), "") == 0){
                    count = 0;
                }else{
                    char aux[200];
                    strcpy(aux, search_symbol(symbol_table, getchild(expression, 0)->token)->func_parameters);
                    while(aux[i] != '\0'){
                        if (aux[i] == ','){
                            count++;
                        }
                        i++;
                    }
                    i = 0;
                }
                
                i = 1;
                while(getchild(expression, i) != NULL){
                    i++;
                }
                if (count != (i-1)){
                    for (int j = 0; j < i; j++){
                        check_expression(getchild(expression, j), scope);
                    }
                    printf("Line %d, column %d: Wrong number of arguments to function %s (got %d, required %d)\n", getchild(expression, 0)->token_line, getchild(expression, 0)->token_column, getchild(expression, 0)->token, i-1, count);
                }else{
                    i = 1;
                    int check = 0;
                    char *ptr, aux[200];
                    strcpy(aux, search_symbol(symbol_table, getchild(expression, 0)->token)->func_parameters);
                    ptr = strtok(aux, "(");
                    for(int j = i; j < count+i-1; j++){
                        check = 1;
                        check_expression(getchild(expression, j), scope);
                        ptr = strtok(ptr, ",");
                        if ((strcmp(type_name(getchild(expression,j)->type), ptr) != 0)){
                            if ((strcmp(ptr, "short") && ((getchild(expression, j)->type == no_type) || (getchild(expression, j)->type == void_type))) || (strcmp(ptr, "int") && ((getchild(expression, j)->type == no_type) || (getchild(expression, j)->type == void_type)))  || (strcmp(ptr, "char") == 0) || (strcmp(ptr, "double") && ((getchild(expression, j)->type == no_type) || (getchild(expression, j)->type == void_type))) || (strcmp(ptr, "void") == 0) || (strcmp(ptr, "undef") == 0)){
                                printf("Line %d, column %d: Conflicting types (got %s, expected %s)\n", getchild(expression, j)->token_line, getchild(expression, j)->token_column, type_name(getchild(expression, j)->type), ptr);
                                break;
                            }
                        }
                    }
                    int j = count+i-1;
                    if (check == 0){
                        ptr = strtok(ptr, ")");
                    }else{
                        ptr = strtok(NULL, ")");
                    }
                    check_expression(getchild(expression, j), scope);
                    if ((strcmp(type_name(getchild(expression, j)->type), ptr) != 0)){
                        if ((strcmp(ptr, "short") && ((getchild(expression, j)->type == no_type) || (getchild(expression, j)->type == void_type))) || (strcmp(ptr, "int") && ((getchild(expression, j)->type == no_type) || (getchild(expression, j)->type == void_type)))  || (strcmp(ptr, "char") == 0) || (strcmp(ptr, "double") && ((getchild(expression, j)->type == no_type) || (getchild(expression, j)->type == void_type))) || (strcmp(ptr, "void") == 0) || (strcmp(ptr, "undef") == 0)){
                            printf("Line %d, column %d: Conflicting types (got %s, expected %s)\n", getchild(expression, j)->token_line, getchild(expression, j)->token_column, type_name(getchild(expression, j)->type), ptr);
                            break;
                        }
                    }
                    check_expression(getchild(expression, 0), scope);
    
                }
                expression->type = search_symbol(symbol_table, getchild(expression, 0)->token)->type;
            }else if (search_symbol(scope, getchild(expression, 0)->token) != NULL){
                int i = 1;
                while(getchild(expression, i) != NULL){
                    i++;
                }
                if (i > 1){
                    printf("Line %d, column %d: Wrong number of arguments to function %s (got %d, required 0)\n", getchild(expression, 0)->token_line, getchild(expression, 0)->token_column, getchild(expression, 0)->token, i-1);
                }
            }else{
                printf("Line %d, column %d: Unknown symbol %s\n", getchild(expression, 0)->token_line, getchild(expression, 0)->token_column, getchild(expression, 0)->token);
                
                int i = 1;
                while(getchild(expression, i) != NULL){
                    i++;
                }
                if (i > 1){
                    printf("Line %d, column %d: Wrong number of arguments to function %s (got %d, required 0)\n", getchild(expression, 0)->token_line, getchild(expression, 0)->token_column, getchild(expression, 0)->token, i-1);
                }
                expression->type = no_type;
            }
            /*printf("%d\n", isBody);
            printf("Type: %s\n", type_name(expression->type));*/
            break;
        case Store:
            check_expression(getchild(expression, 0), scope);
            check_expression(getchild(expression, 1), scope);
            if(getchild(expression, 0)->category != Identifier){
                printf("Line %d, column %d: Lvalue required\n", getchild(expression, 0)->token_line, getchild(expression, 0)->token_column);
                break;
            }
            if(getchild(expression, 0)->type == no_type || getchild(expression, 1)->type == no_type){
                printf("Line %d, column %d: Operator = cannot be applied to types %s, %s\n", getchild(expression, 1)->token_line, getchild(expression, 1)->token_column-2, type_name(getchild(expression, 0)->type), type_name(getchild(expression, 1)->type));
                expression->type = no_type;
            }else if (getchild(expression, 0)->type != getchild(expression, 1)->type){
                if ((getchild(expression, 0)->type == double_type && ((getchild(expression, 1)->type == void_type) || getchild(expression, 1)->type == no_type)) || ((getchild(expression, 0)->type == char_type) && ((getchild(expression, 1)->type == void_type) || (getchild(expression, 1)->type == double_type))) || (getchild(expression, 0)->type == void_type) || (getchild(expression, 0)->type == no_type) || ((getchild(expression, 0)->type == short_type) && ((getchild(expression, 1)->type == void_type) || (getchild(expression, 1)->type == no_type) || (getchild(expression, 1)->type == double_type))) || ((getchild(expression, 0)->type == int_type) && ((getchild(expression, 1)->type == void_type) || (getchild(expression, 1)->type == no_type) || (getchild(expression, 1)->type == double_type)))){
                    printf("Line %d, column %d: Operator = cannot be applied to types %s, %s\n", getchild(expression, 1)->token_line, getchild(expression, 1)->token_column-1, type_name(getchild(expression, 0)->type), type_name(getchild(expression, 1)->type));
                    expression->type = getchild(expression, 0)->type;
                }else{
                    expression->type = getchild(expression, 0)->type;
                }
            }else{
                expression->type = getchild(expression, 0)->type;
            }
            break;
        case StatList:
            check_statements(expression, scope);
            break;
        case Ne:
            check_expression(getchild(expression, 0), scope);
            check_expression(getchild(expression, 1), scope);
            if ((getchild(expression, 0)->type == getchild(expression, 1)->type) || (getchild(expression, 0)->type == short_type && getchild(expression, 1)->type == int_type) || (getchild(expression, 0)->type == int_type && getchild(expression, 1)->type == short_type) || (getchild(expression, 0)->type == double_type && getchild(expression, 1)->type == int_type) || (getchild(expression, 0)->type == int_type && getchild(expression, 1)->type == double_type) || (getchild(expression, 0)->type == double_type && getchild(expression, 1)->type == short_type) || (getchild(expression, 0)->type == short_type && getchild(expression, 1)->type == double_type) || (getchild(expression, 0)->type == char_type && getchild(expression, 1)->type == int_type) || (getchild(expression, 0)->type == int_type && getchild(expression, 1)->type == char_type) || (getchild(expression, 0)->type == char_type && getchild(expression, 1)->type == short_type) || (getchild(expression, 0)->type == short_type && getchild(expression, 1)->type == char_type) || (getchild(expression, 0)->type == char_type && getchild(expression, 1)->type == double_type) || (getchild(expression, 0)->type == double_type && getchild(expression, 1)->type == char_type)){
                expression->type = int_type;
            }else{
                expression->type = int_type;
                printf("Line %d, column %d: Operator != cannot be applied to types %s, %s\n", getchild(expression, 1)->token_line, getchild(expression, 1)->token_column-3, type_name(getchild(expression, 0)->type), type_name(getchild(expression, 1)->type));
            }
            break;
        case Eq:
            check_expression(getchild(expression, 0), scope);
            check_expression(getchild(expression, 1), scope);
            if ((getchild(expression, 0)->type == getchild(expression, 1)->type) || (getchild(expression, 0)->type == short_type && getchild(expression, 1)->type == int_type) || (getchild(expression, 0)->type == int_type && getchild(expression, 1)->type == short_type) || (getchild(expression, 0)->type == double_type && getchild(expression, 1)->type == int_type) || (getchild(expression, 0)->type == int_type && getchild(expression, 1)->type == double_type) || (getchild(expression, 0)->type == double_type && getchild(expression, 1)->type == short_type) || (getchild(expression, 0)->type == short_type && getchild(expression, 1)->type == double_type) || (getchild(expression, 0)->type == char_type && getchild(expression, 1)->type == int_type) || (getchild(expression, 0)->type == int_type && getchild(expression, 1)->type == char_type) || (getchild(expression, 0)->type == char_type && getchild(expression, 1)->type == short_type) || (getchild(expression, 0)->type == short_type && getchild(expression, 1)->type == char_type) || (getchild(expression, 0)->type == char_type && getchild(expression, 1)->type == double_type) || (getchild(expression, 0)->type == double_type && getchild(expression, 1)->type == char_type)){
                expression->type = int_type;
            }else{
                expression->type = int_type;
                printf("Line %d, column %d: Operator == cannot be applied to types %s, %s\n", getchild(expression, 1)->token_line, getchild(expression, 1)->token_column-3, type_name(getchild(expression, 0)->type), type_name(getchild(expression, 1)->type));
            }
            break;
        case Lt:
            check_expression(getchild(expression, 0), scope);
            check_expression(getchild(expression, 1), scope);
            if ((getchild(expression, 0)->type == getchild(expression, 1)->type) || (getchild(expression, 0)->type == short_type && getchild(expression, 1)->type == int_type) || (getchild(expression, 0)->type == int_type && getchild(expression, 1)->type == short_type) || (getchild(expression, 0)->type == double_type && getchild(expression, 1)->type == int_type) || (getchild(expression, 0)->type == int_type && getchild(expression, 1)->type == double_type) || (getchild(expression, 0)->type == double_type && getchild(expression, 1)->type == short_type) || (getchild(expression, 0)->type == short_type && getchild(expression, 1)->type == double_type) || (getchild(expression, 0)->type == char_type && getchild(expression, 1)->type == int_type) || (getchild(expression, 0)->type == int_type && getchild(expression, 1)->type == char_type) || (getchild(expression, 0)->type == char_type && getchild(expression, 1)->type == short_type) || (getchild(expression, 0)->type == short_type && getchild(expression, 1)->type == char_type) || (getchild(expression, 0)->type == char_type && getchild(expression, 1)->type == double_type) || (getchild(expression, 0)->type == double_type && getchild(expression, 1)->type == char_type)){
                expression->type = int_type;
            }else{
                expression->type = int_type;
                printf("Line %d, column %d: Operator < cannot be applied to types %s, %s\n", getchild(expression, 1)->token_line, getchild(expression, 1)->token_column-2, type_name(getchild(expression, 0)->type), type_name(getchild(expression, 1)->type));
            }
            break;
        case Gt:   
            check_expression(getchild(expression, 0), scope);
            check_expression(getchild(expression, 1), scope);
            if ((getchild(expression, 0)->type == getchild(expression, 1)->type) || (getchild(expression, 0)->type == short_type && getchild(expression, 1)->type == int_type) || (getchild(expression, 0)->type == int_type && getchild(expression, 1)->type == short_type) || (getchild(expression, 0)->type == double_type && getchild(expression, 1)->type == int_type) || (getchild(expression, 0)->type == int_type && getchild(expression, 1)->type == double_type) || (getchild(expression, 0)->type == double_type && getchild(expression, 1)->type == short_type) || (getchild(expression, 0)->type == short_type && getchild(expression, 1)->type == double_type) || (getchild(expression, 0)->type == char_type && getchild(expression, 1)->type == int_type) || (getchild(expression, 0)->type == int_type && getchild(expression, 1)->type == char_type) || (getchild(expression, 0)->type == char_type && getchild(expression, 1)->type == short_type) || (getchild(expression, 0)->type == short_type && getchild(expression, 1)->type == char_type) || (getchild(expression, 0)->type == char_type && getchild(expression, 1)->type == double_type) || (getchild(expression, 0)->type == double_type && getchild(expression, 1)->type == char_type)){
                expression->type = int_type;
            }else{
                expression->type = int_type;
                printf("Line %d, column %d: Operator > cannot be applied to types %s, %s\n", getchild(expression, 1)->token_line, getchild(expression, 1)->token_column-2, type_name(getchild(expression, 0)->type), type_name(getchild(expression, 1)->type));
            }
            break;
        case Le:
            check_expression(getchild(expression, 0), scope);
            check_expression(getchild(expression, 1), scope);
            if ((getchild(expression, 0)->type == getchild(expression, 1)->type) || (getchild(expression, 0)->type == short_type && getchild(expression, 1)->type == int_type) || (getchild(expression, 0)->type == int_type && getchild(expression, 1)->type == short_type) || (getchild(expression, 0)->type == double_type && getchild(expression, 1)->type == int_type) || (getchild(expression, 0)->type == int_type && getchild(expression, 1)->type == double_type) || (getchild(expression, 0)->type == double_type && getchild(expression, 1)->type == short_type) || (getchild(expression, 0)->type == short_type && getchild(expression, 1)->type == double_type) || (getchild(expression, 0)->type == char_type && getchild(expression, 1)->type == int_type) || (getchild(expression, 0)->type == int_type && getchild(expression, 1)->type == char_type) || (getchild(expression, 0)->type == char_type && getchild(expression, 1)->type == short_type) || (getchild(expression, 0)->type == short_type && getchild(expression, 1)->type == char_type) || (getchild(expression, 0)->type == char_type && getchild(expression, 1)->type == double_type) || (getchild(expression, 0)->type == double_type && getchild(expression, 1)->type == char_type)){
                expression->type = int_type;
            }else{
                expression->type = int_type;
                printf("Line %d, column %d: Operator <= cannot be applied to types %s, %s\n", getchild(expression, 1)->token_line, getchild(expression, 1)->token_column-2, type_name(getchild(expression, 0)->type), type_name(getchild(expression, 1)->type));
            }
            break;  
        case Ge:
            check_expression(getchild(expression, 0), scope);
            check_expression(getchild(expression, 1), scope);
            if ((getchild(expression, 0)->type == getchild(expression, 1)->type) || (getchild(expression, 0)->type == short_type && getchild(expression, 1)->type == int_type) || (getchild(expression, 0)->type == int_type && getchild(expression, 1)->type == short_type) || (getchild(expression, 0)->type == double_type && getchild(expression, 1)->type == int_type) || (getchild(expression, 0)->type == int_type && getchild(expression, 1)->type == double_type) || (getchild(expression, 0)->type == double_type && getchild(expression, 1)->type == short_type) || (getchild(expression, 0)->type == short_type && getchild(expression, 1)->type == double_type) || (getchild(expression, 0)->type == char_type && getchild(expression, 1)->type == int_type) || (getchild(expression, 0)->type == int_type && getchild(expression, 1)->type == char_type) || (getchild(expression, 0)->type == char_type && getchild(expression, 1)->type == short_type) || (getchild(expression, 0)->type == short_type && getchild(expression, 1)->type == char_type) || (getchild(expression, 0)->type == char_type && getchild(expression, 1)->type == double_type) || (getchild(expression, 0)->type == double_type && getchild(expression, 1)->type == char_type)){
                expression->type = int_type;
            }else{
                expression->type = int_type;
                printf("Line %d, column %d: Operator >= cannot be applied to types %s, %s\n", getchild(expression, 1)->token_line, getchild(expression, 1)->token_column-2, type_name(getchild(expression, 0)->type), type_name(getchild(expression, 1)->type));
            }
            break;
        case Or:
            check_expression(getchild(expression, 0), scope);
            check_expression(getchild(expression, 1), scope);

            if (getchild(expression, 0)->type == double_type || getchild(expression, 1)->type == double_type || getchild(expression, 0)->type == no_type || getchild(expression, 1)->type == no_type || getchild(expression, 0)->type == void_type || getchild(expression, 1)->type == void_type){
                expression->type = int_type;
                printf("Line %d, column %d: Operator || cannot be applied to types %s, %s\n", getchild(expression, 1)->token_line, getchild(expression, 1)->token_column-2, type_name(getchild(expression, 0)->type), type_name(getchild(expression, 1)->type));
            }else{
                expression->type = int_type;
            }
            break;
        case And:
            check_expression(getchild(expression, 0), scope);
            check_expression(getchild(expression, 1), scope);
            if (getchild(expression, 0)->type == double_type || getchild(expression, 1)->type == double_type || getchild(expression, 0)->type == no_type || getchild(expression, 1)->type == no_type || getchild(expression, 0)->type == void_type || getchild(expression, 1)->type == void_type){
                expression->type = int_type;
                printf("Line %d, column %d: Operator && cannot be applied to types %s, %s\n", getchild(expression, 1)->token_line, getchild(expression, 1)->token_column-2, type_name(getchild(expression, 0)->type), type_name(getchild(expression, 1)->type));
            }else{
                expression->type = int_type;
            }
            break;
        case BitWiseAnd:
            check_expression(getchild(expression, 0), scope);
            check_expression(getchild(expression, 1), scope);
            if (getchild(expression, 0)->type == double_type || getchild(expression, 1)->type == double_type || getchild(expression, 0)->type == no_type || getchild(expression, 1)->type == no_type || getchild(expression, 0)->type == void_type || getchild(expression, 1)->type == void_type){
                expression->type = int_type;
                printf("Line %d, column %d: Operator & cannot be applied to types %s, %s\n", getchild(expression, 1)->token_line, getchild(expression, 1)->token_column-1, type_name(getchild(expression, 0)->type), type_name(getchild(expression, 1)->type));
            }else{
                expression->type = int_type;
            }
            break;
        case BitWiseOr:
            check_expression(getchild(expression, 0), scope);
            check_expression(getchild(expression, 1), scope);
            if (getchild(expression, 0)->type == double_type || getchild(expression, 1)->type == double_type || getchild(expression, 0)->type == no_type || getchild(expression, 1)->type == no_type || getchild(expression, 0)->type == void_type || getchild(expression, 1)->type == void_type){
                expression->type = int_type;
                printf("Line %d, column %d: Operator | cannot be applied to types %s, %s\n", getchild(expression, 1)->token_line, getchild(expression, 1)->token_column-1, type_name(getchild(expression, 0)->type), type_name(getchild(expression, 1)->type));
            }else{
                expression->type = int_type;
            }
            break;
        case BitWiseXor:
            check_expression(getchild(expression, 0), scope);
            check_expression(getchild(expression, 1), scope);
            if (getchild(expression, 0)->type == double_type || getchild(expression, 1)->type == double_type || getchild(expression, 0)->type == no_type || getchild(expression, 1)->type == no_type || getchild(expression, 0)->type == void_type || getchild(expression, 1)->type == void_type){
                expression->type = int_type;
                printf("Line %d, column %d: Operator ^ cannot be applied to types %s, %s\n", getchild(expression, 1)->token_line, getchild(expression, 1)->token_column-1, type_name(getchild(expression, 0)->type), type_name(getchild(expression, 1)->type));
            }else{
                expression->type = int_type;
            }
            break;
        case If:
            check_ifstatement(expression, scope);
            break;
        case While:
            check_while(expression, scope);
            break;
        case Plus:
            check_expression(getchild(expression, 0), scope);
            if ((getchild(expression, 0)->type == void_type) || (getchild(expression, 0)->type == no_type)){
                expression->type = no_type;
                printf("Line %d, column %d: Operator + cannot be applied to type %s\n", getchild(expression, 0)->token_line, getchild(expression, 0)->token_column-1, type_name(getchild(expression, 0)->type));
            }else{
                expression->type = getchild(expression, 0)->type;
            }
            break;
        case Minus:
            check_expression(getchild(expression, 0), scope);
            if ((getchild(expression, 0)->type == void_type) || (getchild(expression, 0)->type == no_type)){
                expression->type = no_type;
                printf("Line %d, column %d: Operator - cannot be applied to type %s\n", getchild(expression, 0)->token_line, getchild(expression, 0)->token_column-1, type_name(getchild(expression, 0)->type));
            }else{
                expression->type = getchild(expression, 0)->type;
            }
            break;
        case Not:
            check_expression(getchild(expression, 0), scope);
            if ((getchild(expression, 0)->type == void_type) || (getchild(expression, 0)->type == no_type)){
                expression->type = no_type;
                printf("Line %d, column %d: Operator ! cannot be applied to type %s\n", getchild(expression, 0)->token_line, getchild(expression, 0)->token_column-1, type_name(getchild(expression, 0)->type));
            }else{
                expression->type = int_type;
            }
            break;
        
    }
}

int find_declaration(char *id) {
    for (int i = 0; i < num_list_func_declarations; i++) {
        if (list_func_declarations[i] != NULL){
            if (strcmp(id, list_func_declarations[i]) == 0) {
                return i;
            }
        }
    }
    return -1;
}

int find_func(char *id) {
    for (int i = 0; i < num_list_func_decla_all; i++) {
        if (list_func_decla_all[i] != NULL){
            if (strcmp(id, list_func_decla_all[i]) == 0) {
                return i;
            }
        }
    }
    return -1;
}

void remove_declaration(int pos){
    num_list_func_declarations--;
    free(list_func_declarations[pos]);

    for (int i = pos; i < num_list_func_declarations; i++) {
        list_func_declarations[i] = list_func_declarations[i + 1];
    }

    list_func_declarations[num_list_func_declarations] = NULL;

    free(list_func_decla_param[pos]);

    for (int i = pos; i < num_list_func_declarations; i++) {
        list_func_decla_param[i] = list_func_decla_param[i + 1];
    }

    list_func_declarations[num_list_func_declarations] = NULL;
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

//CORRIGIR PARA FAZER A TROCA DE CENAS QUANDO NÃO ESTÁ NA ORDEM CERTA

void check_scope(struct symbol_list_list **scope) {
    struct symbol_list_list *currentScope = *scope;

    while (currentScope != NULL) {
        struct symbol_list_list *auxScope = *scope;
        int index_1 = -1, index_2 = -1;

        while (auxScope != NULL) {
            for (int i = 0; i < num_list_func_decla_all; i++) {
                if (strcmp(auxScope->func_name, list_func_decla_all[i]) == 0) {
                    index_1 = i;
                }
                if (auxScope->next != NULL && strcmp(auxScope->next->func_name, list_func_decla_all[i]) == 0) {
                    index_2 = i;
                }
            }

            if (auxScope->next != NULL && index_2 < index_1) {

                char temp[100];
                strcpy(temp, auxScope->func_name);
                strcpy(auxScope->func_name, auxScope->next->func_name);
                strcpy(auxScope->next->func_name, temp);

                // Troca de symbol_list
                struct symbol_list *temp_list = auxScope->symbol_list;
                auxScope->symbol_list = auxScope->next->symbol_list;
                auxScope->next->symbol_list = temp_list;

                index_1 = -1;
                index_2 = -1;
            }

            auxScope = auxScope->next;
        }
        currentScope = currentScope->next;
    } 
}

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

    printf("===== Global Symbol Table =====\n");
    for (symbol = symbol_table->next; symbol != NULL; symbol = symbol->next) {
        printf("%s\t%s%s\n", symbol->identifier, type_name(symbol->type), symbol->func_parameters);
    }

    struct symbol_list_list *currentScope = scope_list;
    reverse_scope(&currentScope);
    check_scope(&currentScope);
    while (currentScope != NULL) {
        printf("\n===== Function %s Symbol Table =====\n", currentScope->func_name);
        for (symbol = currentScope->symbol_list->next; symbol != NULL; symbol = symbol->next) {
            printf("%s\t%s%s\n", symbol->identifier, type_name(symbol->type), symbol->func_parameters);
        }
        currentScope = currentScope->next;
    }
}