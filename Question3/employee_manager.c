/* ==============================================================================
 * BITS Graded Lab Assignment - Question 3
 * File: employee_manager.c
 * Purpose: Secure Employee Record Management System using POSIX System Calls only.
 * System Calls Used: open(), read(), write(), lseek(), close()
 * Constraints: NO stdio file functions (fopen, fread, fwrite). Direct binary I/O.
 * ============================================================================== */

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define DB_FILE "employees.db"

// Fixed-size Employee Record Structure for O(1) Random Access
#pragma pack(push, 1)
typedef struct {
    int id;               // 4 bytes
    char name[32];        // 32 bytes
    char department[24];  // 24 bytes
    double salary;        // 8 bytes
    int active;           // 4 bytes
} Employee;
#pragma pack(pop)

// Helper function to print messages to standard output using write() system call
static void sys_print(const char *msg) {
    write(STDOUT_FILENO, msg, strlen(msg));
}

// Helper function to format employee output without stdio file I/O
static void print_employee_record(const Employee *emp, const char *prefix) {
    char buf[256];
    snprintf(buf, sizeof(buf), "%s [ID: %d] Name: %-16s | Dept: %-12s | Salary: $%.2f | Active: %s\n",
             prefix, emp->id, emp->name, emp->department, emp->salary, emp->active ? "YES" : "NO");
    sys_print(buf);
}

// Function 1: Create or open employee database file using open()
int open_database(const char *filename) {
    // open() system call with Read/Write, Create, and Permissions 0644 (-rw-r--r--)
    int fd = open(filename, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fd < 0) {
        char errbuf[128];
        snprintf(errbuf, sizeof(errbuf), "[ERROR] System call open() failed: %s\n", strerror(errno));
        sys_print(errbuf);
        exit(EXIT_FAILURE);
    }
    return fd;
}

// Function 2: Insert employee record at specific index offset using lseek() and write()
int insert_employee(int fd, int record_index, int id, const char *name, const char *dept, double salary) {
    Employee emp;
    memset(&emp, 0, sizeof(Employee));
    emp.id = id;
    strncpy(emp.name, name, sizeof(emp.name) - 1);
    strncpy(emp.department, dept, sizeof(emp.department) - 1);
    emp.salary = salary;
    emp.active = 1;

    // Calculate byte offset based on record index
    off_t offset = (off_t)record_index * sizeof(Employee);

    // lseek() system call to seek directly to byte offset relative to file start (SEEK_SET)
    if (lseek(fd, offset, SEEK_SET) == (off_t)-1) {
        sys_print("[ERROR] lseek() system call failed during insert.\n");
        return -1;
    }

    // write() system call to write binary struct data directly to disk
    ssize_t bytes_written = write(fd, &emp, sizeof(Employee));
    if (bytes_written != sizeof(Employee)) {
        sys_print("[ERROR] write() system call failed during insert.\n");
        return -1;
    }

    print_employee_record(&emp, "[INSERT]");
    return 0;
}

// Function 3: Retrieve employee record from arbitrary location using lseek() and read()
int retrieve_employee(int fd, int record_index, Employee *out_emp) {
    off_t offset = (off_t)record_index * sizeof(Employee);

    // lseek() to jump directly to record position
    if (lseek(fd, offset, SEEK_SET) == (off_t)-1) {
        sys_print("[ERROR] lseek() system call failed during seek.\n");
        return -1;
    }

    // read() system call to fetch fixed-size struct
    ssize_t bytes_read = read(fd, out_emp, sizeof(Employee));
    if (bytes_read <= 0) {
        sys_print("[ERROR] read() system call returned 0 bytes (EOF or invalid index).\n");
        return -1;
    }

    return 0;
}

// Function 4: Modify record directly in-place using lseek(), read(), and write()
int modify_employee_salary(int fd, int record_index, double new_salary) {
    Employee emp;
    off_t offset = (off_t)record_index * sizeof(Employee);

    // Step 1: lseek() to record offset
    if (lseek(fd, offset, SEEK_SET) == (off_t)-1) {
        sys_print("[ERROR] lseek() system call failed.\n");
        return -1;
    }

    // Step 2: read() record into memory
    if (read(fd, &emp, sizeof(Employee)) != sizeof(Employee)) {
        sys_print("[ERROR] read() failed during modification.\n");
        return -1;
    }

    double old_salary = emp.salary;
    emp.salary = new_salary;

    // Step 3: lseek() back to original record offset for in-place overwrite
    if (lseek(fd, offset, SEEK_SET) == (off_t)-1) {
        sys_print("[ERROR] lseek() rewind failed.\n");
        return -1;
    }

    // Step 4: write() updated record in-place (WITHOUT rewriting the file)
    if (write(fd, &emp, sizeof(Employee)) != sizeof(Employee)) {
        sys_print("[ERROR] write() overwrite failed.\n");
        return -1;
    }

    char buf[256];
    snprintf(buf, sizeof(buf), "[MODIFY] Record #%d (ID: %d, %s) Salary Updated: $%.2f -> $%.2f\n",
             record_index, emp.id, emp.name, old_salary, new_salary);
    sys_print(buf);

    return 0;
}

