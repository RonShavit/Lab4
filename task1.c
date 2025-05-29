#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>


char debug_mode = 0;
char file_name[128];
int unit_size = 1;
unsigned char mem_buf[10000];
size_t mem_count;

typedef struct menu_option
{
    char *name;
    void (*func)();
} menu_option;

menu_option *menu[9];
char *names[9] = {"toggle debug mode", "set file name", "set unit size", "load into memory", "toggle display mode", "memoty display", "save info file", "memory modify", "quit"};

void tglDebugMode()
{
    debug_mode = 1 - debug_mode;
}

void setFileName()
{
    printf("Enter file name:\n");
    fgets(file_name, 101, stdin);
    int i=0;
    while (i<128)
    {
        if (file_name[i] == '\n')
        {
            file_name[i] = '\0';
        }
        if(file_name[i] == '\0')
        {
            i = 130;
        }
        i++;

    }
    if (debug_mode)
    {
        printf("DEBUG : read file name - %s\n", file_name);
    }
}

void setUnitSize()
{
    char* buff = malloc(128);
    int newSize = 0;
    printf("Enter unit size:\n");
    fgets(buff, 128, stdin);
    newSize = atoi(buff);
    if (newSize == 1 || newSize == 2 || newSize ==4)
    {
        unit_size = newSize;
        if(debug_mode)
        {
            printf("DEBUG : set size to %d\n",unit_size);
        }
    }
    else
    {
        printf("ERROR : %s is invalid unit size\n",buff);
    }
    free(buff);


}

void ldIntoMem()
{
    int err = 0; // error checker
    char* buffer; //Str got from user
    char* second; //second number from input
    long loc = 0;
    long len = 0;

    if(file_name[0] == '\0')
    {
        printf("ERROR : No file name set\n");
    }
    else
    {
        err = open(file_name,O_RDWR);
        if(err == -1)
        {
            printf("ERROR : error while opening file %s\n",file_name);
            perror("err code");
        }
        else
        {
            printf("Please enter <location> <length>\n");
            buffer = malloc(128);
            buffer = fgets(buffer,128,stdin);
            loc = strtol(buffer,&second,16);
            len = strtol(second, NULL, 10);
            if(debug_mode)
            {
                fprintf(stderr, "Filename: %s\nlocation: %lX\nlength: %ld\n",file_name,loc,len);
            }

            lseek(err,loc,SEEK_SET);
            read(err, mem_buf,unit_size*len);

            printf("read %ld bytes from memory\n",len*unit_size);
            lseek(err,0,SEEK_SET);


        }
    }
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
    printf("NYI: memory modify\n");
}

void quit()
{
    printf("quitting\n");
    exit(0);
}

void *functions[9] = {tglDebugMode, setFileName, setUnitSize, ldIntoMem, tglDispMode, memDisplay, saveInfoFile, memModify, quit};

void makeMenu()
{
    for (int i = 0; i <= 8; i++)
    {
        menu[i] = malloc(sizeof(menu_option));
        menu[i]->name = names[i];
        menu[i]->func = functions[i];
    }
}

int checkIfZero(char *buffer)
{
    char check;
    if (buffer == NULL)
        return -1;
    else
    {
        while (1)
        {
            check = *buffer;
            printf("%c\n",check);
            if (check == '\0'||check == '\n')
                return 1;
            else if (check != '0')
            {
                return 0;
            }
            else
            {
                buffer++;
            }
        }
    }
}

int main(int argc, char **argv)
{
    char *buffer = malloc(128);
    int choosen;

    file_name[0] = '\0';

    makeMenu();
    printf("choose option:\n");
    for (int i = 0; i <= 8; i++)
    {
        printf("%d-%s\n", i, menu[i]->name);
    }
    while (1)
    {
        buffer = fgets(buffer, 128, stdin);
        if (buffer == NULL)
        {
            quit();
        }
        else
        {
            choosen = atoi(buffer);
            if (choosen == 0)
            {
                if (checkIfZero(buffer) < 1)
                    choosen = -1;
            }
            printf("%d\n",choosen);
            if (choosen < 0 || choosen > 8)
            {
                printf("Please enter valid option number\n");
            }
            else
            {
                int i = 0;
                while (i != choosen)
                {
                    i++;
                }
                (menu[i]->func)();
            }
        }
        printf("choose option:\n");
        for (int i = 0; i <= 8; i++)
        {
            printf("%d-%s\n", i, menu[i]->name);
        }
    }
}