#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "operations.h"
#include "avltree.h"
#include "dynarray.h"

void to_lowercase(char *s) {
    for (int i = 0; s[i]; i++)
        s[i] = tolower(s[i]);
}
static void AVL_to_DA(Node *node, DynArray *result) {       //store the keys af AVL in a DA inorder
    if (node == NULL) return;
    AVL_to_DA(node->left, result);
    da_push(result, (void *)node->key);
    AVL_to_DA(node->right, result);
}

static void store_filter(Node *node, Node *filter, DynArray *result, int keep_if_found) {       //store the node in a DA if it is found in the filter tree and keep_if_found is 1, or if it is not found and keep_if_found is 0
    if (node == NULL) return;
    store_filter(node->left, filter, result, keep_if_found);
    int found = avl_search(filter, node->key) != NULL;
    if (found == keep_if_found)
        da_push(result, (void *)node->key);
    store_filter(node->right, filter, result, keep_if_found);
}
DynArray para_union(Node *root1, Node *root2) {
    DynArray result = da_new(1);
    AVL_to_DA(root1, &result);
    store_filter(root2, root1, &result, 0);
    return result;
}

DynArray para_intersection(Node *root1, Node *root2) {
    DynArray result = da_new(1);
    store_filter(root1, root2, &result, 1);
    return result;
}

DynArray para_difference(Node *root1, Node *root2) {
    DynArray result = da_new(1);
    store_filter(root1, root2, &result, 0);
    return result;
}
