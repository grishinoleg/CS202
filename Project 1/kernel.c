#include <stdio.h>
#include <stdlib.h>

#include "hardware.h"
#include "drivers.h"
#include "kernel.h"


/* You may use the definitions below, if you are so inclined, to
   define the process table entries. Feel free to use your own
   definitions, though. */

typedef enum { RUNNING, READY, BLOCKED , UNINITIALIZED } PROCESS_STATE;


typedef struct process_table_entry {
  PROCESS_STATE state;
  int total_CPU_time_used;
} PROCESS_TABLE_ENTRY;

PROCESS_TABLE_ENTRY process_table[MAX_NUMBER_OF_PROCESSES];



/* Since you will have a ready queue as well as a queue for each semaphore,
   where each queue contains PIDs, here are two structure definitions you can
   use, if you like, to implement a single queue.  
   In this case, a queue is a linked list with a head pointer 
   and a tail pointer. */


typedef struct PID_queue_elt {
  struct PID_queue_elt *next;
  PID_type pid;
} PID_QUEUE_ELT;


typedef struct {
  PID_QUEUE_ELT *head;
  PID_QUEUE_ELT *tail;
} PID_QUEUE;


/* This constant defines the number of semaphores that your code should
   support */

#define NUMBER_OF_SEMAPHORES 16

/* This is the initial integer value for each semaphore (remember that
   each semaphore should have a queue associated with it, too). */

#define INITIAL_SEMAPHORE_VALUE 1


/* A quantum is 40 ms */

#define QUANTUM 40


/* This variable can be used to store the current value of the clock
   when a process starts its quantum. Later on, when an interrupt
   (of any kind) occurs, if the difference between the current time
   and the quantum start time is greater or equal to QUANTUM (40), 
   then the current process has used up its quantum. */

int current_quantum_start_time;



/* This procedure is automatically called when the 
   (simulated) machine boots up */

void initialize_kernel()
{
  // Put any initialization code you want here.
  // Remember, the process 0 will automatically be
  // executed after initialization (and current_pid
  // will automatically be set to 0), 
  // so the your process table should initially reflect 
  // that fact.

  // Don't forget to populate the interrupt table
  // (see hardware.h) with the interrupt handlers
  // that you are writing in this file.

  // Also, be sure to initialize the semaphores as well
  // as the current_quantum_start_time variable.

}

