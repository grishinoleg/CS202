#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "tlb.h"
#include "mmu.h"
#include "cpu.h"
#include "page.h"

//This is used to keep track of how many
//TLB misses there are.
unsigned int tlb_miss_count;

// The MMU keeps bitmaps for the M and R bits of each pageframe.
// This is the bitmap of the R bits, one bit per pageframe
unsigned int *rbit_bitmap;

// This is the bitmap of the M bits, one bit per pageframe
unsigned int *mbit_bitmap;

// This serves as the bitmap of the "present" bits, one
// bit per pageframe. If the N'th bit of this bitmap is 1,
// it means that the N'th page frame holds a virtual page.
// Otherwise, it means that the N'th page frame is empty.
unsigned int *pageframe_bitmap;

#define VPAGE_MASK  0xFFFFF800  // highest 21 bits
#define OFFSET_MASK 0x000007FF
#define MY_VERBOSE 0 // my verbose for printing additional info
#define MY_DEEP_VERBOSE 0

unsigned int bit_size;
unsigned int num_page;

//This procedure is called when the simulation starts. It should
//allocate the R bit bitmap, M bit bitmap, and page frame bitmap (i.e.
//pageframe_bitmap). Each bitmap should be an array of words (unsigned ints),
//where the number of elements of the array is num_page_frames/32
//(on a 32-bit machine).
void mmu_initialize()
{
  if (MY_VERBOSE)
    printf("Entered mmu_initialize\n");

  num_page = num_page_frames;

  // rounding to closest divisible by 32
  if (num_page % 32 != 0)
  {
    num_page += (32 - num_page % 32);
  }

  bit_size = (unsigned int) num_page_frames / 32;

  rbit_bitmap = (unsigned int *) malloc (bit_size * sizeof(unsigned int));
  mbit_bitmap = (unsigned int *) malloc (bit_size * sizeof(unsigned int));
  pageframe_bitmap = (unsigned int *) malloc (bit_size * sizeof(unsigned int));

  //Fill this in. Make sure that, in the case where num_page_frames
  //is not divisible by 32, you round up the number of words used
  //to store the bits.

  tlb_initialize();
  pt_initialize_page_table();

  if (MY_VERBOSE)
    printf("Leaving mmu_initialize\n");

}


//This procedure sets the pframe'th bit of the R bit bitmap to val (0 or 1).
void mmu_modify_rbit_in_bitmap(PAGEFRAME_NUMBER pframe, int val)
{
  if (MY_DEEP_VERBOSE)
    printf("Entered mmu_modify_rbit_in_bitmap\n");

  // casting is going to round the number
  unsigned int pf_num = (unsigned int) pframe / 32;
  // unsigned int pf_offset = 31 - (pframe % 32);
  unsigned int pf_offset = pframe % 32;
  if (val)
  {
    rbit_bitmap[pf_num] |= (1 << pf_offset);
  }
  else
  {
    rbit_bitmap[pf_num] &= ~(1 << pf_offset);
  }
  //Fill this in. Compute the word of the bitmap to be modified
  //and then use a bitwise operator to select the bit to be modified.

  if (MY_DEEP_VERBOSE)
    printf("Leaving mmu_modify_rbit_in_bitmap\n");

}

//This procedure returns the pframe'th bit (0 or 1) of the R bit bitmap.
int mmu_get_rbit_in_bitmap_value(PAGEFRAME_NUMBER pframe)
{
  if (MY_DEEP_VERBOSE)
    printf("Entered mmu_get_rbit_in_bitmap_value\n");

  // casting is going to round the number
  unsigned int pf_num = (unsigned int) pframe / 32;
  // unsigned int pf_offset = 31 - (pframe % 32);
  unsigned int pf_offset = pframe % 32;
  int bit = rbit_bitmap[pf_num] & (1 << pf_offset);

  if (MY_DEEP_VERBOSE)
    printf("Leaving mmu_get_rbit_in_bitmap_value\n");

  return bit ? 1 : 0;

  //Fill this in. Compute the word of the bitmap to be selected
  //and then use a bitwise operator to select the bit to be read. Be
  //sure to return 0 or 1.
}


