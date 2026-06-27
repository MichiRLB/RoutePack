// package_avl.h
#ifndef PACKAGE_AVL_H
#define PACKAGE_AVL_H

#include "package_list.h"

// Cada nodo del AVL apunta al paquete real almacenado en la lista enlazada.
typedef struct AVLNode {
    PackageNode* package;
    int height;
    struct AVLNode* left;
    struct AVLNode* right;
} AVLNode;

int avl_height(AVLNode* node);
int avl_balance_factor(AVLNode* node);
AVLNode* avl_rotate_right(AVLNode* y);
AVLNode* avl_rotate_left(AVLNode* x);
AVLNode* avl_insert(AVLNode* node, PackageNode* pkg, int* success);
AVLNode* avl_delete(AVLNode* node, const char* code);
PackageNode* avl_search(AVLNode* node, const char* code);
void avl_print_inorder(AVLNode* node);
void avl_print_structure(AVLNode* node, int depth);
void avl_free(AVLNode* node);

#endif // PACKAGE_AVL_H
