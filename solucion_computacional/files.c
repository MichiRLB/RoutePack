// files.c
#include "files.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// split_fields
// Descripción: Divide una línea en campos separados por ';' (modifica la cadena en sitio).
// Entradas: line (línea a dividir), fields (arreglo de salida), max_fields (capacidad).
// Salidas: Retorna la cantidad de campos encontrados, llena fields con los punteros.
// Restricciones: Ninguna.
static int split_fields(char* line, char* fields[], int max_fields) {
    int n = 0;
    char* start = line;
    for (char* p = line; ; p++) {
        if (*p == ';' || *p == '\0') {
            int end = (*p == '\0');
            *p = '\0';
            if (n < max_fields) fields[n++] = start;
            start = p + 1;
            if (end) break;
        }
    }
    return n;
}

// ===================== CLIENTES (BST) =====================

// write_clients
// Descripción: Escribe recursivamente (inorden) los clientes del BST en el archivo.
// Entradas: root (raíz del BST), f (archivo abierto en escritura).
// Salidas: Escribe una línea por cliente, no retorna valor.
// Restricciones: f debe estar abierto.
static void write_clients(ClientNode* root, FILE* f) {
    if (!root) return;
    write_clients(root->left, f);
    fprintf(f, "%s;%s;%s;%s;%s\n",
            root->id, root->name, root->phone, root->email, root->address);
    write_clients(root->right, f);
}

// save_clients
// Descripción: Guarda todos los clientes del BST en un archivo de texto.
// Entradas: root (raíz del BST), filename (nombre del archivo).
// Salidas: Retorna 1 si guardó, 0 si no pudo abrir el archivo.
// Restricciones: Sobrescribe el archivo existente.
int save_clients(ClientNode* root, const char* filename) {
    FILE* f = fopen(filename, "w");
    if (!f) {
        printf("\n[ERROR] Cannot open %s for writing.\n", filename);
        return 0;
    }
    write_clients(root, f);
    fclose(f);
    return 1;
}

// load_clients
// Descripción: Carga clientes desde un archivo de texto y los inserta en el BST.
// Entradas: root (puntero a la raíz del BST), filename (nombre del archivo).
// Salidas: Retorna 1 si abrió el archivo, 0 si no existe, informa cuántos cargó.
// Restricciones: Ignora líneas mal formadas y descarta IDs duplicados.
int load_clients(ClientNode** root, const char* filename) {
    FILE* f = fopen(filename, "r");
    if (!f) {
        printf("\n[INFO] No client file found (%s).\n", filename);
        return 0;
    }
    char line[512];
    int count = 0;
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strlen(line) == 0) continue;

        char* fld[5];
        int n = split_fields(line, fld, 5);
        if (n < 5) continue;

        ClientNode* c = (ClientNode*)malloc(sizeof(ClientNode));
        if (!c) break;
        c->left = NULL;
        c->right = NULL;
        snprintf(c->id, sizeof(c->id), "%s", fld[0]);
        snprintf(c->name, sizeof(c->name), "%s", fld[1]);
        snprintf(c->phone, sizeof(c->phone), "%s", fld[2]);
        snprintf(c->email, sizeof(c->email), "%s", fld[3]);
        snprintf(c->address, sizeof(c->address), "%s", fld[4]);

        int success = 0;
        *root = insert_client(*root, c, &success);
        if (!success) free(c);
        else count++;
    }
    fclose(f);
    printf("\n[INFO] %d client(s) loaded from %s.\n", count, filename);
    return 1;
}

// ===================== PAQUETES (Lista) =====================

// save_packages
// Descripción: Guarda todos los paquetes de la lista en un archivo de texto.
// Entradas: head (cabeza de la lista), filename (nombre del archivo).
// Salidas: Retorna 1 si guardó, 0 si no pudo abrir el archivo.
// Restricciones: Sobrescribe el archivo existente.
int save_packages(PackageNode* head, const char* filename) {
    FILE* f = fopen(filename, "w");
    if (!f) {
        printf("\n[ERROR] Cannot open %s for writing.\n", filename);
        return 0;
    }
    for (PackageNode* c = head; c != NULL; c = c->next) {
        fprintf(f, "%s;%s;%s;%s;%.2f;%d;%d\n",
                c->code, c->client_id, c->recipient_name, c->destination,
                c->weight, c->priority, (int)c->status);
    }
    fclose(f);
    return 1;
}

