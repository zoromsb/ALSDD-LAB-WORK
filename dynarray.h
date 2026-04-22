/*
 * dynarray.h — Dynamic Array Abstract Machine
 *
 * Element type: void*  — store any pointer: char*, int*, struct*, etc.
 *
 * State    : (void **data, size_t size, size_t capacity)
 * Invariant: size <= capacity
 *            capacity doubles when full (amortized O(1) push)
 *
 * Complexity
 *   da_get / da_set   O(1)
 *   da_push / da_pop  O(1) amortized
 *   da_insert         O(n)
 *   da_delete         O(n)
 *
 * Usage — strings (store char* pointers)
 *   DynArray a = da_new(4);
 *   da_push(&a, "hello");
 *   da_push(&a, "world");
 *   printf("%s\n", (char *)da_get(&a, 0));
 *   da_free(&a);          // frees the array buffer; caller owns the strings
 *
 * Usage — integers (cast to void*)
 *   da_push(&a, (void *)(intptr_t)42);
 *   int v = (int)(intptr_t)da_get(&a, 0);
 */

#ifndef DYNARRAY_H
#define DYNARRAY_H

#include <stddef.h>    /* size_t    */
#include <stdint.h>    /* intptr_t  */

/* ── type ───────────────────────────────────────────────────────────── */

typedef struct {
    void **data;      /* heap-allocated pointer buffer                */
    size_t size;      /* number of live elements                      */
    size_t capacity;  /* allocated slots (always >= size)             */
} DynArray;

/* ── lifecycle ──────────────────────────────────────────────────────── */

/** da_new — allocate a new empty array. initial_cap rounded to 4 if 0. */
DynArray da_new(size_t initial_cap);

/** da_free — release the internal buffer. Does NOT free the elements. */
void     da_free(DynArray *a);

/* ── element access ─────────────────────────────────────────────────── */

/** da_get — return pointer at index i. Aborts on out-of-range. */
void *da_get(const DynArray *a, size_t i);

/** da_set — store pointer at index i. Aborts on out-of-range. */
void  da_set(DynArray *a, size_t i, void *value);

/* ── stack operations ───────────────────────────────────────────────── */

/** da_push — append pointer at the end. Doubles capacity if full. */
void  da_push(DynArray *a, void *value);

/** da_pop — remove and return the last pointer. Aborts on empty. */
void *da_pop(DynArray *a);

/* ── positional operations ──────────────────────────────────────────── */

/** da_insert — insert before index i, shifting right. O(n). */
void da_insert(DynArray *a, size_t i, void *value);

/** da_delete — remove at index i, shifting left. O(n). */
void da_delete(DynArray *a, size_t i);

/* ── diagnostics ────────────────────────────────────────────────────── */

/** da_print_str — print array elements as char* strings. */
void da_print_str(const DynArray *a);

/** da_print_int — print array elements as integers (cast from void*). */
void da_print_int(const DynArray *a);

#endif /* DYNARRAY_H */
