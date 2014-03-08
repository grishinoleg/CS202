
/* This must be implemented by the kernel. It will
   be called automatically during (simulated) system
   initialization */

extern void initialize_kernel();

// Handles behavior for TRAP signal
void handle_trap();

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

// Handles a blocking disk read
void handle_disk_read();

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
void schedule(PID_QUEUE *queue);

// Put a process at the end of the queue
void enqueue(PID_QUEUE **pointer_to_queue, PID_type pid);
