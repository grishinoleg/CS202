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

// Pointer to the current ready queue entry

PID_QUEUE_ELT *ready_queue_entry;


typedef struct {
  PID_QUEUE_ELT *head;
  PID_QUEUE_ELT *tail;
} PID_QUEUE;

//pointer to the ready queue
PID_QUEUE *ready_queue;

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

int active_processes;

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

  // Populate interrupt table
  INTERRUPT_TABLE[TRAP] = handle_trap;
  INTERRUPT_TABLE[CLOCK_INTERRUPT] = handle_clock_interrupt;
  INTERRUPT_TABLE[DISK_INTERRUPT] = handle_disk_interrupt;
  INTERRUPT_TABLE[KEYBOARD_INTERRUPT] =  handle_keyboard_interrupt;

  // Put first process into the process table
  process_table[current_pid].state = RUNNING;
  process_table[current_pid].total_CPU_time_used = 0;

  // Malloc the ready queue
  // ready_queue_element = (PID_QUEUE_ELT *) malloc(sizeof(PID_QUEUE_ELT));
  ready_queue = (PID_QUEUE *) malloc(sizeof(PID_QUEUE));

  // // Init the ready queue
  // ready_queue_element->next = NULL;
  // ready_queue_element->pid = current_pid;
  ready_queue->head = NULL;
  ready_queue->tail = NULL;

  // Initialize current quantum time
  current_quantum_start_time = clock;
  active_processes = 1;

}

void handle_trap()
{
  // Switch that handles what kind of trap occured
  switch (R1)
  {
    case DISK_READ:
      // handle_disk_read();
      break;
    case DISK_WRITE:
      disk_write_req(current_pid);
      printf("Time %d: Process %d issues disk write request\n",
        clock, current_pid);
      break;
    // case KEYBOARD_READ:
    //
    //   break;
    case FORK_PROGRAM:
      handle_fork();
      break;
    case END_PROGRAM:
      handle_kill();
      break;
    case SEMAPHORE_OP:
      handle_semaphore();
  }
}

void handle_fork()
{
  // Update process table with the new process
  process_table[R2].state = READY;
  process_table[R2].total_CPU_time_used = 0;

  // STDOUT for creating new process
  printf("Time %d: Creating process entry for pid %d\n", clock, R2);

  // Put new process to the ready queue. Malloc the new node
  enqueue(R2);

  active_processes++;
}

void handle_kill()
{
  // Update process table (total time gets rewritten for each new process)
  process_table[current_pid].state = UNINITIALIZED;
  active_processes--;

  // STDOUT for killing process
  printf("Time %d: Process %d exits. Total CPU time = %d\n", clock, current_pid,
    process_table[current_pid].total_CPU_time_used);

  // Start the process on the ready queue
  schedule();

}

void handle_semaphore()
{

}

void handle_clock_interrupt()
{
  process_table[current_pid].total_CPU_time_used += CLOCK_INTERRUPT_PERIOD;
  if (clock - current_quantum_start_time == 40)
  {
    // Reschedule the process
    enqueue(current_pid);

    // Schedule new process
    schedule();

    current_quantum_start_time = clock;
  }
}

void handle_disk_interrupt()
{
  enqueue(R1);
  printf("Time %d: Handled DISK_INTERRUPT for pid %d\n", clock, R1);
}

void handle_keyboard_interrupt()
{

}

void handle_disk_read()
{
  disk_read_req(current_pid, R2);

  process_table[current_pid].state = BLOCKED;

  schedule();

  printf("Time %d: Process %d issues disk read request\n",
    clock, current_pid);
}

void schedule()
{
  // if (ready_queue->head == NULL)
  if (!active_processes)
  {
    printf("-- No more processes to execute --\n");
    exit(0);
  }
  current_pid = ready_queue->head->pid;
  process_table[current_pid].state = RUNNING;
  ready_queue->head = ready_queue->head->next;
  printf("Time %d: Process %d runs\n", clock, current_pid);
}

void enqueue(PID_type pid)
{
  PID_QUEUE_ELT *new_ready_queue_element = (PID_QUEUE_ELT *)
    malloc(sizeof(PID_QUEUE_ELT));
  new_ready_queue_element->next = NULL;
  new_ready_queue_element->pid = pid;
  if (ready_queue->head == NULL)
    ready_queue->head = new_ready_queue_element;
  if (ready_queue->tail != NULL)
    ready_queue->tail->next = new_ready_queue_element;
  ready_queue->tail = new_ready_queue_element;
  process_table[pid].state = READY;
}
