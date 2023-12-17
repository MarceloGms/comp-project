#ifndef _AST_H
#define _AST_H

// the order of the enum and the #define must precisely match
enum category
{
    Program, Declaration, FuncDeclaration, FuncDefinition, ParamList, FuncBody, ParamDeclaration, StatList, If, While, Return, Or, And, Eq, Ne, Lt, Gt, Le, Ge,
    Add, Sub, Mul, Div, Mod, Not, Minus, Plus, Store, Comma, Call, BitWiseAnd, BitWiseXor, BitWiseOr, Char, ChrLit, Identifier, Int, Short, Natural, Double,
    Decimal, Void, Error, Null, Temp
};
#define names {"Program", "Declaration", "FuncDeclaration", "FuncDefinition", "ParamList", "FuncBody", "ParamDeclaration", "StatList", "If", "While", "Return", "Or", "And", "Eq", "Ne", "Lt", "Gt", "Le", "Ge", "Add", "Sub", "Mul", "Div", "Mod", "Not", "Minus", "Plus", "Store", "Comma", "Call", "BitWiseAnd", "BitWiseXor", "BitWiseOr", "Char", "ChrLit", "Identifier", "Int", "Short", "Natural", "Double", "Decimal", "Void", "Error", "Null", "Temp"};

enum type
{
    no_type,
    int_type,
    double_type,
    short_type,
    char_type,
    void_type,
    type_none
};

#define type_name(type) (type == int_type ? "int" : (type == double_type ? "double" : (type == short_type ? "short" : (type == char_type ? "char" : (type == void_type ? "void" : (type == no_type ? "undef" : "none"))))))
#define category_type(symbol) (category == Int ? int_type : (category == Double ? double_type : (category == Short ? short_type : (category == Char ? char_type : (category == Void ? void_type :no_type)))))

struct node {
  enum category category;
  char *token;
  int token_line, token_column;
  enum type type;
  struct node_list *children;
  int isExpr;
  char *p;
};

struct node_list {
  struct node *node;
  struct node_list *next;
};

struct node *newnode(enum category category, char *token);
void addchild(struct node *parent, struct node *child);
void addChildNull(struct node *parent, struct node *child, int flag);
void inheritChilds(struct node *root, struct node * parent);
void show(struct node *root, int depth);
void addTypeToDeclarators(struct node *declaratorList, struct node *type);
struct node *getchild(struct node *parent, int position);
int countChildren(struct node *root);
void deallocate(struct node *root);

#endif