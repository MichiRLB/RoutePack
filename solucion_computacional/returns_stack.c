// returns_stack.c
#include "returns_stack.h"
#include <stdio.h>
#include <stdlib.h>

// initStack
// Descripción: Inicializa la pila de devoluciones vacía.
// Entradas: s (puntero a la pila).
// Salidas: Deja la pila con top=NULL y size=0, no retorna valor.
// Restricciones: Debe llamarse antes de usar la pila.
void initStack(Stack* s) {
    s->top = NULL;
    s->size = 0;
}

// isEmpty
// Descripción: Indica si la pila no tiene elementos.
// Entradas: s (puntero a la pila).
// Salidas: Retorna 1 si está vacía, 0 en caso contrario.
// Restricciones: Ninguna.
int isEmpty(Stack* s) {
    return s->top == NULL;
}

// push
// Descripción: Apila un paquete devuelto (comportamiento LIFO).
// Entradas: s (pila), pkg (puntero al paquete real de la lista).
// Salidas: Agrega el paquete al tope, no retorna valor.
// Restricciones: pkg no debe ser NULL, el nodo guarda un puntero, no una copia.
void push(Stack* s, PackageNode* pkg) {
    if (pkg == NULL) return;
    ReturnedNode* newNode = (ReturnedNode*)malloc(sizeof(ReturnedNode));
    if (!newNode) {
        printf("\n[ERROR] Memory allocation failed.\n");
        return;
    }
    newNode->package = pkg;
    newNode->next = s->top;
    s->top = newNode;
    s->size++;
    printf("\n>>> Returned package pushed onto stack: %s <<<\n", pkg->code);
}

// pop
// Descripción: Desapila el paquete más reciente y lo retorna para reprocesarlo.
// Entradas: s (pila).
// Salidas: Retorna el paquete del tope, o NULL si la pila está vacía.
// Restricciones: Solo libera el nodo de la pila, no el paquete (pertenece a la lista).
PackageNode* pop(Stack* s) {
    if (isEmpty(s)) {
        printf("\n[INFO] No returned packages to reprocess.\n");
        return NULL;
    }
    ReturnedNode* temp = s->top;
    PackageNode* pkg = temp->package;
    s->top = temp->next;
    free(temp); // Solo se libera el nodo de la pila, no el paquete (pertenece a la lista)
    s->size--;
    return pkg;
}

// peek
// Descripción: Consulta el último paquete devuelto sin extraerlo.
// Entradas: s (pila).
// Salidas: Retorna el paquete del tope, o NULL si la pila está vacía.
// Restricciones: Ninguna.
PackageNode* peek(Stack* s) {
    if (isEmpty(s)) return NULL;
    return s->top->package;
}

// printStack
// Descripción: Muestra los paquetes devueltos del más reciente al más antiguo.
// Entradas: s (pila).
// Salidas: Imprime los paquetes en consola, no retorna valor.
// Restricciones: Si la pila está vacía muestra un mensaje informativo.
void printStack(Stack* s) {
    if (isEmpty(s)) {
        printf("\n[INFO] Returns stack is empty.\n");
        return;
    }
    printf("\n--- RETURNED PACKAGES STACK (top first) ---\n");
    ReturnedNode* current = s->top;
    while (current) {
        printf("Code: %s | Recipient: %s | Status: %s\n",
               current->package->code,
               current->package->recipient_name,
               get_status_string(current->package->status));
        current = current->next;
    }
    printf("-------------------------------------------\n");
}

// free_stack
// Descripción: Libera todos los nodos de la pila (no los paquetes).
// Entradas: s (pila a liberar).
// Salidas: Deja la pila vacía, no retorna valor.
// Restricciones: Los paquetes los libera la lista principal, no esta función.
void free_stack(Stack* s) {
    ReturnedNode* current = s->top;
    while (current != NULL) {
        ReturnedNode* temp = current;
        current = current->next;
        free(temp); // Solo el nodo de la pila, el paquete lo libera la lista
    }
    s->top = NULL;
    s->size = 0;
}
