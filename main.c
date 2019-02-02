#include <stdio.h>
#include <string.h>

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
int isWord(char * token);
void processLine(char * line, WordTree * wordTree);


int main() {
    buildTree("./reuters_train.txt");
}

/**
 * This method get a token and returns 1 if the token is a word (includes only english chararcters) and 0 if not.
 * This method expects "token" to have lowercase characters only.
 * @param token
 * @return
 */
int isWord(char * token) {
    for (int i = 0; i < strlen(token); i++) {
        if (token[i] < 'a' || token[i] > 'z') {
            return 0;
        }
    }

    return 1;
}

/**
 * This method get a line and a wordTree. The method will parse the line into tokens,
 * and for every token which is a word - it will update the word tree with the specific token.
 * @param line
 * @param wordTree
 */
void processLine(char * line, WordTree * wordTree) {
    char *token, *delimiters = " ,.;:?!-\t'()[]{}<>~_";
    token = strtok(line, delimiters);

    while (token != NULL) {
        if (isWord(token)) {
            updateTree(wordTree, token);
        }

        token = strtok(NULL, delimiters);
    }
}

/**
 * This method get a file name, reads it content and prases every line of the file.
 * Will return a wordTree generated from the file contents.
 * @param fileName
 * @return
 */
WordTree buildTree(char * fileName) {
    FILE *file = fopen(fileName, "r");
    char line[150];

    // Initialize the tree
    WordTree wordTree;
    wordTree.root = NULL;

    // Read every line, and process it
    while (fgets(line, sizeof(line), file)) {
        processLine(line, &wordTree);
    }

    // Todo: check if we need to free the line or not
    free(line);
    fclose(file);
    return wordTree;
}