# Question 4: Live Log Monitoring & Filtering Command Pipeline

## 1. Question Statement
> **Scenario:**  
> A Linux administrator wants a live log monitoring tool.  
> Design an efficient command pipeline that:  
> * monitors a growing log file  
> * displays new log entries  
> * extracts only `ERROR` lines  
> * writes `ERROR` entries into a report file  
> * suppresses unnecessary output  
>  
> **Use:** `tail`, `grep`, pipes (`|`), `tee` (if useful), output redirection (`>`), error redirection (`2>`), `/dev/null`.  
> **Create:** sample log (`app.log`), execution commands, pipeline explanation, generated report (`error_report.log`). Explain why pipes are efficient.

---

## 2. Objective
The objective of this task is to construct a lightweight, high-performance Linux command pipeline that continuously monitors an actively growing application log file ([`app.log`](file:///Users/athish/%5B2%5D%20Bits/%5B1%5D%20Command%20Line%20Interface%20SGA%20submmison/BITS-Graded-Lab/Question4/app.log)), extracts line-by-line `ERROR` messages in real time, logs extracted errors into a separate report file ([`error_report.log`](file:///Users/athish/%5B2%5D%20Bits/%5B1%5D%20Command%20Line%20Interface%20SGA%20submmison/BITS-Graded-Lab/Question4/error_report.log)), and suppresses diagnostic clutter by redirecting standard error streams to `/dev/null`.

---

## 3. Core Command Pipeline

### Console-Streaming Pipeline
```bash
tail -f app.log 2>/dev/null | grep --line-buffered "ERROR" | tee error_report.log
```

### Background / Silent Logging Pipeline
```bash
tail -f app.log 2>/dev/null | grep --line-buffered "ERROR" | tee error_report.log > /dev/null
```

---

## 4. Component-by-Component Explanation

### Component Breakdown
1. **`tail -f app.log`**: Continuously monitors the growing log file `app.log` and outputs newly appended log lines to standard output as they are written in real time.
2. **`2>/dev/null`**: Redirects standard error (file descriptor 2) from `tail` to the `/dev/null` bit-bucket device, suppressing warning messages (such as temporary file access delays or file rotation warnings).
3. **Pipes (`|`)**: Connects the standard output (`stdout`) of the preceding command directly to the standard input (`stdin`) of the next command in RAM, creating an in-memory stream without writing temporary files to disk.
4. **`grep --line-buffered "ERROR"`**: Inspects incoming stream lines from `tail` and passes only lines matching the text pattern `"ERROR"`. The `--line-buffered` flag forces `grep` to flush every matching line immediately down the pipeline without waiting for standard 4KB buffer accumulation.
5. **`tee error_report.log`**: Duplicates the incoming stream of extracted `ERROR` lines, writing one copy into the destination file `error_report.log` while passing the second copy to standard output.
6. **Output Redirection (`> /dev/null`)**: In the silent pipeline variant, `> /dev/null` redirects the standard output of `tee` to `/dev/null`, writing errors strictly to `error_report.log` while keeping the terminal interface silent.

### Integrated Pipeline Workflow
- **Real-time Monitoring**: `tail -f` tracks live log file growth continuously.
- **Selective Filtering**: `grep` filters out `INFO`, `DEBUG`, and `WARN` lines, passing only `ERROR` entries.
- **Dual Destination & Report Logging**: `tee` simultaneously updates `error_report.log` on disk and renders output to screen.
- **Error Suppression**: Redirection (`2>/dev/null`) discards system warning noise cleanly.

---

## 5. Executed Commands & Output Log

### Command 1
#### Command
```bash
cat app.log
```
#### Output
```text
[2026-07-23 10:00:01] [INFO]  [AuthService] User admin logged in successfully from IP 192.168.1.50
[2026-07-23 10:00:05] [DEBUG] [Database] Connection pool initialized with 20 active connections
[2026-07-23 10:00:12] [WARN]  [StorageService] Disk usage on /var/log reached 78% threshold
[2026-07-23 10:00:15] [ERROR] [PaymentGateway] Timeout connecting to payment gateway api.stripe.com:443 after 5000ms
[2026-07-23 10:00:18] [INFO]  [OrderService] Order #84920 created for customer ID 39201
[2026-07-23 10:00:22] [ERROR] [Database] Failed to acquire lock on table 'orders' - Transaction aborted due to deadlock
[2026-07-23 10:00:25] [DEBUG] [CacheService] Redis key 'user_session_39201' set with TTL 3600s
[2026-07-23 10:00:30] [ERROR] [AuthService] Invalid OAuth token signature from client IP 203.0.113.42
[2026-07-23 10:00:35] [WARN]  [MemoryManager] High memory usage detected: Heap allocated 85% of max pool
[2026-07-23 10:00:40] [INFO]  [Scheduler] Cron job 'daily_backup' started successfully
[2026-07-23 10:00:45] [ERROR] [StorageService] Permission denied writing to /mnt/backup/daily_20260723.tar.gz
[2026-07-23 10:00:50] [INFO]  [HealthCheck] All microservices reporting status UP (200 OK)
```
#### Explanation
* **What it does**: Displays the sample application log file [`app.log`](file:///Users/athish/%5B2%5D%20Bits/%5B1%5D%20Command%20Line%20Interface%20SGA%20submmison/BITS-Graded-Lab/Question4/app.log).
* **Why it was used**: To inspect the initial un-filtered multi-level log dataset containing `INFO`, `DEBUG`, `WARN`, and `ERROR` entries.
* **What was observed**: The raw log file contains 12 lines of mixed operational data with 4 initial `ERROR` entries.

---

### Command 2
#### Command
```bash
./monitor_logs.sh
```
#### Output
```text
[2026-07-23 10:00:15] [ERROR] [PaymentGateway] Timeout connecting to payment gateway api.stripe.com:443 after 5000ms
[2026-07-23 10:00:22] [ERROR] [Database] Failed to acquire lock on table 'orders' - Transaction aborted due to deadlock
[2026-07-23 10:00:30] [ERROR] [AuthService] Invalid OAuth token signature from client IP 203.0.113.42
[2026-07-23 10:00:45] [ERROR] [StorageService] Permission denied writing to /mnt/backup/daily_20260723.tar.gz
[2026-07-23 10:01:10] [ERROR] [Database] Failed to execute SQL query: Deadlock detected on record #9402
```
#### Explanation
* **What it does**: Executes the test harness script [`monitor_logs.sh`](file:///Users/athish/%5B2%5D%20Bits/%5B1%5D%20Command%20Line%20Interface%20SGA%20submmison/BITS-Graded-Lab/Question4/monitor_logs.sh), which runs the core command pipeline while dynamically appending live log lines.
* **Why it was used**: To test live streaming, line filtering, error logging, and error suppression under real execution conditions.
* **What was observed**: Only lines containing the pattern `[ERROR]` were printed to the terminal, including newly appended live lines, while non-error entries were filtered out.

---

### Command 3
#### Command
```bash
cat error_report.log
```
#### Output
```text
[2026-07-23 10:00:15] [ERROR] [PaymentGateway] Timeout connecting to payment gateway api.stripe.com:443 after 5000ms
[2026-07-23 10:00:22] [ERROR] [Database] Failed to acquire lock on table 'orders' - Transaction aborted due to deadlock
[2026-07-23 10:00:30] [ERROR] [AuthService] Invalid OAuth token signature from client IP 203.0.113.42
[2026-07-23 10:00:45] [ERROR] [StorageService] Permission denied writing to /mnt/backup/daily_20260723.tar.gz
[2026-07-23 10:01:10] [ERROR] [Database] Failed to execute SQL query: Deadlock detected on record #9402
```
#### Explanation
* **What it does**: Displays the contents of the generated report file [`error_report.log`](file:///Users/athish/%5B2%5D%20Bits/%5B1%5D%20Command%20Line%20Interface%20SGA%20submmison/BITS-Graded-Lab/Question4/error_report.log).
* **Why it was used**: To verify that `tee` successfully logged all extracted `ERROR` entries to disk.
* **What was observed**: The report file contains 100% accurate `ERROR` entries matching terminal output.

---

## 6. Efficiency Discussion

### 1. In-Memory Kernel Ring Buffering & Zero Intermediate Storage
UNIX pipes (`|`) transfer data strictly in memory via kernel circular ring buffers (64 KB). The producer (`tail`) and consumer (`grep`) stream data directly without creating temporary intermediate files on disk, eliminating storage write amplification.

### 2. Low Memory Footprint & Unbounded Log Streaming
Instead of loading massive multi-gigabyte log files into system RAM, the pipeline processes data line-by-line as a stream. System RAM consumption remains minimal and constant regardless of whether `app.log` is 10 MB or 100 GB.

### 3. Immediate Line Buffering (`grep --line-buffered`)
Standard Linux I/O buffers 4KB of data before flushing down a pipe. For live log monitoring, this introduces unacceptable delays. Using `--line-buffered` forces `grep` to flush every matching error line immediately, ensuring zero-latency monitoring.

### 4. Noise Suppression via `/dev/null`
Redirecting stderr (`2>/dev/null`) to the kernel pseudo-device discards warning messages without cluttering monitoring output or corrupting error report files.

---

## 7. Learning Outcome
Through this exercise, we demonstrated how Linux inter-process communication (IPC) via pipes enables high-performance streaming analytics. We learned how to combine `tail -f`, line-buffered `grep`, and `tee` to build a live log monitoring tool with minimal CPU and RAM footprint.
