#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

#define SIZE 150
#define DELIMITERS " ,.;:?!-\t'()[]{}<>~_"


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

TreeNode *updateTreeRec(TreeNode *node, char *word);

int compareShorts(const void *a, const void *b);

int isNumberExistsInArr(unsigned short *arr, unsigned int size, unsigned short num);

Document processToDoc(char *docStr, WordTree *wt);

void printDoc(Document *doc);

void freeTree(WordTree tr);

void freeTreeRec(TreeNode *root);

void documentFileToDocArr(char *filename, WordTree *wt,
                          Document **documentsArr, char ***rawDocumentsArr, int *numDocs);

int docSimTrain(Document *testDoc, Document **trainDocumentsArr, int numDocsTrain);

float docSimBinary(Document *doc1, Document *doc2);

void freeDocument(Document *document);

void freeArrayOfDocuments(Document *documents, int size);

void freeArrayOfCharArrays(char **arr, int size);

void convertStringToLowercase(char *str);


/**
 * This method get a token and returns 1 if the token is a word (includes only english chararcters) and 0 if not.
 * This method expects "token" to have lowercase characters only.
 * @param token
 * @return
 */
int isWord(char *token) {
    if(token){
        int i;
        for (i = 0; i < strlen(token); i++) {

            // check if char ASCII is outside the ABCabc boundaries
            if (token[i] < 'A' || token[i] > 'z') {
                return 0;
            }
        }
        return 1;
    }
    return 0; // no valid token
}

/**
 * This method get a line and a wordTree. The method will parse the line into tokens,
 * and for every token which is a word - it will update the word tree with the specific token.
 * @param line
 * @param wordTree
 */
