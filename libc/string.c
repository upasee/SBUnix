#include<string.h>
#include<stdio.h>
#include<stdlib.h>
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

char *strcat(char *dest, const char *src)
{
        int i=0, j=0;
        while(dest[i] != '\0')
        {
                i++;
        }
        while(src[j] != '\0')
        {
                dest[i] = src[j];
                i++;
                j++;
        }
        dest[i] = '\0';
        return dest;
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

char *strncpy(char *dest, const char *src, size_t n)
{
	int i=0;
	while((n > 0) && src[i] != '\0')
	{
		dest[i] = src[i];
		i++;
		n--;
	}
	dest[i] = '\0';
	return dest;
}

size_t strlen(const char*s)
{
	size_t i=0;
	while(s[i] != '\0')
	{
		i++;
	}
	return i;
}

void *memset(void *s, int c, size_t n)
{
	unsigned char *p = s;
	while((n > 0) && s)
	{
		*p = c;
		p++;
		n--;
	}
	return s;
}

/* tokenize */

int tokenize(char str[], char *input[],char delim)
{
        int i=0,j=0,k=0;
        for(i=0;str[i]!='\0' && str[i] == delim;i++);

        for(;str[i]!='\0';)
        {
                k = 0;
                input[j]=(char*)malloc(60);
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

void itoa(unsigned long num, int base, char *stritoa)
{
        int i= 0;
        if (num == 0)
        {
                stritoa[0] = '0';
                stritoa[1] = '\0';
		return;
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
        reverse_string(stritoa);
}

void reverse_string(char str[])
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
}

void remove_slash_before(char *s)
{
        int i=0;
        if(s[0]=='/')
        {
		if(s[1] == '\0')
			return;
                i++;
                while(s[i]!='\0')
                {
                        s[i-1]=s[i];
                        i++;
                }
                s[i-1]='\0';
        }
}

