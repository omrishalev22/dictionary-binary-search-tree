#include <stdio.h>
#include "helpers.h"

#define AMOUNT_OF_INPUT_SENTENCES 5

void getMatchingDocumentToUserSearchingQueries(Document testDoc, WordTree wordTree, Document *trainDocsArr, int trainNumDocs,
                                    char **rawDocumentsArr);

void freeMemoryAllocation(WordTree wordTree, Document *trainDocsArr, int trainNumDocs, char **rawDocumentsArr);

int main() {
    char *fileName = "../reuters_train.txt";
    Document *trainDocsArr;
    char **rawDocumentsArr;
    int trainNumDocs;
    Document testDoc; // represents user search query in a Document object

    WordTree wordTree = buildTree(fileName);
    printf("The Tree of words was built successfully ...\n");

    documentFileToDocArr(fileName, &wordTree, &trainDocsArr, &rawDocumentsArr, &trainNumDocs);
    printf("Train file read and processed...\n\n");

    getMatchingDocumentToUserSearchingQueries(testDoc, wordTree, trainDocsArr, trainNumDocs, rawDocumentsArr);
    freeMemoryAllocation(wordTree, trainDocsArr, trainNumDocs, rawDocumentsArr);

}

/**
 * Method gets search query from user, turns it into a document and searches the phrase over the
 * binary search tree we built earlier to find te greatest match
 * @param testDoc - The user query after DELIMITERS cleanup represented in a Document format
 * @param wordTree - Binary search tree of words
 * @param trainDocsArr - list of all sentences from file represented as Documents.
 * @param trainNumDocs - number of lines in the given file
 * @param rawDocumentsArr - list of lines as they appear in the given file ( without any processing )
 */
void getMatchingDocumentToUserSearchingQueries(Document testDoc, WordTree wordTree, Document *trainDocsArr, int trainNumDocs,
                                    char **rawDocumentsArr) {
    int i = 0;
    int docIdx; // best matching Document line number from original file.
    char docStr[SIZE];

    for (i = 0; i < AMOUNT_OF_INPUT_SENTENCES; i++) {
        printf("Enter a sentence %d/%d:", i+1, AMOUNT_OF_INPUT_SENTENCES);
        gets(docStr); // deprecated but Gidon gave permission to use it
        testDoc = processToDoc(docStr, &wordTree);
        docIdx = docSimTrain(&testDoc, &trainDocsArr, trainNumDocs);
        printf("Best matching document: %d %s\n", docIdx, rawDocumentsArr[docIdx]);
    }
}

/**
 * Free all memory allocation
 * @param wordTree - Binary search tree of words
 * @param trainDocsArr - list of all sentences from file represented as Documents.
 * @param trainNumDocs - number of lines in the given file
 * @param testDoc - The user query after DELIMITERS cleanup represented in a Document format
 * @param rawDocumentsArr - list of lines as they appear in the given file ( without any processing )
 */
void freeMemoryAllocation(WordTree wordTree, Document *trainDocsArr, int trainNumDocs, char **rawDocumentsArr) {
    // Free the memory
    freeTree(wordTree);
    freeArrayOfDocuments(trainDocsArr, trainNumDocs);
    freeArrayOfCharArrays(rawDocumentsArr, trainNumDocs);
}