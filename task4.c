#include <stdlib.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
    int counter = 0;//digit counter
    if(argc<2)
    {
        printf("please run using ./digit_counter <string>\n");
    }
    else
    {
        char* c = argv[1];
        while (*c!='\0')
        {
            if(*c>='0'&&*c<='9')
            {
                counter++;
            }
            c++;
        }
        printf("String contains %d digits, for sure this time\n",counter);
    }
}