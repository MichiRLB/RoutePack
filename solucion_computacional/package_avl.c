// package_avl.c
#include "package_avl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// max_int
// Descripción: Retorna el mayor de dos enteros.
// Entradas: a, b (enteros a comparar).
// Salidas: Retorna el mayor.
// Restricciones: Ninguna.
static int max_int(int a, int b) {
    return (a > b) ? a : b;
}

// avl_height
// Descripción: Calcula la altura de un nodo.
// Entradas: node (nodo del AVL).
// Salidas: Retorna la altura almacenada, o 0 si el nodo es NULL.
// Restricciones: Ninguna.
int avl_height(AVLNode* node) {
    if (node == NULL) return 0;
    return node->height;
}

// avl_balance_factor
// Descripción: Calcula el factor de balance del nodo: altura(izq) - altura(der).
// Entradas: node (nodo del AVL).
// Salidas: Retorna el factor de balance (0 si el nodo es NULL).
// Restricciones: Ninguna.
int avl_balance_factor(AVLNode* node) {
    if (node == NULL) return 0;
    return avl_height(node->left) - avl_height(node->right);
}

// avl_create_node
// Descripción: Crea un nodo AVL que apunta a un paquete, con altura inicial 1.
// Entradas: pkg (puntero al paquete de la lista).
// Salidas: Retorna el nuevo nodo, o NULL si falla la reserva de memoria.
// Restricciones: pkg no debe ser NULL.
static AVLNode* avl_create_node(PackageNode* pkg) {
    AVLNode* node = (AVLNode*)malloc(sizeof(AVLNode));
    if (!node) return NULL;
    node->package = pkg;
    node->left = NULL;
    node->right = NULL;
    node->height = 1;
    return node;
}

// avl_rotate_right
// Descripción: Aplica una rotación simple a la derecha para rebalancear.
// Entradas: y (raíz del subárbol desbalanceado).
// Salidas: Retorna la nueva raíz del subárbol tras la rotación.
// Restricciones: y y su hijo izquierdo no deben ser NULL.
AVLNode* avl_rotate_right(AVLNode* y) {
    AVLNode* x = y->left;
    AVLNode* t2 = x->right;

    x->right = y;
    y->left = t2;

    y->height = max_int(avl_height(y->left), avl_height(y->right)) + 1;
    x->height = max_int(avl_height(x->left), avl_height(x->right)) + 1;

    return x; // Nueva raíz del subárbol
}

// avl_rotate_left
// Descripción: Aplica una rotación simple a la izquierda para rebalancear.
// Entradas: x (raíz del subárbol desbalanceado).
// Salidas: Retorna la nueva raíz del subárbol tras la rotación.
// Restricciones: x y su hijo derecho no deben ser NULL.
AVLNode* avl_rotate_left(AVLNode* x) {
    AVLNode* y = x->right;
    AVLNode* t2 = y->left;

    y->left = x;
    x->right = t2;

    x->height = max_int(avl_height(x->left), avl_height(x->right)) + 1;
    y->height = max_int(avl_height(y->left), avl_height(y->right)) + 1;

    return y; // Nueva raíz del subárbol
}

// avl_insert
// Descripción: Inserta un paquete por código y rebalancea con rotaciones simples o dobles.
// Entradas: node (raíz del subárbol), pkg (paquete a insertar), success (salida por puntero: 1 si insertó, 0 si el código estaba duplicado).
// Salidas: Retorna la nueva raíz del subárbol balanceado.
// Restricciones: No se permiten códigos duplicados, el árbol queda balanceado tras insertar.
AVLNode* avl_insert(AVLNode* node, PackageNode* pkg, int* success) {
    if (node == NULL) {
        *success = 1;
        return avl_create_node(pkg);
    }

    int cmp = strcmp(pkg->code, node->package->code);
    if (cmp < 0) {
        node->left = avl_insert(node->left, pkg, success);
    } else if (cmp > 0) {
        node->right = avl_insert(node->right, pkg, success);
    } else {
        *success = 0; // Código duplicado
        return node;
    }

    // Actualizar altura del nodo actual
    node->height = 1 + max_int(avl_height(node->left), avl_height(node->right));

    // Obtener el factor de balance para decidir si se rota
    int balance = avl_balance_factor(node);

    // Caso Izquierda-Izquierda
    if (balance > 1 && strcmp(pkg->code, node->left->package->code) < 0)
        return avl_rotate_right(node);

    // Caso Derecha-Derecha
    if (balance < -1 && strcmp(pkg->code, node->right->package->code) > 0)
        return avl_rotate_left(node);

    // Caso Izquierda-Derecha
    if (balance > 1 && strcmp(pkg->code, node->left->package->code) > 0) {
        node->left = avl_rotate_left(node->left);
        return avl_rotate_right(node);
    }

    // Caso Derecha-Izquierda
    if (balance < -1 && strcmp(pkg->code, node->right->package->code) < 0) {
        node->right = avl_rotate_right(node->right);
        return avl_rotate_left(node);
    }

    return node;
}

