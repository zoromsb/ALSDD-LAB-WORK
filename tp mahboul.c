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
    // Step 0 — Read file path safely
    DynArray texts = da_new(10);
    char *input_path = NULL;

    // Use %ms to allocate memory for the string automatically (POSIX)
    if (scanf("%ms", &input_path) == 1) {
        da_push(&texts, input_path);
    }
    printf("=== Found %zu texts ===\n\n", texts.size);
    // Step 1 — Read paragraphs from file
    DynArray paragraphs = da_new(10);
    for (size_t i = 0; i < texts.size; i++) {
        char *path = (char *)da_get(&texts, i);
        read_para(path, &paragraphs);
    }
    printf("=== Found %zu paragraphs ===\n\n", paragraphs.size);

    // Step 2 — Paragraphs → AVL Trees
    // Your use of an AVL tree here ensures O(log n) search/insertion 
    // for the sentences within each paragraph.
    DynArray trees = da_new(10);
    for (size_t i = 0; i < paragraphs.size; i++) {
        char *para = (char *)da_get(&paragraphs, i);
        Node *root = store_sentences(para);
        da_push(&trees, root);
    }

    // Step 3 — Display
    for (size_t i = 0; i < trees.size; i++) {
        printf("=== Paragraph %zu ===\n", i + 1);
        Node *tree = (Node *)da_get(&trees, i);
        display_avlchar(tree); 
        avl_print(tree);
        printf("\n");
    }

    // Cleanup — Free in reverse order of allocation
    for (size_t i = 0; i < trees.size; i++) {
        avl_free((Node *)da_get(&trees, i));
    }
    da_free(&trees);

    for (size_t i = 0; i < paragraphs.size; i++) {
        free(da_get(&paragraphs, i));
    }
    da_free(&paragraphs);

    for (size_t i = 0; i < texts.size; i++) {
        free(da_get(&texts, i));
    }
    da_free(&texts);

    return 0;
}