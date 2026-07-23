# BITS Graded Lab Assignment
**Course / Lab Submission Repository**  
**Author**: Technical Documentation & Linux Systems Engineering  
**Environment**: macOS / Linux POSIX Shell & C11 Toolchain

---

## Executive Summary & Repository Sitemap

This repository contains the submission for the **BITS Graded Lab Assignment (Modules 5–10)**, covering Shell Scripting, POSIX System Calls, Process & Signal Management, Live Log Pipelines, and Linux Editor Recovery Internals.

Every directory is self-contained with complete source code, compiled binaries, generated reports, execution output logs, screenshot guides, and technical documentation adhering strictly to submission standards.

```
BITS-Graded-Lab/
├── README.md                          (Main Repository Index & Table of Contents)
├── Question1/
│   ├── README.md                      (Duplicate Submission Deduplication & Backup Script)
│   ├── submission_dedupe.sh           (Executable Shell script)
│   ├── sample_submissions/            (Sample submission files dataset)
│   ├── backup/                        (Generated backup folder for unique files)
│   ├── report.txt                     (Generated audit report)
│   ├── error.log                      (Generated error log)
│   ├── output.log                     (Captured terminal output)
│   └── Screenshots/                   (Captured terminal screenshots)
├── Question2/
│   ├── README.md                      (Process Manager Writeup & POSIX Explanations)
│   ├── process_manager.c              (Commented C source code)
│   ├── process_manager                (Compiled binary executable)
│   ├── output.log                     (Captured terminal execution log)
│   └── Screenshots/                   (Captured terminal screenshots)
├── Question3/
│   ├── README.md                      (Low-Level System Calls Writeup & Efficiency Analysis)
│   ├── employee_manager.c             (C source code using open/read/write/lseek/close)
│   ├── employee_manager               (Compiled binary executable)
│   ├── employees.db                   (Generated binary database file - 360 bytes)
│   ├── output.log                     (Captured terminal execution log)
│   └── Screenshots/                   (Captured terminal screenshots)
├── Question4/
│   ├── README.md                      (Command Pipeline Writeup & Latency Analysis)
│   ├── app.log                        (Sample application log file)
│   ├── monitor_logs.sh                (Pipeline execution test harness)
│   ├── error_report.log               (Generated filtered ERROR log)
│   ├── output.log                     (Captured terminal execution log)
│   └── Screenshots/                   (Captured terminal screenshots)
└── Question5/
    ├── README.md                      (Structured Technical Evaluation of Vi Recovery)
    ├── output.log                     (Captured terminal recovery log)
    └── Screenshots/                   (Captured terminal screenshots)
```

---

## Detailed Directory & Module Matrix

| Directory | Module / Topic | Primary Technical Focus | Key Commands / Functions | Status |
| :--- | :--- | :--- | :--- | :---: |
| [`Question1/`](file:///Users/athish/%5B2%5D%20Bits/%5B1%5D%20Command%20Line%20Interface%20SGA%20submmison/BITS-Graded-Lab/Question1) | Shell Scripting & File Processing | Cryptographic Hash Deduplication, Backups & Reporting | `find`, `sha256sum`, `cp`, `mkdir`, Redirections | **COMPLETE** |
| [`Question2/`](file:///Users/athish/%5B2%5D%20Bits/%5B1%5D%20Command%20Line%20Interface%20SGA%20submmison/BITS-Graded-Lab/Question2) | Process Control & Signal Handling | Process Management, SIGTERM Watchdog & Zombie Prevention | `fork()`, `waitpid()`, `kill()`, `signal()`, `SIGTERM` | **COMPLETE** |
| [`Question3/`](file:///Users/athish/%5B2%5D%20Bits/%5B1%5D%20Command%20Line%20Interface%20SGA%20submmison/BITS-Graded-Lab/Question3) | Low-Level Systems I/O | Random Access Binary Database Management | `open()`, `read()`, `write()`, `lseek()`, `close()` | **COMPLETE** |
| [`Question4/`](file:///Users/athish/%5B2%5D%20Bits/%5B1%5D%20Command%20Line%20Interface%20SGA%20submmison/BITS-Graded-Lab/Question4) | Streams & Text Processing | Live Stream Monitoring & Log Filtering Pipeline | `tail -f`, `grep`, `pipe (|)`, `tee`, `2>/dev/null` | **UPDATED & COMPLETE** |
| [`Question5/`](file:///Users/athish/%5B2%5D%20Bits/%5B1%5D%20Command%20Line%20Interface%20SGA%20submmison/BITS-Graded-Lab/Question5) | Technical Systems Documentation | Structured Technical Evaluation of Vi Recovery Systems | `.swp`, `vim -r`, `undofile`, `writebackup`, `presv` | **UPDATED & COMPLETE** |

---

## Strict Command / Output / Explanation Format Guarantee

Every command documented across all subdirectories follows the mandatory format:

```markdown
#### Command
<command>

#### Output
<actual output>

#### Explanation
<1–2 concise sentences explaining what it does, why it was used, and what the output demonstrates>
```

---

## Quick Verification & Build Commands

```bash
# Question 1: Run deduplication script
cd BITS-Graded-Lab/Question1 && ./submission_dedupe.sh sample_submissions backup report.txt error.log && cd ../..

# Question 2: Compile & run process manager
cd BITS-Graded-Lab/Question2 && gcc -Wall -Wextra -std=c11 process_manager.c -o process_manager && ./process_manager && cd ../..

# Question 3: Compile & run employee database manager
cd BITS-Graded-Lab/Question3 && gcc -Wall -Wextra -std=c11 employee_manager.c -o employee_manager && ./employee_manager && cd ../..

# Question 4: Run live log pipeline harness
cd BITS-Graded-Lab/Question4 && ./monitor_logs.sh && cd ../..

# Question 5: Run swap recovery check
cd BITS-Graded-Lab/Question5 && vim -L && cd ../..
```

---

## Final Submission Verification Checklist
- [x] Questions 1–3 remain completely intact and unchanged.
- [x] Question 4 refined to focus prominently on the Linux command pipeline.
- [x] Question 5 structured as a technical evaluation covering all 5 mechanisms with comparison table & recommendation.
- [x] Every command adheres to the Command / Output / Explanation format.
- [x] All command outputs captured from genuine live execution.
- [x] Repository structured and submission-ready for GitHub push.
