#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

unsigned int atoi(char * str, int base)
{
        int i, res = 0;
        for (i = 0; str[i] != '\0'; ++i)
        {
                if(str[i] == '\n')
                        break;
                if((str[i] >= 'a') && (str[i] <= 'f'))
                {
                        res = res*base+ str[i] + 10 - 'a';
                }
                else if((str[i] >= 'A') && (str[i] <= 'F'))
                {
                        res = res*base + str[i] + 10 - 'A';
                }
                else if((str[i] >= '0') && (str[i] <= '9'))
                {
                        res = res*base + str[i] - '0';
                }
                else
                        break;
        }
        return res;
}

void trim (char *str)
{
        int i=0;
        while(str[i] != '\n')
        {
                i++;
        }
        str[i] = '\0';
}

int scanf(const char *format, ...)
{
        int *d;
        unsigned int *x;
        va_list args;
        char *c, *s;
        const char *temp1;
        const char *temp2;
        s = (char *)malloc(20);
        temp1 = (const char *)malloc(10);
        temp2 = (const char*)malloc(10);
        c = (char *)malloc(2);
        va_start(args, format);
        while (*format)
        {
                if ('%' == *format)
                {
                        format ++;
                        switch (*format)
                        {
                                case 'd':
                                        d = va_arg(args, int*);
                                        read(0,(void *)temp1, sizeof(temp1));
                                        *d = atoi((char *)temp1,10);
                                        break;
                                case 'c':
                                        c = va_arg(args, char *);
                                        read(0, c, 1);
                                        break;
                                case 's':
                                        s = va_arg(args, char*);
                                        read(0, s, 20);
                                        trim(s);
                                        break;
                                case 'x':
                                        x = va_arg(args, unsigned int*);
                                        read(0, (void *)temp2, 2*sizeof(temp2));
                                        *x = atoi((char *)temp2,16);
                        }
                        format++;
                }
                else {
                        format++;
                }
        }
		va_end(args);

        return 0;
}

                                      
