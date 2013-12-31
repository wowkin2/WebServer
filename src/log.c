#ifndef __LOG_C__
#define __LOG_C__
static FILE* fileLogAccess;
static FILE* fileLogError;

/*******************************************************************************
@purpose: open file for appending in next functions
@input: full path and file name
@return: pointer to file
*******************************************************************************/
FILE* initErrorLog(char *path)
{
    fileLogError = fopen(path, "a");
    if (fileLogError == 0 || *path == 0)
    {
        fileLogError = fopen("error.log", "a");
        if (fileLogError != 0)
        {
            printf("Can't open any '%s' file...\n", path);
        }
        else
        {
            puts("Can't open even 'error.log' file...");
        }
    }
    return fileLogError;
}
/*******************************************************************************
@purpose: log each error or just system message in server work
@input: message to be written in file
*******************************************************************************/
void logError(char *Caption){
    char sendBuff[BUFF_LEN] = {0};
    if (fileLogError == 0)
    {
        if (initErrorLog(NULL) == 0)
        {
            printf("There was an error, but we can't write it to log:\n%s\n", Caption);
            return;
        }
    }
    //FILE *fileLogError = fopen("log/error.log", "a+");
    fseek(fileLogError, 0, SEEK_END);
    
    memset(sendBuff, 0, sizeof(sendBuff));
    snprintf(sendBuff, sizeof(sendBuff), "[%s] \"%s\"\n", getTimeS(), Caption);
    fwrite (sendBuff , sizeof(char), strlen(sendBuff), fileLogError);
    
    //fclose(fileLogError);
}
/*******************************************************************************
@purpose: log each connection to server
@input: IP of connection; request to be logged
*******************************************************************************/
void logAccess(char *IP, Request *req){
    char sendBuff[BUFF_LEN] = {0};
    
    //FILE *fileLogAccess = fopen("log/access.log", "a+");
    fseek(fileLogAccess, 0, SEEK_END);
    
    memset(sendBuff, 0, sizeof(sendBuff));
    snprintf(sendBuff, sizeof(sendBuff), 
            "%s [%s] \"%s %s %s\" %d  %d\n", 
            IP, getTimeS(), 
            RequestMethodText[req->method], req->URL, HTTPVersionText[req->version], 
            ResponseCode[req->status], 0/*strlen(req->data)*/
            );
            
    fwrite (sendBuff , sizeof(char), strlen(sendBuff), fileLogAccess);
    
    //fclose(fptr);
}
#endif
