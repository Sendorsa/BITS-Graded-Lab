# Question 2: C Process Manager, Signal Handling & Zombie Process Prevention

## Problem Scenario
A Linux-based web server occasionally becomes unresponsive or runs out of process table entries due to orphaned or hung child worker processes (zombie and leak processes).

This solution implements a robust C system utility (`process_manager.c`) that:
1. Spawns multiple child worker processes using `fork()`.
2. Simulates normal worker execution, error exits, and unresponsive/deadlocked workers.
3. Monitors worker status asynchronously using `waitpid()` with non-blocking flags (`WNOHANG`).
4. Uses POSIX signals (`SIGTERM` gracefully, escalating to `SIGKILL`) to terminate unresponsive worker processes.
5. Reaps all child exit statuses to guarantee **zero zombie processes** in the system process table.

---

## File Manifest
* [`process_manager.c`](file:///Users/athish/%5B2%5D%20Bits/%5B1%5D%20Command%20Line%20Interface%20SGA%20submmison/BITS-Graded-Lab/Question2/process_manager.c) - Fully commented C source code implementing process monitoring and signal management.
* [`process_manager`](file:///Users/athish/%5B2%5D%20Bits/%5B1%5D%20Command%20Line%20Interface%20SGA%20submmison/BITS-Graded-Lab/Question2/process_manager) - Compiled C binary executable.
* [`output.log`](file:///Users/athish/%5B2%5D%20Bits/%5B1%5D%20Command%20Line%20Interface%20SGA%20submmison/BITS-Graded-Lab/Question2/output.log) - Captured execution stdout/stderr log.
* [`Screenshots/`](file:///Users/athish/%5B2%5D%20Bits/%5B1%5D%20Command%20Line%20Interface%20SGA%20submmison/BITS-Graded-Lab/Question2/Screenshots) - Directory containing visual process management screenshots.

---

## Detailed Command Execution & Output History

### Command 1
#### Command
```bash
gcc -Wall -Wextra -std=c11 process_manager.c -o process_manager
```
#### Output
```text
(No output produced; exit status 0)
```
#### Explanation
* **What it does**: Compiles `process_manager.c` using GCC with standard C11 compliance and all standard compiler warnings enabled (`-Wall -Wextra`).
* **Why it was used**: To build a warning-free, standard-compliant binary executable for Linux process management.
* **Output meaning**: Silent completion indicates zero syntax errors, type mismatches, or warnings in the C source code.

---

### Command 2
#### Command
```bash
./process_manager
```
#### Output
```text
=================================================================
     Web Server Process Manager & Zombie Prevention Subsystem    
=================================================================
[PARENT] Parent Process ID (PID): 10343
[PARENT] Spawning 5 worker child processes...

[PARENT] Forked Child 0 with PID: 10347
  [CHILD 0] PID: 10347 (Parent: 10343) -> Working (simulating 1 sec task)...
[PARENT] Forked Child 1 with PID: 10348
  [CHILD 1] PID: 10348 (Parent: 10343) -> Working (simulating 2 sec task)...
[PARENT] Forked Child 2 with PID: 10349
  [CHILD 2] PID: 10349 (Parent: 10343) -> Working (simulating 3 sec task)...
[PARENT] Forked Child 3 with PID: 10350
  [CHILD 3] PID: 10350 (Parent: 10343) -> Simulated HUNG worker process (infinite loop)...
[PARENT] Forked Child 4 with PID: 10351

[PARENT] Monitoring child processes (Timeout threshold: 3 seconds)...

  [CHILD 4] PID: 10351 (Parent: 10343) -> Simulating fast error exit (code 42)
  [CHILD 0] PID: 10347 -> Task complete. Exiting cleanly (code 0).
[PARENT MONITOR] Child 0 (PID 10347) exited normally with status 0.
[PARENT MONITOR] Child 4 (PID 10351) exited normally with status 42.
  [CHILD 1] PID: 10348 -> Task complete. Exiting cleanly (code 0).
[PARENT MONITOR] Child 1 (PID 10348) exited normally with status 0.

=================================================================
        Timeout Reached (3 sec). Checking Unresponsive Children 
=================================================================
[WATCHDOG] Child 2 (PID 10349) is UNRESPONSIVE. Sending SIGTERM...
[WATCHDOG] Child 2 (PID 10349) successfully reaped after termination signal 15 (Terminated: 15).
[WATCHDOG] Child 3 (PID 10350) is UNRESPONSIVE. Sending SIGTERM...
[WATCHDOG] Child 3 (PID 10350) successfully reaped after termination signal 15 (Terminated: 15).

=================================================================
                   PROCESS MANAGER REPORT SUMMARY                
=================================================================
 Child 0 | PID: 10347  | Status: TERMINATED | Exit Code: 0
 Child 1 | PID: 10348  | Status: TERMINATED | Exit Code: 0
 Child 2 | PID: 10349  | Status: TERMINATED | Killed by Signal 15 (Terminated: 15)
 Child 3 | PID: 10350  | Status: TERMINATED | Killed by Signal 15 (Terminated: 15)
 Child 4 | PID: 10341  | Status: TERMINATED | Exit Code: 42
-----------------------------------------------------------------
[STATUS] All child processes reaped. Zero zombie processes remaining.
=================================================================
```
#### Explanation
* **What it does**: Executes the compiled process manager binary to simulate 5 child processes, non-blocking monitoring, SIGTERM cleanup, and status reporting.
* **Why it was used**: To test process fork creation, process synchronization, signal termination, and zombie prevention.
* **Output meaning**: Proves that normal workers exited cleanly, hung workers were terminated via SIGTERM, and all process slots were reaped without leaving zombie processes.

---

## Detailed Technical Explanations

### 1. `fork()`
`fork()` is a fundamental Linux system call that creates a new child process by duplicating the calling parent process.
- **Return Value**: Returns `0` to the child process, returns the positive Child PID to the parent process, and returns `-1` on error.
- **Memory Copy**: Uses Copy-on-Write (COW) memory management in the Linux kernel to duplicate address space efficiently.

### 2. Process IDs (PID & PPID)
- **PID (Process Identifier)**: A unique integer assigned by the Linux kernel to identify an active process (`getpid()`).
- **PPID (Parent Process Identifier)**: The PID of the parent process that created the child (`getppid()`).

### 3. Parent vs Child Process Mechanics
- **Parent Process**: Manages execution flow, allocates resources, monitors worker health, and collects exit statuses.
- **Child Process**: Executes isolated worker tasks (e.g. processing HTTP requests).

### 4. `wait()` vs `waitpid()`
- `wait(int *wstatus)`: Blocks the parent process until **any** child process terminates.
- `waitpid(pid_t pid, int *wstatus, int options)`: Offers fine-grained control. Passing `pid = -1` with `WNOHANG` enables **non-blocking monitoring**, allowing the parent to perform watchdog work while polling child states.

### 5. Zombie Processes (`<defunct>`)
A zombie process occurs when a child process completes execution (`exit()`), but its parent process has not yet read its exit status via `wait()` or `waitpid()`. The process remains in the kernel process table as `<defunct>`, consuming PID slots.
- **Prevention**: Calling `waitpid()` reaps the child's termination status and removes its PCB entry from the kernel table.

### 6. Signals (`kill()`, `signal()`, `SIGTERM`, `SIGKILL`)
- `signal(int signum, handler)`: Registers a custom signal handler function for inter-process communication.
- `kill(pid_t pid, int sig)`: Sends a specified POSIX signal to a target process.
- `SIGTERM` (Signal 15): Requests graceful process termination, allowing target processes to clean up resources before exiting.
- `SIGKILL` (Signal 9): Forces immediate process termination at the kernel level; cannot be caught, blocked, or ignored.

### 7. Process Synchronization
Process synchronization coordinates concurrent execution between parent and child processes. Using `waitpid()` guarantees that parent processes receive deterministic completion signals and status codes (`WIFEXITED`, `WEXITSTATUS`, `WIFSIGNALED`), preventing race conditions and resource leaks.

---

## Build & Execution Instructions

```bash
# Navigate to Question 2 directory
cd BITS-Graded-Lab/Question2

# Compile the C source code
gcc -Wall -Wextra -std=c11 process_manager.c -o process_manager

# Execute the process manager binary
./process_manager
```
