#include <stdio.h>
#include <stdlib.h>

#include "hardware.h"
#include "drivers.h"
#include "kernel.h"

// Everything that should have been in the header file:

// Handles behavior for TRAP signal

void handle_trap();

// Invoked when a TRAP is a disk read

void handle_disk_read();

// Invoked when a TRAP is a keyboard read

void handle_keyboard();

// Invoked when a TRAP is a fork

void handle_fork();

// Invoked when a TRAP is a kill

void handle_kill();

// Invoked when a TRAP is a semaphore signal

void handle_semaphore();

// Handles a clock interrupt

void handle_clock_interrupt();

// Handles a disk interrupt

void handle_disk_interrupt();

// Handles a keyboard interrupt

void handle_keyboard_interrupt();

// Next two methods can be used for both semaphore and process queues
// Moving declarations here makes it easier

typedef struct PID_queue_elt {
  struct PID_queue_elt *next;
  PID_type pid;
} PID_QUEUE_ELT;

typedef struct {
  PID_QUEUE_ELT *head;
  PID_QUEUE_ELT *tail;
} PID_QUEUE;

// Schedules a process to run now

void schedule(int queue_num);

// Put a process at the end of the queue

void enqueue(PID_QUEUE *queue, PID_type pid);

typedef enum { RUNNING, READY, BLOCKED , UNINITIALIZED } PROCESS_STATE;


typedef struct process_table_entry {
  PROCESS_STATE state;
  int total_CPU_time_used;
  int priority;
} PROCESS_TABLE_ENTRY;

// Designated initializer for array semaphores (just in case) so random
// values aren't put there

PROCESS_TABLE_ENTRY process_table[MAX_NUMBER_OF_PROCESSES] =
  {[0 ... MAX_NUMBER_OF_PROCESSES-1] = { UNINITIALIZED, 0, 0}};

// Pointer to the current ready queue entry

PID_QUEUE_ELT *ready_queue_entry;

// Pointer to the ready queue array

PID_QUEUE ready_queues[5] = {[0 ... 4] = {NULL, NULL}};

// Semaphore struct

typedef struct {
  PID_QUEUE *ready_queue;
  int value;
} SEMAPHORE;

/* This constant defines the number of semaphores that your code should
   support */

#define NUMBER_OF_SEMAPHORES 16

/* This is the initial integer value for each semaphore (remember that
   each semaphore should have a queue associated with it, too). */

#define INITIAL_SEMAPHORE_VALUE 1

// Designated initializer for array semaphores (just in case) so random
// values aren't put there

SEMAPHORE semaphores[NUMBER_OF_SEMAPHORES] = {[0 ... NUMBER_OF_SEMAPHORES-1]
    = { NULL, 1}};

/* A quantum is 40 ms */

#define QUANTUM 40


/* This variable can be used to store the current value of the clock
   when a process starts its quantum. Later on, when an interrupt
   (of any kind) occurs, if the difference between the current time
   and the quantum start time is greater or equal to QUANTUM (40),
   then the current process has used up its quantum. */

int current_quantum_start_time;

// Counter to keep track of how many active process there are at the moment

int active_processes;

// Counter to check for deadlocked system (stores number of current IO blocks)

int io_processes;

/* This procedure is automatically called when the
   (simulated) machine boots up */

void initialize_kernel()
{
  // Populate interrupt table

  INTERRUPT_TABLE[TRAP] = handle_trap;
  INTERRUPT_TABLE[CLOCK_INTERRUPT] = handle_clock_interrupt;
  INTERRUPT_TABLE[DISK_INTERRUPT] = handle_disk_interrupt;
  INTERRUPT_TABLE[KEYBOARD_INTERRUPT] =  handle_keyboard_interrupt;

  // Put first process into the process table

  process_table[current_pid].state = RUNNING;

  // Initialize current quantum time and counters

  current_quantum_start_time = clock;
  active_processes = 1;
  io_processes = 0;

}

