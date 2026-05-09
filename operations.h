#ifndef OPERATIONS_H   
#define OPERATIONS_H


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "avltree.h"   
#include "dynarray.h"   

void to_lowercase(char *s);
DynArray para_union(Node *root1, Node *root2);
DynArray para_intersection(Node *root1, Node *root2);
DynArray para_difference(Node *root1, Node *root2);

#endif