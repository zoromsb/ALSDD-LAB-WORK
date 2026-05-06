#include "fileam.c"
#include "dynarray.c"
#include "avltree.c"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* print the DynArray result of any operation */
void print_result(DynArray *result) {
    if (result->size == 0) {
        printf("  (empty)\n");
        return;
    }
    for (size_t i = 0; i < result->size; i++)
        printf("  [%zu] %s\n", i + 1, (char *)da_get(result, i));
}

int main(void) {

    /* load file paths from user into a DynArray */
    DynArray text = da_new(1);
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

    /* read paragraphs from each file, count how many belong to each file */
    DynArray para = da_new(1);
    size_t *para_count = calloc(text.size, sizeof(size_t));
    size_t tot_para = 0;
    for (size_t i = 0; i < text.size; i++) {
        size_t before = para.size;
        char *path = da_get(&text, i);
        read_para(path, &para);
        para_count[i] = para.size - before;
        tot_para += para_count[i];
    }
    printf("=== Found %zu paragraphs across %zu files ===\n\n", tot_para, text.size);

    /* build one AVL tree of sentences per paragraph, store all trees in sent */
    DynArray sent = da_new(1);
    for (size_t i = 0; i < para.size; i++)
        da_push(&sent, store_sentences((char *)da_get(&para, i)));

    /* display each paragraph and its sentence tree */
    size_t para_indx = 0;
    for (size_t i = 0; i < text.size; i++) {
        printf("=== File %zu: %s ===\n", i + 1, (char *)da_get(&text, i));
        for (size_t j = 0; j < para_count[i]; j++, para_indx++) {
            printf("  --- Paragraph %zu.%zu ---\n", i + 1, j + 1);
            Node *tree = da_get(&sent, para_indx);
            display_avlchar(tree);
            printf("\n");
        }
    }

    /* ask user which set operation to apply */
    int op;
    printf("=== Choose operation ===\n");
    printf("  1) Union\n");
    printf("  2) Intersection\n");
    printf("  3) Difference\n");
    printf("Choose: ");
    scanf("%d", &op);
    while (getchar() != '\n');

    DynArray result;
    Node *tree1 = NULL;
    Node *tree2 = NULL;
    int paragraph_level = 0; /* 1 if we built tree1/tree2 with store_paragraphs so we free them after */

    if (text.size == 1) {
        /* single file: work at sentence level between 2 paragraphs */
        size_t pr1, pr2;
        printf("Enter first paragraph number: ");
        scanf("%zu", &pr1);
        printf("Enter second paragraph number: ");
        scanf("%zu", &pr2);
        while (getchar() != '\n');

        if (pr1 < 1 || pr1 > para_count[0] || pr2 < 1 || pr2 > para_count[0]) {
            printf("Invalid paragraph numbers.\n");
            goto cleanup;
        }

        /* get the AVL trees of the chosen paragraphs directly from sent */
        tree1 = da_get(&sent, pr1 - 1);
        tree2 = da_get(&sent, pr2 - 1);

        printf("\n=== Sentence-level: paragraph %zu vs paragraph %zu ===\n", pr1, pr2);

    } else {
        /* multiple files: ask which two files and which level */
        size_t f1, f2;
        printf("Enter first file number: ");
        scanf("%zu", &f1);
        printf("Enter second file number: ");
        scanf("%zu", &f2);
        while (getchar() != '\n');

        if (f1 < 1 || f1 > text.size || f2 < 1 || f2 > text.size) {
            printf("Invalid file numbers.\n");
            goto cleanup;
        }

        int level;
        printf("Work at which level?\n  1) Sentence (pick one paragraph per file)\n  2) Paragraph (whole files)\nChoose: ");
        scanf("%d", &level);
        while (getchar() != '\n');

        /* compute where each file's paragraphs start in the para/sent arrays */
        size_t start1 = 0, start2 = 0;
        for (size_t i = 0; i < f1 - 1; i++) start1 += para_count[i];
        for (size_t i = 0; i < f2 - 1; i++) start2 += para_count[i];

        if (level == 1) {
            /* sentence level: pick one paragraph from each file */
            size_t pr1, pr2;
            printf("Enter paragraph number from file %zu: ", f1);
            scanf("%zu", &pr1);
            printf("Enter paragraph number from file %zu: ", f2);
            scanf("%zu", &pr2);
            while (getchar() != '\n');

            if (pr1 < 1 || pr1 > para_count[f1-1] || pr2 < 1 || pr2 > para_count[f2-1]) {
                printf("Invalid paragraph numbers.\n");
                goto cleanup;
            }

            tree1 = da_get(&sent, start1 + pr1 - 1);
            tree2 = da_get(&sent, start2 + pr2 - 1);

            printf("\n=== Sentence-level: file %zu para %zu vs file %zu para %zu ===\n", f1, pr1, f2, pr2);

        } else {
            /* paragraph level: build one AVL per file containing all its paragraphs */
            tree1 = store_paragraphs(&para, start1, para_count[f1 - 1]);
            tree2 = store_paragraphs(&para, start2, para_count[f2 - 1]);
            paragraph_level = 1;

            printf("\n=== Paragraph-level: file %zu vs file %zu ===\n", f1, f2);
        }
    }

    /* apply the chosen operation and print the result */
    if      (op == 1) { result = para_union(tree1, tree2);        printf("--- Union ---\n"); }
    else if (op == 2) { result = para_intersection(tree1, tree2); printf("--- Intersection ---\n"); }
    else if (op == 3) { result = para_difference(tree1, tree2);   printf("--- Difference ---\n"); }
    else { printf("Invalid operation.\n"); goto cleanup; }

    print_result(&result);
    da_free(&result);

    /* free paragraph-level trees only — sent trees are freed in cleanup */
    if (paragraph_level) {
        avl_free(tree1);
        avl_free(tree2);
    }

cleanup:
    for (size_t i = 0; i < sent.size; i++)
        avl_free_keys((Node *)da_get(&sent, i)); /* keys were malloc'd in store_sentences so free them too */
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
