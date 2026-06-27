// routes_graph.c
#include "routes_graph.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// init_graph
// Descripción: Inicializa la matriz llenándola de infinitos y las diagonales en 0.
// Entradas: g (puntero al grafo).
// Salidas: Deja el grafo vacío (sin puntos ni rutas), no retorna valor.
// Restricciones: Debe llamarse antes de usar el grafo.
void init_graph(Graph* g) {
    g->num_places = 0;
    for (int i = 0; i < MAX_PLACES; i++) {
        for (int j = 0; j < MAX_PLACES; j++) {
            if (i == j) g->adjacency_matrix[i][j] = 0;
            else g->adjacency_matrix[i][j] = INF;
        }
    }
}

// get_place_index
// Descripción: Obtiene el índice de un punto a partir de su nombre.
// Entradas: g (grafo), name (nombre del punto).
// Salidas: Retorna el índice del punto, o -1 si no existe.
// Restricciones: name no debe ser NULL.
int get_place_index(Graph* g, const char* name) {
    for (int i = 0; i < g->num_places; i++) {
        if (strcmp(g->names[i], name) == 0) {
            return i;
        }
    }
    return -1;
}

// add_place
// Descripción: Registra un nuevo punto (vértice) en el grafo.
// Entradas: g (grafo), name (nombre del punto).
// Salidas: Retorna 1 si se agregó, 0 si falló.
// Restricciones: No se permiten nombres duplicados ni superar MAX_PLACES.
int add_place(Graph* g, const char* name) {
    if (g->num_places >= MAX_PLACES) {
        printf("\n[ERROR] Maximum number of places reached.\n");
        return 0;
    }
    if (get_place_index(g, name) != -1) {
        printf("\n[ERROR] Place '%s' already exists.\n", name);
        return 0; // Evitar duplicados
    }
    strcpy(g->names[g->num_places], name);
    g->num_places++;
    return 1;
}

// add_route
// Descripción: Registra una ruta (arista no dirigida) con su peso entre dos puntos.
// Entradas: g (grafo), src y dest (nombres de los puntos), distance (peso en km).
// Salidas: Actualiza la matriz en ambos sentidos, no retorna valor.
// Restricciones: Ambos puntos deben existir, si no no crea la ruta.
void add_route(Graph* g, const char* src, const char* dest, int distance) {
    int src_idx = get_place_index(g, src);
    int dest_idx = get_place_index(g, dest);

    if (src_idx == -1 || dest_idx == -1) {
        printf("\n[ERROR] Cannot create route. One or both places do not exist.\n");
        return;
    }

    g->adjacency_matrix[src_idx][dest_idx] = distance;
    g->adjacency_matrix[dest_idx][src_idx] = distance; // Grafo no dirigido (ida y vuelta)
}

// display_places
// Descripción: Muestra en consola los puntos de distribución disponibles.
// Entradas: g (grafo).
// Salidas: Imprime la lista de puntos, no retorna valor.
// Restricciones: Ninguna.
void display_places(Graph* g) {
    printf("\nAvailable Distribution Points:\n");
    for (int i = 0; i < g->num_places; i++) {
        printf("- %s\n", g->names[i]);
    }
}

// display_matrix
// Descripción: Muestra la matriz de adyacencia con las distancias (INF si no hay ruta directa).
// Entradas: g (grafo).
// Salidas: Imprime la matriz alineada en consola, no retorna valor.
// Restricciones: Ninguna.
void display_matrix(Graph* g) {
    printf("\n=== ADJACENCY MATRIX (DISTANCES) ===\n");
    printf("%-12s", ""); // Espacio para la columna de etiquetas de fila
    for (int i = 0; i < g->num_places; i++) printf("%-11s", g->names[i]);
    printf("\n");

    for (int i = 0; i < g->num_places; i++) {
        printf("%-12s", g->names[i]);
        for (int j = 0; j < g->num_places; j++) {
            if (g->adjacency_matrix[i][j] == INF) printf("%-11s", "INF");
            else printf("%-11d", g->adjacency_matrix[i][j]);
        }
        printf("\n");
    }
}