// load_packages
// Descripción: Carga paquetes desde un archivo de texto y los registra en la lista.
// Entradas: head (puntero a la cabeza de la lista), bst_root (BST para validar cliente), filename (nombre del archivo).
// Salidas: Retorna 1 si abrió el archivo, 0 si no existe, informa cuántos cargó.
// Restricciones: Cada paquete debe referir a un cliente existente, descarta duplicados.
int load_packages(PackageNode** head, ClientNode* bst_root, const char* filename) {
    FILE* f = fopen(filename, "r");
    if (!f) {
        printf("\n[INFO] No package file found (%s).\n", filename);
        return 0;
    }
    char line[1024];
    int count = 0;
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strlen(line) == 0) continue;

        char* fld[7];
        int n = split_fields(line, fld, 7);
        if (n < 7) continue;

        PackageNode* p = (PackageNode*)malloc(sizeof(PackageNode));
        if (!p) break;
        p->next = NULL;
        snprintf(p->code, sizeof(p->code), "%s", fld[0]);
        snprintf(p->client_id, sizeof(p->client_id), "%s", fld[1]);
        snprintf(p->recipient_name, sizeof(p->recipient_name), "%s", fld[2]);
        snprintf(p->destination, sizeof(p->destination), "%s", fld[3]);
        p->weight = (float)atof(fld[4]);
        p->priority = atoi(fld[5]);
        p->status = (PackageStatus)atoi(fld[6]);

        if (register_package(head, p, bst_root)) count++;
        else free(p);
    }
    fclose(f);
    printf("\n[INFO] %d package(s) loaded from %s.\n", count, filename);
    return 1;
}

// ===================== GRAFO (Rutas) =====================

// save_graph
// Descripción: Guarda los puntos y las rutas del grafo en un archivo de texto.
// Entradas: g (grafo), filename (nombre del archivo).
// Salidas: Retorna 1 si guardó, 0 si no pudo abrir el archivo.
// Restricciones: Ninguna.
int save_graph(Graph* g, const char* filename) {
    FILE* f = fopen(filename, "w");
    if (!f) {
        printf("\n[ERROR] Cannot open %s for writing.\n", filename);
        return 0;
    }
    // Primero los puntos, luego las rutas (solo la mitad superior de la matriz)
    for (int i = 0; i < g->num_places; i++) {
        fprintf(f, "P;%s\n", g->names[i]);
    }
    for (int i = 0; i < g->num_places; i++) {
        for (int j = i + 1; j < g->num_places; j++) {
            if (g->adjacency_matrix[i][j] != INF && g->adjacency_matrix[i][j] != 0) {
                fprintf(f, "R;%s;%s;%d\n",
                        g->names[i], g->names[j], g->adjacency_matrix[i][j]);
            }
        }
    }
    fclose(f);
    return 1;
}

// load_graph
// Descripción: Reinicia el grafo y carga puntos y rutas desde un archivo de texto.
// Entradas: g (grafo), filename (nombre del archivo).
// Salidas: Retorna 1 si abrió el archivo, 0 si no existe.
// Restricciones: Reinicia el grafo antes de cargar, ignora líneas mal formadas.
int load_graph(Graph* g, const char* filename) {
    FILE* f = fopen(filename, "r");
    if (!f) {
        printf("\n[INFO] No graph file found (%s).\n", filename);
        return 0;
    }
    init_graph(g); // Reiniciar el grafo antes de cargar
    char line[256];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strlen(line) == 0) continue;

        char* fld[4];
        int n = split_fields(line, fld, 4);
        if (n < 2) continue;

        if (strcmp(fld[0], "P") == 0) {
            add_place(g, fld[1]);
        } else if (strcmp(fld[0], "R") == 0 && n >= 4) {
            add_route(g, fld[1], fld[2], atoi(fld[3]));
        }
    }
    fclose(f);
    printf("\n[INFO] Graph loaded from %s.\n", filename);
    return 1;
}
