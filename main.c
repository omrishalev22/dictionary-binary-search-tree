#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

#define SIZE 150
#define SPACE " "
#define AMOUNT_OF_INPUT_SENTENCES 5

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

void toLowerCase(char * word);

TreeNode *updateTreeRec(TreeNode *node, char *word);

int compareShorts(const void * a, const void * b);

int isNumberExistsInArr(unsigned short * arr, unsigned int size, unsigned short num);

Document processToDoc(char * docStr, WordTree * wt);

void printDoc(Document * doc);

void FreeTree(WordTree tr);

void FreeTreeRec(TreeNode *root);

void documentFileToDocArr(char * filename, WordTree * wt,
                          Document ** documentsArr, char *** rawDocumentsArr, int * numDocs);

int docSimTrain(Document * testDoc, Document ** trainDocumentsArr);

int main() {
    char *fileName = "../reuters_train.txt", *docStr;
    Document **trainDocsArr, testDoc;
    char ***rawDocumentsArr;
    int trainNumDocs, docIdx, i;

    WordTree wordTree = buildTree(fileName);
    printf("wordTree successfully built...");

    documentFileToDocArr(fileName, &wordTree, trainDocsArr, rawDocumentsArr, &trainNumDocs);
    printf("train file read and processed...");

    for (i = 0; i < AMOUNT_OF_INPUT_SENTENCES; i++) {
        printf("enter sentence %d/%d    :", i, AMOUNT_OF_INPUT_SENTENCES);
        gets(docStr);
        testDoc = processToDoc(docStr, &wordTree);
        docIdx = docSimTrain(&testDoc, trainDocsArr);
        printf("best matching train document: %d %s", docIdx, *rawDocumentsArr[docIdx]);
    }

    FreeTree(wordTree);
}

/**
 * This method get a token and returns 1 if the token is a word (includes only english chararcters) and 0 if not.
 * This method expects "token" to have lowercase characters only.
 * @param token
 * @return
 */
