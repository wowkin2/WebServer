#ifndef __MY_AUX_C__
#define __MY_AUX_C__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

/*******************************************************************************
@purpose: check if there is a folder by path 
@input: path
@return: 1 if folder; 0 if not
*******************************************************************************/
char isFolder(char *path)
{
    struct stat s;
    if( stat(path,&s) == 0 )
    {
        if( s.st_mode & S_IFDIR )
        {
            return 1;
            //it's a directory
        }
        else if( s.st_mode & S_IFREG )
        {
            //it's a file
        }
        else
        {
            //something else
        }
    }
    else
    {
        //error
    }
    return 0;
}
/*******************************************************************************
@purpose: convert type from string to unsigned integer (string must to start from number)
@input: string to be converted
@return: return converted unsigned integer number
*******************************************************************************/
unsigned int a2i(const char* string)
{
    unsigned int result = 0;
    if(!string) return 0;

    while(*string)
    {
        if( (*string < '0' || *string> '9') ) break;
        result = result * 10 + (*string - '0');
        string++;
    }
    return result;
}

/*******************************************************************************
@purpose: to reverse string
@input: string to be reversed
@return: reversed string
@comment: 
    The function reverse used above is implemented two pages earlier
    that next - 'itoa' (in book "The C Programming Language")
*******************************************************************************/
void reverse(char *s)
{
    int i, j;
    char c;

    for (i = 0, j = strlen(s)-1; i<j; i++, j--) 
    {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

/*******************************************************************************
@purpose: convert type from integer to string
@input: integer to be converted
@return: return string to buffer 's[]'
@comment: 
    The function itoa appeared in the first edition of Kernighan and Ritchie's 
    "The C Programming Language", on page 60
*******************************************************************************/
void itoa(int n, char s[])
{
    int i, sign;

    if ((sign = n) < 0)  // write sign
        n = -n;          // make positive
    i = 0;
    do              // generation cyphers in reverse
    {       
        s[i++] = n % 10 + '0';   // get next cypher
    } while ((n /= 10) > 0);     // remove it
    if (sign < 0)
        s[i++] = '-';
    s[i] = '\0';
    reverse(s);
}

/*******************************************************************************
@purpose: get file in single byte massive
@input: full file path
@return: pointer to byte massive with file content
*******************************************************************************/
/*char* getTempl(char *fileName){
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
}*/
/*
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
}*/

/*******************************************************************************
@purpose: generate time string
@return: return time in string W3C format
*******************************************************************************/
char* getTimeS(){
    char *tmpTime = (char*) malloc ( 30 );
    
    time_t rawtime;
    struct tm * timeinfo;

    time (&rawtime);
    timeinfo = localtime (&rawtime);
    strftime (tmpTime, 30, "%d/%h/%G:%T %z", timeinfo);
    return tmpTime;
}
#ifdef __DEBUG_MODE__
/*******************************************************************************
@purpose: Get command from file after each connection
@return: 0 if command == shutdown
*******************************************************************************/
int getComm(){
    FILE *ptrFileIn;
    char buff[255];
    ptrFileIn = fopen("bin/command.txt", "r");
    if (ptrFileIn == NULL) 
    {
        printf("Can't open command file!\n");
        return 1;
    }
    fgets(buff, 255, ptrFileIn);
    fclose(ptrFileIn);
    return strcmp(buff, "shutdown\n");
}
#endif
/*******************************************************************************
@purpose: get size of file
@input: string with file path
@return: return file size in bytes
*******************************************************************************/
unsigned long getFileSize(char *fileName){
    if (fileName == NULL) return -1;
    if (strlen(fileName) == 0) return -1;
    
    long fsize = 0;
    FILE *f = fopen(fileName, "rb");
    if (f == NULL) 
        return 0;
    fseek(f, 0, SEEK_END);
    fsize = ftell(f);
    fclose(f);
    return fsize;
}

/*******************************************************************************
@purpose: check if file exists
@input: string with file path
@return: return true(0) if exist and false(1) if not
*******************************************************************************/
int fileExists(const char *fpath)
{
    FILE *file;
    if (file = fopen(fpath, "r"))
    {
        fclose(file);
        return 0;
    }
    return 1;
}
/*******************************************************************************
@purpose: generate hash of string
@input: string
@return: unsigned long hash number 
*******************************************************************************/
unsigned long
getHash(unsigned char *str)
{
    unsigned long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}
#endif
