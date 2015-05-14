#include<sys/sbunix.h>
#include<stdarg.h>
#include<sys/string.h>
#include<sys/defs.h>

#define CONSOLE_START 0xffffffff800b8000

#define MAX_ROWS        24
#define MAX_COLS        80

static uint64_t offset = 0;
static int row = 0;
static int col = 0;

void update_cursor()
{
        offset = (80*row) + col;
}

void scroll(int colour, char *string)
{
	update_cursor();
	volatile char *video = (volatile char*)(CONSOLE_START + ((uintptr_t)offset)*2);
	while(*string != 0)
	{
		*video++ = *string++;
        	*video++ = colour;
	}
	row++;
}

void write_string(int colour, const char *string, int size)
{
        int length = strlen(string);
        if (length > 2000)
                return;
        update_cursor();
	int m =0;
        volatile char *video = (volatile char*)(CONSOLE_START + ((uintptr_t)offset)*2);
        while( *string != 0  && m < size)
        {
		m++;
                if (*string == '\n')
                {
                        col = 0;
			if (row < MAX_ROWS-1)
                        	row++;
			else
			{
				memcpy((void *)0xFFFFFFFF800B8000, (void *)0xFFFFFFFF800B80A0, 3680);
				memset((void *)0xFFFFFFFF800B8E60, ' ', 80);
				col = 0;
				row = MAX_ROWS -1;
			}
                        update_cursor();
                        string++;
                        video = (volatile char*)(CONSOLE_START + ((uintptr_t)offset)*2);
			continue;
                }

                else if (*string == '\r')
                {
                        col = 0;
                        update_cursor();
                        string++;
                        video = (volatile char*)(CONSOLE_START + ((uintptr_t)offset)*2);
			continue;
                }
		else if(*string == '\b')
		{
			video = video-2;
			*video++ = ' ';
			*video++ = colour;
			offset = offset-1;
			col = col- 1;
			video = (volatile char*)(CONSOLE_START + ((uintptr_t)offset)*2);
			string++;
			
		}
		else if (*string == '\t')
                {
                        col = col+4;
                        update_cursor();
                        string++;
                        video = (volatile char*)(CONSOLE_START + ((uintptr_t)offset)*2);

                }
                if (*string == '\0')
                        break;
                *video++ = *string++;
		*video++ = colour;
		if ((row == MAX_ROWS-1) && (col == MAX_COLS - 1))
		{
			int i=0;
			while(i < 1000000)
                                {
                                        i++;
                                }
			//memcpy((void *)0xB8000, (void *)0xB80A0, 3680);
			memcpy((void *)0xFFFFFFFF800B8000, (void *)0xFFFFFFFF800B80A0, 3680);
			//memset((void *)0xB8E60, ' ', 80);
			memset((void *)0xFFFFFFFF800B8E60, ' ', 80);
			col =0;
			row = MAX_ROWS -1;
			update_cursor();
		}
                if (col == MAX_COLS-1)
                {
                        col = 0;
                        row++;
                }
                else
                        col++;
	
        }
}

void kprintf(const char *format, ...)
{
	int d;
        char *s;
        char  c; 
        void *p = NULL;
        const char *temp1, *temp2, *temp3, *temp4;
        unsigned long x;
        va_list args;
        va_start(args, format);
	char str1[4096];
        int i=0;
	
	while (*format)
        {
                if ('%' == *format)
                {
                        format ++;
                        switch (*format)
                        {
                                case 'd':
                                        d = va_arg(args, int);
                                        temp1 = (const char *)itoa(d, 10);
                                        while(*temp1 != '\0')
					{
                                                str1[i++]= *temp1++;
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
                                        temp2 = &c;
					str1[i++] = *temp2++;
                                        break;

                                case 'x':
                                        x = va_arg(args, unsigned int);
                                        temp3 = (const char *)itoa(x,16);
                                        while(*temp3 != '\0')
					{
                                                str1[i++] = *temp3++;
					}
                                        break;
                                case 'p':
                                        p = va_arg(args, void *);
                                        temp4 = (const char *)itoa((unsigned long)p,16);
                                        str1[i++] = '0';
                                        str1[i++] = 'x';
                                        while(*temp4 != '\0')
					{
                                                str1[i++] = *temp4++;
					}
                                        break;
                        }
                        format++;
                }
		else 
		{
                        str1[i] = *format;
                        format++;
                        i++;
		}
        }
        str1[i] = '\0';
        write_string(7, (const char*)str1, strlen(str1));
        va_end(args);
}
