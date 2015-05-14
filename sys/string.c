#include<sys/string.h>
#include<sys/paging.h>
#include<sys/defs.h>

size_t strlen(const char*s)
{
        size_t count = 0, i=0;
        while(s[i] != '\0')
        {
                count++;
                i++;
        }
        return count;
}

int strncmp(const char *str1, const char *str2, size_t n)
{
        int i=0;
        while ((n > 0) && ((str1[i] != '\0') || (str2[i] != '\0')))
        {
                if (str1[i] > str2[i])
                        return 1;
                else if(str1[i] < str2[i])
                        return -1;
                i++;
                n--;
        }
        if (n == 0)
                return 0;
        return 0;
}

int strcmp(const char *str1, const char *str2)
{
        int i=0;
        while((str1[i]!='\0') || (str2[i]!='\0'))
        {
                if(str1[i] > str2[i])
                        return 1;
                if(str1[i] < str2[i])
                        return -1;
                i++;
        }
        return 0;
}

char* reverse_string(char str[])
{
        char temp;
        int len = strlen(str);
        int i;
        int k = len-1;
        for(i = 0; i < len-1; i++)
        {
                temp = str[k];
                str[k] = str[i];
                str[i] = temp;
                k--;
                if(k == ((len / 2)-1))
                {
                        break;
                }
        }
        return str;
}

char stritoa[400];
char* itoa(unsigned long num, int base)
{

        int i= 0;
        if (num == 0)
        {
		stritoa[0] = '0';
                stritoa[1] = '\0';
                return stritoa;
        }
        if (num < 0)
        {
                num = -num;
        }
        while (num != 0)
        {
                int rem = num % base;
                if (rem < 10)
                        stritoa[i] = (char)(((int)'0')+rem);
                else
                        stritoa[i] = (char)(((int)'a')+(rem-10));
                num = num/base;
                i++;
        }
        stritoa[i] = '\0';
        char *str1 = reverse_string(stritoa);
        return str1;
}

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

void *memset(void *s, int c, size_t n)
{
        unsigned char *p = s;
        while((n > 0) && s)
        {
                *p++ = c;
                *p++ = 0x7;
                n--;
        }
        return s;
}

void *memset1(void *s, int c, size_t n)
{
        unsigned char *p = s;
        while((n > 0) && s)
        {
                *p++ = c;
                n--;
        }
        return s;
}


char *strcpy(char*d, const char*s)
{
        int i=0;
        while(s[i]!='\0')
        {
                d[i] = s[i];
                i++;
        }
        d[i] = '\0';
        return d;
}

void memcpy(void *dest, volatile void *src, size_t size)
{
        char *csrc = (char *)src; 
        char *cdest = (char *)dest;
        while(size > 0)
        {
                 *cdest=*csrc;
                 cdest++;
                 csrc++;
                 size--;
        }
}

int tokenize(char str[], char *input[],char delim)
{
        int i=0,j=0,k=0;
        for(i=0;str[i]!='\0' && str[i] == delim;i++);

        for(;str[i]!='\0';)
        {
                k = 0;
                input[j]=(char*)kmalloc(20);
                while(str[i]!=delim && str[i]!='\0')
                {
                        input[j][k] = str[i];
                        i++;
                        k++;
                }
                if(str[i]!='\0')
                        i++;
		input[j][k]='\0';
                j++;
        }
        input[j]=NULL;
        return j;
}

