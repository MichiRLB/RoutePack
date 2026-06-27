// returns_stack.h
#ifndef RETURNS_STACK_H
#define RETURNS_STACK_H

#include "package_list.h"

// Cada nodo de la pila apunta al paquete real almacenado en la lista enlazada, de modo que los cambios de estado se reflejan en el repositorio principal.
typedef struct ReturnedNode {
    PackageNode* package;
    struct ReturnedNode* next;
} ReturnedNode;

typedef struct {
    ReturnedNode* top;
    int size;
} Stack;

void initStack(Stack* s);
int isEmpty(Stack* s);
void push(Stack* s, PackageNode* pkg);
PackageNode* pop(Stack* s);    // Desapila y retorna el paquete del tope (NULL si vacía)
PackageNode* peek(Stack* s);   // Consulta el último devuelto sin desapilarlo
void printStack(Stack* s);
void free_stack(Stack* s);

#endif // RETURNS_STACK_H
