#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// tokenize.c

// トークンの種類
typedef enum {
  TK_RESERVED, // 記号
  TK_IDENT,    // 識別子(変数)
  TK_NUM,      // 整数トークン
  TK_EOF,      // 入力の終わりを表すトークン
  TK_RETURN,   // returnを表すトークン
} TokenKind;

typedef struct Token Token;

// トークン型
struct Token {
  TokenKind kind; // トークンの型
  Token *next;    // 次の入力トークン
  int val;        // kindがTK_NUMの場合、その数値
  char *str;      // トークン文字列
  int len;
};

void error(char *fmt, ...);

Token *new_token(TokenKind kind, Token *cur, char *str, int len);

Token *tokenize(char *p);

bool is_ident1(char c);

bool is_ident2(char c);

bool startswith(char *p, char *q);


// parse.c

void program();

typedef struct LVar LVar;

// ローカル変数の型
struct LVar {
  LVar *next;
  char *name; // 変数の名前
  int len;    // 名前の長さ
  int offset; // RBPからのオフセット
};

// 抽象構文木のノードの種類
typedef enum {
  ND_ADD, // +
  ND_SUB, // -
  ND_MUL, // *
  ND_DIV, // /
  ND_ASSIGN, // =
  ND_LVAR, // variables
  ND_EQ,  // ==
  ND_NE,  // !=
  ND_LT,  // <  (>, >= についてはノードの順を逆にすることで対応する)
  ND_LE,  // <= 
  ND_NUM, // Integer
  ND_RETURN,
} NodeKind;

typedef struct Node Node;

// 抽象構文木のノードの型
struct Node {
  NodeKind kind; // ノードの型
  Node *lhs;     // 左辺
  Node *rhs;     // 右辺
  int val;       // kindがND_NUMの場合のみ使う
  int offset;    // kindがND_LVARの場合のみ使う
};

Node *code[100];

Node *new_node(NodeKind kind);

Node *new_binary(NodeKind kind, Node *lhs, Node *rhs);

Node *new_num(int val);

bool consume(char *op);

Token *consume_ident();

void expect(char *op);

int expect_number();

bool at_eof();


// codegen.c

void gen(Node *node);
