#include <stdio.h>
#include <stdlib.h>

typedef union _DATE
{
    struct
    {
        unsigned day : 5;
        unsigned month : 4;
        unsigned year : 23;
    } sub;
    unsigned int dateval;
} DATE;

typedef struct _USER_MODEL
{
    char szFullName[31];
    char szEmail[63];
    DATE birthdate;
} USER_MODEL;

typedef struct Node
{
    long next;
    USER_MODEL data;
} Node;

// You must call this function while file pointer was at the end of the file. There is no control for efficency
void addNodeAfterTargetNode(FILE *persistedData, long prevNodeOffset, Node newNode)
{
    long newNodeOffset = ftell(persistedData);

    // Read prev's next for insertion
    long prevNext;
    fseek(persistedData, prevNodeOffset, SEEK_SET);
    fread(&prevNext, sizeof(long), 1, persistedData);
    newNode.next = prevNext;

    // Update prev new next (node that will be created)
    fseek(persistedData, prevNodeOffset, SEEK_SET);
    fwrite(&newNodeOffset, sizeof(long), 1, persistedData);

    // Save created value by prevs next
    fseek(persistedData, 0, SEEK_END);
    fwrite(&newNode, sizeof(Node), 1, persistedData);
}

Node generateNodeWithData(int seed)
{
    Node newNode;
    sprintf(newNode.data.szFullName, "Fullname_%d", seed);
    sprintf(newNode.data.szEmail, "Email_%d", seed);
    newNode.data.birthdate.sub.day = seed % 29;
    newNode.data.birthdate.sub.month = seed % 11;
    newNode.data.birthdate.sub.year = (seed % 60) + 1960;
    newNode.next = 0;

    return newNode;
}

void printWithOrderFromTillEnd(FILE *persistedData, long startItemIndex)
{
    fseek(persistedData, startItemIndex * sizeof(Node), SEEK_SET);
    Node nodeBuffer;

    //If there is no node different that root, table is empty. Dont print
    fread(&nodeBuffer, sizeof(Node), 1, persistedData);
    if  (nodeBuffer.next==0){
        return;
    }

    //Saved items read loop  
    do
    {
        fseek(persistedData, nodeBuffer.next, SEEK_SET);
        fread(&nodeBuffer, sizeof(Node), 1, persistedData);
        printf("Fullname : %s\n", nodeBuffer.data.szFullName);
        printf("Email : %s\n", nodeBuffer.data.szEmail);
        printf("Birth Date : %02d/%02d/%d , %d\n\n",
               nodeBuffer.data.birthdate.sub.day,
               nodeBuffer.data.birthdate.sub.month,
               nodeBuffer.data.birthdate.sub.year,
               nodeBuffer.data.birthdate.dateval);

    } while (nodeBuffer.next != 0);
}

//Gets last element of linked list
long iterateFilePointerToLastNode (FILE *persistedData){
    fseek(persistedData, sizeof(Node), SEEK_SET);
    long next;

    fread(&next, sizeof(long), 1, persistedData);
    if  (next==0){
        return 0;
    }

    long lastItemIndex;
    do
    {
        lastItemIndex=next;
        fseek(persistedData, next, SEEK_SET);
        fread(&next, sizeof(long), 1, persistedData);

    } while (next != 0);
    return lastItemIndex;
}

int main()
{

    FILE *persistedData = fopen("./user-table.deondb", "rb+");

    if(persistedData==NULL){
        persistedData= fopen("./user-table.deondb", "a");
        fclose(persistedData);
        persistedData = fopen("./user-table.deondb", "rb+");
    }

    fseek(persistedData, 0, SEEK_END);

    //I am creating root node with 0 values for preventing extra cases about prev node not exist problem if db is empty. I think this is most efficent way for this scenario
    if (ftell(persistedData) == 0)
    {
        Node rootCleanNode = {0};
        fwrite(&rootCleanNode, sizeof(Node), 1, persistedData);
    }

    
    long offsetIterator = iterateFilePointerToLastNode(persistedData);
    fseek(persistedData, 0, SEEK_END);

    // Chained seed data generation
    for (int i = 1; i <= 10; i++)
    {

        Node tempNewNode = generateNodeWithData(i);
        addNodeAfterTargetNode(persistedData, offsetIterator, tempNewNode);

        offsetIterator = ftell(persistedData) - sizeof(Node);
    }
    //Add seed data 16 after fourth node
    Node pTempNewNode;
    pTempNewNode = generateNodeWithData(16);
    addNodeAfterTargetNode(persistedData, sizeof(Node)*4, pTempNewNode);

        //Add seed data 17 after second node
    pTempNewNode = generateNodeWithData(17);
    addNodeAfterTargetNode(persistedData, sizeof(Node)*2, pTempNewNode);

    printWithOrderFromTillEnd(persistedData, 0);

    fclose(persistedData);
    return 0;
}
