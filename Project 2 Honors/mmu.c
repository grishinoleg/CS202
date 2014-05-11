#include <stdio.h>
#include <stdlib.h>
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


//This procedure is called when the simulation starts. It should
//allocate the R bit bitmap, M bit bitmap, and page frame bitmap (i.e.
//pageframe_bitmap). Each bitmap should be an array of words (unsigned ints), 
//where the number of elements of the array is num_page_frames/32 
//(on a 32-bit machine).
void mmu_initialize()
{
  //Fill this in. Make sure that, in the case where num_page_frames
  //is not divisible by 32, you round up the number of words used
  //to store the bits.
}


//This procedure sets the pframe'th bit of the R bit bitmap to val (0 or 1).
void mmu_modify_rbit_in_bitmap(PAGEFRAME_NUMBER pframe, int val)
{
  //Fill this in. Compute the word of the bitmap to be modified
  //and then use a bitwise operator to select the bit to be modified.
}

//This procedure returns the pframe'th bit (0 or 1) of the R bit bitmap.
int mmu_get_rbit_in_bitmap_value(PAGEFRAME_NUMBER pframe)
{
  //Fill this in. Compute the word of the bitmap to be selected
  //and then use a bitwise operator to select the bit to be read. Be
  //sure to return 0 or 1.
}


//This procedure clears all the bits in the R bit bitmap and
//calls tlb_clear_all_R_bits() to clear the R bits in the TLB.
void mmu_clear_rbits()
{
  //Fill this in
}   


//This procedure set the pframe'th bit of the M bit bitmap to val (0 or 1).
void mmu_modify_mbit_in_bitmap(PAGEFRAME_NUMBER pframe, int val)
{
  //Fill this in. Compute the word of the bitmap to be modified
  //and then use a bitwise operator to select the bit to be modified.
}


//This procedure returns the pframe'th bit (0 or 1) of the M bit bitmap.
int mmu_get_mbit_in_bitmap_value(PAGEFRAME_NUMBER pframe)
{
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
}


//This procedure returns the bit of the pageframe bitmap corresponding
//to the specified page frame.
unsigned int mmu_get_pageframe_bitmap_value(PAGEFRAME_NUMBER pframe)
{
  // Fill this in
}


// This sets the bit in the pageframe bitmap (indicating
// which page frames are occupied) corresponding
// to the specified page frame to the specified value (0 or 1)
void mmu_modify_pageframe_bitmap(PAGEFRAME_NUMBER pframe, int val) 
{
  // Fill this in
}


// This procedure is called from kernel.c. It returns a 
// free page frame, if there is one. It records 
// in the pageframe bitmap that the returned page frame is
// no longer available. If there is no free pageframe, 
// NO_FREE_PAGEFRAME (declared in mmu.h) is returned.
PAGEFRAME_NUMBER mmu_get_free_page_frame()
{
  //Fill this in
}

