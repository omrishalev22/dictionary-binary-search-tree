#include "helpers.h"

#define AMOUNT_OF_INPUT_SENTENCES 5

void findUserSearchingQueriesInFile(Document testDoc, WordTree wordTree, Document *trainDocsArr, int trainNumDocs,
                                    char **rawDocumentsArr);

void freeMemoryAllocation(WordTree wordTree, Document *trainDocsArr, int trainNumDocs, char **rawDocumentsArr);

int main() {
    char *fileName = "../reuters_train.txt";
    Document *trainDocsArr;
    char **rawDocumentsArr;
    int trainNumDocs;
    Document testDoc; // represents user search query in a Document object

    WordTree wordTree = buildTree(fileName);
    printf("wordTree successfully built...\n");

    documentFileToDocArr(fileName, &wordTree, &trainDocsArr, &rawDocumentsArr, &trainNumDocs);
    printf("train file read and processed...\n\n");

    findUserSearchingQueriesInFile(testDoc, wordTree, trainDocsArr, trainNumDocs, rawDocumentsArr);
    freeMemoryAllocation(wordTree, trainDocsArr, trainNumDocs, rawDocumentsArr);

}

/**
 * Method gets search query from user, turns it into a documents and searches over the
 * binary search tree to find te highest match
 * @param testDoc
 * @param wordTree
 * @param trainDocsArr
 * @param trainNumDocs
 * @param rawDocumentsArr
 */
void findUserSearchingQueriesInFile(Document testDoc, WordTree wordTree, Document *trainDocsArr, int trainNumDocs,
                                    char **rawDocumentsArr) {
    int i = 0;
    int docIdx;
    char docStr[SIZE];

    for (i = 0; i < AMOUNT_OF_INPUT_SENTENCES; i++) {
        printf("enter sentence %d/%d:", i, AMOUNT_OF_INPUT_SENTENCES);
        gets(docStr); // deprecated but Gidon gave permission to use it
        testDoc = processToDoc(docStr, &wordTree);
        printDoc(&testDoc);
        docIdx = docSimTrain(&testDoc, &trainDocsArr, trainNumDocs);
        printf("best matching train document: %d %s\n", docIdx, rawDocumentsArr[docIdx]);
    }
}

/**
 * Free all memory allocation
 * @param wordTree
 * @param trainDocsArr
 * @param trainNumDocs
 * @param testDoc
 * @param rawDocumentsArr
 */
void freeMemoryAllocation(WordTree wordTree, Document *trainDocsArr, int trainNumDocs, char **rawDocumentsArr) {
    // Free the memory
    freeTree(wordTree);
    freeArrayOfDocuments(trainDocsArr, trainNumDocs);
    freeArrayOfCharArrays(rawDocumentsArr, trainNumDocs);
}