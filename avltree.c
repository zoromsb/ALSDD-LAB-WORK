/*
 * avltree.c — AVL Tree Abstract Machine  (implementation)
 * See avltree.h for full API documentation.
 */

#include "avltree.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ── helpers ────────────────────────────────────────────────────────── */

static int  ht(const Node *n)       { return n ? n->height : 0; }
static int  bf(const Node *n)       { return n ? ht(n->left) - ht(n->right) : 0; }
static int  max2(int a, int b)      { return a > b ? a : b; }
static void upd(Node *n)            { if (n) n->height = 1 + max2(ht(n->left), ht(n->right)); }

static Node *new_node(const char *key, void *value) {
    Node *n  = malloc(sizeof(Node));
    if (!n) { perror("avl: malloc"); exit(1); }
    n->key    = key;
    n->value  = value;
    n->height = 1;
    n->left   = n->right = NULL;
    return n;
}

/* ── rotations ──────────────────────────────────────────────────────── */

static Node *rot_right(Node *y) {
    Node *x = y->left, *B = x->right;
    x->right = y; y->left = B;
    upd(y); upd(x);
    return x;
}

static Node *rot_left(Node *x) {
    Node *y = x->right, *B = y->left;
    y->left = x; x->right = B;
    upd(x); upd(y);
    return y;
}

/* ── rebalance ──────────────────────────────────────────────────────── */

static Node *rebalance(Node *n) {
    upd(n);
    int b = bf(n);
    if (b ==  2) { if (bf(n->left)  < 0) n->left  = rot_left(n->left);   return rot_right(n); }
    if (b == -2) { if (bf(n->right) > 0) n->right = rot_right(n->right); return rot_left(n);  }
    return n;
}

/* ── core operations ────────────────────────────────────────────────── */

Node *avl_insert(Node *n, const char *key, void *value) {
    if (!n) return new_node(key, value);
    int cmp = strcmp(key, n->key);
    if      (cmp < 0) n->left  = avl_insert(n->left,  key, value);
    else if (cmp > 0) n->right = avl_insert(n->right, key, value);
    else { n->value = value; return n; }   /* update value on duplicate key */
    return rebalance(n);
}

static Node *min_node(Node *n) { while (n->left) n = n->left; return n; }

Node *avl_delete(Node *n, const char *key) {
    if (!n) return NULL;
    int cmp = strcmp(key, n->key);
    if      (cmp < 0) n->left  = avl_delete(n->left,  key);
    else if (cmp > 0) n->right = avl_delete(n->right, key);
    else {
        if (!n->left || !n->right) {
            Node *child = n->left ? n->left : n->right;
            free(n);
            return child;
        }
        Node *succ = min_node(n->right);
        n->key   = succ->key;
        n->value = succ->value;
        n->right = avl_delete(n->right, succ->key);
    }
    return rebalance(n);
}

Node *avl_search(Node *n, const char *key) {
    while (n) {
        int cmp = strcmp(key, n->key);
        if      (cmp < 0) n = n->left;
        else if (cmp > 0) n = n->right;
        else return n;
    }
    return NULL;
}

void avl_free(Node *n) {
    if (!n) return;
    avl_free(n->left);
    avl_free(n->right);
    free(n);
}

/* ── diagnostics ────────────────────────────────────────────────────── */

static void print_tree(const Node *n, int indent) {
    if (!n) return;
    print_tree(n->right, indent + 4);
    printf("%*s\"%s\" (h=%d, bf=%+d)\n", indent, "", n->key, n->height, bf(n));
    print_tree(n->left,  indent + 4);
}

void avl_print(const Node *root) {
    printf("AVL tree (rotated 90 deg, right subtree at top):\n");
    if (!root) { printf("  (empty)\n"); return; }
    print_tree(root, 2);
    printf("  root=\"%s\"  height=%d\n", root->key, ht(root));
}
