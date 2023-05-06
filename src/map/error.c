#include "wolfenstein.h"

//generic function to give info to user
//0 = log
//1 = warning
//2 = error
void log_string(char *message, int level)
{
    if(level == 0)
        printf("[LOG]: ");
    else if(level == 1)
        printf("[WARNING]: ");
    else if (level == 2)
        printf("[ERROR]: ");
    printf("%s\n",message);
}

//a function specifically made to free a char * and log an error
void free_and_warn(char ** str)
{
    log_string("Texture with same Identifier detected",1);
    free(*str);
    *str = NULL;
}
