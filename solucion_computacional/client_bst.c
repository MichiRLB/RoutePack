// client_bst.c
#include "client_bst.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// insert_client
// Descripción: Inserta recursivamente un cliente en el BST usando el ID como llave.
// Entradas: root (raíz actual del subárbol), new_node (nodo de cliente ya reservado), success (salida por puntero: 1 si insertó, 0 si el ID estaba duplicado).
// Salidas: Retorna la nueva raíz del subárbol.
// Restricciones: new_node debe estar reservado en memoria, no se permiten IDs duplicados.
ClientNode* insert_client(ClientNode* root, ClientNode* new_node, int* success) {
    // Si llegamos a una posición vacía, aquí se coloca el nuevo nodo
    if (root == NULL) {
        *success = 1;
        return new_node;
    }

    // Comparamos el ID del cliente actual con el del nuevo cliente
    int comparison = strcmp(new_node->id, root->id);

    if (comparison < 0) {
        // El ID nuevo es menor, va a la izquierda
        root->left = insert_client(root->left, new_node, success);
    } else if (comparison > 0) {
        // El ID nuevo es mayor, va a la derecha
        root->right = insert_client(root->right, new_node, success);
    } else {
        // El ID es idéntico, es un duplicado
        *success = 0;
    }

    return root;
}

// search_client
// Descripción: Busca un cliente por su ID recorriendo el BST.
// Entradas: root (raíz del árbol), id (identificación a buscar).
// Salidas: Retorna el puntero al nodo encontrado, o NULL si no existe.
// Restricciones: id no debe ser NULL.
ClientNode* search_client(ClientNode* root, const char* id) {
    if (root == NULL || strcmp(root->id, id) == 0) {
        return root;
    }

    if (strcmp(id, root->id) < 0) {
        return search_client(root->left, id);
    } else {
        return search_client(root->right, id);
    }
}

// print_inorder
// Descripción: Recorrido inorden (Izquierda -> Raíz -> Derecha), muestra los clientes ordenados por ID.
// Entradas: root (raíz del árbol).
// Salidas: Imprime los clientes en consola, no retorna valor.
// Restricciones: Ninguna.
void print_inorder(ClientNode* root) {
    if (root != NULL) {
        print_inorder(root->left);
        printf("\n[ID: %s] Name: %s | Phone: %s | Email: %s | Zone: %s\n",
               root->id, root->name, root->phone, root->email, root->address);
        print_inorder(root->right);
    }
}

// print_preorder
// Descripción: Recorrido preorden (Raíz -> Izquierda -> Derecha).
// Entradas: root (raíz del árbol).
// Salidas: Imprime los clientes en consola, no retorna valor.
// Restricciones: Ninguna.
void print_preorder(ClientNode* root) {
    if (root != NULL) {
        printf("\n[ID: %s] Name: %s\n", root->id, root->name);
        print_preorder(root->left);
        print_preorder(root->right);
    }
}

// print_postorder
// Descripción: Recorrido postorden (Izquierda -> Derecha -> Raíz).
// Entradas: root (raíz del árbol).
// Salidas: Imprime los clientes en consola, no retorna valor.
// Restricciones: Ninguna.
void print_postorder(ClientNode* root) {
    if (root != NULL) {
        print_postorder(root->left);
        print_postorder(root->right);
        printf("\n[ID: %s] Name: %s\n", root->id, root->name);
    }
}

// free_bst
// Descripción: Libera recursivamente toda la memoria del árbol de clientes.
// Entradas: root (raíz del árbol a liberar).
// Salidas: No retorna valor, deja los nodos liberados.
// Restricciones: Debe llamarse al cerrar el programa para evitar fugas de memoria.
void free_bst(ClientNode* root) {
    if (root != NULL) {
        free_bst(root->left);
        free_bst(root->right);
        free(root); // Libera el nodo actual
    }
}