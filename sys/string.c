#include<string.h>
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


void memcpy(volatile char *dst, volatile char *src, size_t size)
{

	while(size > 0)
	{
		*dst=*src;
		dst++;
		src++;
		size--;
	}
	*dst='\0';
}
