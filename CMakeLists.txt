cmake_minimum_required(VERSION 3.20)
project(AbstractMachines C)

set(CMAKE_C_STANDARD 11)

# ── dynarray ────────────────────────────────────────────────────────────
add_executable(dynarray_demo
    dynarray.c
    dynarray_demo.c
)

# ── avltree ─────────────────────────────────────────────────────────────
add_executable(avltree_demo
    avltree.c
    avltree_demo.c
)

# ── fileam ──────────────────────────────────────────────────────────────
add_executable(fileam_demo
    fileam.c
    fileam_demo.c
)
