#include "fileam.c"
#include "dynarray.c"
#include "avltree.c"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//==============================================================================
//==========================this is mabe by AI==================================
//======================just to test the functions==============================


int main(void) {
    // Step 1 — read paragraphs from file
    DynArray paragraphs = da_new(4);
    read_para("text.txt", &paragraphs);
    
    printf("=== Found %zu paragraphs ===\n\n", paragraphs.size);
    
    // Step 2 — convert each paragraph to an AVL tree of sentences
    DynArray trees = da_new(4);
    for (size_t i = 0; i < paragraphs.size; i++) {
        char *para = (char *)da_get(&paragraphs, i);
        Node *tree = store_sentences(para);
        da_push(&trees, tree);
    }
    
    // Step 3 — display each tree
    for (size_t i = 0; i < trees.size; i++) {
        printf("=== Paragraph %zu ===\n", i + 1);
        Node *tree = (Node *)da_get(&trees, i);
        display_avlchar(tree);
        printf("\n");
    }
    
    return 0;
}