// delivery_queue.h
#ifndef DELIVERY_QUEUE_H
#define DELIVERY_QUEUE_H

#include "package_list.h"
#include "returns_stack.h"

// Cada nodo de la cola apunta al paquete real almacenado en la lista enlazada.
typedef struct QueueNode {
    PackageNode* package;
    struct QueueNode* next;
} QueueNode;

typedef struct {
    QueueNode* front;
    QueueNode* rear;
    int size;
} DeliveryQueue;

void init_queue(DeliveryQueue* q);
int queue_is_empty(DeliveryQueue* q);
int enqueue_package(DeliveryQueue* q, PackageNode* pkg);   // Cambia el estado a Pendiente de entrega
void display_queue(DeliveryQueue* q);
PackageNode* peek_queue(DeliveryQueue* q);                 // Consulta el siguiente sin desencolar
void process_next_delivery(DeliveryQueue* q, Stack* returns_stack); // Desencola y procesa
void free_queue(DeliveryQueue* q);

#endif // DELIVERY_QUEUE_H
