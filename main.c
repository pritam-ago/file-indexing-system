#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bst.h"
#include <direct.h>

void ensureDirExists(const char* path) {
    char temp[PATH_LEN];
    strncpy(temp, path, PATH_LEN);
    for (char* p = temp + 3; *p; p++) { // skip "C:\"
        if (*p == '\\' || *p == '/') {
            char old = *p;
            *p = '\0';
            _mkdir(temp);  // create intermediate folder
            *p = old;
        }
    }
}

void writeToFile(Node* root) {
    char filename[MAX_LEN];
    printf("Enter filename to write: ");
    scanf("%s", filename);

    Node* fileNode = findNodeByName(root, filename);
    if (!fileNode) {
        printf("File not found in index.\n");
        return;
    }

    printf("Found: %s\nFull Path: %s\n", fileNode->filename, fileNode->path);
    printf("Do you want to (1) Overwrite or (2) Append? ");
    int mode;
    scanf("%d", &mode);
    getchar();

    const char* modeStr = (mode == 1) ? "w" : "a";

    ensureDirExists(fileNode->path);  // ✅ create folders automatically

    FILE* fp = fopen(fileNode->path, modeStr);
    if (!fp) {
        perror("❌ Cannot open or create file");
        return;
    }

    printf("Enter content (end with a single line containing END):\n");
    char line[512];
    while (1) {
        fgets(line, sizeof(line), stdin);
        if (strncmp(line, "END", 3) == 0) break;
        fputs(line, fp);
    }

    fclose(fp);
    refreshMetadata(fileNode);  // ✅ Update size & last-modified in BST
    printf("✅ File written successfully! Metadata refreshed.\n");

}

int main() {
    Node* root = NULL;
    int choice;
    char path[PATH_LEN];
    FILE *fp;

    do {
        printf("\n===== FILE INDEXING SYSTEM =====\n");
        printf("1. Add File\n2. Search File\n3. Display All Files\n4. Delete File\n");
        printf("5. Save Index to Disk\n6. Load Index from Disk\n");
        printf("7. Exit\n8. Visualize Tree\n9. Export to CSV\n10. Import from CSV\n");
        printf("11. Write to File\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        getchar();

        switch (choice) {
            case 1:
                printf("Enter file path: ");
                fgets(path, sizeof(path), stdin);
                path[strcspn(path, "\n")] = 0;
                root = insertNode(root, path);
                break;
            case 2: {
                char fname[MAX_LEN];
                printf("Enter filename to search: ");
                scanf("%s", fname);
                Node* found = searchNode(root, fname);
                if (found)
                    printf("Found: %s (%s)\n", found->filename, found->path);
                else
                    printf("Not found.\n");
                break;
            }
            case 3:
                inorderDisplay(root);
                break;
            case 4: {
                char fname[MAX_LEN];
                printf("Enter filename to delete: ");
                scanf("%s", fname);
                root = deleteNode(root, fname);
                break;
            }
            case 5:
                fp = fopen("index.txt", "w");
                saveTreeToFile(root, fp);
                fclose(fp);
                printf("Saved to index.txt\n");
                break;
            case 6:
                fp = fopen("index.txt", "r");
                if (fp) {
                    root = loadTreeFromFile(fp);
                    fclose(fp);
                    printf("Loaded index.txt\n");
                } else printf("No file found.\n");
                break;
            case 8:
                visualizeTree(root, 0);
                break;
            case 9:
                fp = fopen("index.csv", "w");
                fprintf(fp, "\"Filename\",\"Path\",\"Size\",\"Last Modified\"\n");
                saveToCSV(root, fp);
                fclose(fp);
                printf("Exported to index.csv\n");
                break;
            case 10:
                root = loadFromCSV("index.csv");
                printf("Imported from index.csv\n");
                break;
            case 11:
                writeToFile(root);
                break;
        }
    } while (choice != 7);

    freeTree(root);
    return 0;
}
