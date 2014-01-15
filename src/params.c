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

char *action_input[] = {
    "--h",
    "--help",
    "--i",
    "--k",
  /*"",
    ""*/
    "--t",
};
typedef enum _ACTION{
    SHOW_HELP,
    SHOW_HELP_LONG,
    SHOW_INTERFACES,
    SERVER_START,
  /*SERVER_STOP,      //TODO
    SERVER_RESTART,*/
    CHECK_CONFIG,
    LAST_ACTION
}ACTION;
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
    char ErrBuff[256] = {0};

    ptrFileIn = fopen(fileName, "r");
    if (ptrFileIn == NULL) {
        snprintf(ErrBuff, sizeof(ErrBuff), 
                "Could not open '%s'!", fileName);
        puts(ErrBuff);
        logError(ErrBuff);
        return 1;
    }
    if (lst == NULL){
        puts("Error. List is not initialized");
        logError("Error. List is not initialized");
        return 3;
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
/*******************************************************************************
@purpose: get string parameter from file and set them to MAP(key, value)
@input: full file path, list to be set, delimiter between key and value in file
@return: 0 - if everything is OK; 1 - if can't open file; 2 - if file contain errors
*******************************************************************************/
int getParamsFromString(char *str, List *lst, char delimiter)
{
    char *buff = 0;
    char *ptrDelim = 0;
    unsigned int trimCnt = 0;

    if (lst == NULL){
        puts("Error. List is not initialized");
        logError("Error. List is not initialized");
        return 3;
    }
    if (str == NULL) {
        return 1;
    }
    buff = strtok(str, "\n");
    while(buff != NULL)
    {
        ptrDelim = strchr(buff, delimiter);
        if (ptrDelim)
        {
            trimCnt = 0;
            while(*(ptrDelim + 1 + trimCnt) == 32) trimCnt++; // L_TRIM
            Params a = {0};
            a.key = (char *)malloc(ptrDelim - buff + 1);
            a.val = (char *)malloc(strlen(ptrDelim) - trimCnt); // trimCnt for ltrim 'value'
            memcpy(a.key, buff, (ptrDelim - buff));
            *(a.key+(ptrDelim - buff)) = 0;
            strcpy(a.val, ptrDelim+1  + trimCnt); // to skip delimiter and spaces

            insertNode(lst, &a);
        }
        buff = strtok(NULL, "\n");
    }
    return 0;
}
/*******************************************************************************
@purpose: get command-line string parameters and choise one to execute
@input: command-line parameters
@return: number of action to be done
*******************************************************************************/
ACTION getStartupParams(int argc, char *argv[])
{
    int i;
    if (argc == 2)
    {
        for(i=1; i<LAST_ACTION; i++)
            if (strcmp(argv[1], action_input[i]) == 0)
                return i;
    }
    return SHOW_HELP;
}
#endif
