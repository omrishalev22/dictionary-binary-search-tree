#include <stdio.h>

typedef struct treeNode {
    char *word;
    unsigned short wordId;
    struct treeNode *left;
    struct treeNode *right;
} TreeNode;

typedef struct tree {
    TreeNode *root;
} WordTree;

WordTree buildTree(char * fileName);
void updateTree(WordTree *wt, char * word);
TreeNode * createNode (char  * word);


int main() {
    FILE * f = fopen("./reuters_train.txt","r");
    buildTree(f);

}

WordTree buildTree(char * fileName) {

}