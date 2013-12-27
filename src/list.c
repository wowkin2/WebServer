/************  USING  **********
    List *lst = 0;
    Params a, *found;
    a.key=malloc(11);
    a.val=malloc(11);
    strcpy(a.key, "somekey");
    strcpy(a.val, "somevalue");
    
    myList = initList();
    insert(lst, &a);
    printAll(lst);
    found = getNode(lst, "portListen");
    puts(getNodeValue(lst, "portListen"));
    removeNode(lst, found); // found || lStart || lEnd
    removeAll(lst);
*******************************/
#ifndef __LIST_C_
#define __LIST_C_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LEN 256

typedef struct _node
{
    Params* data;
    struct _node* next;
}node;
typedef struct _List
{
    node* lStart;
    node* lEnd;
}List;

List *initList()
{
    return malloc(sizeof(List));
}
void insertNode(List* l, Params *element)
{
    node *lNew;
    
    lNew = (node *)malloc (sizeof(node));
    lNew->data = (Params *)malloc(sizeof(Params));
    memcpy(lNew->data, element, sizeof(Params));
    if (l->lStart == 0 || l->lEnd == 0) //init with first element
    {
        l->lStart = l->lEnd = lNew;
    }
    l->lEnd->next = lNew;
    lNew->next = l->lStart;
    l->lEnd = lNew;
}
inline void freeNode(node *elem)
{
    memset(elem->data->key, 0, strlen(elem->data->key));
    memset(elem->data->val, 0, strlen(elem->data->val));
    free(elem->data->key);
    free(elem->data->val);
    memset(elem, 0, sizeof(node));
    free(elem);
}
void removeNode(List* l, node* lElem)
{
    if (l->lStart == l->lEnd) // remove one single element
    {
        freeNode(lElem);
        l->lStart = l->lEnd = 0;
    }
    else if (l->lStart == lElem) //remove first element
    {
        l->lEnd->next = l->lStart->next;
        l->lStart = l->lStart->next;
        freeNode(lElem);
    }
    else if (l->lEnd == lElem) //remove last element
    {
        node *tmp = l->lStart;
        while(tmp->next != l->lEnd)
        {
            tmp = tmp->next;
        }
        tmp->next = l->lStart;
        freeNode(l->lEnd);
        l->lEnd = tmp;
    }
    else    //remove middle element
    {
        node *tmp = lElem->next;
        lElem->next = lElem->next->next;
        freeNode(tmp);

    }

}
void removeAll(List* l)
{
    while(l->lStart) removeNode(l, l->lStart);
}

node *getNode(List* l, char *key)
{
    node* list = l->lStart;
    if (list == 0)
    {
        return 0;
    }

    do
    {
        if (strcmp(list->data->key, key) == 0)
        {
            return list;
        }
        else
        {
            list = list->next;
        }
    }while(list != l->lStart);
    
    return 0;
}
char *getNodeValue(List* list, char *key)
{
    char *string = 0;
    node *tmpNode = getNode(list, key);
    if (tmpNode != 0)
    {
        string = malloc(strlen(tmpNode->data->val));
        strcpy(string, tmpNode->data->val);
        return string;
    }
    return 0;
}
void printAll(List* list)
{
    node* tmpList = list->lStart;
    if (tmpList == 0)
    {
        puts("List is empty");
        return;
    }
    do
    {
        printf("%s = %s\n", tmpList->data->key, tmpList->data->val);
        tmpList = tmpList->next;
    
    }while(tmpList != list->lStart);
}
/*
int main(void) 
{
    node *list;
    Params a;
    a.key = "11111";
    insert(a);
    insert(a);
    insert(a);
    insert(a);
    insert("22222");
    insert("33333");
    insert("44444");
    insert("55555");
    insert("12311");
    insert("12345");
    insert("");

    printAll();

    list = getMostDiff();
    if (list != 0)
    {
        printf("Most count of symbols: %d\t%s\n", getMaxSymbols(list->data), list->data );
        removeNode(list);
        printAll();
    }
    system("pause");
    return 0;
}*/
#endif