void processLine(char *line, WordTree *wordTree) {
    char *token;
    token = strtok(line, DELIMITERS);

    while (token != NULL) {
        if (isWord(token)) {
            convertStringToLowercase(token);
            updateTree(wordTree, token);
        }

        token = strtok(NULL, DELIMITERS);
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
    WordTree wordTree;
    wordTree.root = NULL;

    // Read every line, and process it
    while (fgets(line, SIZE, file)) {
        strtok(line, "\n"); // remove \n from end of line before processing
        processLine(line, &wordTree);
    }

    fclose(file);
    return wordTree;
}


/**
 * This method get a word tree, and a word. The method will search for the word in the
 * word tree, and will return it's wordId.
 * Will return -1 if the word is not found.
 * @param wt - word tree
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
    strcpy(newNode->word, word);
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
    if (wt->root == NULL) {
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


void freeTree(WordTree tr) {
    freeTreeRec(tr.root);
}

// helper function for freeTree
void freeTreeRec(TreeNode *root) {
    if (root != NULL) {
        freeTreeRec(root->left);
        freeTreeRec(root->right);
        free(root->word);
        free(root);
    }
}

/**
 * Comparison function between 2 shorts, helper for the generic function qsort.
 * @param a - wordId
 * @param b - wordId
 * @return
 */
int compareShorts(const void *a, const void *b) {
    return (*(short *) a - *(short *) b);
}

/**
 * Checks if a short number exists in a short array.
 * @param arr
 * @param size
 * @param num
 * @return
 */
int isNumberExistsInArr(unsigned short *arr, unsigned int size, unsigned short num) {
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
Document processToDoc(char *docStr, WordTree *wt) {
    char *word, tempDocStr[SIZE];
    int currentWordId;
    unsigned int logicalSize = 0, physicalSize = 1;
    unsigned short *wordIds;
    Document result;

    // Initialize word Ids array
    wordIds = (unsigned short *) malloc(physicalSize * sizeof(unsigned short));
    assert(wordIds);

    // Since strtok replaces the spaces with \0, we need to copy the
    // string to another place in the memory in order not to corrupt the original docStr
    strcpy(tempDocStr, docStr);

    // Lowercase the given string
    convertStringToLowercase(tempDocStr);

    word = strtok(tempDocStr, DELIMITERS);

    while (word != NULL) {

        // Get word ID for current word
        currentWordId = isWord(word) ? findWordId(wt, word) : -1;

        if (currentWordId != -1) {
            // Check if the word ID already exists in the list
            // If not, add the word ID to the list
            if (!isNumberExistsInArr(wordIds, logicalSize, (unsigned short) currentWordId)) {
                wordIds[logicalSize] = (unsigned short) currentWordId;
                logicalSize++;
            }
        }

        // Extend wordIds memory space if needed
        if (logicalSize == physicalSize) {
            physicalSize *= 2;
            wordIds = realloc(wordIds, physicalSize * sizeof(unsigned short));
        }

        // Get next word
        word = strtok(NULL, DELIMITERS);
    }

    wordIds = realloc(wordIds, logicalSize * sizeof(unsigned short));
    qsort(wordIds, logicalSize, sizeof(unsigned short), compareShorts);

    result.docLength = logicalSize;
    result.wordIdArr = wordIds;
    return result;
}

/**
 * Prints a Document metadata to the stdout.
 * metadata - Documents size and for every word by user its corresponding WordId from the "wordTree"
 * @param doc
 */
void printDoc(Document *doc) {
    printf("docLength:%d wordIds: ", doc->docLength);
    for (int i = 0; i < (int) doc->docLength; i++) {
        printf("%d ", doc->wordIdArr[i]);
    }
    printf("\n");
}

/**
 * This method will read a file with sentences, parse them to documents
 * and will save the documents and the sentences into documentsArr and
 * rawDocumentsArr.
 * In addition, it will save the amount of generated documents in numDocs.
 * @param filename
 * @param wt - Binary search tree of words and their IDs
 * @param documentsArr - List of Documents. Each item represents a line in the given text file as Document.
 * @param rawDocumentsArr - List of sentences ( raw line from file )
 * @param numDocs - Basically represnts the num of lines in file / created documents.
 */
void documentFileToDocArr(char *filename, WordTree *wt,
                          Document *documentsArr[], char **rawDocumentsArr[], int *numDocs) {
    FILE *file = fopen(filename, "r");
    unsigned int logicalSize = 0, physicalSize = 1;

    // set memory to both documentArr
    *documentsArr = (Document *) malloc(physicalSize * sizeof(Document));
    assert(documentsArr);

    //set memory to rawDocumentsArr - the arr of lines from File
    *rawDocumentsArr = (char **) malloc(physicalSize * sizeof(char *));
    assert(rawDocumentsArr);

    char line[SIZE];

    while (fgets(line, SIZE, file)) {
        strtok(line,"\n"); // remove \n from line
        char *newLine = (char *) malloc(SIZE);

        // copy doc to array
        (*documentsArr)[logicalSize] = processToDoc(line, wt);
        memcpy(newLine, line, SIZE);

        // set line in rawDocumentsArr
        (*rawDocumentsArr)[logicalSize] = newLine;
        logicalSize++;

        // Extend memory space if needed
        if (logicalSize == physicalSize) {
            physicalSize *= 2;
            *documentsArr = realloc(*documentsArr, physicalSize * sizeof(Document));
            *rawDocumentsArr = realloc(*rawDocumentsArr, physicalSize * (sizeof(char **)));
        }
    }

    // shrinkg both arrays to their logical size if needed
    *documentsArr = realloc(*documentsArr, physicalSize * sizeof(Document));
    *rawDocumentsArr = realloc(*rawDocumentsArr, physicalSize * (sizeof(char **)));


    *numDocs = logicalSize;
    fclose(file);
}


/**
 * This method get a document and a list of documents, and using docSimBinary
 * method it will find the index of the document which has highest similarity, from the documents array.
 * @param testDoc - The user query represented in a Document format
 * @param trainDocumentsArr - List of all sentences from file represented as Documents.
 * @return matchIndex - The best matching document line from original file.
 */
int docSimTrain(Document *testDoc, Document **trainDocumentsArr, int numDocsTrain) {
    int i = 0, matchIndex = 0;
    float maxSimValue = 0, tempSimValue;

    for (i = 0; i < numDocsTrain; i++) {
        tempSimValue = docSimBinary(testDoc, *trainDocumentsArr + i);
        if (tempSimValue > maxSimValue) {
            matchIndex = i;
            maxSimValue = tempSimValue;
        }
    }

    return matchIndex;
}

/**
 * checks match ratio between two documents.
 * @param doc1 - The user query represented in a Document format - Ids are sorted in an ASC order
 * @param doc2 - A single line from original file in a Document format. - Ids are sorted in an ASC order
 * @return counter - Number of matching words between doc1 and doc2.
 */
float docSimBinary(Document *doc1, Document *doc2) {
    float counter = 0;
    unsigned int indexDoc1 = 0, indexDoc2 = 0;
    unsigned int doc1Size = doc1->docLength, doc2Size = doc2->docLength;

    while ((indexDoc1 < doc1Size) && (indexDoc2 < doc2Size)) {
        unsigned int wordId1 = doc1->wordIdArr[indexDoc1], wordId2 = doc2->wordIdArr[indexDoc2];

        if (wordId1 < wordId2) {
            indexDoc1++;
        } else if (wordId1 > wordId2) {
            indexDoc2++;
        } else {
            counter++;
            indexDoc1++;
            indexDoc2++;
        }
    }
    return counter;
}

/**
 * Frees document struct from the memory.
 * @param document
 */
void freeDocument(Document *document) {
    free(document->wordIdArr);
    document->wordIdArr = NULL;
}

/**
 * Frees array of document structs from the memory.
 * @param documents
 */
void freeArrayOfDocuments(Document *documents, int size) {
    for (int i = 0; i < size; i++) {
        freeDocument(documents + i);
    }
    free(documents);
}

/**
 * Frees array of char arrays from the memory.
 * @param arr
 * @param size
 * @return
 */
void freeArrayOfCharArrays(char **arr, int size) {
    for (int i = 0; i < size; i++) {
        free(arr[i]);
    }
    free(arr);
}

/**
 * This method get a string and lowercase it.
 * @param str
 */
void convertStringToLowercase(char *str) {
    for (int i = 0; i < strlen(str); i++) {
        str[i] = (char) tolower(str[i]);
    }
}
