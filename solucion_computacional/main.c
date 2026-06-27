// main.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "client_bst.h"
#include "package_list.h"
#include "package_avl.h"
#include "routes_graph.h"
#include "returns_stack.h"
#include "delivery_queue.h"
#include "files.h"

// Nombres de los archivos de persistencia
#define CLIENTS_FILE  "clients.txt"
#define PACKAGES_FILE "packages.txt"
#define GRAPH_FILE    "graph.txt"

// flush_input
// Descripción: Limpia el resto de la línea de entrada de forma segura ante EOF.
// Entradas: Ninguna (lee de stdin).
// Salidas: No retorna valor, consume caracteres hasta '\n' o EOF.
// Restricciones: Evita bucles infinitos cuando la entrada llega a su fin.
void flush_input(void) {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF);
}

// read_string
// Descripción: Lee una línea de texto de la consola y elimina el salto de línea.
// Entradas: buffer (destino), size (tamaño del buffer).
// Salidas: Deja en buffer la cadena leida (vacía si hay EOF), no retorna valor.
// Restricciones: buffer debe tener al menos size bytes.
void read_string(char* buffer, int size) {
    if (fgets(buffer, size, stdin) == NULL) {
        buffer[0] = '\0'; // EOF o error: cadena vacía
        return;
    }
    buffer[strcspn(buffer, "\n")] = 0; // Elimina el '\n' al final
    buffer[strcspn(buffer, "\r")] = 0; // Elimina el '\r' por si estás en Windows
}

// === PROTOTIPOS ===
void menu_clients(ClientNode** bst_root);
void menu_packages(PackageNode** package_head, ClientNode* bst_root, AVLNode** avl_root);
void menu_deliveries(PackageNode** package_head, DeliveryQueue* q, Stack* s);
void menu_avl(AVLNode** avl_root, PackageNode* package_head);
void menu_routes(Graph* g);
void menu_reports(PackageNode* package_head, ClientNode* bst_root, AVLNode* avl_root, Graph* g);
void menu_files(ClientNode** bst_root, PackageNode** package_head, Graph* g, AVLNode** avl_root);
void rebuild_avl(AVLNode** avl_root, PackageNode* package_head);

int main() {
    ClientNode* bst_root = NULL;       // Raíz del árbol BST de clientes
    PackageNode* package_head = NULL;  // Cabeza de la lista enlazada de paquetes
    AVLNode* avl_root = NULL;          // Raíz del árbol AVL de paquetes por código

    // Inicialización del grafo de rutas
    Graph route_graph;
    init_graph(&route_graph);

    // Inicialización de la cola de entregas y la pila de devoluciones
    DeliveryQueue delivery_queue;
    init_queue(&delivery_queue);
    Stack returned_stack;
    initStack(&returned_stack);

    // Puntos y rutas de ejemplo por defecto
    add_place(&route_graph, "Centro");
    add_place(&route_graph, "San Pedro");
    add_place(&route_graph, "Tres Rios");
    add_place(&route_graph, "Cartago");
    add_place(&route_graph, "Heredia");

    add_route(&route_graph, "Centro", "San Pedro", 5);
    add_route(&route_graph, "San Pedro", "Tres Rios", 7);
    add_route(&route_graph, "Tres Rios", "Cartago", 10);
    add_route(&route_graph, "Centro", "Heredia", 12);

    int main_option = 0;

    do {
        printf("\n=================================");
        printf("\n      ROUTEPACK SYSTEM - MAIN    ");
        printf("\n=================================");
        printf("\n1. Manage Clients (BST)");
        printf("\n2. Manage Packages (Linked List)");
        printf("\n3. Manage Deliveries (Queue + Returns Stack)");
        printf("\n4. Manage Packages by Code (AVL)");
        printf("\n5. Manage Routes & Dijkstra (Graph)");
        printf("\n6. Reports Dashboard");
        printf("\n7. Save / Load Data (Files)");
        printf("\n8. Exit");
        printf("\n=================================");
        printf("\nSelect an option: ");

        if (scanf("%d", &main_option) != 1) {
            if (feof(stdin)) { main_option = 8; break; }
            printf("\n[ERROR] Invalid input. Please enter a number.\n");
            flush_input();
            continue;
        }
        flush_input(); // Consumir el resto de la línea

        switch (main_option) {
            case 1: menu_clients(&bst_root); break;
            case 2: menu_packages(&package_head, bst_root, &avl_root); break;
            case 3: menu_deliveries(&package_head, &delivery_queue, &returned_stack); break;
            case 4: menu_avl(&avl_root, package_head); break;
            case 5: menu_routes(&route_graph); break;
            case 6: menu_reports(package_head, bst_root, avl_root, &route_graph); break;
            case 7: menu_files(&bst_root, &package_head, &route_graph, &avl_root); break;
            case 8: printf("\nExiting RoutePack and freeing all memory resources...\n"); break;
            default: printf("\nOption not available. Try again.\n");
        }

    } while (main_option != 8);

    // Liberación de memoria
    avl_free(avl_root);             // Solo libera nodos AVL (los paquetes los libera la lista)
    free_queue(&delivery_queue);    // Solo libera nodos de la cola
    free_stack(&returned_stack);    // Solo libera nodos de la pila
    free_package_list(package_head);// Libera los paquetes
    free_bst(bst_root);             // Libera los clientes

    return 0;
}

