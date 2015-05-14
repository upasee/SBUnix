#ifndef _PAGING_H
#define _PAGING_H

#include <sys/defs.h>
#include <stdlib.h>

// Page table/directory entry flags.
#define PTE_P           0x001   // Present
#define PTE_W           0x002   // Writeable
#define PTE_U           0x004   // User
#define PTE_D           0x040   // Dirty
#define PTE_COW         0x800   // Copy-On-Write

#define KERNBASE 	0xffffffff80000000
#define PTE_NP 		0xfffffffffffff000
#define PTE_NW          0xfffffffffffffffd
#define PTE_NCOW        0xfffffffffffff7ff

struct Page
{
        struct Page *nextPage;
        int refcount;
};

struct smap_t {
                uint64_t base, length;
                uint32_t type;
        }__attribute__((packed)) *smap;


struct Page* PageFromPhysAddr(uint64_t physAddr);
uint64_t roundDown(void *addr, size_t size);
uint64_t roundUp(void *addr, size_t size);
uint64_t *kmalloc(uint64_t sz);
void *physicalAddr(void *virtualAddr);
size_t kern_pages(void *physbase, void *physfree);
uint64_t * pml4_init(void *physfree);
int setPageVal();
int markAvailablePages(struct smap_t *smap);
int createPageFreeList();
uint64_t pml4_index(void* va);
uint64_t pdp_index(void* va);
uint64_t pgdir_index(void* va);
uint64_t pt_index(void* va);
uint64_t physAddrFromPage(struct Page *p);
struct Page* phys_page_alloc();
void phys_page_free(struct Page *p);
uint64_t* page_walk(uint64_t *global_dir, void* va, bool createFlag, int perm);
void vm_setup(uint64_t *global_dir, int npages);
int markKernelUsed();

#endif



