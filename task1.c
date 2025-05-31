#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/param.h>

static char *hex_formats[] = {"%#hhx\n", "%#hx\n", "No such unit", "%#x\n"};
static char *dec_formats[] = {"%#hhd\n", "%#hd\n", "No such unit", "%#d\n"};
char debug_mode = 0;
char file_name[128];
int unit_size = 1;
unsigned char mem_buf[10000];
size_t mem_count;
char display_mode_dec = 0;
unsigned int mem_buf_size = 0;
unsigned long filesize;

typedef struct menu_option
{
    char *name;
    void (*func)();
} menu_option;

menu_option *menu[9];
char *names[9] = {"toggle debug mode", "set file name", "set unit size", "load into memory", "toggle display mode", "memory display", "save info file", "memory modify", "quit"};

void tglDebugMode()
{
    debug_mode = 1 - debug_mode;
}

void setFileName()
{
    printf("Enter file name:\n");
    fgets(file_name, 101, stdin);
    int i = 0;
    while (i < 128)
    {
        if (file_name[i] == '\n')
        {
            file_name[i] = '\0';
        }
        if (file_name[i] == '\0')
        {
            i = 130;
        }
        i++;
    }
    if (debug_mode)
    {
        printf("DEBUG : read file name - %s\n", file_name);
    }

    int op = open(file_name, O_RDONLY);
    if (op != -1)
    {
        lseek(op, 0, SEEK_SET);
        filesize = lseek(op, 0, SEEK_END);
        lseek(op, 0, SEEK_SET);
        close(op);
    }
    else
    {
        filesize = 0;
    }
    if (debug_mode)
    {
        printf("DEBUG : file %s size is %ld|%lX\n", file_name, filesize, filesize);
    }
}

void setUnitSize()
{
    char *buff = malloc(128);
    int newSize = 0;
    printf("Enter unit size:\n");
    fgets(buff, 128, stdin);
    newSize = atoi(buff);
    if (newSize == 1 || newSize == 2 || newSize == 4)
    {
        unit_size = newSize;
        if (debug_mode)
        {
            printf("DEBUG : set size to %d\n", unit_size);
        }
    }
    else
    {
        printf("ERROR : %s is invalid unit size\n", buff);
    }
    free(buff);
}

void ldIntoMem()
{
    int err = 0;  // error checker
    char *buffer; // Str got from user
    char *second; // second number from input
    long loc = 0;
    long len = 0;
    int readNum = 0;

    if (file_name[0] == '\0')
    {
        printf("ERROR : No file name set\n");
    }
    else
    {

        err = open(file_name, O_RDWR);
        if (err == -1)
        {
            printf("ERROR : error while opening file %s\n", file_name);
            perror("err code");
        }
        else
        {
            printf("Please enter <location> <length>\n");
            buffer = malloc(128);
            buffer = fgets(buffer, 128, stdin);
            loc = strtol(buffer, &second, 16);
            len = strtol(second, NULL, 10);
            if (debug_mode)
            {
                fprintf(stderr, "Filename: %s\nlocation: %lX\nlength: %ld\n", file_name, loc, len);
            }

            if (loc < len)
            {
                lseek(err, loc, SEEK_SET);
                readNum = read(err, mem_buf, unit_size * len);
                printf("%s\n", mem_buf);

                printf("read %d bytes from memory\n", readNum);
                mem_buf_size = readNum;
                lseek(err, 0, SEEK_SET);
            }
            else
            {
                printf("ERROR : Address 0x%lX is out of bound for file %s (0-0x%lX)\n", loc, file_name, filesize);
            }
            close(err);
        }
    }
}

void tglDispMode()
{
    display_mode_dec = 1 - display_mode_dec;
    if (display_mode_dec)
    {
        printf("Display flag now on, decimal representation\n");
    }
    else
    {
        printf("Display flag now off, hexadecimal representation\n");
    }
}

void memDisplay()
{
    char buffer[128];
    int add = 0;
    int len = 0;
    int data = 0;
    printf("enter address and length:\n");
    fgets(buffer, 128, stdin);
    sscanf(buffer, "%x %d", &add, &len);
    if (add == 0)
    {
        for (int i = 0; i < MIN(MIN(10000, len * unit_size), mem_buf_size); i += unit_size)
        {
            data = 0;
            for (int j = 0; j < unit_size; j++)
            {
                data *= 0x100;
                data += mem_buf[j + i];
            }
            if (display_mode_dec)
            {
                printf(dec_formats[unit_size - 1], data);
            }
            else
            {
                printf(hex_formats[unit_size - 1], data);
            }
        }
    }
    else
    {
        int openRes = open(file_name, O_RDONLY);
        if (openRes == -1)
        {
            printf("ERROR : error opening file %s\n", file_name);
        }
        else
        {
            lseek(openRes, add, SEEK_SET);
            char *rbuff = malloc(unit_size);

            for (int i = 0; i < len * unit_size; i += unit_size)
            {
                data = 0;
                read(openRes, rbuff, unit_size);
                for (int j = 0; j < unit_size; j++)
                {
                    data *= 0x100;
                    data += *(rbuff + j);
                }
                if (display_mode_dec)
                {
                    printf(dec_formats[unit_size - 1], data);
                }
                else
                {
                    printf(hex_formats[unit_size - 1], data);
                }
            }
            lseek(openRes, 0, SEEK_SET);
            close(openRes);
        }
    }
}

void saveInfoFile()
{
    int source = 0;
    int tar = 0;
    int len = 0;
    char readbuf[128];
    char *rwbuff;
    int written = -1;
    printf("Please enter <source-address> <target-location> <length>\n");
    fgets(readbuf, 128, stdin);
    sscanf(readbuf, "%x %x %d", &source, &tar, &len);
    int ope = open(file_name, O_RDWR);
    if (ope == -1)
    {
        printf("ERROR : error opening file %s\n", file_name);
    }
    else
    {
        if (filesize <= source)
        {
            printf("ERROR : Source address 0x%X is out of bound for file %s (0-0x%lX)\n", source, file_name, filesize);
        }
        else
        {
            if (source != 0)
            {
                rwbuff = malloc(len * unit_size);
                int rSize = read(ope, rwbuff, len * unit_size);
                lseek(ope, tar, SEEK_SET);
                written = write(ope, rwbuff, rSize);
                printf("written %s\n", rwbuff);
                free(rwbuff);
                lseek(ope, 0, SEEK_SET);
            }
            else
            {
                lseek(ope, tar, SEEK_SET);
                written = write(ope, mem_buf, MIN(len * unit_size, mem_buf_size));
                printf("written %s fom mem\n", mem_buf);
                lseek(ope, 0, SEEK_SET);
            }
        }
        close(ope);
    }
}

void memModify()
{
    int loc = 0;
    int val = 0;
    char input[128];
    printf("Please enter <location> <val>\n");
    fgets(input, 128, stdin);
    sscanf(input, "%x %x", &loc, &val);
    if(loc+unit_size<10000)
    {
        for (int i = unit_size; i > 0; i--)
        {
            char data = val % 0x100;
            mem_buf[i - 1] = data;
            val /= 0x100;
        }
    }
}

void quit()
{
    for (int i = 0; i <= 8; i++)
    {
        free(menu[i]);
    }
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

            if (check == '\0' || check == '\n')
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