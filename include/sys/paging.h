#ifndef _PAGING_H
#define _PAGING_H

#include <sys/defs.h>
#include <stdlib.h>

struct Page
{
        struct Page *nextPage;
        int refcount;
};


struct smap_t {
                uint64_t base, length;
                uint32_t type;
        }__attribute__((packed)) *smap;


uint64_t roundDown(void *addr);
uint64_t roundUp(void *addr);
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
void middle_backtrack(struct Page *middlePage, uint64_t *upper_dir,void *va);
void upper_backtrack(struct Page *upperPage, uint64_t *global_dir, void *va);
uint64_t* page_walk(uint64_t *global_dir, void* va);
void vm_setup(uint64_t *global_dir, int npages);
int markKernelUsed();



// Page table/directory entry flags.
#define PTE_P           0x001   // Present
#define PTE_W           0x002   // Writeable
#define PTE_U           0x004   // User
#define PTE_PWT         0x008   // Write-Through
#define PTE_PCD         0x010   // Cache-Disable
#define PTE_A           0x020   // Accessed
#define PTE_D           0x040   // Dirty
#define PTE_PS          0x080   // Page Size
#define PTE_MBZ         0x180   // Bits must be zero

// The PTE_AVAIL bits aren't used by the kernel or interpreted by the
// hardware, so user processes are allowed to set them arbitrarily.
#define PTE_AVAIL       0xE00   // Available for software use



#endif



