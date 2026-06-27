// package_list.h
#ifndef PACKAGE_LIST_H
#define PACKAGE_LIST_H

#include "client_bst.h" // Importante: para poder validar si el cliente existe

// Estados mínimos permitidos usando un enum
typedef enum { REGISTERED, PENDING, IN_ROUTE, DELIVERED, RETURNED } PackageStatus;

typedef struct PackageNode {
    char code[20];
    char client_id[20];
    char recipient_name[100];
    char destination[100];
    float weight;
    int priority;
    PackageStatus status;
    struct PackageNode* next; // Puntero al siguiente nodo de la lista enlazada
} PackageNode;

// Funciones requeridas
int register_package(PackageNode** head, PackageNode* new_node, ClientNode* bst_root);
void print_all_packages(PackageNode* head);
PackageNode* search_package(PackageNode* head, const char* code);
int modify_package_status(PackageNode* head, const char* code, PackageStatus new_status);
int delete_package(PackageNode** head, const char* code);
void count_packages_by_status(PackageNode* head);
void free_package_list(PackageNode* head);

// Función auxiliar para transformar el Enum a Texto legible
const char* get_status_string(PackageStatus status);

#endif // PACKAGE_LIST_H