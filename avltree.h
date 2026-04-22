/*
 * avltree.h — AVL Tree Abstract Machine
 *
 * Key type : char*  — strings compared with strcmp().
 *            The tree stores a pointer to the key; it does NOT copy it.
 *            The caller must keep the key string alive for the node's lifetime.
 *
 * Value type: void* — attach any payload to a node (or NULL if unused).
 *
 * State    : rooted BST where |height(left) - height(right)| <= 1 everywhere
 * Complexity: search / insert / delete all O(log n)
 *
 * Usage
 *   Node *root = NULL;
 *   root = avl_insert(root, "banana", NULL);
 *   root = avl_insert(root, "apple",  NULL);
 *   Node *n = avl_search(root, "apple");   // not NULL
 *   root = avl_delete(root, "banana");
 *   avl_free(root);
 *
 * Usage with values (e.g. a simple string->int dictionary)
 *   int count = 5;
 *   root = avl_insert(root, "hello", (void *)(intptr_t)count);
 *   Node *n = avl_search(root, "hello");
 *   int v = (int)(intptr_t)n->value;
 */

#ifndef AVLTREE_H
#define AVLTREE_H

#include <stdint.h>   /* intptr_t */

/* ── type ───────────────────────────────────────────────────────────── */

typedef struct Node {
    const char  *key;     /* string key — pointer, not a copy         */
    void        *value;   /* arbitrary payload; NULL if unused         */
    int          height;  /* cached subtree height (leaf=1, NULL=0)   */
    struct Node *left;    /* keys < this->key                         */
    struct Node *right;   /* keys > this->key                         */
} Node;

/* ── core operations ────────────────────────────────────────────────── */

/**
 * avl_insert — insert (key, value) into subtree rooted at n.
 * If key already exists its value is updated; no duplicate nodes.
 * Returns the (possibly new) subtree root.
 *
 *   root = avl_insert(root, "hello", my_ptr);
 */
Node *avl_insert(Node *n, const char *key, void *value);

/**
 * avl_delete — remove node with key from subtree rooted at n.
 * Returns the (possibly new) subtree root.  No-op if key not found.
 *
 *   root = avl_delete(root, "hello");
 */
Node *avl_delete(Node *n, const char *key);

/**
 * avl_search — find node with key.  Returns NULL if not found.
 * O(log n) iterative — does not modify the tree.
 */
Node *avl_search(Node *n, const char *key);

/* ── memory ─────────────────────────────────────────────────────────── */

/**
 * avl_free — recursively free all nodes.
 * Does NOT free key strings or value pointers — caller owns them.
 */
void avl_free(Node *n);

/* ── diagnostics ────────────────────────────────────────────────────── */

/**
 * avl_print — pretty-print the tree rotated 90° (right subtree at top).
 * Shows key, height, and balance factor for each node.
 */
void avl_print(const Node *root);

#endif /* AVLTREE_H */