// Function 5: Display entire database sequentially
void display_all_records(int fd) {
    sys_print("\n--------------------------------------------------------------------------------\n");
    sys_print("                     FULL DATABASE DUMP (Sequential Scan)                       \n");
    sys_print("--------------------------------------------------------------------------------\n");

    // Seek to beginning of file (SEEK_SET, 0)
    lseek(fd, 0, SEEK_SET);

    Employee emp;
    int index = 0;
    while (read(fd, &emp, sizeof(Employee)) == sizeof(Employee)) {
        if (emp.active) {
            char idx_str[32];
            snprintf(idx_str, sizeof(idx_str), "[REC #%d]", index);
            print_employee_record(&emp, idx_str);
        }
        index++;
    }
    sys_print("--------------------------------------------------------------------------------\n\n");
}

int main(void) {
    sys_print("================================================================================\n");
    sys_print("       POSIX Low-Level System Call Employee Management Subsystem               \n");
    sys_print("================================================================================\n");
    sys_print("[INIT] Creating/Opening database file 'employees.db' via open() system call...\n\n");

    // Step 1: Open/Create file via open() system call
    int fd = open(DB_FILE, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fd < 0) {
        perror("Failed to create database");
        return EXIT_FAILURE;
    }

    // Step 2: Insert Sample Records (using write() and lseek())
    sys_print("--- PHASE 1: Inserting Employee Records (System Call: write & lseek) ---\n");
    insert_employee(fd, 0, 101, "Alice Smith",    "Engineering", 95000.00);
    insert_employee(fd, 1, 102, "Bob Jones",      "Finance",     82000.00);
    insert_employee(fd, 2, 103, "Carol Williams", "Marketing",   78000.00);
    insert_employee(fd, 3, 104, "David Brown",    "Engineering", 105000.00);
    insert_employee(fd, 4, 105, "Eve Davis",      "HR",          71000.00);

    // Display initial database contents
    display_all_records(fd);

    // Step 3: Direct Arbitrary Retrieval using lseek() and read()
    sys_print("--- PHASE 2: Random Access Retrieval (System Call: lseek & read) ---\n");
    Employee emp_query;
    sys_print("[QUERY] Requesting Record #2 directly via lseek(fd, 2 * sizeof(Employee), SEEK_SET)...\n");
    if (retrieve_employee(fd, 2, &emp_query) == 0) {
        print_employee_record(&emp_query, "[FETCHED]");
    }

    sys_print("[QUERY] Requesting Record #0 directly via lseek(fd, 0 * sizeof(Employee), SEEK_SET)...\n");
    if (retrieve_employee(fd, 0, &emp_query) == 0) {
        print_employee_record(&emp_query, "[FETCHED]");
    }

    // Step 4: In-Place Modification without rewriting the full file
    sys_print("\n--- PHASE 3: In-Place Direct Record Modification (System Call: lseek, read, write) ---\n");
    sys_print("[UPDATE] Modifying Salary for Bob Jones (Record #1) to $89,500.00 in-place...\n");
    modify_employee_salary(fd, 1, 89500.00);

    sys_print("[UPDATE] Modifying Salary for Alice Smith (Record #0) to $102,000.00 in-place...\n");
    modify_employee_salary(fd, 0, 102000.00);

    // Display updated database contents to confirm in-place modification
    display_all_records(fd);

    // Step 5: Close file descriptor using close() system call
    sys_print("[CLEANUP] Closing database file descriptor via close() system call...\n");
    if (close(fd) < 0) {
        perror("close() system call failed");
        return EXIT_FAILURE;
    }

    sys_print("================================================================================\n");
    sys_print("[STATUS] Execution clean. All file operations completed using POSIX system calls.\n");
    sys_print("================================================================================\n");

    return EXIT_SUCCESS;
}
