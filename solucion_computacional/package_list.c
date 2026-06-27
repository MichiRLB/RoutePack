// package_list.c
#include "package_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// get_status_string
// Descripción: Convierte el valor del enum PackageStatus a texto legible.
// Entradas: status (estado del paquete).
// Salidas: Retorna una cadena constante con el nombre del estado.
// Restricciones: Si el valor no es válido retorna "Unknown".
const char* get_status_string(PackageStatus status) {
    switch (status) {
        case REGISTERED: return "Registered";
        case PENDING:    return "Pending Delivery";
        case IN_ROUTE:   return "In Route";
        case DELIVERED:  return "Delivered";
        case RETURNED:   return "Returned";
        default:         return "Unknown";
    }
}

// register_package
// Descripción: Registra un paquete al inicio de la lista enlazada principal.
// Entradas: head (puntero a la cabeza de la lista), new_node (paquete a registrar), bst_root (raíz del BST de clientes para validar el cliente asociado).
// Salidas: Retorna 1 si se registró, 0 si falló.
// Restricciones: El cliente debe existir en el BST y el código no debe estar duplicado.
int register_package(PackageNode** head, PackageNode* new_node, ClientNode* bst_root) {
    // Validar que el cliente exista en el BST
    if (search_client(bst_root, new_node->client_id) == NULL) {
        printf("\n[ERROR] Client with ID '%s' does not exist. Package rejected.\n", new_node->client_id);
        return 0; // Falla
    }

    // Validar códigos duplicados en la lista de paquetes
    if (search_package(*head, new_node->code) != NULL) {
        printf("\n[ERROR] Package code '%s' already exists. Package rejected.\n", new_node->code);
        return 0; // Falla
    }

    // Si pasa las validaciones, se inserta al principio de la lista enlazada
    new_node->next = *head;
    *head = new_node;
    return 1; // Éxito
}

// search_package
// Descripción: Busca un paquete por su código recorriendo la lista enlazada.
// Entradas: head (cabeza de la lista), code (código a buscar).
// Salidas: Retorna el puntero al paquete encontrado, o NULL si no existe.
// Restricciones: code no debe ser NULL.
PackageNode* search_package(PackageNode* head, const char* code) {
    PackageNode* current = head;
    while (current != NULL) {
        if (strcmp(current->code, code) == 0) {
            return current; // Encontrado
        }
        current = current->next;
    }
    return NULL; // No encontrado
}

// print_all_packages
// Descripción: Muestra todos los paquetes registrados en la lista.
// Entradas: head (cabeza de la lista).
// Salidas: Imprime los paquetes en consola, no retorna valor.
// Restricciones: Si la lista esta vacía muestra un mensaje informativo.
void print_all_packages(PackageNode* head) {
    if (head == NULL) {
        printf("\nNo packages registered in the system.\n");
        return;
    }
    PackageNode* current = head;
    printf("\n--- GLOBAL PACKAGE REPOSITORY ---");
    while (current != NULL) {
        printf("\nCode: %s | Client ID: %s | Recipient: %s | Dest: %s | Weight: %.2fkg | Priority: %d | Status: %s",
               current->code, current->client_id, current->recipient_name,
               current->destination, current->weight, current->priority, get_status_string(current->status));
        current = current->next;
    }
    printf("\n---------------------------------\n");
}

// modify_package_status
// Descripción: Cambia el estado de un paquete identificado por su código.
// Entradas: head (cabeza de la lista), code (código del paquete), new_status (nuevo estado).
// Salidas: Retorna 1 si se modificó, 0 si el paquete no existe.
// Restricciones: new_status debe ser un valor válido del enum PackageStatus.
int modify_package_status(PackageNode* head, const char* code, PackageStatus new_status) {
    PackageNode* pkg = search_package(head, code);
    if (pkg != NULL) {
        pkg->status = new_status;
        return 1;
    }
    return 0;
}

// delete_package
// Descripción: Elimina de la lista el paquete con el código indicado y libera su memoria.
// Entradas: head (puntero a la cabeza de la lista), code (código del paquete a eliminar).
// Salidas: Retorna 1 si se eliminó, 0 si no se encontró.
// Restricciones: El nodo del AVL asociado debe eliminarse antes para evitar punteros colgantes.
int delete_package(PackageNode** head, const char* code) {
    PackageNode* current = *head;
    PackageNode* previous = NULL;

    while (current != NULL && strcmp(current->code, code) != 0) {
        previous = current;
        current = current->next;
    }

    if (current == NULL) return 0; // No se encontró

    // Si el nodo a eliminar es la cabeza (el primero)
    if (previous == NULL) {
        *head = current->next;
    } else {
        previous->next = current->next;
    }

    free(current); // Liberar memoria del nodo eliminado
    return 1;
}

// count_packages_by_status
// Descripción: Cuenta los paquetes por cada estado y el total, para los reportes.
// Entradas: head (cabeza de la lista).
// Salidas: Imprime el total y el conteo por estado en consola, no retorna valor.
// Restricciones: Ninguna.
void count_packages_by_status(PackageNode* head) {
    int counts[5] = {0, 0, 0, 0, 0}; // Índices corresponden al enum PackageStatus
    int total = 0;
    PackageNode* current = head;

    while (current != NULL) {
        counts[current->status]++;
        total++;
        current = current->next;
    }

    printf("\n=== PACKAGE STATUS REPORT ===");
    printf("\nTotal Packages: %d", total);
    printf("\nRegistered: %d", counts[REGISTERED]);
    printf("\nPending:    %d", counts[PENDING]);
    printf("\nIn Route:   %d", counts[IN_ROUTE]);
    printf("\nDelivered:  %d", counts[DELIVERED]);
    printf("\nReturned:   %d\n", counts[RETURNED]);
}

// free_package_list
// Descripción: Libera toda la memoria de la lista de paquetes.
// Entradas: head (cabeza de la lista a liberar).
// Salidas: No retorna valor, deja los nodos liberados.
// Restricciones: Debe llamarse al cerrar el programa, después de liberar AVL/cola/pila.
void free_package_list(PackageNode* head) {
    PackageNode* current = head;
    while (current != NULL) {
        PackageNode* next = current->next;
        free(current);
        current = next;
    }
}