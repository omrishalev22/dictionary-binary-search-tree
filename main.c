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
    FILE * f = fopen("./reuters_train.txt","r");
    buildTree(f);

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

WordTree buildTree(char * fileName) {

}