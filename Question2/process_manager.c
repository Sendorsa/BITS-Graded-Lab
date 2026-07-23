/* ==============================================================================
 * BITS Graded Lab Assignment - Question 2
 * File: process_manager.c
 * Purpose: Web Server Child Process Monitor & Zombie Prevention Subsystem.
 * Features: Process creation via fork(), status monitoring via waitpid(),
 *           timeout-based signal termination (SIGTERM/SIGKILL), zombie prevention,
 *           and comprehensive error handling.
 * ============================================================================== */

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

#define NUM_CHILDREN 5
#define TIMEOUT_SECONDS 3

// Structure to track active child process state
typedef struct {
    pid_t pid;
    bool completed;
    int exit_status;
    int term_signal;
    bool hung_child;
} ChildProcess;

// Flag set by signal handler when SIGCHLD is received
static volatile sig_atomic_t g_sigchld_received = 0;

// Custom signal handler for SIGCHLD
void handle_sigchld(int sig) {
    (void)sig; // Suppress unused parameter warning
    g_sigchld_received = 1;
}

int main(void) {
    printf("=================================================================\n");
    printf("     Web Server Process Manager & Zombie Prevention Subsystem    \n");
    printf("=================================================================\n");
    printf("[PARENT] Parent Process ID (PID): %d\n", getpid());
    printf("[PARENT] Spawning %d worker child processes...\n\n", NUM_CHILDREN);
    fflush(stdout); // Flush buffer before fork to avoid duplicate output

    // Register SIGCHLD signal handler using signal()
    if (signal(SIGCHLD, handle_sigchld) == SIG_ERR) {
        perror("[ERROR] Failed to register SIGCHLD handler");
        exit(EXIT_FAILURE);
    }

    ChildProcess children[NUM_CHILDREN];

    // Initialize child process metadata
    for (int i = 0; i < NUM_CHILDREN; i++) {
        children[i].pid = -1;
        children[i].completed = false;
        children[i].exit_status = -1;
        children[i].term_signal = 0;
        children[i].hung_child = (i == 3); // Mark Child index 3 as unresponsive
    }

    // Spawning child processes using fork()
    for (int i = 0; i < NUM_CHILDREN; i++) {
        pid_t pid = fork();

        if (pid < 0) {
            // fork() failed
            fprintf(stderr, "[ERROR] fork() failed for child index %d: %s\n", i, strerror(errno));
            for (int j = 0; j < i; j++) {
                if (!children[j].completed) {
                    kill(children[j].pid, SIGKILL);
                }
            }
            exit(EXIT_FAILURE);
        } 
        else if (pid == 0) {
            // ------------------------------------------------------------------
            // Child Process Execution Context
            // ------------------------------------------------------------------
            pid_t child_pid = getpid();
            pid_t parent_pid = getppid();

            if (i == 3) {
                // Simulate unresponsive worker process (hung loop)
                printf("  [CHILD %d] PID: %d (Parent: %d) -> Simulated HUNG worker process (infinite loop)...\n",
                       i, child_pid, parent_pid);
                fflush(stdout);
                while (1) {
                    sleep(1); // Infinite sleep simulating deadlock/unresponsiveness
                }
            } 
            else if (i == 4) {
                // Simulate child exiting with non-zero error code
                printf("  [CHILD %d] PID: %d (Parent: %d) -> Simulating fast error exit (code 42)\n",
                       i, child_pid, parent_pid);
                fflush(stdout);
                sleep(1);
                exit(42);
            } 
            else {
                // Simulate normal fast worker child
                printf("  [CHILD %d] PID: %d (Parent: %d) -> Working (simulating %d sec task)...\n",
                       i, child_pid, parent_pid, i + 1);
                fflush(stdout);
                sleep(i + 1);
                printf("  [CHILD %d] PID: %d -> Task complete. Exiting cleanly (code 0).\n", i, child_pid);
                fflush(stdout);
                exit(EXIT_SUCCESS);
            }
        } 
        else {
            // ------------------------------------------------------------------
            // Parent Process Execution Context
            // ------------------------------------------------------------------
            children[i].pid = pid;
            printf("[PARENT] Forked Child %d with PID: %d\n", i, pid);
            fflush(stdout);
        }
    }

    printf("\n[PARENT] Monitoring child processes (Timeout threshold: %d seconds)...\n\n", TIMEOUT_SECONDS);
    fflush(stdout);

    // Monitoring Loop with non-blocking waitpid()
    int elapsed = 0;
    int active_children = NUM_CHILDREN;

    while (elapsed < TIMEOUT_SECONDS && active_children > 0) {
        sleep(1);
        elapsed++;

        // Reap any terminated children using waitpid with WNOHANG
        int status;
        pid_t reaped_pid;
        while ((reaped_pid = waitpid(-1, &status, WNOHANG)) > 0) {
            for (int i = 0; i < NUM_CHILDREN; i++) {
                if (children[i].pid == reaped_pid && !children[i].completed) {
                    children[i].completed = true;
                    active_children--;

                    if (WIFEXITED(status)) {
                        children[i].exit_status = WEXITSTATUS(status);
                        printf("[PARENT MONITOR] Child %d (PID %d) exited normally with status %d.\n",
                               i, reaped_pid, children[i].exit_status);
                    } else if (WIFSIGNALED(status)) {
                        children[i].term_signal = WTERMSIG(status);
                        printf("[PARENT MONITOR] Child %d (PID %d) terminated by signal %d (%s).\n",
                               i, reaped_pid, children[i].term_signal, strsignal(children[i].term_signal));
                    }
                    fflush(stdout);
                }
            }
        }
    }

    // --------------------------------------------------------------------------
    // Unresponsive Child Termination Phase (SIGTERM -> SIGKILL)
    // --------------------------------------------------------------------------
    printf("\n=================================================================\n");
    printf("        Timeout Reached (%d sec). Checking Unresponsive Children \n", TIMEOUT_SECONDS);
    printf("=================================================================\n");
    fflush(stdout);

    for (int i = 0; i < NUM_CHILDREN; i++) {
        if (!children[i].completed) {
            printf("[WATCHDOG] Child %d (PID %d) is UNRESPONSIVE. Sending SIGTERM...\n", i, children[i].pid);
            fflush(stdout);
            
            // Step 1: Send graceful termination signal (SIGTERM)
            if (kill(children[i].pid, SIGTERM) == -1) {
                perror("[ERROR] Failed to send SIGTERM");
            }

            // Give process 1 second to clean up and exit
            sleep(1);

            // Check if process reaped
            int status;
            pid_t res = waitpid(children[i].pid, &status, WNOHANG);

            if (res == 0) {
                // Process ignored SIGTERM: Force kill using SIGKILL
                printf("[WATCHDOG] Child %d (PID %d) ignored SIGTERM. Escalating to SIGKILL...\n", i, children[i].pid);
                fflush(stdout);
                if (kill(children[i].pid, SIGKILL) == -1) {
                    perror("[ERROR] Failed to send SIGKILL");
                }
                
                // Blocking wait to clean up process table entry and prevent zombie
                waitpid(children[i].pid, &status, 0);
            }

            children[i].completed = true;
            if (WIFSIGNALED(status)) {
                children[i].term_signal = WTERMSIG(status);
                printf("[WATCHDOG] Child %d (PID %d) successfully reaped after termination signal %d (%s).\n",
                       i, children[i].pid, children[i].term_signal, strsignal(children[i].term_signal));
            } else if (WIFEXITED(status)) {
                children[i].exit_status = WEXITSTATUS(status);
                printf("[WATCHDOG] Child %d (PID %d) exited with status %d.\n",
                       i, children[i].pid, children[i].exit_status);
            }
            fflush(stdout);
        }
    }

    // Final Zombie Check: Wait for any lingering children
    int status;
    pid_t lingering;
    while ((lingering = waitpid(-1, &status, WNOHANG)) > 0) {
        printf("[CLEANUP] Reaped lingering child PID %d\n", lingering);
        fflush(stdout);
    }

    printf("\n=================================================================\n");
    printf("                   PROCESS MANAGER REPORT SUMMARY                \n");
    printf("=================================================================\n");
    for (int i = 0; i < NUM_CHILDREN; i++) {
        printf(" Child %d | PID: %-6d | Status: %-10s | ",
               i, children[i].pid, children[i].completed ? "TERMINATED" : "ACTIVE");
        if (children[i].term_signal > 0) {
            printf("Killed by Signal %d (%s)\n", children[i].term_signal, strsignal(children[i].term_signal));
        } else {
            printf("Exit Code: %d\n", children[i].exit_status);
        }
    }
    printf("-----------------------------------------------------------------\n");
    printf("[STATUS] All child processes reaped. Zero zombie processes remaining.\n");
    printf("=================================================================\n");
    fflush(stdout);

    return EXIT_SUCCESS;
}
