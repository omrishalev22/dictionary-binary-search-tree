#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

#define SIZE 150

typedef struct treeNode {
    char *word;
    unsigned short wordId;
    struct treeNode *left;
    struct treeNode *right;
} TreeNode;

typedef struct tree {
    TreeNode *root;
} WordTree;

typedef struct document {
    unsigned int docLength;
    unsigned short *wordIdArr;
} Document;


int isWord(char *token);

void processLine(char *line, WordTree *wordTree);

WordTree buildTree(char *fileName);

void updateTree(WordTree *wt, char *word);

TreeNode *createNode(char *word);

int findWordId(WordTree *wt, char *word);

int findWordIdRec(TreeNode *root, char *word);

void updateTree(WordTree *wt, char *word);

TreeNode *updateTreeRec(TreeNode *node, char *word);

void verifyMemoryAllocation(void *ptr);

int main() {
    buildTree("./reuters_train.txt");
}

/**
 * Verifies memory allocation.
 * @param ptr
 */
void verifyMemoryAllocation(void *ptr)
{
    if (ptr == NULL)
    {
        puts("Memory allocation error detected!");
        exit(1);
    }
}

/**
 * This method get a token and returns 1 if the token is a word (includes only english chararcters) and 0 if not.
 * This method expects "token" to have lowercase characters only.
 * @param token
 * @return
 */
int isWord(char *token) {
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
void processLine(char *line, WordTree *wordTree) {
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
WordTree buildTree(char *fileName) {
    FILE *file = fopen(fileName, "r");
    char line[SIZE];

    // Initialize the tree
    WordTree wordTree;
    wordTree.root = NULL;

    // Read every line, and process it
    while (fgets(line, SIZE, file)) {
        processLine(line, &wordTree);
    }

    fclose(file);
    return wordTree;
}


/**
 * This method get a word tree, and a word. The method will search for the word in the
 * word tree, and will return it's wordId.
 * Will return -1 if the word is not found.
 * @param wt
 * @param word
 * @return
 */
int findWordId(WordTree *wt, char *word) {
    return findWordIdRec(wt->root, word);
}

/**
 * This is the recursive method of findWordId, used to find the wordId of a specific word.
 * @param root
 * @param word
 * @return
 */
int findWordIdRec(TreeNode *root, char *word) {
    if (root == NULL) {
        return -1;
    }

    int compareResults = strcmp(root->word, word);

    if (compareResults == 0) {
        // We found our word
        return (int) root->wordId;
    } else if (compareResults > 0) {
        // We need to look for words in the left side of the tree node
        return findWordIdRec(root->left, word);
    } else {
        // We need to look for words in the right side of the tree node
        return findWordIdRec(root->right, word);
    }
}


TreeNode *createNode(char *word) {
    static unsigned short index = 0;
    TreeNode *newNode = (TreeNode *) malloc(sizeof(TreeNode));
    newNode->word = (char *) calloc(strlen(word) + 1, sizeof(char));
    newNode->wordId = index;
    newNode->left = newNode->right = NULL;
    index++;
    return newNode;
}

void updateTree(WordTree *wt, char *word) {
    updateTreeRec(wt->root, word);
}

/**
 * Function recursively go over the tree and insert new word
 * based on its lexicography order, bigger goes right, smaller left.
 * if word exists it will not be inserted into tree again.
 * @param node
 * @param word
 * @return
 */
TreeNode *updateTreeRec(TreeNode *node, char *word) {
    if (node == NULL) return createNode(word);

    int isLexBigger = strcmp(word, node->word);

    if (isLexBigger < 0) {
        node->left = updateTreeRec(node->left, word);
    } else if (isLexBigger > 0) {
        node->right = updateTreeRec(node->right, word);
    }

    return node;
}
