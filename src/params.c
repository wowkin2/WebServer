#ifndef __PARAMS_C__
#define __PARAMS_C__
#include "stdio.h"
#include "string.h"
#include "stdlib.h"

#define TEMP_BUFF_SIZE 255

typedef struct Map
{
    char *key;
    char *val;
}Params;
#include "list.c"           //this include must be after this typedef

/*******************************************************************************
@purpose: get integer parameter from list, on error return default value
@input: list, key for search, default value that returns if key is not set
@return: value by key from list
*******************************************************************************/
inline int getParamInt(List* list, char *key, int defValue)
{
    int tmp = a2i(getNodeValue(list, key));
    if (tmp == 0)
        return defValue;
    else
        return tmp;
}

/*******************************************************************************
@purpose: get string parameter from list, on error return default value
@input: list, key for search, default value that returns if key is not set
@return: value by key from list
*******************************************************************************/
inline char* getParamS(List* list, char *key, char* defValue)
{
    char *tmp = getNodeValue(list, key);
    if (tmp == 0)
        return defValue;
    else
        return tmp;
}

/*******************************************************************************
@purpose: get string parameter from file and set them to MAP(key, value)
@input: full file path, list to be set, delimiter between key and value in file
@return: 0 - if everything is OK; 1 - if can't open file; 2 - if file contain errors
*******************************************************************************/
int getParamsFromFile(const char*fileName, List *lst, char delimiter)
{
    FILE *ptrFileIn;
    char buff[TEMP_BUFF_SIZE] = {0};
    char *ptrDelim;
    unsigned int lineCnt = 0;

    ptrFileIn = fopen(fileName, "r");
    if (ptrFileIn == NULL) {
        char ErrBuff[256] = {0};
        snprintf(ErrBuff, sizeof(ErrBuff), 
                "Could not open '%s'!", fileName);
        puts(ErrBuff);
        logError(ErrBuff);
        return 1;
    }

    while (fgets(buff, TEMP_BUFF_SIZE, ptrFileIn))
    {
        if ( *buff != '#' && strlen(buff) > 2 )// String is not comment and not empty
        {
            ptrDelim = strchr(buff, delimiter);
            memset(buff + strlen(buff) - 1, 0, 1); //remove newline symbol at the EOL
            if (ptrDelim)
            {
                Params a;
                a.key = (char *)malloc(ptrDelim - buff);
                a.val = (char *)malloc(strlen(ptrDelim));
                memcpy(a.key, buff, (ptrDelim - buff));
                strcpy(a.val, ptrDelim+1);
                insertNode(lst, &a);
            }
            else
            {
                char ErrBuff[256] = {0};
                snprintf(ErrBuff, sizeof(ErrBuff), 
                        "There are errors in '%s' at line: %d", fileName, lineCnt + 1);
                puts("ERROR. Open 'log/error.log' to see more.");
                logError(ErrBuff);
                return 2;
            }
        }
        lineCnt++;
        memset(buff, 0, TEMP_BUFF_SIZE);
    }
    fclose(ptrFileIn);
    return 0;
}

#endif