void handle_trap()
{
  // Switch that handles what kind of trap occured

  switch (R1)
  {
    case DISK_READ:
      handle_disk_read();
      break;
    case DISK_WRITE:
      disk_write_req(current_pid);
      printf("Time %d: Process %d issues disk write request\n",
        clock, current_pid);
      break;
    case KEYBOARD_READ:
      handle_keyboard();
      break;
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

void handle_disk_read()
{
  printf("Time %d: Process %d issues disk read request\n",
    clock, current_pid);

  // Mark the process as blocked in the table

  process_table[current_pid].state = BLOCKED;

  if (clock - current_quantum_start_time < QUANTUM)
  {
    if (process_table[current_pid].priority < 4)
      process_table[current_pid].priority++;
  }

  // Put request and update all the necessary counters

  disk_read_req(current_pid, R2);
  io_processes++;

  process_table[current_pid].total_CPU_time_used +=
    (clock - current_quantum_start_time);
  current_quantum_start_time = clock;

  // Schedule a process (since the current one gets blocked)

  schedule(4);
}

void handle_keyboard()
{
  printf("Time %d: Process %d issues keyboard read request\n",
    clock, current_pid);

  // Mark the process as blocked in the table

  process_table[current_pid].state = BLOCKED;

  // Put request and update all the necessary counters

  if (clock - current_quantum_start_time < QUANTUM)
  {
    if (process_table[current_pid].priority < 4)
      process_table[current_pid].priority++;
  }

  keyboard_read_req(current_pid);
  io_processes++;

  process_table[current_pid].total_CPU_time_used +=
    (clock - current_quantum_start_time);
  current_quantum_start_time = clock;

  // Schedule a process (since the current one gets blocked)

  schedule(4);
}

void handle_fork()
{
  printf("Time %d: Creating process entry for pid %d\n", clock, R2);

  // Update process table with the new process; update counters

  process_table[R2].state = READY;
  active_processes++;

  // Put new process to the ready queue.
  // Malloc the new node (hence passing address)

  enqueue(&ready_queues[process_table[R2].priority], R2);

}

void handle_kill()
{
  // Update process table and counter

  process_table[current_pid].state = UNINITIALIZED;
  process_table[current_pid].total_CPU_time_used +=
    (clock - current_quantum_start_time);
  process_table[current_pid].priority = 0;
  active_processes--;

  //STDOUT kill process message (after updating table since total time changes)
  printf("Time %d: Process %d exits. Total CPU time = %d\n", clock, current_pid,
    process_table[current_pid].total_CPU_time_used);

  // Start the process on the ready queue
  current_quantum_start_time = clock;
  schedule(4);

}

void handle_semaphore()
{
  // Temporary variable for semaphore (for simplicity)

  SEMAPHORE *sem = &semaphores[R2];

  if (R3) // UP
  {
    printf("Time %d: Process %d issues UP operation on semaphore %d\n", clock,
      current_pid, R2);

    // Check if the semaphore value is 0; check for NULL pointers (nothing in
    // the queue or it wasn't initialized, which is done by enqueu())

    if (!sem->value && (sem->ready_queue != NULL) &&
      (sem->ready_queue->head != NULL))
    {
      // Get new pid; update the table; put it in the ready queue; rm 1st node

      PID_type pid = sem->ready_queue->head->pid;
      process_table[pid].state = READY;
      enqueue(&ready_queues[process_table[pid].priority], pid);
      sem->ready_queue->head = sem->ready_queue->head->next;
    }
    else
    {
      sem->value++;
    }
  }
  else // DOWN
  {
    printf("Time %d: Process %d issues DOWN operation on semaphore %d\n", clock,
      current_pid, R2);

    // Check for non-zero value

    if (sem->value)
    {
      sem->value--;
    }
    else
    {
      // Block the process; init or update the semaphore's ready queue

      process_table[current_pid].state = BLOCKED;


      // If a queue is null, then we initialize it

      if (sem->ready_queue == NULL)
      {
        sem->ready_queue = (PID_QUEUE *) malloc(sizeof(PID_QUEUE));
        sem->ready_queue->head = NULL;
        sem->ready_queue->tail = NULL;
      }

      enqueue(sem->ready_queue, current_pid);

      if (clock - current_quantum_start_time < QUANTUM)
      {
        if (process_table[current_pid].priority < 4)
          process_table[current_pid].priority++;
      }

      // Restart current quantum when a process gets blocked and start a process

      process_table[current_pid].total_CPU_time_used +=
        (clock - current_quantum_start_time);
      current_quantum_start_time = clock;
      schedule(4);
    }
  }
}

void handle_clock_interrupt()
{
  // Check for idle process and for going over quantum limit

  if ((current_pid != IDLE_PROCESS) &&
    ((clock - current_quantum_start_time) >= QUANTUM))
  {
    // Update the table

    process_table[current_pid].state = READY;
    process_table[current_pid].total_CPU_time_used +=
      (clock - current_quantum_start_time);
    if (process_table[current_pid].priority > 0)
      process_table[current_pid].priority--;

    // Reschedule the process

    enqueue(&ready_queues[process_table[current_pid].priority], current_pid);

    // Schedule new process and update the clock

    schedule(4);
    current_quantum_start_time = clock;

  }
}

void handle_disk_interrupt()
{
  printf("Time %d: Handled DISK_INTERRUPT for pid %d\n", clock, R1);

  // Update the table and counters

  process_table[R1].state = READY;
  io_processes--;

  // Enqueue the process or start a new one if idle

  enqueue(&ready_queues[process_table[R1].priority], R1);

  if (current_pid == IDLE_PROCESS)
  {
    current_quantum_start_time = clock;
    schedule(4);
  }

}

void handle_keyboard_interrupt()
{
  printf("Time %d: Handled KEYBOARD_INTERRUPT for pid %d\n", clock, R1);

  // Update table and counters; enqueue process or start a new one if idle

  process_table[R1].state = READY;
  io_processes--;
  enqueue(&ready_queues[process_table[R1].priority], R1);
  if (current_pid == IDLE_PROCESS)
  {
    current_quantum_start_time = clock;
    schedule(4);
  }

}

void schedule(queue_num)
{
  // Exit if no active processes left

  if (!active_processes)
  {
    printf("-- No more processes to execute --\n");
    exit(0);
  }

  // Handle case when the queue is empty

  if (ready_queues[queue_num].head == NULL)
  {

    // If no IO pending - deadlocked system

    if (!queue_num && !io_processes)
    {
      printf("DEADLOCKED SYSTEM\n");
      exit(0);
    }

    if (queue_num)
    {
      schedule(queue_num-1);
    }
    else
    {
      // If IO present - process idle; update pid

      printf("Time %d: Processor is idle\n", clock);
      current_pid = IDLE_PROCESS;
    }
  }
  else
  {
    // Clear the queue of heading blocked processes (i.e. IO'd). Recursive

    if (process_table[ready_queues[queue_num].head->pid].state == BLOCKED)
    {
      ready_queues[queue_num].head = ready_queues[queue_num].head->next;
      schedule(queue_num);
    }
    else
    {
      // Update the table and the queue; run a process
      current_pid = ready_queues[queue_num].head->pid;
      process_table[current_pid].state = RUNNING;
      ready_queues[queue_num].head = ready_queues[queue_num].head->next;
      printf("Time %d: Process %d runs\n", clock, current_pid);
    }
  }
}

void enqueue(PID_QUEUE *queue, PID_type pid)
{

  // Init a node

  PID_QUEUE_ELT *new_ready_queue_element = (PID_QUEUE_ELT *)
    malloc(sizeof(PID_QUEUE_ELT));
  new_ready_queue_element->next = NULL;
  new_ready_queue_element->pid = pid;

  // Add a node to tail (and head if it's empty, i.e. just got initialized)
  if (queue->head == NULL)
    queue->head = new_ready_queue_element;
  if (queue->tail != NULL)
    queue->tail->next = new_ready_queue_element;
  queue->tail = new_ready_queue_element;
}
