#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

int printf(const char *format, ...)
{
        int d;
        char c;
	unsigned long x;
        char temp[100];
	char *temp1;
	char str1[100];
	char *s;
	int i=0,k=0;
        va_list args;
        va_start(args, format);
        while (*format)
        {
                if ('%' == *format)
                {
			format++;
			switch (*format)
                        {
                                case 'd':
                                        d = va_arg(args, int);
					itoa(d, 10, temp);
                                        while(temp[k] != '\0')
                                        {
                                                str1[i++]= temp[k++];
                                        }
					break;
				case 'x':
					x = va_arg(args, unsigned int);
					itoa(x,16,temp);
                                        while(temp[k] != '\0')
                                        {
                                                str1[i++] = temp[k++];
                                        }
                                        break;
				case 's':
                                        s = va_arg(args, char *);
                                        while(*s != '\0')
                                        {
                                                str1[i++] = *s++;
                                        }
                                        break;
				case 'c':
					c = va_arg(args, int);
                                        temp1 = &c;
                                        while(*temp1 != '\0')
                                        {
                                                str1[i++] = *temp1++;
                                        }
                                        break;
			}
			format ++;
		}
               else 
		{
			str1[i] = *format;
                        format++;
                        i++;
                }
        }
	str1[i]='\0';
	write(1, str1, strlen(str1));
        va_end(args);
        return 0;
}

