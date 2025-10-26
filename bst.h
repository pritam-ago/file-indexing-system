#ifndef BST_H
#define BST_H

#define MAX_LEN 256
#define PATH_LEN 512
#define DATE_LEN 64

typedef struct Node {
    char filename[MAX_LEN];
    char path[PATH_LEN];
    long size;
    char last_modified[DATE_LEN];
    struct Node *left, *right;
} Node;

Node* insertNode(Node* root, const char* path);
Node* searchNode(Node* root, const char* filename);
Node* deleteNode(Node* root, const char* filename);
void inorderDisplay(Node* root);
void saveTreeToFile(Node* root, FILE* fp);
Node* loadTreeFromFile(FILE* fp);
void freeTree(Node* root);
void visualizeTree(Node* root, int space);
void saveToCSV(Node* root, FILE* fp);
Node* loadFromCSV(const char* filepath);
Node* findNodeByName(Node* root, const char* filename);

#endif
