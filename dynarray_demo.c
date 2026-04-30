/*
 * dynarray_demo.c — demo for the dynamic array abstract machine.
 * Compile: gcc -Wall -o dynarray_demo dynarray_demo.c dynarray.c
 */

#include "dynarray.c"
#include <stdio.h>

int main(void) {
    printf("=== Dynamic Array — string demo ===\n\n");

    DynArray words = da_new(4);

    /* push strings */
    da_push(&words, "apple");
    da_push(&words, "banana");
    da_push(&words, "cherry");
    da_push(&words, "date");
    da_push(&words, "elderberry");   /* triggers realloc */
    da_print_str(&words);

    /* pop */
    printf("\npopped: \"%s\"\n", (char *)da_pop(&words));
    da_print_str(&words);

    /* insert at index 1 */
    printf("\ninsert \"blueberry\" at index 1\n");
    da_insert(&words, 1, "blueberry");
    da_print_str(&words);

    /* overwrite index 0 */
    printf("\nset index 0 = \"apricot\"\n");
    da_set(&words, 0, "apricot");
    da_print_str(&words);

    /* delete index 2 */
    printf("\ndelete index 2\n");
    da_delete(&words, 2);
    da_print_str(&words);

    /* random access */
    printf("\nwords[1] = \"%s\"\n", (char *)da_get(&words, 1));

    da_free(&words);

    /* ── integer demo ─────────────────────────────────────── */
    printf("\n=== Dynamic Array — integer demo ===\n\n");

    DynArray nums = da_new(2);
    for (int i = 1; i <= 6; i++)
        da_push(&nums, (void *)(intptr_t)(i * 10));
    da_print_int(&nums);

    printf("popped: %d\n", (int)(intptr_t)da_pop(&nums));
    da_print_int(&nums);

    da_free(&nums);
    return 0;
}