// =================================================================
// RECONSTRUCCIÓN DEL AVL A PARTIR DE LA LISTA
// =================================================================
// rebuild_avl
// Descripción: Reconstruye el AVL desde cero insertando todos los paquetes de la lista.
// Entradas: avl_root (puntero a la raíz del AVL), package_head (cabeza de la lista).
// Salidas: Deja *avl_root apuntando al nuevo árbol, no retorna valor.
// Restricciones: Útil tras cargar desde archivo, ya que el AVL guarda punteros a la lista.
void rebuild_avl(AVLNode** avl_root, PackageNode* package_head) {
    avl_free(*avl_root);
    *avl_root = NULL;
    int success;
    for (PackageNode* c = package_head; c != NULL; c = c->next) {
        *avl_root = avl_insert(*avl_root, c, &success);
    }
}

// =================================================================
// MENÚ DE CLIENTES (BST)
// =================================================================
// menu_clients
// Descripción: Submenu de gestión de clientes (BST): registrar, buscar y recorridos.
// Entradas: bst_root (puntero a la raíz del BST de clientes).
// Salidas: Modifica el árbol según la opción elegida, no retorna valor.
// Restricciones: No permite IDs duplicados al registrar.
void menu_clients(ClientNode** bst_root) {
    int option = 0;
    do {
        printf("\n>>> ROUTEPACK - CLIENTS MANAGEMENT <<<");
        printf("\n1. Register New Client");
        printf("\n2. Search Client by ID");
        printf("\n3. Display Clients (Inorder - Sorted)");
        printf("\n4. Display Clients (Preorder / Postorder)");
        printf("\n5. Return to Main Menu");
        printf("\nSelect an option: ");

        if (scanf("%d", &option) != 1) {
            if (feof(stdin)) return;
            flush_input();
            continue;
        }
        flush_input();

        switch (option) {
            case 1: {
                ClientNode* new_client = (ClientNode*)malloc(sizeof(ClientNode));
                if (!new_client) break;
                new_client->left = NULL;
                new_client->right = NULL;

                printf("\nEnter Client ID: ");            read_string(new_client->id, 20);
                printf("Enter Full Name: ");              read_string(new_client->name, 100);
                printf("Enter Phone: ");                  read_string(new_client->phone, 20);
                printf("Enter Email: ");                  read_string(new_client->email, 100);
                printf("Enter Delivery Zone/Address: ");  read_string(new_client->address, 200);

                int success = 0;
                *bst_root = insert_client(*bst_root, new_client, &success);

                if (success) printf("\n>>> Client registered successfully! <<<\n");
                else {
                    printf("\n[ERROR] ID '%s' already exists.\n", new_client->id);
                    free(new_client);
                }
                break;
            }
            case 2: {
                char search_id[20];
                printf("\nEnter Client ID to search: ");
                read_string(search_id, 20);
                ClientNode* found = search_client(*bst_root, search_id);
                if (found) {
                    printf("\nClient Found:\nID: %s\nName: %s\nPhone: %s\nEmail: %s\nAddress: %s\n",
                           found->id, found->name, found->phone, found->email, found->address);
                } else {
                    printf("\nClient with ID '%s' not found.\n", search_id);
                }
                break;
            }
            case 3:
                printf("\n--- CLIENTS (INORDER) ---");
                print_inorder(*bst_root);
                break;
            case 4:
                printf("\n--- CLIENTS (PREORDER) ---");
                print_preorder(*bst_root);
                printf("\n--- CLIENTS (POSTORDER) ---");
                print_postorder(*bst_root);
                break;
        }
    } while (option != 5);
}

