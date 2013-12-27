#ifndef __MY_AUX_C__
#define __MY_AUX_C__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> 

char* getTempl(char *fileName){
    if (fileName == 0) return 0;
    if (strlen(fileName) == 0) return 0;
    
    FILE *f = fopen(fileName, "rb");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *string = malloc(fsize + 1);
    fread(string, fsize, 1, f);
    fclose(f);

    string[fsize] = 0;
    return string;
}

int getTempl2(char *fileName, char **outBuff){
    if (fileName == 0) return 0;
    if (strlen(fileName) == 0) return 0;
    
    FILE *f = fopen(fileName, "rb");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    *outBuff = malloc(fsize);
    fread(*outBuff, 1, fsize, f);
    fclose(f);

    return fsize;
}
char* getTempl1(int connfd, char *fileName){
    if (fileName == 0) return 0;
    if (strlen(fileName) == 0) return 0;
    FILE *f = fopen(fileName, "rb");
    printf("HERE\n");
    fseek(f, 0, SEEK_END);
    printf("NOT HERE\n");
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *string = malloc(fsize + 1);
    fread(string, fsize, 1, f);
    write(connfd, string, fsize);
    fclose(f);

    string[fsize] = 0;
    return string;
}

/*********************
@purpose: generate time string
@return: return time in string W3C format
*********************/
char* getTimeS(){
    char *tmpTime = (char*) malloc ( 30 );
    
    time_t rawtime;
    struct tm * timeinfo;

    time (&rawtime);
    timeinfo = localtime (&rawtime);
    strftime (tmpTime, 30, "%d/%h/%G:%T %z", timeinfo);
    return tmpTime;
}
/*********************
@purpose: compare N symbols of two strings
@input: string1, string2 and N of symbols to be compared
@return: return -1 if equal
*********************/
int strcmpn(char *ptr1, char *ptr2, int n){
    int i;
    for (i = 0; i < n; i++)
       if (*ptr1 != *ptr2)
       {
           printf("%c!=%c\n", *ptr1, *ptr2);
           return 0;
       }
    return -1;
}
/*********************
@purpose: Get command from file after each connection
@return: 0 if command == shutdown
*********************/
int getComm(){
    FILE *ptrFileIn;
    char buff[255];
    ptrFileIn = fopen("command.txt", "r");
    if (ptrFileIn == NULL) 
    {
        printf("Can't open command file in!\n");
        return 1;
    }
    fgets(buff, 255, ptrFileIn);
    fclose(ptrFileIn);
    return strcmp(buff, "shutdown\n");
}

/*********************
@purpose: get size of file
@input: string with file path
@return: return file size in bytes
*********************/
unsigned long getFileSize(char *fileName){
    if (fileName == 0) return -1;
    if (strlen(fileName) == 0) return -1;
    FILE *f = 0;
    long fsize = 0;
    f = fopen(fileName, "rb");
    fseek(f, 0, SEEK_END);
    fsize = ftell(f);
    fclose(f);
    return fsize;
}

#endif