int isWord(char *token) {
    int i;
    for (i = 0; i < strlen(token); i++) {

        // check if char ASCII is outside the ABCabc boundaries
        if (token[i] < 'A' || token[i] > 'z') {
            return 0;
        }
        token[i] = (char)tolower(token[i]);
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
    assert(file);

    char line[SIZE];

    // Initialize the tree
    // Todo: check if need to allocate memory to WordTree
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

/**
 * Creates new node to insert in the binary search tree
 * assign memory to new word and updates counter or wordId
 * @param word
 * @return
 */
TreeNode *createNode(char *word) {
    static unsigned short wordId = 0;
    TreeNode *newNode = (TreeNode *) malloc(sizeof(TreeNode));
    assert(newNode);
    newNode->word = (char *) calloc(strlen(word) + 1, sizeof(char));
    assert(newNode->word);
    strcpy(newNode->word,word);
    newNode->wordId = wordId;
    newNode->left = newNode->right = NULL;
    wordId++;
    return newNode;
}

/**
 * Updates binary search tree with new word
 * in case word exists it won't be inserted
 * @param wt
 * @param word
 */
void updateTree(WordTree *wt, char *word) {
    if(wt-> root == NULL) {
        wt->root = createNode(word);
        return;
    }
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


void FreeTree(WordTree tr) {
    FreeTreeRec(tr.root);
}

void FreeTreeRec(TreeNode *root)
{
    if (root != NULL)
    {
        FreeTreeRec(root->left);
        FreeTreeRec(root->right);
        free(root);
    }
}

/**
 * Comparison function between 2 shorts used with qsort.
 * @param a
 * @param b
 * @return
 */
int compareShorts(const void * a, const void * b) {
    return ( *(short*)a - *(short*)b );
}

/**
 * Checks if a short number exists in a short array.
 * @param arr
 * @param size
 * @param num
 * @return
 */
int isNumberExistsInArr(unsigned short * arr, unsigned int size, unsigned short num) {
    for (int i = 0; i < size; i++) {
        if (arr[i] == num) {
            return 1;
        }
    }

    return 0;
}
/**
 * The method get a string of words (seperated by a space) and a word tree.
 * The method will parse the string and return a Document object, includes the
 * amount of DIFFERENT words, and a list of the word IDs of the different words, sorted.
 * @param docStr
 * @param wt
 * @return
 */
Document processToDoc(char * docStr, WordTree * wt) {
    char *word;
    int currentWordId;
    unsigned int logicalSize = 0, physicalSize = 1;
    unsigned short * wordIds;
    Document *result;

    // Initialize document in memory
    result = (Document *)malloc(sizeof(Document));
    assert(result);

    // Initialize word Ids array
    wordIds = (unsigned short *)malloc(physicalSize * sizeof(unsigned short));
    assert(wordIds);


    // Todo: Work also with all cases , not only spaces!
    word = strtok(docStr, " ");

    while (word != NULL) {
        // Get word ID for current word
        currentWordId = findWordId(wt, word);

        if (currentWordId != -1) {
            // Check if the word ID already exists in the list
            // If not, add the word ID to the list
            if (!isNumberExistsInArr(wordIds, logicalSize, (unsigned short)currentWordId)) {
                wordIds[logicalSize] = (unsigned short)currentWordId;
                logicalSize++;
            }
        }

        // Extend wordIds memory space if needed
        if (logicalSize == physicalSize) {
            physicalSize *= 2;
            wordIds = realloc(wordIds, physicalSize * sizeof(unsigned short));
        }

        // Get next word
        word = strtok(NULL, SPACE);
    }

    wordIds = realloc(wordIds, logicalSize * sizeof(unsigned short));
    qsort(wordIds, logicalSize, sizeof(unsigned short), compareShorts);

    result->docLength = logicalSize;
    result->wordIdArr = wordIds;
    return *result;
}

/**
 * Prints a Document to the stdout.
 * @param doc
 */
void printDoc(Document * doc) {
    printf("docLength:%d wordIds:", doc->docLength);
    for (int i = 0; i < (int)doc->docLength; i++) {
        printf("%d ", doc->wordIdArr[i]);
    }
    printf("\n");
}

void documentFileToDocArr(char * filename, WordTree * wt,
                          Document ** documentsArr, char *** rawDocumentsArr, int * numDocs) {
    FILE *file = fopen(filename, "r");
    unsigned int logicalSize = 0, physicalSize = 1;

    documentsArr = (Document **)malloc(physicalSize * sizeof(Document *));
    assert(documentsArr);
    rawDocumentsArr = (char ***)malloc(physicalSize * sizeof(char **));
    assert(rawDocumentsArr);
    char line[SIZE];

    while (fgets(line, SIZE, file)) {
        char * newLine = (char *) malloc(strlen(line)+1);

        Document doc = processToDoc(line, wt);
        documentsArr[logicalSize] = &doc;
        rawDocumentsArr[logicalSize] = &newLine;

        logicalSize++;

        // Extend memory space if needed
        if (logicalSize == physicalSize) {
            physicalSize *= 2;
            documentsArr = (Document **)realloc(documentsArr, physicalSize * sizeof(Document *));
            rawDocumentsArr = (char ***)realloc(rawDocumentsArr, physicalSize * sizeof(char **));
        }
    }

    *documentsArr = (Document **)realloc(documentsArr, logicalSize * sizeof(Document *));
    *rawDocumentsArr = (char ***)realloc(rawDocumentsArr, logicalSize * sizeof(char **));
    *numDocs = logicalSize;

    fclose(file);
}


int docSimTrain(Document * testDoc,
                Document ** trainDocumentsArr) {
    // Todo: Check if DocumentArr is a thing or not
    int i = 0, matchIndex = 0;
    float maxSimValue = 0, tempSimValue;

    while (trainDocumentsArr[i] != NULL) {
        tempSimValue = docSimBinary(testDoc, trainDocumentsArr[i]);
        if (tempSimValue > maxSimValue) {
            matchIndex = i;
            maxSimValue = tempSimValue;
        }
        i++;
    }

    return matchIndex;
}