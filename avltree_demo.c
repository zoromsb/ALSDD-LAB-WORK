/*
 * avltree_demo.c — demo for the AVL tree abstract machine.
 * Compile: gcc -Wall -o avltree_demo avltree_demo.c avltree.c
 */

#include "avltree.c"
#include <stdio.h>
#include <stdint.h>

/* in-order walk prints keys sorted */
static void inorder(const Node *n) {
    if (!n) return;
    inorder(n->left);
    printf("  \"%s\"", n->key);
    if (n->value) printf(" -> %d", (int)(intptr_t)n->value);
    printf("\n");
    inorder(n->right);
}

int main(void) {
    printf("=== AVL Tree — string key demo ===\n\n");

    Node *root = NULL;

    /* plain string set */
    const char *fruits[] = {
        "mango", "apple", "cherry", "banana",
        "date",  "fig",   "grape",  "kiwi"
    };
    int n = sizeof(fruits) / sizeof(fruits[0]);

    printf("-- inserting %d fruits\n", n);
    for (int i = 0; i < n; i++)
        root = avl_insert(root, fruits[i], NULL);

    avl_print(root);

    printf("\n-- in-order (alphabetically sorted):\n");
    inorder(root);

    printf("\n-- search \"banana\": %s\n", avl_search(root, "banana") ? "found" : "not found");
    printf("-- search \"papaya\": %s\n",  avl_search(root, "papaya") ? "found" : "not found");

    printf("\n-- delete \"apple\"\n");
    root = avl_delete(root, "apple");
    avl_print(root);

    avl_free(root);
    root = NULL;

    /* ── string -> int dictionary demo ──────────────────── */
    printf("\n=== AVL Tree — string->int dictionary ===\n\n");

    root = avl_insert(root, "one",   (void *)(intptr_t)1);
    root = avl_insert(root, "two",   (void *)(intptr_t)2);
    root = avl_insert(root, "three", (void *)(intptr_t)3);
    root = avl_insert(root, "four",  (void *)(intptr_t)4);
    root = avl_insert(root, "five",  (void *)(intptr_t)5);

    printf("-- in-order (key -> value):\n");
    inorder(root);

    /* update a value */
    printf("\n-- update \"two\" -> 22\n");
    root = avl_insert(root, "two", (void *)(intptr_t)22);
    Node *hit = avl_search(root, "two");
    printf("   search \"two\" -> %d\n", (int)(intptr_t)hit->value);

    avl_free(root);
    return 0;
}
