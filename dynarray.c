/*
 * dynarray.c — Dynamic Array Abstract Machine  (implementation)
 * See dynarray.h for full API documentation.
 */

#include "dynarray.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ── lifecycle ──────────────────────────────────────────────────────── */

DynArray da_new(size_t initial_cap) {
    if (initial_cap == 0) initial_cap = 4;
    DynArray a;
    a.data     = malloc(initial_cap * sizeof(void *));
    a.size     = 0;
    a.capacity = initial_cap;
    if (!a.data) { perror("da_new: malloc"); exit(1); }
    return a;
}

void da_free(DynArray *a) {
    free(a->data);
    a->data     = NULL;
    a->size     = 0;
    a->capacity = 0;
}

/* ── internal ───────────────────────────────────────────────────────── */

static void da_grow(DynArray *a) {
    size_t  new_cap = a->capacity * 2;
    void  **tmp     = realloc(a->data, new_cap * sizeof(void *));
    if (!tmp) { perror("da_grow: realloc"); exit(1); }
    a->data     = tmp;
    a->capacity = new_cap;
    printf("  [realloc] capacity doubled -> %zu\n", new_cap);
}

/* ── element access ─────────────────────────────────────────────────── */

void *da_get(const DynArray *a, size_t i) {
    if (i >= a->size) { fprintf(stderr, "da_get: index %zu out of range\n", i); exit(1); }
    return a->data[i];
}

void da_set(DynArray *a, size_t i, void *value) {
    if (i >= a->size) { fprintf(stderr, "da_set: index %zu out of range\n", i); exit(1); }
    a->data[i] = value;
}

/* ── stack operations ───────────────────────────────────────────────── */

void da_push(DynArray *a, void *value) {
    if (a->size == a->capacity) da_grow(a);
    a->data[a->size++] = value;
}

void *da_pop(DynArray *a) {
    if (a->size == 0) { fprintf(stderr, "da_pop: underflow\n"); exit(1); }
    return a->data[--a->size];
}

/* ── positional operations ──────────────────────────────────────────── */

void da_insert(DynArray *a, size_t i, void *value) {
    if (i > a->size) { fprintf(stderr, "da_insert: index out of range\n"); exit(1); }
    if (a->size == a->capacity) da_grow(a);
    memmove(&a->data[i + 1], &a->data[i], (a->size - i) * sizeof(void *));
    a->data[i] = value;
    a->size++;
}

void da_delete(DynArray *a, size_t i) {
    if (i >= a->size) { fprintf(stderr, "da_delete: index out of range\n"); exit(1); }
    memmove(&a->data[i], &a->data[i + 1], (a->size - i - 1) * sizeof(void *));
    a->size--;
}

/* ── diagnostics ────────────────────────────────────────────────────── */

void da_print_str(const DynArray *a) {
    printf("DynArray[%zu] = [", a->size);
    for (size_t i = 0; i < a->size; i++)
        printf("%s\"%s\"", i ? ", " : "", (char *)a->data[i]);
    printf("]  (cap=%zu)\n", a->capacity);
}

void da_print_int(const DynArray *a) {
    printf("DynArray[%zu] = [", a->size);
    for (size_t i = 0; i < a->size; i++)
        printf("%s%d", i ? ", " : "", (int)(intptr_t)a->data[i]);
    printf("]  (cap=%zu)\n", a->capacity);
}
