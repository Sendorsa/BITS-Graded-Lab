# Question 1: Automated Assignment Submission Deduplication & Backup System

## Problem Scenario
A university Linux server stores assignment submissions from hundreds of students. Over time, students resubmit identical files or share solutions, resulting in duplicate files that waste server disk space and complicate grading. 

This solution provides a robust Shell script (`submission_dedupe.sh`) that:
1. Automatically traverses the student submissions directory.
2. Computes cryptographic hashes (SHA-256 / MD5) for each submission.
3. Distinguishes unique submissions from duplicate files.
4. Backs up **only unique files** into a designated backup directory.
5. Generates a comprehensive summary report (`report.txt`) detailing total processed files, duplicate files, and backed-up files.
6. Isolates all execution and filesystem errors into an error log (`error.log`).

---

## File Manifest
* [`submission_dedupe.sh`](file:///Users/athish/%5B2%5D%20Bits/%5B1%5D%20Command%20Line%20Interface%20SGA%20submmison/BITS-Graded-Lab/Question1/submission_dedupe.sh) - Production-grade Shell deduplication script.
* [`sample_submissions/`](file:///Users/athish/%5B2%5D%20Bits/%5B1%5D%20Command%20Line%20Interface%20SGA%20submmison/BITS-Graded-Lab/Question1/sample_submissions) - Sample dataset containing unique and duplicate student assignments.
* [`backup/`](file:///Users/athish/%5B2%5D%20Bits/%5B1%5D%20Command%20Line%20Interface%20SGA%20submmison/BITS-Graded-Lab/Question1/backup) - Target backup directory containing backed-up unique files.
* [`report.txt`](file:///Users/athish/%5B2%5D%20Bits/%5B1%5D%20Command%20Line%20Interface%20SGA%20submmison/BITS-Graded-Lab/Question1/report.txt) - Generated audit report file.
* [`error.log`](file:///Users/athish/%5B2%5D%20Bits/%5B1%5D%20Command%20Line%20Interface%20SGA%20submmison/BITS-Graded-Lab/Question1/error.log) - Separate error output log.
* [`output.log`](file:///Users/athish/%5B2%5D%20Bits/%5B1%5D%20Command%20Line%20Interface%20SGA%20submmison/BITS-Graded-Lab/Question1/output.log) - Terminal stdout/stderr capture log.
* [`screenshots/`](file:///Users/athish/%5B2%5D%20Bits/%5B1%5D%20Command%20Line%20Interface%20SGA%20submmison/BITS-Graded-Lab/Question1/screenshots) - Directory containing visual execution screenshots.

---

## Detailed Command Execution & Output History

### Command 1
#### Command
```bash
chmod +x submission_dedupe.sh
```
#### Output
```text
(No output produced; exit status 0)
```
#### Explanation
* **What it does**: Grants execute permissions to the script file `submission_dedupe.sh` for the file owner and group.
* **Why it was used**: Shell scripts must have executable permissions (`+x`) before they can be executed directly as standalone commands in Linux.
* **Output meaning**: Silent completion indicates permission bits were successfully modified without errors.

---

### Command 2
#### Command
```bash
ls -la sample_submissions/
```
#### Output
```text
drwxr-xr-x  8 athish  staff  256 Jul 23 13:06 .
drwxr-xr-x  9 athish  staff  288 Jul 23 13:07 ..
-rw-r--r--  1 athish  staff   81 Jul 23 13:06 student_101_assignment1.c
-rw-r--r--  1 athish  staff   81 Jul 23 13:06 student_102_assignment1.c
-rw-r--r--  1 athish  staff   92 Jul 23 13:06 student_103_assignment1.c
-rw-r--r--  1 athish  staff   81 Jul 23 13:06 student_104_assignment1.c
-rw-r--r--  1 athish  staff   88 Jul 23 13:06 student_105_module5.py
-rw-r--r--  1 athish  staff   88 Jul 23 13:06 student_106_module5.py
```
#### Explanation
* **What it does**: Lists all files and metadata in long listing format inside `sample_submissions/`.
* **Why it was used**: To inspect the initial set of student files and verify the sample dataset before executing deduplication.
* **Output meaning**: Displays 6 student files where students 101, 102, 104 have identical content (81 bytes) and students 105, 106 have identical Python code (88 bytes).

---

### Command 3
#### Command
```bash
./submission_dedupe.sh sample_submissions backup report.txt error.log
```
#### Output
```text
==================================================
 Starting Assignment Submission Deduplication
 Source Directory : sample_submissions
 Backup Directory : backup
 Report File      : report.txt
 Error Log        : error.log
==================================================

[BACKUP]    sample_submissions/student_101_assignment1.c -> backup/student_101_assignment1.c
[DUPLICATE] sample_submissions/student_102_assignment1.c matches hash of sample_submissions/student_101_assignment1.c
[BACKUP]    sample_submissions/student_103_assignment1.c -> backup/student_103_assignment1.c
[DUPLICATE] sample_submissions/student_104_assignment1.c matches hash of sample_submissions/student_101_assignment1.c
[BACKUP]    sample_submissions/student_105_module5.py -> backup/student_105_module5.py
[DUPLICATE] sample_submissions/student_106_module5.py matches hash of sample_submissions/student_105_module5.py
==================================================
       SUBMISSION DEDUPLICATION REPORT            
==================================================
Timestamp           : 2026-07-23 13:07:20
Source Directory    : sample_submissions
Backup Directory    : backup
Total Files Scanned : 6
Backed-Up (Unique)  : 3
Duplicate Files     : 3
--------------------------------------------------
BACKED UP UNIQUE FILES:
  - sample_submissions/student_101_assignment1.c -> backup/student_101_assignment1.c
  - sample_submissions/student_103_assignment1.c -> backup/student_103_assignment1.c
  - sample_submissions/student_105_module5.py -> backup/student_105_module5.py
--------------------------------------------------
IDENTIFIED DUPLICATE FILES:
  - sample_submissions/student_102_assignment1.c (Duplicate of sample_submissions/student_101_assignment1.c)
  - sample_submissions/student_104_assignment1.c (Duplicate of sample_submissions/student_101_assignment1.c)
  - sample_submissions/student_106_module5.py (Duplicate of sample_submissions/student_105_module5.py)
==================================================

Deduplication complete. Report saved to report.txt.
Errors (if any) saved to error.log.
```
#### Explanation
* **What it does**: Executes the deduplication shell script passing the source directory, backup directory, report file, and error log file paths.
* **Why it was used**: To scan files, hash them, back up unique files, identify duplicates, and generate the final audit report.
* **Output meaning**: Out of 6 files scanned, exactly 3 unique files were copied to `backup/` and 3 duplicate submissions were detected.

---

### Command 4
#### Command
```bash
ls -la backup/
```
#### Output
```text
drwxr-xr-x  5 athish  staff  160 Jul 23 13:07 .
drwxr-xr-x  9 athish  staff  288 Jul 23 13:07 ..
-rw-r--r--  1 athish  staff   81 Jul 23 13:07 student_101_assignment1.c
-rw-r--r--  1 athish  staff   92 Jul 23 13:07 student_103_assignment1.c
-rw-r--r--  1 athish  staff   88 Jul 23 13:07 student_105_module5.py
```
#### Explanation
* **What it does**: Lists the contents of the generated `backup/` directory.
* **Why it was used**: To confirm that only unique files were backed up and duplicate files were excluded.
* **Output meaning**: Verifies that `backup/` contains exactly 3 files representing unique submission contents.

---

### Command 5
#### Command
```bash
cat error.log
```
#### Output
```text
(Empty file)
```
#### Explanation
* **What it does**: Displays the contents of `error.log`.
* **Why it was used**: To verify whether any permission failures or file access errors occurred during execution.
* **Output meaning**: An empty file confirms clean execution with zero runtime errors.

---

## Explanation of Key Linux Utilities & Concepts

### 1. `find`
The `find` command recursively searches the directory hierarchy for files meeting specific criteria. In `submission_dedupe.sh`, `find "$SRC_DIR" -type f` locates regular files while excluding subdirectories, pipe endpoints, and sockets.

### 2. Cryptographic Hashing (`sha256sum` / `md5sum`)
Cryptographic hash functions generate fixed-size hash signatures (e.g., 256-bit for SHA-256) based on file byte contents. If two files have identical content, their hash signatures match perfectly regardless of file names. The script uses these hashes to identify duplicate submissions.

### 3. `cp` and `mkdir`
- `mkdir -p` creates target backup directories recursively and suppresses errors if the directory already exists.
- `cp` copies unique files from `sample_submissions/` into `backup/` while leaving original student submissions intact.

### 4. Redirection Operators (`>`, `>>`, `2>`, `2>>`)
- `>` overwrites or creates a file with stdout output.
- `>>` appends stdout output to an existing file.
- `2>` overwrites stderr output to a specified log file.
- `2>>` appends stderr (such as permission errors) into `error.log` without cluttering stdout.

### 5. Loops & Process Substitution
The script uses a `while IFS= read -r file; do ... done < <(find ...)` process substitution loop. This construct prevents subshell variable isolation, enabling accurate count updates for scanned, backed-up, and duplicate files.

### 6. Conditional Statements & Variables
- `if [ ! -r "$file" ]; then` tests file read permissions before processing.
- `if [ -n "$existing_entry" ]; then` evaluates whether a calculated hash has previously been registered in the database.
- Shell variables capture file paths, counts, and hash strings safely using double quotes to handle filenames containing spaces.

---

## Build & Execution Instructions

```bash
# Navigate to Question 1 directory
cd BITS-Graded-Lab/Question1

# Grant execution permissions
chmod +x submission_dedupe.sh

# Run script with default parameters
./submission_dedupe.sh sample_submissions backup report.txt error.log

# Verify results
cat report.txt
cat error.log
ls -la backup/
```
