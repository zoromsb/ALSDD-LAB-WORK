#include "fileam.c"
#include "dynarray.c"
#include "avltree.c"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

int main(void) {
    DynArray text = da_new(10);
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
                da_push(&text, strdup(path));
        }
        printf("Load another text file? [1/0]: ");
        scanf("%d", &more);
        while (getchar() != '\n');
    }
    printf("=== Loaded %zu text file(s) ===\n\n", text.size);
    DynArray para = da_new(10);
    size_t *para_count = calloc(text.size, sizeof(size_t));
    size_t tot_para = 0;
    for (size_t i = 0; i < text.size; i++) {
        size_t before = para.size;
        char *path = da_get(&text, i);
        read_para(path, &para);
        para_count[i] = para.size - before;
        tot_para =tot_para + para_count[i];
    }
    printf("=== Found %zu paragraphs across %zu files ===\n\n", tot_para, text.size);
    DynArray sent = da_new(10);
    for (size_t i = 0; i < para.size; i++) {
        char *para_a = da_get(&para, i);
        da_push(&sent, store_sentences(para_a));
    }
    size_t para_indx = 0;
    for (size_t i = 0; i < text.size; i++) {
        char *f_path = da_get(&text, i);
        printf("=== File %zu: %s ===\n", i + 1, f_path);
        for (size_t j = 0; j < para_count[i]; j++, para_indx++) {
            printf("=== Paragraph %zu.%zu ===\n", i + 1, j + 1);
            Node *tree = da_get(&sent, para_indx);
            display_avlchar(tree);
            avl_print(tree);
            printf("\n");
        }
    }
    for (size_t i = 0; i < sent.size; i++) {
        Node *tree = da_get(&sent, i);
        avl_free(tree);
    }
    da_free(&sent);
    for (size_t i = 0; i < para.size; i++)
        free(da_get(&para, i));
    da_free(&para);
    free(para_count);
    for (size_t i = 0; i < text.size; i++)
        free(da_get(&text, i));
    da_free(&text);
    return 0;
}
