#include "wolfenstein.h"
#include <stdarg.h>

//a function specifically made to free a char * and log an error
void free_and_warn(char ** str)
{
    log_string(1, 1, "Texture with same Identifier detected");
    free(*str);
    *str = NULL;
}

//generic function to give info to user
//0 = log
//1 = warning
//2 = error
void log_string(int level,int amount, ...)
{
    va_list args;

    if(level == 0)
        printf("[LOG]: ");
    else if(level == 1)
        printf("[WARNING]: ");
    else if (level == 2)
        printf("[ERROR]: ");

    va_start(args,amount);
    for (int i = 0; i < amount; i++)
    {
        char *str = va_arg(args,char *);
        printf("%s ",str);
    }
    printf("\n");
    va_end(args);
}
