// delivery_queue.c
#include "delivery_queue.h"
#include <stdio.h>
#include <stdlib.h>

// init_queue
// Descripción: Inicializa la cola de entregas vacía.
// Entradas: q (puntero a la cola).
// Salidas: Deja la cola con front y rear en NULL y size=0, no retorna valor.
// Restricciones: Debe llamarse antes de usar la cola.
void init_queue(DeliveryQueue* q) {
    q->front = NULL;
    q->rear = NULL;
    q->size = 0;
}

// queue_is_empty
// Descripción: Indica si la cola no tiene elmentos.
// Entradas: q (puntero a la cola).
// Salidas: Retorna 1 si está vacía, 0 en caso contrario.
// Restricciones: Ninguna.
int queue_is_empty(DeliveryQueue* q) {
    return q->front == NULL;
}

// enqueue_package
// Descripción: Encola un paquete registrado (FIFO) y cambia su estado a Pendiente de entrega.
// Entradas: q (cola), pkg (puntero al paquete real de la lista).
// Salidas: Retorna 1 si se encoló, 0 si falló.
// Restricciones: pkg no debe ser NULL ni estar ya Entregado, el nodo guarda un puntero.
int enqueue_package(DeliveryQueue* q, PackageNode* pkg) {
    if (pkg == NULL) return 0;
    if (pkg->status == DELIVERED) {
        printf("\n[INFO] Package '%s' is already delivered.\n", pkg->code);
        return 0;
    }
    QueueNode* node = (QueueNode*)malloc(sizeof(QueueNode));
    if (!node) {
        printf("\n[ERROR] Memory allocation failed.\n");
        return 0;
    }
    node->package = pkg;
    node->next = NULL;

    if (queue_is_empty(q)) {
        q->front = node;
    } else {
        q->rear->next = node;
    }
    q->rear = node;
    q->size++;

    pkg->status = PENDING; // Requisito: cambiar el estado al encolar
    printf("\n>>> Package '%s' enqueued. Status -> Pending Delivery <<<\n", pkg->code);
    return 1;
}

// display_queue
// Descripción: Muestra todos los paquetes en cola, del primero al último.
// Entradas: q (cola).
// Salidas: Imprime los paquetes en consola, no retorna valor.
// Restricciones: Si la cola está vacía muestra un mensaje informativo.
void display_queue(DeliveryQueue* q) {
    if (queue_is_empty(q)) {
        printf("\n[INFO] Delivery queue is empty.\n");
        return;
    }
    printf("\n--- DELIVERY QUEUE (front first) ---\n");
    QueueNode* cur = q->front;
    int pos = 1;
    while (cur) {
        printf("%d) Code: %s | Recipient: %s | Dest: %s | Status: %s\n",
               pos++, cur->package->code, cur->package->recipient_name,
               cur->package->destination, get_status_string(cur->package->status));
        cur = cur->next;
    }
    printf("------------------------------------\n");
}

// peek_queue
// Descripción: Consulta el siguiente paquete a entregar sin desencolarlo.
// Entradas: q (cola).
// Salidas: Retorna el paquete del frente o NULL si la cola está vacía.
// Restricciones: Ninguna.
PackageNode* peek_queue(DeliveryQueue* q) {
    if (queue_is_empty(q)) return NULL;
    return q->front->package;
}

// process_next_delivery
// Descripción: Desencola la siguiente entrega y pregunta si fue exitosa. Si lo fue, el paquete pasa a Entregado, si falló, pasa a Devuelto y se apila en la pila.
// Entradas: q (cola), returns_stack (pila de devoluciones).
// Salidas: Actualiza el estado del paquete, no retorna valor.
// Restricciones: Si la cola está vacía muestra un mensaje y no hace nada.
void process_next_delivery(DeliveryQueue* q, Stack* returns_stack) {
    if (queue_is_empty(q)) {
        printf("\n[INFO] No deliveries to process.\n");
        return;
    }
    QueueNode* node = q->front;
    PackageNode* pkg = node->package;
    q->front = node->next;
    if (q->front == NULL) q->rear = NULL;
    free(node);
    q->size--;

    printf("\nProcessing delivery of package '%s' to %s...\n", pkg->code, pkg->destination);
    printf("Was the delivery successful? (1 = Yes, 0 = No): ");
    int ok = 0;
    if (scanf("%d", &ok) != 1) {
        ok = 0;
    }
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF); // limpiar buffer (seguro ante EOF)

    if (ok == 1) {
        pkg->status = DELIVERED;
        printf("\n>>> Package '%s' marked as DELIVERED. <<<\n", pkg->code);
    } else {
        pkg->status = RETURNED;
        push(returns_stack, pkg);
        printf("\n>>> Delivery failed. Package '%s' marked as RETURNED. <<<\n", pkg->code);
    }
}

// free_queue
// Descripción: Libera todos los nodos de la cola (no los paquetes).
// Entradas: q (cola a liberar).
// Salidas: Deja la cola vacía, no retorna valor.
// Restricciones: Los paquetes los libera la lista principal, no esta función.
void free_queue(DeliveryQueue* q) {
    QueueNode* cur = q->front;
    while (cur) {
        QueueNode* tmp = cur;
        cur = cur->next;
        free(tmp); // Solo el nodo de la cola, el paquete lo libera la lista
    }
    q->front = NULL;
    q->rear = NULL;
    q->size = 0;
}
