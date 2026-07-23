#!/usr/bin/env bash
# ==============================================================================
# BITS Graded Lab Assignment - Question 4
# Script: monitor_logs.sh
# Purpose: Execution harness for the live log monitoring command pipeline.
# ==============================================================================

set -u

LOG_FILE="app.log"
REPORT_FILE="error_report.log"

# Clean up previous run files
rm -f "$REPORT_FILE"

# Re-create clean initial app.log
cat << 'EOF' > "$LOG_FILE"
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
EOF

# Execute the core command pipeline
# Reads initial entries, filters ERROR lines, streams to console and writes to error_report.log
tail -n +1 "$LOG_FILE" 2>/dev/null | grep "ERROR" | tee "$REPORT_FILE"

# Simulate live incoming log records by appending new lines
echo "[2026-07-23 10:01:02] [ERROR] [NetworkService] Interface eth0 packet loss exceeded 15% threshold" >> "$LOG_FILE"
echo "[2026-07-23 10:01:05] [INFO]  [NetworkService] Traffic rerouted to secondary interface eth1" >> "$LOG_FILE"
echo "[2026-07-23 10:01:10] [ERROR] [Database] Failed to execute SQL query: Deadlock detected on record #9402" >> "$LOG_FILE"

# Process newly appended ERROR entries into the report file
tail -n 2 "$LOG_FILE" 2>/dev/null | grep "ERROR" | tee -a "$REPORT_FILE"
