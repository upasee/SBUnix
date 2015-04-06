#include<sys/paging.h>
#include<string.h>
#include<sys/defs.h>
#include<sys/sbunix.h>

struct Page *pagefreelist;

uint64_t kernmem1 = 0xffffffff80200000;
uint64_t kernbase = 0xffffffff80000000;
uint64_t placement_address = 0xffffffff80000000;

uint64_t roundDown(void* addr)
{
        uint64_t newaddr = (uint64_t)(((uint64_t)addr/0x1000) * 0x1000);
        return newaddr;
}

uint64_t roundUp(void *addr)
{
	uint64_t newaddr;
	if ((uint64_t)addr%0x1000 == 0)	
	        newaddr = (uint64_t )(roundDown(addr));
	else
        	newaddr = (uint64_t )(roundDown(addr) + 0x1000);
        return newaddr;
}

uint64_t* kmalloc(uint64_t sz)
{
        placement_address = roundUp((void *)placement_address);
        uint64_t* tmp = (uint64_t *)placement_address;
        placement_address += (uint64_t)sz;
        return tmp;
}

void* physicalAddr(void *virtualAddr)
{
        return (void *)(virtualAddr - (void *)kernbase);
}

static size_t noOfPages;

int countPages(struct smap_t *smap)
{
        void *startAddr = (void *)roundUp((void *)smap->base);
        void *endAddr = (void *)roundDown((void *)(smap->base + smap->length));
        noOfPages = noOfPages + (endAddr - startAddr)/0x1000;
	return 0;
}

size_t kern_pages(void *physbase, void *physfree)
{
        uint64_t size = (uint64_t)(physfree - physbase);
	return size;
}


uint64_t * pml4_init(void *physfree)
{
	placement_address = placement_address + (uint64_t)physfree;
        uint64_t *pml4 = kmalloc(0x1000);
        return pml4;
}

//static size_t numpages = 0x8000000/0x1000;
static size_t numpages = 0x7ffe000/0x1000;
static struct Page *pages;

int setPageVal()
{
        pages = (struct Page *)kmalloc((sizeof(struct Page) * numpages));
	physicalAddr(pages);
        int i;
        for (i=0; i<numpages; i++)
        {
                pages[i].refcount = 1;
        }
        return 0;

}

int markKernelUsed()
{
	
	int i;
	for (i=0; i< numpages; i++)
        {
                if((physAddrFromPage(&pages[i]) >= kernmem1) && (physAddrFromPage(&pages[i]) <= placement_address))
                {
                        pages[i].refcount = 1;
                }
        }
	return 0;

}

int markAvailablePages(struct smap_t *smap)
{
        void *startAddr = (void *)roundUp((void *)smap->base);
        void *endAddr = (void *)roundDown((void *)(smap->base + smap->length));
        size_t nPages = (endAddr - startAddr)/0x1000;
        noOfPages = noOfPages + nPages;

        int i=0;
        for (i=0; i< numpages; i++)
        {
                if(((void *)physAddrFromPage(&pages[i]) >= startAddr) && ((void *)physAddrFromPage(&pages[i]) <= endAddr))
                {
                        pages[i].refcount = 0;
                }
        }
        return 0;
}


int createPageFreeList()
{
        int i=0;
        for (i=0; i<numpages; i++)
        {
                if (pages[i].refcount == 0)
                {
                        pages[i].nextPage = pagefreelist;
                        pagefreelist = &pages[i];
                }
        }
	return 0;
}

// bits 39-47 of pml4
uint64_t pml4_index(void* va)
{
	return ((((uint64_t)va) >> 39) & 0x1ff);
}

// bits 30-38 of pml4
uint64_t pdp_index(void* va)
{
        return ((((uint64_t)va) >> 30) & 0x1ff);
}

// bits 21-29 of pml4
uint64_t pgdir_index(void* va)
{
        return ((((uint64_t)va) >> 21) & 0x1ff);
}

// bits 12-20 of pml4
uint64_t pt_index(void* va)
{
        return ((((uint64_t)va) >> 12) & 0x1ff);
}