// =================================================================
// MENÚ DE PAQUETES (Lista enlazada)
// =================================================================
// menu_packages
// Descripción: Submenu de gestión de paquetes (lista): registrar, mostrar, buscar, modificar y eliminar.
// Entradas: package_head (puntero a la lista), bst_root (BST de clientes), avl_root (AVL de paquetes).
// Salidas: Modifica la lista y mantiene el AVL sincronizado, no retorna valor.
// Restricciones: Al registrar valida cliente existente y código único.
void menu_packages(PackageNode** package_head, ClientNode* bst_root, AVLNode** avl_root) {
    int option = 0;
    do {
        printf("\n>>> ROUTEPACK - PACKAGES MANAGEMENT <<<");
        printf("\n1. Register New Package");
        printf("\n2. Display All Packages");
        printf("\n3. Search Package by Code");
        printf("\n4. Modify Package Status");
        printf("\n5. Delete Package");
        printf("\n6. Return to Main Menu");
        printf("\nSelect an option: ");

        if (scanf("%d", &option) != 1) {
            if (feof(stdin)) return;
            flush_input();
            continue;
        }
        flush_input();

        switch (option) {
            case 1: {
                PackageNode* new_pkg = (PackageNode*)malloc(sizeof(PackageNode));
                if (!new_pkg) break;
                new_pkg->next = NULL;
                new_pkg->status = REGISTERED;

                printf("\nEnter Package Unique Code: "); read_string(new_pkg->code, 20);
                printf("Enter Associated Client ID: ");  read_string(new_pkg->client_id, 20);
                printf("Enter Recipient Name: ");        read_string(new_pkg->recipient_name, 100);
                printf("Enter Destination Point: ");     read_string(new_pkg->destination, 100);

                printf("Enter Weight (kg): ");
                if (scanf("%f", &new_pkg->weight) != 1) new_pkg->weight = 0.0f;

                printf("Enter Priority (1-High, 2-Medium, 3-Low): ");
                if (scanf("%d", &new_pkg->priority) != 1) new_pkg->priority = 3;
                getchar();

                if (register_package(package_head, new_pkg, bst_root)) {
                    // Mantener el AVL sincronizado por código
                    int success = 0;
                    *avl_root = avl_insert(*avl_root, new_pkg, &success);
                    printf("\n>>> Package registered successfully! <<<\n");
                } else {
                    free(new_pkg);
                }
                break;
            }
            case 2:
                print_all_packages(*package_head);
                break;
            case 3: {
                char code[20];
                printf("\nEnter package code to search: ");
                read_string(code, 20);
                PackageNode* pkg = search_package(*package_head, code);
                if (pkg) {
                    printf("\nPackage Found:\nCode: %s\nClient ID: %s\nRecipient: %s\nDest: %s\nWeight: %.2fkg\nPriority: %d\nStatus: %s\n",
                           pkg->code, pkg->client_id, pkg->recipient_name, pkg->destination,
                           pkg->weight, pkg->priority, get_status_string(pkg->status));
                } else {
                    printf("\nPackage not found.\n");
                }
                break;
            }
            case 4: {
                char code[20];
                int status_choice;
                printf("\nEnter package code to modify: ");
                read_string(code, 20);
                if (search_package(*package_head, code) == NULL) {
                    printf("\nPackage not found.\n");
                    break;
                }
                printf("\n0. Registered | 1. Pending | 2. In Route | 3. Delivered | 4. Returned\nChoice: ");
                if (scanf("%d", &status_choice) == 1 && status_choice >= 0 && status_choice <= 4) {
                    modify_package_status(*package_head, code, (PackageStatus)status_choice);
                    printf("\n>>> Status updated. <<<\n");
                } else {
                    printf("\n[ERROR] Invalid status.\n");
                }
                getchar();
                break;
            }
            case 5: {
                char code[20];
                printf("\nEnter package code to DELETE: ");
                read_string(code, 20);
                if (search_package(*package_head, code) == NULL) {
                    printf("\nPackage not found.\n");
                    break;
                }
                // Eliminar del AVL antes de liberar el nodo de la lista
                *avl_root = avl_delete(*avl_root, code);
                delete_package(package_head, code);
                printf("\n>>> Package deleted. <<<\n");
                break;
            }
        }
    } while (option != 6);
}

