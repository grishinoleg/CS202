#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "mmu.h"
#include "page.h"
#include "cpu.h"

/* The following machine parameters are being used:

   Number of bits in an address:  32
   Page size: 2KB (i.e. 2^11 bytes)
   Number of pages = 4GB address space/ 2KB page size =  2^32/2^11 = 2^21 = 2M pages
   Page Table Type:  2 level page table
   Size of first level page table: 2048 (i.e. 2^11) entries
   Size of first level Page Table Entry:  32 bits
   Size of each 2nd Level Page Table: 1024 (i.e. 2^10) entries
   Size of 2nd Level Page Table Entry: 32 bits

   Bits of address giving index into first level page table: 11
   Bits of address giving index into second level page table: 10
   Bits of address giving offset into page: 11

*/

#define PAGES_1     2048       // Number of pages in level 1
#define PAGES_2     1024       // Number of pages in level 2
#define PT_1        0x001FFC00 // Mask to index first PT
#define PT_2        0x000003FF // Mask to index second PT
#define PRES_BIT    0x80000000 // Present bit
#define PAGE_FRAME  0x001FFFFF // Page frame


/* Each entry of a 2nd level page table has
   the following:
   Present/Absent bit: 1 bit
   Page Frame: 21 bits
*/


// This is the type definition for the
// an entry in a second level page table

typedef unsigned int PT_ENTRY;


// This is the declaration of the variable, first_level_page_table,
// representing the first level page table.

PT_ENTRY **first_level_page_table;



// This sets up the initial page table. The function
// is called by the MMU.
//
// Initially, all the entries of the first level
// page table should be set to NULL. Later on,
// when a new page is referenced by the CPU, the
// second level page table for storing the entry
// for that new page should be created if it doesn't
// exist already.

void pt_initialize_page_table()
{
  first_level_page_table = (PT_ENTRY **) malloc (sizeof(PT_ENTRY *) * PAGES_1);
  int i;
  for (i = 0; i<PAGES_1; i++)
  {
    first_level_page_table[i] = NULL;
  }
}


BOOL page_fault;  //set to true if there is a page fault


// This procedure is called by the MMU when there is a TLB miss.
// The vpage contains 21 significant bits (11 bits to index into first PT,
// then 10 bits to index into second PT  -- it is not an entire 32-bit
// physical address). It should return the corresponding page frame
// number, if there is one.
// It should set page_fault to TRUE if there is a page fault, otherwise
// it should set page_fault to FALSE.

PAGEFRAME_NUMBER pt_get_pframe_number(VPAGE_NUMBER vpage)
{
    unsigned int i1 = (vpage & PT_1) >> 10, i2 = vpage & PT_2;
    if (first_level_page_table[i1] != NULL)
    {
      if (first_level_page_table[i1][i2] & PRES_BIT)
      {
        page_fault = FALSE;
        return (first_level_page_table[i1][i2] & PAGE_FRAME);
      }
      else
      {
        page_fault = TRUE;
      }
    }
    else
    {
      page_fault = TRUE;
    }
    return 0;
}




// This inserts into the page table an entry mapping of the
// the specified virtual page to the specified page frame.
// It might require the creation of a second-level page table
// to hold the entry, if it doesn't already exist.

void pt_update_pagetable(VPAGE_NUMBER vpage, PAGEFRAME_NUMBER pframe)
{
  unsigned int i1 = (vpage & PT_1) >> 10, i2 = vpage & PT_2;

  if (first_level_page_table[i1] == NULL)
  {
    first_level_page_table[i1] = (PT_ENTRY *) malloc(sizeof(PT_ENTRY) * PAGES_2);
  }

  first_level_page_table[i1][i2] = (pframe | PRES_BIT);

  //don't forget to set the present bit for the new entry
}


// This clears the entry of a page table by clearing the present bit.
// It is called by the OS (in kernel.c) when a page is evicted from memory

void pt_clear_page_table_entry(VPAGE_NUMBER vpage)
{
  unsigned int i1 = (vpage & PT_1) >> 10, i2 = vpage & PT_2;
  first_level_page_table[i1][i2] &= (~PRES_BIT);

}
