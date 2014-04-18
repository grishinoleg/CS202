#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "tlb.h"
#include "cpu.h"
#include "mmu.h"


/* This is some of the code that I wrote. You may use any of this code
   you like, but you certainly don't have to.
*/


/* I defined the TLB as an array of entries,
   each containing the following:
   Valid bit: 1bit
   Virtual Page: 21 bits
   Reference bit: 1 bit
   Modified bit: 1 bit
   Page Frame: 21 bits
*/


typedef struct {
  unsigned int vbit_and_vpage;  // 32 bits containing the valid bit and the 21-bit
                                // virtual page number.
  unsigned int mr_pframe;       // 32 bits containing the modified bit, reference bit,
                                // and 21-bit page frame number
} TLB_ENTRY;


// This is the actual TLB array. It should be dynamically allocated
// to the right size, depending on the num_tlb_entries value 
// assigned when the simulation started running.


TLB_ENTRY *tlb;


// This is the TLB size (number of TLB entries) chosen by the 
// user. 

unsigned int num_tlb_entries;


BOOL tlb_miss;  //this is set to TRUE when there is a tlb miss;


#define VBIT_MASK   0x80000000  //VBIT is leftmost bit of first word
#define VPAGE_MASK  0x001FFFFF  //lowest 21 bits of first word
#define MBIT_MASK   0x80000000  //MBIT is leftmost bit of second word
#define RBIT_MASK   0x40000000  //RIT is second leftmost bit of second word
#define PFRAME_MASK 0x001FFFFF  //lowest 21 bits of second word


// Initialize the TLB (called by the mmu)

void tlb_initialize()
{
  //Here's how you can allocate a TLB of the right size
  tlb = (TLB_ENTRY *) malloc(num_tlb_entries * sizeof(TLB_ENTRY));

  //Fill in the rest here...

}

// This clears out the entire TLB, by clearing the
// valid bit for every entry.

void tlb_clear_all() 
{
  // Fill this in.
}


//clears all the R bits in the TLB
void tlb_clear_R_bits() 
{
  // Fill this in.
}


// This clears out the entry in the TLB for the specified
// virtual page, by clearing the valid bit for that entry.

void tlb_clear_entry(VPAGE_NUMBER vpage)
{
  // Fill this in.
}



// Returns a page frame number if there is a TLB hit. If there is a
// TLB miss, then it sets tlb_miss (see above) to TRUE. Otherwise, it
// sets tlb_miss to FALSE, sets the R bit of the entry and, if the
// specified operation is a STORE, sets the M bit.

PAGEFRAME_NUMBER tlb_lookup_vpage(VPAGE_NUMBER vpage, OPERATION op)
{
  // FILL THIS IN
}


// tlb_insert_vpage(), below, inserts a new mapping of
// vpage to pageframe into the TLB. When choosing which
// entry in the TLB to write to, it selects the first
// entry it encounters that either has a zero valid bit 
// or a zero R bit. To avoid choosing the same entries
// over and over, it starts searching the entries from
// where it left off the previous time (see the description
// below.

int next_vpage_to_check = 0;  // points to next TLB entry to consider 
                              // writing to.


void tlb_insert_vpage(VPAGE_NUMBER new_vpage, PAGEFRAME_NUMBER new_pframe,
		BOOL new_rbit, BOOL new_mbit)
{
  // Starting at tlb[next_vpage_to_check], choose the first entry
  // with either valid bit  = 0 or the R bit = 0 to write to. If there
  // is no such entry, then just choose tlb[next_vpage_to_check].

  // If the chosen entry has a valid bit = 1 (i.e. a valid entry is
  // being evicted), then write the M and R bits of the entry back
  // to the M and R bitmaps, respectively, in the MMU (see
  // mmu_modify_rbit_in_bitmap, etc. in mmu.h)

  // Then, insert the new vpage, pageframe, R bit, and M bit into the
  // TLB entry that was just found (and possibly evicted).

  // Finally, set next_vpage_to_check to point to the next entry after the
  // entry found above.
}


//Writes all the R and M bits of the valid entries of the
//TLB back to the page table.

void tlb_write_back_r_m_bits()
{
  //FILL THIS IN.
}