// =================================================================
// MENÚ DE ENTREGAS (Cola FIFO + Pila de devoluciones LIFO)
// =================================================================
// menu_deliveries
// Descripción: Submenu de entregas: cola FIFO (encolar, mostrar, peek, procesar) y pila LIFO de devoluciones (mostrar, peek, reprocesar/reencolar).
// Entradas: package_head (lista), q (cola de entregas), s (pila de devoluciones).
// Salidas: Modifica cola, pila y estados de los paquetes, no retorna valor.
// Restricciones: Solo se pueden encolar paquetes existentes en la lista.
void menu_deliveries(PackageNode** package_head, DeliveryQueue* q, Stack* s) {
    int option = 0;
    do {
        printf("\n>>> ROUTEPACK - DELIVERIES MANAGEMENT <<<");
        printf("\n--- Delivery Queue (FIFO) ---");
        printf("\n1. Enqueue a Registered Package");
        printf("\n2. Show Delivery Queue");
        printf("\n3. Peek Next Delivery");
        printf("\n4. Process Next Delivery");
        printf("\n--- Returns Stack (LIFO) ---");
        printf("\n5. Show Returned Packages");
        printf("\n6. Peek Last Returned Package");
        printf("\n7. Reprocess Last Returned (Pop -> Re-enqueue)");
        printf("\n8. Return to Main Menu");
        printf("\nSelect an option: ");

        if (scanf("%d", &option) != 1) {
            if (feof(stdin)) return;
            flush_input();
            continue;
        }
        flush_input();

        switch (option) {
            case 1: {
                char code[20];
                printf("\nEnter package code to enqueue: ");
                read_string(code, 20);
                PackageNode* pkg = search_package(*package_head, code);
                if (pkg == NULL) {
                    printf("\nPackage not found.\n");
                } else {
                    enqueue_package(q, pkg);
                }
                break;
            }
            case 2:
                display_queue(q);
                break;
            case 3: {
                PackageNode* next = peek_queue(q);
                if (next == NULL) printf("\n[INFO] Delivery queue is empty.\n");
                else printf("\nNext delivery: Code %s | Recipient: %s | Dest: %s\n",
                            next->code, next->recipient_name, next->destination);
                break;
            }
            case 4:
                process_next_delivery(q, s);
                break;
            case 5:
                printStack(s);
                break;
            case 6: {
                PackageNode* last = peek(s);
                if (last == NULL) printf("\n[INFO] Returns stack is empty.\n");
                else printf("\nLast returned: Code %s | Recipient: %s\n",
                            last->code, last->recipient_name);
                break;
            }
            case 7: {
                PackageNode* pkg = pop(s);
                if (pkg != NULL) {
                    enqueue_package(q, pkg); // Reencola y cambia el estado a Pendiente
                }
                break;
            }
        }
    } while (option != 8);
}