// avl_min_node
// Descripción: Obtiene el nodo con el código menor de un subárbol (más a la izquierda).
// Entradas: node (raíz del subárbol).
// Salidas: Retorna el nodo mínimo.
// Restricciones: node no debe ser NULL.
static AVLNode* avl_min_node(AVLNode* node) {
    AVLNode* cur = node;
    while (cur->left != NULL) cur = cur->left;
    return cur;
}

// avl_delete
// Descripción: Elimina por código un nodo del AVL y rebalancea (mantiene sincronizado con la lista).
// Entradas: node (raíz del subárbol), code (código del paquete a eliminar).
// Salidas: Retorna la nueva raíz del subárbol balanceado.
// Restricciones: Solo libera el nodo del AVL, no el paquete (pertenece a la lista).
AVLNode* avl_delete(AVLNode* node, const char* code) {
    if (node == NULL) return NULL;

    int cmp = strcmp(code, node->package->code);
    if (cmp < 0) {
        node->left = avl_delete(node->left, code);
    } else if (cmp > 0) {
        node->right = avl_delete(node->right, code);
    } else {
        // Nodo con uno o ningún hijo
        if (node->left == NULL || node->right == NULL) {
            AVLNode* temp = node->left ? node->left : node->right;
            if (temp == NULL) {
                // Sin hijos
                temp = node;
                node = NULL;
            } else {
                // Un hijo: copiar contenido del hijo
                *node = *temp;
            }
            free(temp);
        } else {
            // Dos hijos: usar el sucesor inorden (menor del subárbol derecho)
            AVLNode* succ = avl_min_node(node->right);
            node->package = succ->package;
            node->right = avl_delete(node->right, succ->package->code);
        }
    }

    if (node == NULL) return NULL;

    node->height = 1 + max_int(avl_height(node->left), avl_height(node->right));
    int balance = avl_balance_factor(node);

    // Rebalanceo tras la eliminación
    if (balance > 1 && avl_balance_factor(node->left) >= 0)
        return avl_rotate_right(node);
    if (balance > 1 && avl_balance_factor(node->left) < 0) {
        node->left = avl_rotate_left(node->left);
        return avl_rotate_right(node);
    }
    if (balance < -1 && avl_balance_factor(node->right) <= 0)
        return avl_rotate_left(node);
    if (balance < -1 && avl_balance_factor(node->right) > 0) {
        node->right = avl_rotate_right(node->right);
        return avl_rotate_left(node);
    }

    return node;
}

// avl_search
// Descripción: Busca un paquete por código de forma eficiente (O(log n)).
// Entradas: node (raíz del subárbol), code (código a buscar).
// Salidas: Retorna el paquete encontrado, o NULL si no existe.
// Restricciones: code no debe ser NULL.
PackageNode* avl_search(AVLNode* node, const char* code) {
    if (node == NULL) return NULL;
    int cmp = strcmp(code, node->package->code);
    if (cmp == 0) return node->package;
    if (cmp < 0) return avl_search(node->left, code);
    return avl_search(node->right, code);
}

// avl_print_inorder
// Descripción: Recorrido inorden, muestra los paquetes ordenados por código.
// Entradas: node (raíz del subárbol).
// Salidas: Imprime los paquetes en consola, no retorna valor.
// Restricciones: Ninguna.
void avl_print_inorder(AVLNode* node) {
    if (node == NULL) return;
    avl_print_inorder(node->left);
    PackageNode* p = node->package;
    printf("Code: %s | Client: %s | Recipient: %s | Status: %s\n",
           p->code, p->client_id, p->recipient_name, get_status_string(p->status));
    avl_print_inorder(node->right);
}

// avl_print_structure
// Descripción: Muestra la estructura del árbol (rotado 90 grados) con altura y factor de balance.
// Entradas: node (raíz del subárbol), depth (profundidad actual, 0 al inicio).
// Salidas: Imprime el árbol en consola, no retorna valor.
// Restricciones: Ninguna.
void avl_print_structure(AVLNode* node, int depth) {
    if (node == NULL) return;
    avl_print_structure(node->right, depth + 1);
    for (int i = 0; i < depth; i++) printf("        ");
    printf("[%s] (h=%d, bf=%d)\n", node->package->code, node->height, avl_balance_factor(node));
    avl_print_structure(node->left, depth + 1);
}

// avl_free
// Descripción: Libera todos los nodos del AVL (no los paquetes).
// Entradas: node (raíz del árbol a liberar).
// Salidas: No retorna valor, deja los nodos liberados.
// Restricciones: Los paquetes los libera la lista principal, no esta función.
void avl_free(AVLNode* node) {
    if (node == NULL) return;
    avl_free(node->left);
    avl_free(node->right);
    free(node); // Solo el nodo del AVL, el paquete lo libera la lista
}
