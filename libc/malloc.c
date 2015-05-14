#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <syscall.h>

#define RLIMIT_DATA        2

struct sblock{
	size_t size;
	struct sblock *next;
	int free;	
	void *ptr;
	char data[1];			
};

void *head = NULL;

struct sblock * findFreeBlock(struct sblock **last, size_t size)
{
	struct sblock *blk = head;
	while(blk && !(blk->free && blk->size >= size))
	{
		*last = blk;
		blk = blk->next;
	} 
	return blk;
}

uint64_t roundDown(void* addr, size_t size)
{
        uint64_t newaddr = (uint64_t)(((uint64_t)addr/size) * size);
        return newaddr;
}


uint64_t roundUp(void *addr, size_t size)
{
        uint64_t newaddr;
        if ((uint64_t)addr%size == 0)
                newaddr = (uint64_t )(roundDown(addr, size));
        else
                newaddr = (uint64_t )(roundDown(addr, size) + size);
        return newaddr;
}


void *malloc(size_t size)
{
	struct sblock *blk, *last;
	if (head)
		last = head;
	void *q = NULL;
//	size = roundUp((void *)size, 0x8);
	blk = findFreeBlock(&last, size);
	if(blk)
	{
		blk->free = 0;
		blk->ptr = (blk->data + 4);
		return blk->ptr;
	}
	else 
	{
		
		blk = (void *)sbrk(0);
		q = (void *)sbrk(sizeof(struct sblock) + size);
		if(q == (void *)-1)
		{
			return NULL;
		}
		blk->size = size;
		blk->free = 0;
		blk->next = NULL;
		blk->ptr = (blk->data + 4);
		return (blk->ptr);
	}
}

int brk(void *end_data_segment)
{
	void *addr;
	addr = (void *)syscall_1(SYS_brk, 0);
	if(/*(end_data_segment <= (void *)heap_limit->rlim_cur )&&*/ (end_data_segment >= addr))
	{
		addr = (void *)syscall_1(SYS_brk, (uint64_t)end_data_segment);
		return 0;
	}
	return -1;
	
}

void* sbrk(uintptr_t increment)
{
	void *p;
	int r;
	p = (void *)syscall_1(SYS_brk,0);
	void *addr = (void *)((uintptr_t)p + (uintptr_t)increment); 
	r = brk(addr);
	if(r < 0)
	{
		printf("sbrk failed\n");
	}
	return p;
}

struct sblock * validaddr(void *ptr)
{
       void *p;
       p = (void *)sbrk(0);
       if ((ptr > head) && ptr < p )
       {
               struct sblock *blk;
               blk = (struct sblock *)((uintptr_t)ptr - sizeof(struct sblock));
               return blk;
       }
       return NULL;
}

void free(void *ptr)
{
       struct sblock *sblk;
       if ((sblk = validaddr(ptr)) !=NULL)
       {
               if (sblk->ptr == ptr)
               {
                       sblk->free = 1;
                       sblk->ptr = NULL;
                       memset(sblk->ptr, '0', sblk->size);
               }
       }
}