// =================================================================
// MENÚ DE PAQUETES POR CÓDIGO (AVL)
// =================================================================
// menu_avl
// Descripción: Submenu del AVL de paquetes por código: buscar, inorden, estructura y reconstruir.
// Entradas: avl_root (puntero a la raíz del AVL), package_head (lista, para reconstruir).
// Salidas: Consulta o reconstruye el AVL, no retorna valor.
// Restricciones: El AVL se mantiene balanceado y sincronizado con la lista.
void menu_avl(AVLNode** avl_root, PackageNode* package_head) {
    int option = 0;
    do {
        printf("\n>>> ROUTEPACK - PACKAGES BY CODE (AVL) <<<");
        printf("\n1. Search Package by Code");
        printf("\n2. Display Packages Inorder (Sorted by Code)");
        printf("\n3. Show Tree Structure (Height & Balance)");
        printf("\n4. Rebuild AVL from Package List");
        printf("\n5. Return to Main Menu");
        printf("\nSelect an option: ");

        if (scanf("%d", &option) != 1) {
            if (feof(stdin)) return;
            flush_input();
            continue;
        }
        flush_input();

        switch (option) {
            case 1: {
                char code[20];
                printf("\nEnter package code to search: ");
                read_string(code, 20);
                PackageNode* pkg = avl_search(*avl_root, code);
                if (pkg) {
                    printf("\nPackage Found (AVL):\nCode: %s\nClient ID: %s\nRecipient: %s\nStatus: %s\n",
                           pkg->code, pkg->client_id, pkg->recipient_name, get_status_string(pkg->status));
                } else {
                    printf("\nPackage not found in AVL.\n");
                }
                break;
            }
            case 2:
                if (*avl_root == NULL) {
                    printf("\n[INFO] AVL is empty.\n");
                } else {
                    printf("\n--- PACKAGES INORDER (AVL) ---\n");
                    avl_print_inorder(*avl_root);
                }
                break;
            case 3:
                if (*avl_root == NULL) {
                    printf("\n[INFO] AVL is empty.\n");
                } else {
                    printf("\n--- AVL TREE STRUCTURE (rotated 90deg) ---\n");
                    avl_print_structure(*avl_root, 0);
                    printf("Tree height: %d\n", avl_height(*avl_root));
                }
                break;
            case 4:
                rebuild_avl(avl_root, package_head);
                printf("\n>>> AVL rebuilt from package list. <<<\n");
                break;
        }
    } while (option != 5);
}

// =================================================================
// MENÚ DE RUTAS (Grafo + Dijkstra)
// =================================================================
// menu_routes
// Descripción: Submenu de rutas (grafo): mostrar puntos, registrar puntos y rutas, ver la matriz de adyacencia y calcular la ruta más corta (Dijkstra).
// Entradas: g (grafo de rutas).
// Salidas: Modifica el grafo o muestra resultados, no retorna valor.
// Restricciones: No crea rutas con puntos inexistentes.
void menu_routes(Graph* g) {
    int option = 0;
    do {
        printf("\n>>> ROUTEPACK - ROUTING MANAGEMENT <<<");
        printf("\n1. Display Available Places");
        printf("\n2. Register New Distribution Point");
        printf("\n3. Register Route between Points");
        printf("\n4. Display Adjacency Matrix");
        printf("\n5. Calculate Shortest Route (Dijkstra)");
        printf("\n6. Return to Main Menu");
        printf("\nSelect an option: ");

        if (scanf("%d", &option) != 1) {
            if (feof(stdin)) return;
            flush_input();
            continue;
        }
        flush_input();

        switch (option) {
            case 1: display_places(g); break;
            case 2: {
                char name[50];
                printf("\nEnter new place name: ");
                read_string(name, 50);
                if (add_place(g, name)) printf("\n>>> Place added. <<<\n");
                break;
            }
            case 3: {
                char src[50], dest[50];
                int dist;
                printf("\nEnter Origin Place: "); read_string(src, 50);
                printf("Enter Destination: ");    read_string(dest, 50);
                printf("Enter Distance (km): ");
                if (scanf("%d", &dist) == 1) add_route(g, src, dest, dist);
                getchar();
                break;
            }
            case 4: display_matrix(g); break;
            case 5: {
                char start[50], end[50];
                printf("\nEnter Origin Point: ");      read_string(start, 50);
                printf("Enter Destination Point: ");   read_string(end, 50);
                calculate_dijkstra(g, start, end);
                break;
            }
        }
    } while (option != 6);
}

