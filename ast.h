#ifndef _AST_H
#define _AST_H

// the order of the enum and the #define must precisely match
enum category
{
  Program,
  Declaration,
  FuncDeclaration,
  FuncDefinition,
  ParamList,
  FuncBody,
  ParamDeclaration,
  StatList,
  If,
  While,
  Return,
  Or,
  And,
  Eq,
  Ne,
  Lt,
  Gt,
  Le,
  Ge,
  Add,
  Sub,
  Mul,
  Div,
  Mod,
  Not,
  Minus,
  Plus,
  Store,
  Comma,
  Call,
  BitWiseAnd,
  BitWiseXor,
  BitWiseOr,
  Char,
  ChrLit,
  Identifier,
  Int,
  Short,
  Natural,
  Double,
  Decimal,
  Void,
  Error,
  Null,
  Temp
};
#define names {"Program", "Declaration", "FuncDeclaration", "FuncDefinition", "ParamList", "FuncBody", "ParamDeclaration", "StatList", "If", "While", "Return", "Or", "And", "Eq", "Ne", "Lt", "Gt", "Le", "Ge", "Add", "Sub", "Mul", "Div", "Mod", "Not", "Minus", "Plus", "Store", "Comma", "Call", "BitWiseAnd", "BitWiseXor", "BitWiseOr", "Char", "ChrLit", "Identifier", "Int", "Short", "Natural", "Double", "Decimal", "Void", "Null", "Null"};

enum type
{
  integer_type,
  double_type,
  no_type
};

struct node
{
  enum category category;
  char *token;
  struct node_list *children;
};

struct node_list
{
  struct node *node;
  struct node_list *next;
};

struct node *newnode(enum category category, char *token);
void addchild(struct node *parent, struct node *child);
void addChildNull(struct node *parent, struct node *child, int flag);
void inheritChilds(struct node *root, struct node *parent);
void show(struct node *root, int depth);
void addTypeToDeclarators(struct node *declaratorList, struct node *type);
int countChildren(struct node *root);
void deallocate(struct node *root);

#endif