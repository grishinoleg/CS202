
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

// Schedules a process to run now
void schedule();

// Put a process at the end of the queue
void enqueue(PID_type pid);
