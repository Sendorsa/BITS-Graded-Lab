# Question 3: Low-Level POSIX System Calls & Random-Access Database Management

## Problem Scenario
A enterprise company requires a high-performance, secure employee record management system built directly on top of Linux kernel primitives. 

Higher-level C standard library I/O functions (`fopen()`, `fread()`, `fwrite()`) introduce internal user-space buffering and lack precise byte-level position control over disk blocks. To achieve maximum I/O performance and atomic in-place record updates, this solution bypasses stdio and operates exclusively via POSIX system calls (`open()`, `read()`, `write()`, `lseek()`, `close()`).

---

## File Manifest
* [`employee_manager.c`](file:///Users/athish/%5B2%5D%20Bits/%5B1%5D%20Command%20Line%20Interface%20SGA%20submmison/BITS-Graded-Lab/Question3/employee_manager.c) - C source code using system calls for database management.
* [`employee_manager`](file:///Users/athish/%5B2%5D%20Bits/%5B1%5D%20Command%20Line%20Interface%20SGA%20submmison/BITS-Graded-Lab/Question3/employee_manager) - Compiled binary executable.
* [`employees.db`](file:///Users/athish/%5B2%5D%20Bits/%5B1%5D%20Command%20Line%20Interface%20SGA%20submmison/BITS-Graded-Lab/Question3/employees.db) - Binary employee database storage file (360 bytes).
* [`output.log`](file:///Users/athish/%5B2%5D%20Bits/%5B1%5D%20Command%20Line%20Interface%20SGA%20submmison/BITS-Graded-Lab/Question3/output.log) - Captured execution stdout/stderr log.
* [`Screenshots/`](file:///Users/athish/%5B2%5D%20Bits/%5B1%5D%20Command%20Line%20Interface%20SGA%20submmison/BITS-Graded-Lab/Question3/Screenshots) - Directory containing visual system call database screenshots.

---

## Detailed Command Execution & Output History

### Command 1
#### Command
```bash
gcc -Wall -Wextra -std=c11 employee_manager.c -o employee_manager
```
#### Output
```text
(No output produced; exit status 0)
```
#### Explanation
* **What it does**: Compiles `employee_manager.c` using GCC with `-Wall -Wextra` warning flags and C11 standard mode.
* **Why it was used**: To generate a clean binary executable ensuring zero compiler warnings or POSIX header issues.
* **Output meaning**: Silent completion confirms clean compilation.

---

### Command 2
#### Command
```bash
./employee_manager
```
#### Output
```text
================================================================================
       POSIX Low-Level System Call Employee Management Subsystem               
================================================================================
[INIT] Creating/Opening database file 'employees.db' via open() system call...

--- PHASE 1: Inserting Employee Records (System Call: write & lseek) ---
[INSERT] [ID: 101] Name: Alice Smith      | Dept: Engineering  | Salary: $95000.00 | Active: YES
[INSERT] [ID: 102] Name: Bob Jones        | Dept: Finance      | Salary: $82000.00 | Active: YES
[INSERT] [ID: 103] Name: Carol Williams   | Dept: Marketing    | Salary: $78000.00 | Active: YES
[INSERT] [ID: 104] Name: David Brown      | Dept: Engineering  | Salary: $105000.00 | Active: YES
[INSERT] [ID: 105] Name: Eve Davis        | Dept: HR           | Salary: $71000.00 | Active: YES

--------------------------------------------------------------------------------
                     FULL DATABASE DUMP (Sequential Scan)                       
--------------------------------------------------------------------------------
[REC #0] [ID: 101] Name: Alice Smith      | Dept: Engineering  | Salary: $95000.00 | Active: YES
[REC #1] [ID: 102] Name: Bob Jones        | Dept: Finance      | Salary: $82000.00 | Active: YES
[REC #2] [ID: 103] Name: Carol Williams   | Dept: Marketing    | Salary: $78000.00 | Active: YES
[REC #3] [ID: 104] Name: David Brown      | Dept: Engineering  | Salary: $105000.00 | Active: YES
[REC #4] [ID: 105] Name: Eve Davis        | Dept: HR           | Salary: $71000.00 | Active: YES
--------------------------------------------------------------------------------

--- PHASE 2: Random Access Retrieval (System Call: lseek & read) ---
[QUERY] Requesting Record #2 directly via lseek(fd, 2 * sizeof(Employee), SEEK_SET)...
[FETCHED] [ID: 103] Name: Carol Williams   | Dept: Marketing    | Salary: $78000.00 | Active: YES
[QUERY] Requesting Record #0 directly via lseek(fd, 0 * sizeof(Employee), SEEK_SET)...
[FETCHED] [ID: 101] Name: Alice Smith      | Dept: Engineering  | Salary: $95000.00 | Active: YES

--- PHASE 3: In-Place Direct Record Modification (System Call: lseek, read, write) ---
[UPDATE] Modifying Salary for Bob Jones (Record #1) to $89,500.00 in-place...
[MODIFY] Record #1 (ID: 102, Bob Jones) Salary Updated: $82000.00 -> $89500.00
[UPDATE] Modifying Salary for Alice Smith (Record #0) to $102,000.00 in-place...
[MODIFY] Record #0 (ID: 101, Alice Smith) Salary Updated: $95000.00 -> $102000.00

--------------------------------------------------------------------------------
                     FULL DATABASE DUMP (Sequential Scan)                       
--------------------------------------------------------------------------------
[REC #0] [ID: 101] Name: Alice Smith      | Dept: Engineering  | Salary: $102000.00 | Active: YES
[REC #1] [ID: 102] Name: Bob Jones        | Dept: Finance      | Salary: $89500.00 | Active: YES
[REC #2] [ID: 103] Name: Carol Williams   | Dept: Marketing    | Salary: $78000.00 | Active: YES
[REC #3] [ID: 104] Name: David Brown      | Dept: Engineering  | Salary: $105000.00 | Active: YES
[REC #4] [ID: 105] Name: Eve Davis        | Dept: HR           | Salary: $71000.00 | Active: YES
--------------------------------------------------------------------------------

[CLEANUP] Closing database file descriptor via close() system call...
================================================================================
[STATUS] Execution clean. All file operations completed using POSIX system calls.
================================================================================
```
#### Explanation
* **What it does**: Runs the employee database binary, demonstrating record insertion, arbitrary location retrieval, and in-place salary updates.
* **Why it was used**: To verify system call behavior, offset calculation, and in-place struct modifications on disk.
* **Output meaning**: Confirms that records were inserted, fetched by offset, and updated in-place without corrupting adjacent database records.

---

### Command 3
#### Command
```bash
ls -la employees.db
```
#### Output
```text
-rw-r--r--  1 athish  staff  360 Jul 23 13:08 employees.db
```
#### Explanation
* **What it does**: Inspects file metadata and size of the created database binary file `employees.db`.
* **Why it was used**: To confirm that 5 employee records of exactly 72 bytes each occupy 360 bytes on disk.
* **Output meaning**: Proves fixed-length record alignment ($5 \times 72 = 360$ bytes) essential for $O(1)$ random access.

---

## Technical Explanation of System Calls

### 1. `open(const char *pathname, int flags, mode_t mode)`
- **Function**: Establishes a connection between a file entry in the VFS (Virtual File System) and the process, returning a non-negative **File Descriptor (FD)** integer.
- **Flags Used**:
  - `O_RDWR`: Opens file for both reading and writing.
  - `O_CREAT`: Creates the file if it does not exist.
  - `O_TRUNC`: Truncates existing file length to 0 upon opening.
- **Permissions**: `S_IRUSR | S_IWUSR` assigns `0644` read/write permissions.

### 2. `read(int fd, void *buf, size_t count)`
- **Function**: Reads up to `count` bytes from the file descriptor `fd` starting at the current file offset into memory buffer `buf`.
- **Behavior**: Advances the file offset by the number of bytes read. Returns `0` at End-Of-File (EOF) and `-1` on error.

### 3. `write(int fd, const void *buf, size_t count)`
- **Function**: Writes `count` bytes from memory buffer `buf` to the file descriptor `fd` at the current file offset.
- **Behavior**: Overwrites existing bytes on disk if writing to an existing offset, or expands file size if writing at EOF.

### 4. `lseek(int fd, off_t offset, int whence)`
- **Function**: Repositions the read/write file offset of the open file descriptor `fd`.
- **Whence Directives**:
  - `SEEK_SET`: Sets offset to `offset` bytes from the start of file.
  - `SEEK_CUR`: Sets offset to current location plus `offset`.
  - `SEEK_END`: Sets offset to file size plus `offset`.
- **Return Value**: Returns the resulting file offset from the beginning of the file.

### 5. `close(int fd)`
- **Function**: Deallocates the specified file descriptor integer, flushing pending kernel buffers and releasing file locks.

---

## Why Random Access via `lseek()` Improves Efficiency

In conventional sequential file management, modifying a single employee record inside a 10 GB file requires:
1. Reading the entire file sequentially into memory or a temporary file.
2. Locating the modified line/record.
3. Writing all unchanged records plus the modified record to a new file ($O(N)$ time and $O(N)$ disk I/O).
4. Replacing the old file atomically.

### Efficiency Comparison Table

| Metric | Sequential Rewrite Approach | Random Access via `lseek()` |
| :--- | :--- | :--- |
| **Time Complexity** | $O(N)$ proportional to total database size | **$O(1)$ constant time lookup and update** |
| **Disk I/O Cost** | Reads & Writes 100% of database blocks | **Reads & Writes exactly 1 record block (72 bytes)** |
| **Memory Overhead** | High memory allocation for file buffer | **Minimal overhead (single struct buffer)** |
| **Storage Impact** | Requires 2x storage during temporary write | **Zero extra storage required (in-place modification)** |
| **Wear on SSD/Storage** | High write amplification | **Minimal write amplification** |

By enforcing fixed-size records (`sizeof(Employee) = 72` bytes), any record at index $N$ is accessed directly at offset:
$$\text{Offset} = N \times \text{sizeof}(Employee)$$

Using `lseek(fd, Offset, SEEK_SET)` allows the OS storage subsystem to jump directly to the target disk block in $O(1)$ time, reading and updating **only that specific record** in-place without touching any other data on disk.

---

## Build & Execution Instructions

```bash
# Navigate to Question 3 directory
cd BITS-Graded-Lab/Question3

# Compile C source code using GCC
gcc -Wall -Wextra -std=c11 employee_manager.c -o employee_manager

# Execute binary
./employee_manager

# Inspect generated binary database
ls -la employees.db
```
