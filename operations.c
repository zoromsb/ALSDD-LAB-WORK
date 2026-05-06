#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "operations.h"

void to_lowercase(char *s) {
    for (int i = 0; s[i]; i++)
        s[i] = tolower(s[i]);
}
static void preintersection(Node *nodeA, Node *treeB, Node **result) {
    if (nodeA == NULL) return;
    preintersection(nodeA->left, treeB, result);
    if (avl_search(treeB, nodeA->key) != NULL)
        *result = avl_insert(*result, nodeA->key, NULL);
    preintersection(nodeA->right, treeB, result);
}

Node *para_intersection(Node *root1, Node *root2) {
    if (root1 == NULL || root2 == NULL) return NULL;
    Node *result = NULL;
    preintersection(root1, root2, &result);
    return result;
}
