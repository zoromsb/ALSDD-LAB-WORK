
#include "fileam.h"
#include "dynarray.h"
#include "avltree.h"
#include <stdio.h>

static void inorder(const Node *n) {
    if (!n) return;
    inorder(n->left);
    printf("%d ", n->key);
    inorder(n->right);
}

int main(void) {
    da_new()
    return 0;
}