//This procedure clears all the bits in the R bit bitmap and
//calls tlb_clear_all_R_bits() to clear the R bits in the TLB.
void mmu_clear_rbits()
{
  if (MY_VERBOSE)
    printf("Entered mmu_clear_rbits\n");

  memset(rbit_bitmap, 0, bit_size * sizeof(unsigned int));
  tlb_clear_R_bits();

  if (MY_VERBOSE)
    printf("Leaving mmu_clear_rbits\n");
}


//This procedure set the pframe'th bit of the M bit bitmap to val (0 or 1).
void mmu_modify_mbit_in_bitmap(PAGEFRAME_NUMBER pframe, int val)
{
  if (MY_DEEP_VERBOSE)
    printf("Entered mmu_modify_mbit_in_bitmap\n");

  // casting is going to round the number
  unsigned int pf_num = (unsigned int) pframe / 32;
  // unsigned int pf_offset = 31 - (pframe % 32);
  unsigned int pf_offset = pframe % 32;
  if (val)
  {
    mbit_bitmap[pf_num] |= (1 << pf_offset);
  }
  else
  {
    mbit_bitmap[pf_num] &= ~(1 << pf_offset);
  }
  //Fill this in. Compute the word of the bitmap to be modified
  //and then use a bitwise operator to select the bit to be modified.

  if (MY_DEEP_VERBOSE)
    printf("Leaving mmu_modify_mbit_in_bitmap\n");

}


//This procedure returns the pframe'th bit (0 or 1) of the M bit bitmap.
int mmu_get_mbit_in_bitmap_value(PAGEFRAME_NUMBER pframe)
{
  if (MY_DEEP_VERBOSE)
    printf("Entered mmu_get_mbit_in_bitmap_value\n");

  // casting is going to round the number
  unsigned int pf_num = (unsigned int) pframe / 32;
  // unsigned int pf_offset = 31 - (pframe % 32);
  unsigned int pf_offset = pframe % 32;
  int bit = mbit_bitmap[pf_num] & (1 << pf_offset);

  if (MY_DEEP_VERBOSE)
    printf("Leaving mmu_get_mbit_in_bitmap_value\n");

  return bit ? 1 : 0;

  //Fill this in. Compute the word of the bitmap to be selected
  //and then use a bitwise operator to select the bit to be read. Be
  //sure to return 0 or 1.
}



//This procedure, given a virtual address and an operation
//(LOAD or STORE), returns the corresponding physical address.
ADDRESS mmu_translate(ADDRESS vaddress, OPERATION op)
{
  //First, call tlb_lookup on the virtual page to get the
  //page frame. If there is a TLB hit (i.e. if tlb_miss is
  //false), then return the physical address constructed from
  //the page frame and the offset.

  //Otherwise (i.e. TLB miss has occurred): increment tlb_miss_count,
  //call pt_get_pageframe() to look up the pageframe corresponding to
  //the virtual page in the page table. If there is a hit
  //(i.e. page_fault is false), then call tlb_insert() to insert an
  //entry for the virtual page in the TLB, and then return the
  //physical address constructed from the page frame and the offset.

  //Otherwise (i.e. a page fault has occurred), call tlb_write_back()
  //to write the M and R bits from the TLB back to the bitmaps and
  //call issue_page_fault_trap() (declared in cpu.h) to trap to the OS
  //to handle the page fault.

  if (MY_VERBOSE)
    printf("Entered mmu_translate with vaddress %u and op %u\n", vaddress, op);

  PAGEFRAME_NUMBER pf_num = tlb_lookup_vpage
    ((vaddress & VPAGE_MASK) >> 11, op);

  if (MY_DEEP_VERBOSE)
    printf("Got pf_num\n");

  unsigned int pf_offset = (vaddress & OFFSET_MASK);

  if (MY_DEEP_VERBOSE)
    printf("Calculated pf_offset\n");

  if (tlb_miss)
  {
    tlb_miss_count++;

    if (MY_DEEP_VERBOSE)
      printf("Getting pf_num after tlb_miss\n");

    pf_num = pt_get_pframe_number((vaddress & VPAGE_MASK) >> 11);

    if (page_fault)
    {
      if (MY_DEEP_VERBOSE)
        printf("Calling tlb_write_back_r_m_bits after\
          page_fault and tlb_miss\n");

      tlb_write_back_r_m_bits();

      if (MY_DEEP_VERBOSE)
        printf("Calling issue_page_fault_trap after page_fault and tlb_miss\n");

      issue_page_fault_trap((vaddress & VPAGE_MASK) >> 11);

    }
    else
    {
      if (MY_DEEP_VERBOSE)
        printf("Calling tlb_insert_vpage\n");

      tlb_insert_vpage((vaddress & VPAGE_MASK) >> 11, pf_num,
        mmu_get_rbit_in_bitmap_value(pf_num),
        mmu_get_mbit_in_bitmap_value(pf_num));
      // Return physical address

      if (MY_VERBOSE)
        printf("Leaving (in !page_fault) mmu_translate\n");

      return ((pf_num << 11) | pf_offset);
    }
  }
  else
  {
    // Return physical address

    if (MY_VERBOSE)
      printf("Leaving (in !tlb_miss) mmu_translate\n");

    return ((pf_num << 11) | pf_offset);
  }
}


