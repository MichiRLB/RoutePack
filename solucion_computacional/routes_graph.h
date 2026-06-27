// routes_graph.h
#ifndef ROUTES_GRAPH_H
#define ROUTES_GRAPH_H

#define MAX_PLACES 10
#define INF 999999 // Valor muy alto para representar aristas inexistentes

typedef struct {
    char names[MAX_PLACES][50];          // Nombres de los puntos (Centro, San Pedro, etc.)
    int adjacency_matrix[MAX_PLACES][MAX_PLACES]; // Matriz de distancias
    int num_places;                      // Contador de puntos registrados
} Graph;

// Funciones requeridas
void init_graph(Graph* g);
int add_place(Graph* g, const char* name);
int get_place_index(Graph* g, const char* name);
void add_route(Graph* g, const char* src, const char* dest, int distance);
void display_matrix(Graph* g);
void display_places(Graph* g);

// Función principal de Dijkstra
void calculate_dijkstra(Graph* g, const char* start_name, const char* end_name);

void display_routes(Graph* g);


#endif // ROUTES_GRAPH_H