uint64_t physAddrFromPage(struct Page *p)
{
	return ((uint64_t)(p-pages)*0x1000);	
}


struct Page* phys_page_alloc()
{
	struct Page *result = pagefreelist;
	if(result == NULL)
		return NULL;
	uint64_t *virt = (uint64_t*)(physAddrFromPage(result)+(uint64_t)kernbase);	
	memset1(virt,'\0',4096);
	pagefreelist = result->nextPage;
	result->nextPage = NULL;
	return result;
}

void phys_page_free(struct Page *p)
{
	if(p->refcount == 0)
	{
		p->nextPage = pagefreelist;
		pagefreelist = p;
	}	
}

void pgdir_backtrack(struct Page *pgdirPage, uint64_t *pdp, void *va)
{
	pgdirPage->refcount--;
	phys_page_free(pgdirPage);
	pdp[pdp_index(va)]=0;
}

void pdp_backtrack(struct Page *pdpPage, uint64_t *pml4, void *va)
{
        pdpPage->refcount--;
        phys_page_free(pdpPage);
        pml4[pml4_index(va)]=0;
}

uint64_t* page_walk(uint64_t *pml4, void* va)
{
        uint64_t pdp_phys_addr = (uint64_t)*(pml4 + pml4_index(va));
        struct Page *pdpPage = NULL;
	struct Page *pgdirPage = NULL;
	struct Page *ptPage = NULL;
       
	//If the page is not present in upper level, create it
        if(pdp_phys_addr == 0)
        {
                pdpPage = phys_page_alloc();
                if(!pdpPage)
		{
			return NULL;
		}
		pdpPage->refcount++;
                pml4[pml4_index(va)] = physAddrFromPage(pdpPage);
		pdp_phys_addr = (uint64_t)pml4[pml4_index(va)];
		pml4[pml4_index(va)] = pml4[pml4_index(va)]|PTE_P|PTE_W;
        }

	
	uint64_t *pdp = (uint64_t *)((pdp_phys_addr & 0xfffffffffffff000) +kernbase);

        uint64_t pgdir_phys_addr = (uint64_t)*(pdp + pdp_index(va));
	
	//If the page is not present in middle level, create it	
	if(pgdir_phys_addr == 0)
        {
                pgdirPage = phys_page_alloc();
                if(!pgdirPage)
		{
			pdp_backtrack(pdpPage,pml4,va);
                        return NULL;
		}
                pgdirPage->refcount++;
                pdp[pdp_index(va)] = physAddrFromPage(pgdirPage);
                pgdir_phys_addr = (uint64_t)pdp[pdp_index(va)];
                pdp[pdp_index(va)] = pdp[pdp_index(va)]|PTE_P|PTE_W;
        }
	
        uint64_t *pgdir = (uint64_t*)((pgdir_phys_addr & 0xfffffffffffff000)+kernbase);
	uint64_t pt_phys_addr = (uint64_t)*(pgdir + pgdir_index(va));

        //If the page is not present in table level, create it  
        if(pt_phys_addr == 0)
        {
                ptPage = phys_page_alloc();
                if(!ptPage)
                {
			pgdir_backtrack(pgdirPage, pdp,va);
			pdp_backtrack(pdpPage, pml4,va);
			return NULL;
		}
                ptPage->refcount++;
                pgdir[pgdir_index(va)] = physAddrFromPage(ptPage);
                pt_phys_addr = (uint64_t)pgdir[pgdir_index(va)];
                pgdir[pgdir_index(va)] = pgdir[pgdir_index(va)]|PTE_P|PTE_W;
        }

        uint64_t *table = (uint64_t *)((pt_phys_addr & 0xfffffffffffff000) +kernbase);
	return table+pt_index(va);	
}


void vm_setup(uint64_t *pml4, int npages)
{
	memset1(pml4, 0, 4096);
	uint64_t *table;
	uint64_t va = roundUp((void *)kernbase);
	uint64_t pa = 0;
	int i;
	for(i=0;i<numpages;i++)
	{
		table = page_walk(pml4,(void *)va);
		*table = (pa + i*4096);
		*table = *table | PTE_W | PTE_P;
		va += 4096;
	}
}

