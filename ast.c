#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ast.h"

// category names #defined in ast.h
char *category_name[] = names;

// create a node of a given category with a given lexical symbol
struct node *newnode(enum category category, char *token)
{
  struct node *new = malloc(sizeof(struct node));
  new->category = category;
  new->token = token;
  new->type = type_none;
  new->isExpr = 0;
  new->children = malloc(sizeof(struct node_list));
  new->children->node = NULL;
  new->children->next = NULL;
  return new;
}

// append a node to the list of children of the parent node
void addchild(struct node *parent, struct node *child)
{
  struct node_list *new = malloc(sizeof(struct node_list));
  new->node = child;
  new->next = NULL;

  struct node_list *children = parent->children;
  while (children->next != NULL)
    children = children->next;
  children->next = new;
}

void addChildNull(struct node *parent, struct node *child, int flag)
{
  if (child != NULL)
    addchild(parent, child);

  if (flag && child == NULL)
  {
    addchild(parent, newnode(Null, NULL));
    return;
  }
}

// traverse the AST and print its content
void show(struct node *node, int depth)
{
  for (int i = 0; i < depth; i++)
    printf("..");
  char type[100];
  strcpy(type, type_name(node->type));
  if (node->p != NULL)
    strcat(type, node->p);
  if (node->token == NULL) {
    if (node->isExpr != 1) {
      printf("%s\n", category_name[node->category]);
    } else {
      switch (node->type)
      {
      case type_none:
        printf("%s\n", category_name[node->category]);
        break;  

      case no_type:
        printf("%s - undef\n", category_name[node->category]);
        break;

      case int_type:
        printf("%s - %s\n", category_name[node->category], type);
        break;

      case double_type:
        printf("%s - %s\n", category_name[node->category], type);
        break;

      case short_type:  
        printf("%s - %s\n", category_name[node->category], type);
        break;

      case char_type:
        printf("%s - %s\n", category_name[node->category], type);
        break;

      case void_type:
        printf("%s - %s\n", category_name[node->category], type);
        break;

      default:
        break;
      }
    }
  }else{
    if (node->isExpr != 1) {
      printf("%s(%s)\n", category_name[node->category], node->token);
    } else {
      switch (node->type)
      {
      case type_none:
        printf("%s(%s)\n", category_name[node->category], node->token);
        break;

      case no_type:
        printf("%s(%s) - undef\n", category_name[node->category], node->token);
        break;

      case int_type:
        printf("%s(%s) - %s\n", category_name[node->category], node->token, type);
        break;

      case double_type:
        printf("%s(%s) - %s\n", category_name[node->category], node->token, type);
        break;

      case short_type:  
        printf("%s(%s) - %s\n", category_name[node->category], node->token, type);
        break;

      case char_type:
        printf("%s(%s) - %s\n", category_name[node->category], node->token, type);
        break;

      case void_type:
        printf("%s(%s) - %s\n", category_name[node->category], node->token, type);
        break;

      default:
        break;
      }
    }
  }

  struct node_list *child = node->children;
  while ((child = child->next) != NULL)
    show(child->node, depth + 1);
}

// inherit the childs of a node to another node
void inheritChilds(struct node *root, struct node *parent)
{
  // make the root inherit all the parent childs
  struct node_list *temp = root->children;
  while (temp->next != NULL)
    temp = temp->next;
  temp->next = parent->children->next;
}

void addTypeToDeclarators(struct node *declaratorList, struct node *type)
{
  struct node_list *temp = declaratorList->children->next;

  while (temp != NULL)
  { // make the first child of the declarator the type and the next child the identifier
    struct node_list *typeNode = malloc(sizeof(struct node_list));
    typeNode->node = type;
    typeNode->next = temp->node->children->next;
    temp->node->children->next = typeNode;
    temp = temp->next;
  }
}

struct node *getchild(struct node *parent, int position)
{
  struct node_list *children = parent->children;
  while ((children = children->next) != NULL)
    if (position-- == 0)
      return children->node;
  return NULL;
}

int countChildren(struct node *root)
{
  int count = 0;
  struct node_list *temp = root->children;
  while (temp->next != NULL)
  {
    count++;
    temp = temp->next;
  }
  return count;
}

// free the AST
void deallocate(struct node *node)
{
  if (node != NULL)
  {
    struct node_list *child = node->children;
    while (child != NULL)
    {
      deallocate(child->node);
      struct node_list *tmp = child;
      child = child->next;
      free(tmp);
    }
    if (node->token != NULL)
      free(node->token);
    free(node);
  }
}