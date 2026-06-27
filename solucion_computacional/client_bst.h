
// client_bst.h
#ifndef CLIENT_BST_H
#define CLIENT_BST_H

typedef struct ClientNode {
    char id[20];
    char name[100];
    char phone[20];
    char email[100];
    char address[200];
    struct ClientNode* left;
    struct ClientNode* right;
} ClientNode;

ClientNode* insert_client(ClientNode* root, ClientNode* new_node, int* success);
ClientNode* search_client(ClientNode* root, const char* id);
void print_inorder(ClientNode* root);
void print_preorder(ClientNode* root);
void print_postorder(ClientNode* root);
void free_bst(ClientNode* root);

#endif // CLIENT_BST_H
