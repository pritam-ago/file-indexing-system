#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include "bst.h"

static void getFileMetadata(const char* path, long* size, char* mod_time) {
    struct stat st;
    if (stat(path, &st) == 0) {
        *size = st.st_size;
        struct tm *tm_info = localtime(&st.st_mtime);
        strftime(mod_time, DATE_LEN, "%Y-%m-%d %H:%M:%S", tm_info);
    } else {
        *size = 0;
        strcpy(mod_time, "N/A");
    }
}

static Node* createNode(const char* path) {
    Node* node = malloc(sizeof(Node));
    if (!node) return NULL;

    const char* fname = strrchr(path, '/');
    if (!fname) fname = strrchr(path, '\\');
    fname = fname ? fname + 1 : path;

    strncpy(node->filename, fname, MAX_LEN - 1);
    strncpy(node->path, path, PATH_LEN - 1);
    getFileMetadata(path, &node->size, node->last_modified);

    node->left = node->right = NULL;
    return node;
}

Node* insertNode(Node* root, const char* path) {
    const char* fname = strrchr(path, '/');
    if (!fname) fname = strrchr(path, '\\');
    fname = fname ? fname + 1 : path;

    if (root == NULL) return createNode(path);

    int cmp = strcmp(fname, root->filename);
    if (cmp < 0)
        root->left = insertNode(root->left, path);
    else if (cmp > 0)
        root->right = insertNode(root->right, path);
    else
        printf("Duplicate ignored: %s\n", fname);

    return root;
}

Node* searchNode(Node* root, const char* filename) {
    if (!root) return NULL;
    int cmp = strcmp(filename, root->filename);
    if (cmp == 0) return root;
    else if (cmp < 0) return searchNode(root->left, filename);
    else return searchNode(root->right, filename);
}

Node* findNodeByName(Node* root, const char* filename) {
    return searchNode(root, filename);
}

Node* deleteNode(Node* root, const char* filename) {
    if (!root) return NULL;
    int cmp = strcmp(filename, root->filename);
    if (cmp < 0) root->left = deleteNode(root->left, filename);
    else if (cmp > 0) root->right = deleteNode(root->right, filename);
    else {
        if (!root->left) {
            Node* tmp = root->right;
            free(root);
            return tmp;
        } else if (!root->right) {
            Node* tmp = root->left;
            free(root);
            return tmp;
        } else {
            Node* succ = root->right;
            while (succ->left) succ = succ->left;
            strcpy(root->filename, succ->filename);
            strcpy(root->path, succ->path);
            root->size = succ->size;
            strcpy(root->last_modified, succ->last_modified);
            root->right = deleteNode(root->right, succ->filename);
        }
    }
    return root;
}

void inorderDisplay(Node* root) {
    if (!root) return;
    inorderDisplay(root->left);
    printf("%-20s | %10ld bytes | %-19s | %s\n",
           root->filename, root->size, root->last_modified, root->path);
    inorderDisplay(root->right);
}

void visualizeTree(Node* root, int space) {
    if (!root) return;
    space += 5;
    visualizeTree(root->right, space);
    printf("\n");
    for (int i = 5; i < space; i++) printf(" ");
    printf("📄 %s\n", root->filename);
    visualizeTree(root->left, space);
}

void saveTreeToFile(Node* root, FILE* fp) {
    if (!root || !fp) return;
    saveTreeToFile(root->left, fp);
    fprintf(fp, "%s|%s|%ld|%s\n", root->filename, root->path, root->size, root->last_modified);
    saveTreeToFile(root->right, fp);
}

Node* loadTreeFromFile(FILE* fp) {
    Node* root = NULL;
    char line[1024];
    while (fgets(line, sizeof(line), fp)) {
        char fname[MAX_LEN], path[PATH_LEN], date[DATE_LEN];
        long size;
        if (sscanf(line, "%[^|]|%[^|]|%ld|%[^\n]", fname, path, &size, date) == 4) {
            root = insertNode(root, path);
        }
    }
    return root;
}

void saveToCSV(Node* root, FILE* fp) {
    if (!root || !fp) return;
    saveToCSV(root->left, fp);
    fprintf(fp, "\"%s\",\"%s\",%ld,\"%s\"\n",
            root->filename, root->path, root->size, root->last_modified);
    saveToCSV(root->right, fp);
}

Node* loadFromCSV(const char* filepath) {
    FILE* fp = fopen(filepath, "r");
    if (!fp) return NULL;
    Node* root = NULL;
    char line[1024];
    fgets(line, sizeof(line), fp); // skip header
    while (fgets(line, sizeof(line), fp)) {
        char fname[MAX_LEN], path[PATH_LEN], date[DATE_LEN];
        long size;
        if (sscanf(line, "\"%[^\"]\",\"%[^\"]\",%ld,\"%[^\"]\"",
                   fname, path, &size, date) == 4) {
            root = insertNode(root, path);
        }
    }
    fclose(fp);
    return root;
}

void freeTree(Node* root) {
    if (!root) return;
    freeTree(root->left);
    freeTree(root->right);
    free(root);
}
