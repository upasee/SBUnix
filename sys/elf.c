#include <sys/elf.h>
#include <string.h>
#include <sys/sbunix.h>

void readElf(struct elfHdr *elf)
{

}

bool isElf(struct elfHdr *elf)
{
	if(elf->e_ident[0]==EI_MAG0 && elf->e_ident[1]==EI_MAG1 && elf->e_ident[2]==EI_MAG2 && elf->e_ident[3]==EI_MAG3)
	{
//		kprintf("Its an elf!\n");
		return true;
	}
	return false;
}

bool checkElfSize(char *size)
{
	uint64_t elf_size = (uint64_t)sizeof(struct elfHdr);
	uint64_t prog_size = (uint64_t)sizeof(struct progHdr);
	uint64_t bin_size = (uint64_t)atoi(size,10);
	if(elf_size + prog_size < bin_size)
		return true;
	return false;
}
