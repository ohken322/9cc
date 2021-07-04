#include "9cc.h"


Node *stmt();
Node *expr();
Node *assign();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *primary();
Node *unary();

LVar *locals = NULL;

Node *new_node(NodeKind kind) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  return node;
}

Node *new_binary(NodeKind kind, Node *lhs, Node *rhs) {
  Node *node = new_node(kind);
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node *new_num(int val) {
  Node *node = new_node(ND_NUM);
  node->val = val;
  return node;
}

// 次のトークンが期待している記号のときには、トークンを1つ読み進めて
// 真を返す。それ以外の場合には偽を返す。
bool consume(char *op) {
  if (token->kind != TK_RESERVED || strlen(op) != token->len || memcmp(token->str, op, token->len))
    return false;
  token = token->next;
  return true;
}

Token *consume_ident() {
  Token *tok;
  if (token->kind != TK_IDENT)
    return NULL;
  tok = token;
  token = token->next;
  return tok;
}

// 次のトークンが期待している記号のときには、トークンを1つ読み進める。
// それ以外の場合にはエラーを報告する。
void expect(char *op) {
  if (token->kind != TK_RESERVED || strlen(op) != token->len || memcmp(token->str, op, token->len))
    error("'%c'ではありません", op);
  token = token->next;
}

// 次のトークンが数値の場合、トークンを1つ読み進めてその数値を返す。
// それ以外の場合にはエラーを報告する。
int expect_number() {
  if (token->kind != TK_NUM)
    error("数ではありません");
  int val = token->val;
  token = token->next;
  return val;
}

bool at_eof() {
  return token->kind == TK_EOF;
}

LVar *find_lvar(Token *tok) {
  for (LVar *var=locals; var; var = var->next){
    if (var->len == tok->len && !memcmp(tok->str, var->name, var->len))
      return var;
  }
  return NULL;
}

void program() {
  int i = 0;
  while (!at_eof())
    code[i++] = stmt();
  code[i] = NULL;
}

Node *stmt() {
  Node *node;

  if (consume("return")) {
    node = new_node(ND_RETURN);
    node->lhs = expr();
    expect(";");
  } else if (consume("if")) {    
    expect("(");
    node = new_node(ND_IF);
    node->cond = expr();
    expect(")");
    node->then = stmt();
    if (consume("else")){
      node->els = stmt();
    }
    // fprintf(stderr, "%s\n", token->str);
  } else if (consume("while")) {    
    expect("(");
    node = new_node(ND_WHILE);
    node->cond = expr();
    expect(")");
    node->then = stmt();
  } else if (consume("for")) {    
    node = new_node(ND_FOR);
    expect("(");
    if (!consume(";")){
      node->init = expr();
      expect(";");
    }
    if (!consume(";")){
      node->cond = expr();
      expect(";");
    }
    if (!consume(")")){
      node->inc = expr();
      expect(")");
    }
    node->then = stmt();
    fprintf(stderr, "%s\n", token->str);
  } else {
    node = expr();
    expect(";");
  }
  return node;
}

Node *expr() {
  return assign();
}

Node *assign() {
  Node *node = equality();
  if (consume("="))
    node = new_binary(ND_ASSIGN, node, assign());
  return node;
}

Node *equality() {
  Node *node = relational();

  for (;;) {
    if (consume("=="))
      node = new_binary(ND_EQ, node, relational());
    else if (consume("!="))
      node = new_binary(ND_NE, node, relational());
    else
      return node;
  }
}

Node *relational() {
  Node *node = add();

  for (;;) {
    if (consume("<"))
      node = new_binary(ND_LT, node, add());
    else if (consume("<="))
      node = new_binary(ND_LE, node, add());
    else if (consume(">"))
      node = new_binary(ND_LT, add(), node);  // 二項演算の順序を逆にする，綺麗に書けるんだな
    else if (consume(">="))
      node = new_binary(ND_LE, add(), node);
    else
      return node;
  }
}

Node *add() {
  Node *node = mul();

  for (;;) {
    if (consume("+"))
      node = new_binary(ND_ADD, node, mul());
    else if (consume("-"))
      node = new_binary(ND_SUB, node, mul());
    else
      return node;
  }
}

Node *mul() {
  Node *node = unary();

  for (;;) {
    if (consume("*"))
      node = new_binary(ND_MUL, node, unary());
    else if (consume("/"))
      node = new_binary(ND_DIV, node, unary());
    else
      return node;
  }
}

Node *unary() {
  if (consume("+"))
    return unary();
  if (consume("-"))
    return new_binary(ND_SUB, new_num(0), unary());
  return primary();
}

Node *primary() {
  // 次のトークンが"("なら、"(" expr ")"のはず
  if (consume("(")) {
    Node *node = expr();
    expect(")");
    return node;
  }

  Token *tok = consume_ident();
  if (tok){
    Node *node = new_node(ND_LVAR);

    LVar *lvar = find_lvar(tok);
    if (lvar) {
      node->offset = lvar->offset;
    } else {
      lvar = calloc(1, sizeof(LVar));
      lvar->next = locals;
      lvar->name = tok->str;
      lvar->len = tok->len;
      if (locals) {
        lvar->offset = locals->offset + 8;
      } else {
        lvar->offset = 8;
      }
      node->offset = lvar->offset;
      // fprintf(stderr, "%d\n", lvar->offset);
      // fprintf(stderr, "%s\n", lvar->name);

      locals = lvar;
    }
    return node;
  }

  return new_num(expect_number());
}