//This procedure returns the bit of the pageframe bitmap corresponding
//to the specified page frame.
unsigned int mmu_get_pageframe_bitmap_value(PAGEFRAME_NUMBER pframe)
{
  if (MY_DEEP_VERBOSE)
    printf("Entered mmu_get_pageframe_bitmap_value\n");

  // casting is going to round the number
  unsigned int pf_num = (unsigned int) pframe / 32;
  // unsigned int pf_offset = 31 - (pframe % 32);
  unsigned int pf_offset = pframe % 32;
  int bit = pageframe_bitmap[pf_num] & (1 << pf_offset);

  if (MY_DEEP_VERBOSE)
    printf("Leaving mmu_get_pageframe_bitmap_value\n");

  return bit ? 1 : 0;
}


// This sets the bit in the pageframe bitmap (indicating
// which page frames are occupied) corresponding
// to the specified page frame to the specified value (0 or 1)
void mmu_modify_pageframe_bitmap(PAGEFRAME_NUMBER pframe, int val)
{
  if (MY_DEEP_VERBOSE)
    printf("Entered mmu_modify_pageframe_bitmap\n");

  // casting is going to round the number
  unsigned int pf_num = (unsigned int) pframe / 32;
  // unsigned int pf_offset = 31 - (pframe % 32);
  unsigned int pf_offset = pframe % 32;
  if (val)
  {
    pageframe_bitmap[pf_num] |= (1 << pf_offset);
  }
  else
  {
    pageframe_bitmap[pf_num] &= ~(1 << pf_offset);
  }

  if (MY_DEEP_VERBOSE)
    printf("Leaving mmu_modify_pageframe_bitmap\n");

}


// This procedure is called from kernel.c. It returns a
// free page frame, if there is one. It records
// in the pageframe bitmap that the returned page frame is
// no longer available. If there is no free pageframe,
// NO_FREE_PAGEFRAME (declared in mmu.h) is returned.
PAGEFRAME_NUMBER mmu_get_free_page_frame()
{
  if (MY_VERBOSE)
    printf("Entered mmu_get_free_page_frame\n");

  int i, j;
  unsigned int pframe;

  for (i = 0; i < bit_size; i++)
  {
    if (pageframe_bitmap[i] != 0xFFFFFFFF)
    {
      for (j = 31; j >= 0; j--)
      {
        pframe = i * 32 + j;

        if (!mmu_get_pageframe_bitmap_value(pframe))
        {
          mmu_modify_pageframe_bitmap(pframe, 1);

          if (MY_VERBOSE)
            printf("Leaving (found free page frame %u)\
              mmu_get_free_page_frame\n", pframe);

          return pframe;
        }
      }
    }
  }

  if (MY_VERBOSE)
    printf("Leaving (NO_FREE_PAGEFRAME) mmu_get_free_page_frame\n");

  return NO_FREE_PAGEFRAME;
}
