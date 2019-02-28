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
