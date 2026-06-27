// files.h
#ifndef FILES_H
#define FILES_H

#include "client_bst.h"
#include "package_list.h"
#include "routes_graph.h"

// Persistencia mediante archivos de texto.
// Retornan 1 en exito, 0 si el archivo no pudo abrirse.
int save_clients(ClientNode* root, const char* filename);
int load_clients(ClientNode** root, const char* filename);
int save_packages(PackageNode* head, const char* filename);
int load_packages(PackageNode** head, ClientNode* bst_root, const char* filename);
int save_graph(Graph* g, const char* filename);
int load_graph(Graph* g, const char* filename);

#endif // FILES_H
