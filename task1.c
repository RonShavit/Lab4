#include <stdlib.h>
#include <stdio.h>

short debugMode = 0;
char* menu_msg = "Choose action:\n";
menu_option* menu = NULL

struct typedef
{
    char* name;
    void* func;
} menu_option

void tglDebugMode()
{
    debugMode = 1-debugMode;
}

void setFileName()
{
    printf("NYI:set file name\n");
}

void ldIntoMem()
{
    printf("NYI: load into memoryn\n");
}

void tglDispMode()
{
    printf("NYI: toggle display mode\n");
}

void memDisplay()
{
    printf("NYI: memory display\n");
}

void saveInfoFile()
{
    printf("NYI: save info file\n");
}

void memModify()
{
    printf("NYI: memory modify");
}

void quit()
{
    if(menu!=NULL)
        free(menu);

    exit(0);
}