// print_path
// Descripción: Reconstruye e imprime recursivamente el camino desde el origen hasta j.
// Entradas: g (grafo), parent (arreglo de predecesores de Dijkstra), j (índice destino actual).
// Salidas: Imprime el recorrido con formato "A -> B -> C", no retorna valor.
// Restricciones: Ninguna.
void print_path(Graph* g, int parent[], int j) {
    if (parent[j] == -1) {
        printf("%s", g->names[j]);
        return;
    }
    print_path(g, parent, parent[j]);
    printf(" -> %s", g->names[j]);
}

// calculate_dijkstra
// Descripción: Calcula la ruta más corta entre dos puntos con el algoritmo de Dijkstra e imprime la distancia mínima y el recorrido recomendado.
// Entradas: g (grafo), start_name y end_name (nombres de origen y destino).
// Salidas: Imprime el resultado en consola, no retorna valor.
// Restricciones: Ambos puntos deben existir, si no hay ruta lo informa.
void calculate_dijkstra(Graph* g, const char* start_name, const char* end_name) {
    int start = get_place_index(g, start_name);
    int end = get_place_index(g, end_name);

    if (start == -1 || end == -1) {
        printf("\n[ERROR] Start or Destination point does not exist.\n");
        return;
    }

    int dist[MAX_PLACES];     // Distancias mínimas desde el origen
    int visited[MAX_PLACES];  // Rastreo de nodos ya procesados
    int parent[MAX_PLACES];   // Guardar el camino reconstruido

    // Inicialización del algoritmo
    for (int i = 0; i < g->num_places; i++) {
        dist[i] = INF;
        visited[i] = 0;
        parent[i] = -1;
    }
    dist[start] = 0; // La distancia al origen es cero

    // Ciclo principal de Dijkstra
    for (int count = 0; count < g->num_places - 1; count++) {
        // Encontrar el vértice con la distancia mínima que no haya sido visitado
        int min = INF, min_index = -1;
        for (int v = 0; v < g->num_places; v++) {
            if (!visited[v] && dist[v] <= min) {
                min = dist[v];
                min_index = v;
            }
        }

        if (min_index == -1) break; // Nodos restantes inalcanzables

        int u = min_index;
        visited[u] = 1; // Marcar como procesado

        // Actualizar el valor de distancia de los vértices adyacentes de 'u'
        for (int v = 0; v < g->num_places; v++) {
            if (!visited[v] && g->adjacency_matrix[u][v] != INF &&
                dist[u] != INF && dist[u] + g->adjacency_matrix[u][v] < dist[v]) {
                dist[v] = dist[u] + g->adjacency_matrix[u][v];
                parent[v] = u; // Guardamos que a 'v' se llega mejor pasando por 'u'
            }
        }
    }

    // Despliegue de los resultados requeridos
    if (dist[end] == INF) {
        printf("\n[INFO] There is no available route between %s and %s.\n", start_name, end_name);
    } else {
        printf("\n=================================");
        printf("\n       DIJKSTRA ROUTE RESULT     ");
        printf("\n=================================");
        printf("\nRuta más corta: ");
        print_path(g, parent, end);
        printf("\nDistancia total: %d km\n", dist[end]);
        printf("=================================\n");
    }
} 

// display_routes
// Descripción: Muestra la lista de rutas existentes (cada arista una sola vez).
// Entradas: g (grafo).
// Salidas: Imprime las rutas con su distancia, no retorna valor.
// Restricciones: Ninguna.
void display_routes(Graph* g) {
    printf("\n=== ROUTE LIST ===\n");
    for (int i = 0; i < g->num_places; i++) {
        for (int j = i + 1; j < g->num_places; j++) {
            if (g->adjacency_matrix[i][j] != INF && g->adjacency_matrix[i][j] != 0) {
                printf("%s <-> %s : %d km\n", g->names[i], g->names[j], g->adjacency_matrix[i][j]);
            }
        }
    }
}
