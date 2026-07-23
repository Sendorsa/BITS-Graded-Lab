#!/usr/bin/env bash
# ==============================================================================
# BITS Graded Lab Assignment - Question 1
# Script: submission_dedupe.sh
# Purpose: Identifies duplicate assignment submissions, backs up unique files,
#          generates an audit report, and logs errors separately.
# Compatibility: Fully POSIX / Bash 3.2+ compatible (works within sandbox).
# ==============================================================================

set -u  # Treat unset variables as an error

# ------------------------------------------------------------------------------
# Configuration & Setup
# ------------------------------------------------------------------------------
SRC_DIR="${1:-sample_submissions}"
BACKUP_DIR="${2:-backup}"
REPORT_FILE="${3:-report.txt}"
ERROR_LOG="${4:-error.log}"

# Working temporary directory inside current workspace
TMP_DIR="./.tmp_dedupe"
mkdir -p "$TMP_DIR"

HASH_DB="$TMP_DIR/seen_hashes.db"
DUPLICATES_TEMP="$TMP_DIR/dups.log"
BACKUPS_TEMP="$TMP_DIR/backups.log"

# Cleanup temporary files on exit
trap 'rm -rf "$TMP_DIR"' EXIT

# Clear previous outputs and logs
> "$REPORT_FILE" 2>/dev/null || true
> "$ERROR_LOG"   2>/dev/null || true
> "$HASH_DB"     2>/dev/null || true
> "$DUPLICATES_TEMP" 2>/dev/null || true
> "$BACKUPS_TEMP"    2>/dev/null || true

# Ensure backup directory exists
mkdir -p "$BACKUP_DIR" 2>>"$ERROR_LOG"

if [ ! -d "$SRC_DIR" ]; then
    echo "ERROR: Source directory '$SRC_DIR' does not exist." | tee -a "$ERROR_LOG"
    exit 1
fi

# Counters
total_files=0
backed_up_count=0
duplicate_count=0

echo "=================================================="
echo " Starting Assignment Submission Deduplication"
echo " Source Directory : $SRC_DIR"
echo " Backup Directory : $BACKUP_DIR"
echo " Report File      : $REPORT_FILE"
echo " Error Log        : $ERROR_LOG"
echo "=================================================="
echo ""

# ------------------------------------------------------------------------------
# Process Submissions
# ------------------------------------------------------------------------------
# Process each file found in the source directory
while IFS= read -r file; do
    [ -z "$file" ] && continue

    # Check file readability
    if [ ! -r "$file" ]; then
        echo "[ERROR] Cannot read file: $file" >> "$ERROR_LOG"
        continue
    fi

    total_files=$((total_files + 1))

    # Compute hash (try sha256sum, then shasum -a 256, then md5sum)
    file_hash=""
    if command -v sha256sum >/dev/null 2>&1; then
        file_hash=$(sha256sum "$file" 2>>"$ERROR_LOG" | awk '{print $1}')
    elif command -v shasum >/dev/null 2>&1; then
        file_hash=$(shasum -a 256 "$file" 2>>"$ERROR_LOG" | awk '{print $1}')
    else
        file_hash=$(md5sum "$file" 2>>"$ERROR_LOG" | awk '{print $1}')
    fi

    if [ -z "$file_hash" ]; then
        echo "[ERROR] Failed to compute checksum for: $file" >> "$ERROR_LOG"
        continue
    fi

    # Check if hash has already been registered in HASH_DB
    existing_entry=""
    if [ -f "$HASH_DB" ]; then
        existing_entry=$(grep "^${file_hash} " "$HASH_DB" 2>/dev/null | head -n 1 || true)
    fi

    if [ -n "$existing_entry" ]; then
        # Duplicate file detected
        original_file=$(echo "$existing_entry" | cut -d' ' -f2-)
        duplicate_count=$((duplicate_count + 1))
        echo "  - $file (Duplicate of $original_file)" >> "$DUPLICATES_TEMP"
        echo "[DUPLICATE] $file matches hash of $original_file"
    else
        # Unique file detected: register hash and original file path
        echo "${file_hash} ${file}" >> "$HASH_DB"
        filename=$(basename "$file")
        
        # Copy to backup directory
        if cp "$file" "$BACKUP_DIR/$filename" 2>>"$ERROR_LOG"; then
            backed_up_count=$((backed_up_count + 1))
            echo "  - $file -> $BACKUP_DIR/$filename" >> "$BACKUPS_TEMP"
            echo "[BACKUP]    $file -> $BACKUP_DIR/$filename"
        else
            echo "[ERROR] Failed to copy $file to $BACKUP_DIR" >> "$ERROR_LOG"
        fi
    fi
done < <(find "$SRC_DIR" -type f 2>>"$ERROR_LOG" | sort)

# ------------------------------------------------------------------------------
# Generate Summary Report
# ------------------------------------------------------------------------------
{
    echo "=================================================="
    echo "       SUBMISSION DEDUPLICATION REPORT            "
    echo "=================================================="
    echo "Timestamp           : $(date '+%Y-%m-%d %H:%M:%S')"
    echo "Source Directory    : $SRC_DIR"
    echo "Backup Directory    : $BACKUP_DIR"
    echo "Total Files Scanned : $total_files"
    echo "Backed-Up (Unique)  : $backed_up_count"
    echo "Duplicate Files     : $duplicate_count"
    echo "--------------------------------------------------"
    echo "BACKED UP UNIQUE FILES:"
    if [ -s "$BACKUPS_TEMP" ]; then
        cat "$BACKUPS_TEMP"
    else
        echo "  (None)"
    fi
    echo "--------------------------------------------------"
    echo "IDENTIFIED DUPLICATE FILES:"
    if [ -s "$DUPLICATES_TEMP" ]; then
        cat "$DUPLICATES_TEMP"
    else
        echo "  (None)"
    fi
    echo "=================================================="
} | tee "$REPORT_FILE"

echo ""
echo "Deduplication complete. Report saved to $REPORT_FILE."
echo "Errors (if any) saved to $ERROR_LOG."