// =================================================================
// MENÚ DE REPORTES
// =================================================================
// menu_reports
// Descripción: Submenu de reportes: totales por estado, clientes (BST), paquetes por código (AVL y ruta más corta entre dos puntos.
// Entradas: package_head (lista), bst_root (BST), avl_root (AVL), g (grafo).
// Salidas: Imprime los reportes en consola, no retorna valor.
// Restricciones: Solo lectura, no modifica las estructuras.
void menu_reports(PackageNode* package_head, ClientNode* bst_root, AVLNode* avl_root, Graph* g) {
    int option = 0;
    do {
        printf("\n>>> ROUTEPACK - REPORTS DASHBOARD <<<");
        printf("\n1. Package Status Summary (Totals)");
        printf("\n2. Registered Clients (BST Inorder)");
        printf("\n3. Packages Ordered by Code (AVL Inorder)");
        printf("\n4. Shortest Route between Two Points (Dijkstra)");
        printf("\n5. Return to Main Menu");
        printf("\nSelect an option: ");

        if (scanf("%d", &option) != 1) {
            if (feof(stdin)) return;
            flush_input();
            continue;
        }
        flush_input();

        switch (option) {
            case 1:
                count_packages_by_status(package_head);
                break;
            case 2:
                if (bst_root == NULL) printf("\n[INFO] No clients registered.\n");
                else {
                    printf("\n--- REGISTERED CLIENTS (BST INORDER) ---");
                    print_inorder(bst_root);
                }
                break;
            case 3:
                if (avl_root == NULL) printf("\n[INFO] No packages in AVL.\n");
                else {
                    printf("\n--- PACKAGES ORDERED BY CODE (AVL) ---\n");
                    avl_print_inorder(avl_root);
                }
                break;
            case 4: {
                char start[50], end[50];
                printf("\nEnter Origin Point: ");    read_string(start, 50);
                printf("Enter Destination Point: "); read_string(end, 50);
                calculate_dijkstra(g, start, end);
                break;
            }
        }
    } while (option != 5);
}

// =================================================================
// MENÚ DE ARCHIVOS (Persistencia)
// =================================================================
// menu_files
// Descripción: Submenu de persistencia: guardar y cargar clientes, paquetes y grafo en archivos.
// Entradas: bst_root (BST), package_head (lista), g (grafo), avl_root (AVL).
// Salidas: Guarda o carga datos, tras cargar reconstruye el AVL, no retorna valor.
// Restricciones: Al cargar paquetes los clientes deben cargarse primero.
void menu_files(ClientNode** bst_root, PackageNode** package_head, Graph* g, AVLNode** avl_root) {
    int option = 0;
    do {
        printf("\n>>> ROUTEPACK - DATA PERSISTENCE <<<");
        printf("\n1. Save All Data to Files");
        printf("\n2. Load All Data from Files");
        printf("\n3. Return to Main Menu");
        printf("\nSelect an option: ");

        if (scanf("%d", &option) != 1) {
            if (feof(stdin)) return;
            flush_input();
            continue;
        }
        flush_input();

        switch (option) {
            case 1:
                save_clients(*bst_root, CLIENTS_FILE);
                save_packages(*package_head, PACKAGES_FILE);
                save_graph(g, GRAPH_FILE);
                printf("\n>>> All data saved successfully. <<<\n");
                break;
            case 2:
                load_clients(bst_root, CLIENTS_FILE);
                load_packages(package_head, *bst_root, PACKAGES_FILE);
                load_graph(g, GRAPH_FILE);
                // Reconstruir el AVL con los paquetes cargados
                rebuild_avl(avl_root, *package_head);
                printf("\n>>> Data load completed. AVL rebuilt. <<<\n");
                break;
        }
    } while (option != 3);
}
