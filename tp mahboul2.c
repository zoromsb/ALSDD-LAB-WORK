#include "fileam.c"
#include "dynarray.c"
#include "avltree.c"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

int main(void) {
    DynArray texts = da_new(10);
    int more = 1;

    while (more == 1) {
        char path[1024];
        printf("Enter the path to the text file: ");
        if (fgets(path, sizeof(path), stdin)) {
            path[strcspn(path, "\n")] = 0;
            if (path[0] == '"') {
                size_t len = strlen(path);
                memmove(path, path + 1, len);
                path[len - 2] = 0;
            }
            if (strlen(path) > 0)
                da_push(&texts, strdup(path));
        }
        printf("Load another text file? [1/0]: ");
        scanf("%d", &more);
        while (getchar() != '\n');
    }
    printf("=== Loaded %zu text file(s) ===\n\n", texts.size);

    DynArray paragraphs = da_new(10);
    size_t *paragraph_counts = calloc(texts.size, sizeof(size_t));
    size_t total_paragraphs = 0;

    for (size_t i = 0; i < texts.size; i++) {
        size_t before = paragraphs.size;
        char *path = da_get(&texts, i);
        read_para(path, &paragraphs);
        paragraph_counts[i] = paragraphs.size - before;
        total_paragraphs += paragraph_counts[i];
    }
    printf("=== Found %zu paragraphs across %zu files ===\n\n", total_paragraphs, texts.size);

    DynArray sentences = da_new(10);
    for (size_t i = 0; i < paragraphs.size; i++) {
        char *para = da_get(&paragraphs, i);
        da_push(&sentences, store_sentences(para));
    }

    size_t paragraph_index = 0;
    for (size_t i = 0; i < texts.size; i++) {
        char *file_path = da_get(&texts, i);
        printf("=== File %zu: %s ===\n", i + 1, file_path);
        for (size_t j = 0; j < paragraph_counts[i]; j++, paragraph_index++) {
            printf("--- Paragraph %zu.%zu ---\n", i + 1, j + 1);
            Node *tree = da_get(&sentences, paragraph_index);
            display_avlchar(tree);
            avl_print(tree);
            printf("\n");
        }
    }

    for (size_t i = 0; i < sentences.size; i++) {
        Node *tree = da_get(&sentences, i);
        avl_free(tree);
    }
    da_free(&sentences);

    for (size_t i = 0; i < paragraphs.size; i++)
        free(da_get(&paragraphs, i));
    da_free(&paragraphs);
    free(paragraph_counts);

    for (size_t i = 0; i < texts.size; i++)
        free(da_get(&texts, i));
    da_free(&texts);

    return 0;
}