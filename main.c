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
    USER_MODEL data;
    struct Node *next;
} Node;

void addNodeAfterTargetNode(Node *prevNode, Node *newNode)
{
    newNode->next = prevNode->next;
    prevNode->next = newNode;
}

Node *generateNodeWithData(int seed)
{
    Node *pNewNode = (Node *)malloc(sizeof(Node));
    sprintf(pNewNode->data.szFullName, "Fullname_%d", seed);
    sprintf(pNewNode->data.szEmail, "Email_%d", seed);
    pNewNode->data.birthdate.sub.day = seed % 29;
    pNewNode->data.birthdate.sub.month = seed % 11;
    pNewNode->data.birthdate.sub.year = (seed % 60) + 1960;
    pNewNode->next = NULL;

    return pNewNode;
}

void printFromTillEnd(Node *startNode)
{
    Node *pItaretor = startNode;
    int index = 0;
    do
    {
        printf("Fullname : %s\n", pItaretor->data.szFullName);
        printf("Email : %s\n", pItaretor->data.szEmail);
        printf("Birth Date : %02d/%02d/%d , %d\n",
               pItaretor->data.birthdate.sub.day,
               pItaretor->data.birthdate.sub.month,
               pItaretor->data.birthdate.sub.year,
               pItaretor->data.birthdate.dateval);

        index++;
        pItaretor=pItaretor->next;
    } while (pItaretor->next != NULL);
}

int main()
{
    struct Node *head = generateNodeWithData(0);


    Node *pItaretor = head;
    for (int i = 1; i <= 10; i++)
    {

        Node* pTempNewNode= generateNodeWithData(i);
        
        addNodeAfterTargetNode(pItaretor, pTempNewNode);
        pItaretor=pTempNewNode;
    }
    printFromTillEnd(head);

    /*
    FILE *sourceFile = fopen("./8k.jpg", "rb");
    FILE *copyFile = fopen("./8kcopy.jpg", "wb");
    int iterator = fgetc(sourceFile);
    while (iterator != EOF)
    {
        fputc(iterator, copyFile);
        iterator = fgetc(sourceFile);
    }

    fclose(sourceFile);
    fclose(copyFile); */

    return 0;
}
