#ifndef _STRING_H
#define _STRING_H
#include<stdlib.h>
char* strcpy(char*,const char*);
size_t strlen(const char*);
char *strcat(char *, const char *);
int strcmp(const char *, const char *);
char *strncpy(char *, const char *, size_t);
int strncmp(const char *, const char *, size_t);
void *memset(void *, int, size_t);
void *memset1(void *, int, size_t);
int tokenize(char str[], char *input[],char delim);
char *itoa(unsigned long num, int base);
unsigned int atoi(char * str, int base);
char * reverse_string(char str[]);
//void memcpy(volatile char * dst, volatile char *src, size_t len);
void memcpy(void * dst, volatile void *src, size_t len);
#endif
