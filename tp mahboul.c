#include "fileam.c"
#include "dynarray.c"
#include "avltree.c"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

//rani na3del main 
int main(void) {
  // Step 0 — Read file path safely 
DynArray texts = da_new(10);
int more = 1;

while (more == 1) {
    char path[1024];
    printf("Enter the path to the text file: ");
    if (fgets(path, sizeof(path), stdin)) {
       
        path[strcspn(path, "\n")] = 0;

        
        char *clean_path = path;
        if (path[0] == '"') {
            clean_path = path + 1; 
            clean_path[strlen(clean_path) - 1] = 0; 
        }

        if (strlen(clean_path) > 0) {
            da_push(&texts, strdup(clean_path));
        }
    }
    printf("Load another text file? [1/0]: ");
    scanf("%d", &more);
    while (getchar() != '\n'); 
}
printf("=== Loaded %zu text file(s) ===\n\n", texts.size);

    // Step 1 — Read paragraphs and remember counts per file
    DynArray paragraphs = da_new(10);
    DynArray paragraph_counts = da_new(10);
    size_t total_paragraphs = 0;

    for (size_t i = 0; i < texts.size; i++) {
        char *path = (char *)da_get(&texts, i);

        size_t before = paragraphs.size;
        read_para(path, &paragraphs);
        size_t count = paragraphs.size - before;
        da_push(&paragraph_counts, (void *)(intptr_t)count);
        total_paragraphs = total_paragraphs + count;
    }
    printf("=== Found %zu paragraphs across %zu files ===\n\n", total_paragraphs, texts.size);

    // Step 2 — AVL Trees of paragraphs
    DynArray sentences = da_new(10);
    for (size_t i = 0; i < paragraphs.size; i++) {
        char *para = (char *)da_get(&paragraphs, i);
        Node *root = store_sentences(para);
        da_push(&sentences, root);
    }

    // Step 3 — Display by file using counts
    size_t paragraph_index = 0;
    for (size_t i = 0; i < texts.size; i++) {
        size_t count = (size_t)(intptr_t)da_get(&paragraph_counts, i);
        char *file_path = (char *)da_get(&texts, i);
        printf("=== File %zu: %s ===\n", i + 1, file_path);

        for (size_t j = 0; j < count; j++, paragraph_index++) {
            printf("--- Paragraph %zu.%zu ---\n", i + 1, j + 1);
            Node *tree = (Node *)da_get(&sentences, paragraph_index);
            display_avlchar(tree);
            avl_print(tree);
            printf("\n");
        }
    }

    // Step 4 — Cleanup
    for (size_t i = 0; i < sentences.size; i++) {
        avl_free((Node *)da_get(&sentences, i));
    }
    da_free(&sentences);

    for (size_t i = 0; i < paragraphs.size; i++) {
        free(da_get(&paragraphs, i));
    }
    da_free(&paragraphs);
    da_free(&paragraph_counts);

    for (size_t i = 0; i < texts.size; i++) {
        free(da_get(&texts, i));
    }
    da_free(&texts);

    return 0;
}