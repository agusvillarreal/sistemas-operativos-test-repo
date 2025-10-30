# Final Project: Enterprise Storage Manager
## Linux Systems Programming - Capstone Project

**Weight:** 300 points (60% of final grade or standalone project)  
**Duration:** 6 weeks  
**Difficulty:** ⭐⭐⭐⭐⭐ (Very Hard - Integrates ALL course concepts)  
**Team Size:** 2-3 students (or individual with reduced scope)

---

## Project Overview

You will design and implement a **complete enterprise-grade storage management system** that integrates all concepts from the course plus advanced file system administration. This is a **real-world** system similar to tools used by system administrators and storage engineers.

### What You'll Build

A production-ready storage management system with:
- Multi-threaded daemon that manages storage resources
- RAID array creation and monitoring
- LVM (Logical Volume Manager) implementation
- Multiple filesystem support (ext4, xfs, btrfs)
- Automated backup system with snapshots
- Real-time performance monitoring
- Security features (encryption, ACLs)
- IPC-based client-server architecture
- Custom kernel module for advanced statistics
- Comprehensive CLI and optional web interface

---

## Course Integration

### Module 1: System Calls Programming
- Direct system calls: `open()`, `read()`, `write()`, `ioctl()`
- File operations for device management
- Error handling with `errno`
- Memory mapping (`mmap()`)

### Module 2: Process Analysis & Monitoring
- Multi-process architecture (daemon + workers)
- Process monitoring and health checks
- `/proc` filesystem for system information
- Zombie prevention in daemon

### Module 3: CPU Scheduling
- Job scheduling for backup tasks
- Priority queue for operations
- I/O scheduler selection and tuning
- Workload-based optimization

### Module 4: Inter-Process Communication
- UNIX domain sockets for client-server
- Shared memory for status information
- Message queues for async operations
- Semaphores for synchronization

### Module 5: Kernel Modules
- Custom kernel module for storage statistics
- `/proc` filesystem interface
- Device driver concepts
- Kernel-space vs user-space data transfer

### File Systems Administration
- `mkfs`, `mount`, `umount`, `fsck`, `tune2fs`
- RAID management with `mdadm`
- LVM: `pvcreate`, `vgcreate`, `lvcreate`, `lvextend`
- Backup: `rsync`, `tar`, LVM snapshots
- Security: `setfacl`, `getfacl`, `cryptsetup`
- Monitoring: `iostat`, `iotop`, `lsof`, `vmstat`
- Performance: `hdparm`, `sysctl`, I/O schedulers

---

## 🏗️ System Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                     CLIENT APPLICATIONS                          │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐          │
│  │ storage_cli  │  │  Web UI      │  │  Scripts     │          │
│  └──────┬───────┘  └──────┬───────┘  └──────┬───────┘          │
│         │                  │                  │                   │
│         └──────────────────┴──────────────────┘                  │
│                            │                                      │
│                   UNIX Domain Sockets                            │
│                            │                                      │
├────────────────────────────┼──────────────────────────────────────┤
│                    IPC SERVER THREAD                              │
│                            │                                      │
│         ┌──────────────────┴──────────────────┐                 │
│         │    STORAGE DAEMON (Main Process)    │                 │
│         │  ┌────────────────────────────────┐ │                 │
│         │  │  Command Processing Thread     │ │                 │
│         │  │  Monitor Thread                │ │                 │
│         │  │  Backup Thread                 │ │                 │
│         │  │  Health Check Thread           │ │                 │
│         │  └────────────────────────────────┘ │                 │
│         └──────────────────┬──────────────────┘                 │
│                            │                                      │
│         ┌──────────────────┴──────────────────┐                 │
│         │                                      │                  │
│    ┌────▼─────┐  ┌──────▼────────┐  ┌────▼────────┐           │
│    │  RAID    │  │     LVM       │  │  Filesystem │           │
│    │ Manager  │  │   Manager     │  │   Manager   │           │
│    └────┬─────┘  └──────┬────────┘  └────┬────────┘           │
│         │                │                 │                     │
│    ┌────▼─────┐  ┌──────▼────────┐  ┌────▼────────┐           │
│    │ Backup   │  │   Security    │  │  Monitor    │           │
│    │ Engine   │  │   Manager     │  │   System    │           │
│    └──────────┘  └───────────────┘  └─────────────┘           │
│                                                                   │
├───────────────────────────────────────────────────────────────────┤
│               KERNEL MODULE (storage_stats)                       │
│         ┌─────────────────────────────────────┐                 │
│         │  /proc/storage_stats                │                 │
│         │  I/O tracking, latency metrics      │                 │
│         └─────────────────────────────────────┘                 │
└───────────────────────────────────────────────────────────────────┘
```

---

## Requirements & Specifications

### Part 1: RAID Management (30 points)

**Objective:** Implement software RAID array management using `mdadm`.

**Required Functionality:**

1. **RAID Array Creation**
   - Support RAID 0 (striping)
   - Support RAID 1 (mirroring)
   - Support RAID 5 (striping with parity)
   - Support RAID 10 (mirroring + striping)
   - Configurable chunk size

2. **RAID Monitoring**
   - Check array status (active, degraded, failed)
   - Monitor all disks in the array
   - Detect disk failures
   - Parse `/proc/mdstat`

3. **RAID Operations**
   - Add disk to array
   - Remove disk from array
   - Rebuild degraded array
   - Stop/start arrays

4. **Alerting**
   - Send alerts on array degradation
   - Log all RAID events
   - Email notifications (optional)

**Implementation Hints:**
```c
// raid_manager.h
typedef struct {
    char name[64];          // e.g., "/dev/md0"
    int raid_level;         // 0, 1, 5, 10
    int num_devices;
    char devices[16][64];   // Array of device paths
    char status[32];        // "active", "degraded", "failed"
    int num_failed;
} raid_array_t;

int raid_create(const char *array_name, int level, char **devices, int count);
int raid_monitor(raid_array_t *array);
int raid_add_disk(const char *array_name, const char *device);
int raid_fail_disk(const char *array_name, const char *device);
int raid_remove_disk(const char *array_name, const char *device);
int raid_stop(const char *array_name);
```

**Testing:**
```bash
# Test RAID 1 creation
./storage_cli raid create /dev/md0 --level=1 --devices=/dev/loop0,/dev/loop1

# Simulate disk failure
./storage_cli raid fail /dev/md0 /dev/loop0

# Monitor and verify degraded state
./storage_cli raid status /dev/md0

# Add replacement disk
./storage_cli raid add /dev/md0 /dev/loop2

# Verify rebuild
watch cat /proc/mdstat
```

**Grading Rubric:**
- Array creation (all 4 levels) 
- Status monitoring and parsing
- Disk operations (add/remove/fail)
- Error handling and logging

---

### Part 2: LVM Implementation (35 points)

**Objective:** Implement Logical Volume Manager operations.

**Required Functionality:**

1. **Physical Volume (PV) Management**
   - Create PV: `pvcreate`
   - List PVs: `pvdisplay`
   - Remove PV: `pvremove`

2. **Volume Group (VG) Management**
   - Create VG: `vgcreate`
   - Extend VG: `vgextend`
   - List VGs: `vgdisplay`
   - Remove VG: `vgremove`

3. **Logical Volume (LV) Management**
   - Create LV: `lvcreate`
   - Extend LV: `lvextend`
   - List LVs: `lvdisplay`
   - Remove LV: `lvremove`

4. **Snapshot Management**
   - Create snapshot: `lvcreate -s`
   - Merge snapshot: `lvconvert --merge`
   - Remove snapshot

5. **Thin Provisioning** (Bonus +10 points)
   - Create thin pool
   - Create thin volumes

**Implementation Hints:**
```c
// lvm_manager.h
typedef struct {
    char pv_name[64];
    unsigned long long size_bytes;
    char vg_name[64];
} pv_info_t;

typedef struct {
    char vg_name[64];
    unsigned long long size_bytes;
    unsigned long long free_bytes;
    int pv_count;
    int lv_count;
} vg_info_t;

typedef struct {
    char lv_name[64];
    char vg_name[64];
    unsigned long long size_bytes;
    char lv_path[256];      // e.g., "/dev/vg0/lv0"
    int is_snapshot;
    char origin[64];        // If snapshot, name of origin
} lv_info_t;

int lvm_pv_create(const char *device);
int lvm_vg_create(const char *vg_name, char **pvs, int pv_count);
int lvm_lv_create(const char *vg_name, const char *lv_name, 
                  unsigned long long size_mb);
int lvm_lv_extend(const char *vg_name, const char *lv_name, 
                  unsigned long long add_size_mb);
int lvm_snapshot_create(const char *vg_name, const char *origin_lv,
                        const char *snapshot_name, unsigned long long size_mb);
```

**Testing:**
```bash
# Create LVM stack
./storage_cli lvm pv-create /dev/loop3
./storage_cli lvm pv-create /dev/loop4
./storage_cli lvm vg-create vg0 /dev/loop3 /dev/loop4

# Create logical volume
./storage_cli lvm lv-create vg0 data 1G

# Create filesystem on LV
./storage_cli fs create /dev/vg0/data ext4

# Create snapshot
./storage_cli lvm snapshot vg0 data snap1 500M

# Extend logical volume
./storage_cli lvm lv-extend vg0 data +500M
```

**Grading Rubric:**
- PV operations
- VG operations
- LV operations
- Snapshot management

---

### Part 3: Filesystem Operations (25 points)

**Objective:** Support multiple filesystem types with full lifecycle management.

**Required Functionality:**

1. **Filesystem Creation**
   - ext4 with `mkfs.ext4`
   - xfs with `mkfs.xfs`
   - btrfs with `mkfs.btrfs` (bonus)
   - Configurable parameters (block size, inode count, etc.)

2. **Mounting Operations**
   - Mount filesystems with options
   - Unmount safely
   - Parse `/proc/mounts`
   - Handle mount points

3. **Filesystem Health**
   - Check filesystem: `fsck` (ext4)
   - Repair filesystem: `xfs_repair` (xfs)
   - Tune parameters: `tune2fs`, `xfs_admin`

4. **Filesystem Resize**
   - Extend ext4: `resize2fs`
   - Grow xfs: `xfs_growfs`
   - Shrink ext4 (unmount required)

**Implementation Hints:**
```c
// filesystem_ops.h
typedef enum {
    FS_TYPE_EXT4,
    FS_TYPE_XFS,
    FS_TYPE_BTRFS
} fs_type_t;

typedef struct {
    char device[256];
    char mount_point[256];
    fs_type_t type;
    char options[256];
    unsigned long long total_bytes;
    unsigned long long used_bytes;
    unsigned long long available_bytes;
} fs_info_t;

int fs_create(const char *device, fs_type_t type, const char *label);
int fs_mount(const char *device, const char *mount_point, 
             fs_type_t type, const char *options);
int fs_unmount(const char *mount_point, int force);
int fs_check(const char *device, fs_type_t type);
int fs_resize(const char *device, fs_type_t type, 
              unsigned long long new_size_mb);
int fs_get_info(const char *mount_point, fs_info_t *info);
```

**Testing:**
```bash
# Create and mount ext4
./storage_cli fs create /dev/vg0/data ext4 --label=data_vol
./storage_cli fs mount /dev/vg0/data /mnt/data

# Write test data
dd if=/dev/urandom of=/mnt/data/testfile bs=1M count=100

# Check filesystem
./storage_cli fs check /dev/vg0/data

# Extend and resize
./storage_cli lvm lv-extend vg0 data +1G
./storage_cli fs resize /dev/vg0/data ext4

# Verify
df -h /mnt/data
```

**Grading Rubric:**
- Filesystem creation (multiple types)
- Mount/unmount operations
- Health check and repair
- Resize operations

---

### Part 4: Virtual Memory Management (20 points)

**Objective:** Manage swap space and monitor virtual memory.

**Required Functionality:**

1. **Swap Management**
   - Create swap file or partition
   - Enable swap: `swapon`
   - Disable swap: `swapoff`
   - Configure swap priority

2. **Memory Monitoring**
   - Parse `/proc/meminfo`
   - Track: total, free, cached, buffers
   - Monitor swap usage
   - Calculate memory pressure

3. **Automatic Swap Adjustment**
   - Detect low memory conditions
   - Dynamically create swap if needed
   - Warn on high swap usage

**Implementation Hints:**
```c
// memory_manager.h
typedef struct {
    unsigned long long total_kb;
    unsigned long long free_kb;
    unsigned long long available_kb;
    unsigned long long cached_kb;
    unsigned long long buffers_kb;
    unsigned long long swap_total_kb;
    unsigned long long swap_free_kb;
    float memory_pressure;  // 0.0 to 1.0
} memory_info_t;

int swap_create(const char *path, unsigned long long size_mb);
int swap_enable(const char *path, int priority);
int swap_disable(const char *path);
int memory_get_info(memory_info_t *info);
int memory_check_pressure(memory_info_t *info);
```

**Testing:**
```bash
# Create swap file
./storage_cli swap create /swapfile 2G

# Enable swap
./storage_cli swap enable /swapfile --priority=10

# Monitor memory
./storage_cli memory status

# Stress test (requires stress tool)
stress --vm 2 --vm-bytes 1G --timeout 60s &
watch -n 1 ./storage_cli memory status
```

**Grading Rubric:**
- Swap creation and management
- Memory monitoring
- Automatic detection and alert

---

### Part 5: Security Features (30 points)

**Objective:** Implement comprehensive security for storage resources.

**Required Functionality:**

1. **Access Control Lists (ACLs)**
   - Set ACLs: `setfacl`
   - Get ACLs: `getfacl`
   - Recursive ACL application
   - Default ACLs for directories

2. **Encryption**
   - LUKS encryption: `cryptsetup`
   - Create encrypted volumes
   - Open/close encrypted devices
   - Key management

3. **Advanced Permissions**
   - Immutable files: `chattr +i`
   - Append-only: `chattr +a`
   - List attributes: `lsattr`

4. **Audit Logging**
   - Log all privileged operations
   - User/timestamp/operation tracking
   - Tamper-evident logs

**Implementation Hints:**
```c
// security_manager.h
typedef struct {
    char user[64];
    char permissions[16];   // "rwx", "r-x", etc.
} acl_entry_t;

typedef struct {
    char device[256];
    char name[64];
    int is_open;
    char dm_name[64];       // e.g., "luks-data"
} encrypted_volume_t;

int acl_set(const char *path, const char *user, const char *perms);
int acl_get(const char *path, acl_entry_t *entries, int *count);
int acl_remove(const char *path, const char *user);

int luks_format(const char *device, const char *passphrase);
int luks_open(const char *device, const char *name, 
              const char *passphrase);
int luks_close(const char *name);

int audit_log(const char *operation, const char *user, 
              const char *details);
```

**Testing:**
```bash
# Create encrypted volume
./storage_cli security encrypt /dev/vg0/secure --name=secure_vol

# Open encrypted volume
./storage_cli security open secure_vol

# Create filesystem on encrypted device
./storage_cli fs create /dev/mapper/secure_vol ext4

# Set ACLs
./storage_cli security acl-set /mnt/secure user:alice:rwx
./storage_cli security acl-set /mnt/secure user:bob:r-x

# Make file immutable
./storage_cli security chattr /mnt/secure/important.txt +i

# View audit log
./storage_cli security audit-log
```

**Grading Rubric:**
- ACL implementation
- Advanced attributes
- Audit logging

---

### Part 6: Monitoring System (30 points)

**Objective:** Comprehensive real-time monitoring and statistics.

**Required Functionality:**

1. **I/O Monitoring**
   - Track read/write operations
   - Monitor I/O bandwidth
   - Latency measurements
   - Per-device statistics

2. **Resource Tracking**
   - Disk usage monitoring
   - Inode usage
   - Open file descriptors: `lsof`
   - Process I/O: `/proc/[pid]/io`

3. **Performance Metrics**
   - IOPS (I/O operations per second)
   - Throughput (MB/s)
   - Average latency
   - Queue depth

4. **Historical Data**
   - Time-series data collection
   - Store metrics in database (SQLite)
   - Graph generation (bonus)

**Implementation Hints:**
```c
// monitor.h
typedef struct {
    char device[64];
    unsigned long long reads;
    unsigned long long writes;
    unsigned long long read_bytes;
    unsigned long long write_bytes;
    double avg_read_latency_ms;
    double avg_write_latency_ms;
    int queue_depth;
} device_stats_t;

typedef struct {
    time_t timestamp;
    double iops;
    double throughput_mbs;
    double latency_ms;
    int active_requests;
} performance_sample_t;

int monitor_get_device_stats(const char *device, device_stats_t *stats);
int monitor_get_io_stats(const char *device, device_stats_t *stats);
int monitor_track_performance(const char *device, 
                               performance_sample_t *sample);
int monitor_list_open_files(const char *mount_point, 
                            char ***files, int *count);
```

**Testing:**
```bash
# Start monitoring
./storage_cli monitor start --interval=1

# Generate I/O load
dd if=/dev/zero of=/mnt/data/testfile bs=1M count=1000

# View real-time stats
./storage_cli monitor stats /dev/vg0/data

# View historical data
./storage_cli monitor history /dev/vg0/data --last=1h

# Generate report
./storage_cli monitor report --output=report.txt
```

**Grading Rubric:**
- I/O monitoring
- Resource tracking
- Performance metrics
- Historical data

---

### Part 7: Backup System (35 points)

**Objective:** Automated backup with verification and recovery.

**Required Functionality:**

1. **Snapshot-Based Backup**
   - LVM snapshots for consistent backups
   - Btrfs snapshots (bonus)
   - Snapshot lifecycle management

2. **Incremental Backups**
   - Full backup with `tar` or `rsync`
   - Incremental with `rsync --link-dest`
   - Differential backups

3. **Backup Scheduling**
   - Cron-like scheduling
   - Systemd timers (bonus)
   - Retention policies (keep N backups)

4. **Verification & Recovery**
   - Verify backup integrity (checksums)
   - Test restore capability
   - Restore individual files or full system

**Implementation Hints:**
```c
// backup_engine.h
typedef enum {
    BACKUP_FULL,
    BACKUP_INCREMENTAL,
    BACKUP_DIFFERENTIAL
} backup_type_t;

typedef struct {
    char backup_id[64];
    time_t timestamp;
    backup_type_t type;
    char source_path[256];
    char dest_path[256];
    unsigned long long size_bytes;
    char checksum[65];      // SHA256
    int success;
} backup_info_t;

int backup_create(const char *source, const char *dest, 
                  backup_type_t type);
int backup_create_snapshot(const char *vg_name, const char *lv_name,
                            const char *snapshot_name);
int backup_verify(const char *backup_id);
int backup_restore(const char *backup_id, const char *dest);
int backup_list(backup_info_t **backups, int *count);
int backup_cleanup(int keep_count);
```

**Testing:**
```bash
# Initial full backup
./storage_cli backup create /mnt/data /backup/data --type=full

# Modify some files
echo "test" >> /mnt/data/file1.txt

# Incremental backup
./storage_cli backup create /mnt/data /backup/data --type=incremental

# List backups
./storage_cli backup list

# Verify backup
./storage_cli backup verify backup-2025-10-27-001

# Restore test
./storage_cli backup restore backup-2025-10-27-001 /restore/test

# Automated scheduling
./storage_cli backup schedule --cron="0 2 * * *" --keep=7
```

**Grading Rubric:**
- Snapshot management
- Full and incremental backups
- Scheduling
- Verification and restore

---

### Part 8: Performance Tuning (20 points)

**Objective:** Optimize storage performance based on workload.

**Required Functionality:**

1. **I/O Scheduler Selection**
   - Support: `noop`, `deadline`, `cfq`, `mq-deadline`, `bfq`, `kyber`
   - Switch schedulers dynamically
   - Recommend scheduler based on workload

2. **Block Device Tuning**
   - Read-ahead adjustment
   - Queue depth configuration
   - DMA settings: `hdparm`

3. **Kernel Parameters**
   - Adjust `sysctl` parameters
   - `vm.swappiness`
   - `vm.dirty_ratio`
   - `vm.vfs_cache_pressure`

4. **Benchmarking**
   - Measure performance before/after tuning
   - Sequential read/write tests
   - Random I/O tests
   - Generate tuning recommendations

**Implementation Hints:**
```c
// performance_tuner.h
typedef enum {
    WORKLOAD_DATABASE,
    WORKLOAD_WEB_SERVER,
    WORKLOAD_FILE_SERVER,
    WORKLOAD_GENERAL
} workload_type_t;

typedef struct {
    char scheduler[32];
    int read_ahead_kb;
    int queue_depth;
    int vm_swappiness;
    int vm_dirty_ratio;
} tuning_profile_t;

int perf_set_scheduler(const char *device, const char *scheduler);
int perf_get_scheduler(const char *device, char *scheduler);
int perf_set_readahead(const char *device, int size_kb);
int perf_benchmark(const char *device, const char *output_file);
int perf_recommend(const char *device, workload_type_t workload,
                   tuning_profile_t *profile);
int perf_apply_profile(const char *device, const tuning_profile_t *profile);
```

**Testing:**
```bash
# Benchmark current configuration
./storage_cli perf benchmark /dev/vg0/data

# Get recommendation
./storage_cli perf recommend /dev/vg0/data --workload=database

# Apply tuning
./storage_cli perf tune /dev/vg0/data --scheduler=deadline --readahead=2048

# Re-benchmark
./storage_cli perf benchmark /dev/vg0/data

# Compare results
./storage_cli perf compare before.txt after.txt
```

**Grading Rubric:**
- I/O scheduler management
- Block device tuning
- Kernel parameter tuning
- Benchmarking and recommendations

---

### Part 9: IPC Architecture (25 points)

**Objective:** Robust multi-client communication system.

**Required Functionality:**

1. **UNIX Domain Sockets**
   - Server listens on `/var/run/storage_mgr.sock`
   - Multi-client support (select/poll/epoll)
   - Non-blocking I/O

2. **Shared Memory**
   - System status in shared memory
   - Real-time statistics
   - Lock-free reading (atomic operations)

3. **Message Queues**
   - Async operation requests
   - Job queue for backup tasks
   - Priority-based processing

4. **Protocol Design**
   - Binary protocol or JSON
   - Request/response pattern
   - Error codes
   - Versioning

**Implementation Hints:**
```c
// ipc_server.h
typedef enum {
    CMD_RAID_CREATE,
    CMD_LVM_CREATE,
    CMD_FS_MOUNT,
    CMD_BACKUP_CREATE,
    CMD_MONITOR_STATS,
    CMD_STATUS
} command_type_t;

typedef struct {
    uint32_t version;
    uint32_t request_id;
    command_type_t command;
    uint32_t payload_size;
    char payload[4096];
} request_t;

typedef struct {
    uint32_t request_id;
    int status;             // 0 = success, <0 = error
    char error_msg[256];
    uint32_t data_size;
    char data[4096];
} response_t;

int ipc_server_init(const char *socket_path);
int ipc_server_run(void);
int ipc_handle_request(int client_fd, request_t *req, response_t *resp);
```

**Testing:**
```bash
# Start daemon
sudo ./storage_daemon

# Multiple clients simultaneously
./storage_cli monitor stats &
./storage_cli raid status /dev/md0 &
./storage_cli lvm list &

# Stress test
for i in {1..100}; do
    ./storage_cli status &
done
wait
```

**Grading Rubric:**
- Socket server implementation
- Shared memory for status
- Message queue for jobs
- Protocol and error handling

---

### Part 10: Kernel Module (20 points)

**Objective:** Kernel module for advanced storage statistics.

**Required Functionality:**

1. **Module Basics**
   - `init` and `exit` functions
   - Module parameters
   - Proper cleanup

2. **/proc Interface**
   - Create `/proc/storage_stats`
   - Readable by user-space
   - Statistics in human-readable format

3. **Statistics Tracking**
   - Count I/O operations
   - Track bytes read/written
   - Measure latency
   - Device-specific stats

4. **User-Space Communication**
   - Read stats from user-space daemon
   - `ioctl` commands (bonus)

**Implementation Hints:**
```c
// storage_stats.c (kernel module)
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

static struct proc_dir_entry *proc_entry;

typedef struct {
    unsigned long read_ops;
    unsigned long write_ops;
    unsigned long long read_bytes;
    unsigned long long write_bytes;
} storage_stats_t;

static storage_stats_t stats;

static int storage_stats_show(struct seq_file *m, void *v) {
    seq_printf(m, "Read Operations: %lu\n", stats.read_ops);
    seq_printf(m, "Write Operations: %lu\n", stats.write_ops);
    seq_printf(m, "Bytes Read: %llu\n", stats.read_bytes);
    seq_printf(m, "Bytes Written: %llu\n", stats.write_bytes);
    return 0;
}

// Module init/exit, proc_create, etc.
```

**Testing:**
```bash
# Compile module
cd kernel_module
make

# Load module
sudo insmod storage_stats.ko

# Verify loaded
lsmod | grep storage_stats

# Read stats
cat /proc/storage_stats

# Generate I/O
dd if=/dev/zero of=/mnt/data/test bs=1M count=100

# Read stats again
cat /proc/storage_stats

# Unload module
sudo rmmod storage_stats

# Check kernel log
dmesg | tail
```

**Grading Rubric:**
- Module loads/unloads correctly
- /proc interface works
- Statistics tracking
- Proper synchronization:

---

### Part 11: Process Management (15 points)

**Objective:** Robust daemon with worker processes.

**Required Functionality:**

1. **Daemon Creation**
   - Proper daemonization (double fork)
   - PID file: `/var/run/storage_mgr.pid`
   - Detach from terminal
   - Set working directory

2. **Signal Handling**
   - SIGTERM: graceful shutdown
   - SIGHUP: reload configuration
   - SIGCHLD: reap zombie workers
   - SIGUSR1: dump status (bonus)

3. **Worker Processes**
   - Fork workers for long operations
   - Monitor worker health
   - Restart failed workers
   - Limit concurrent workers

4. **Resource Management**
   - Close unnecessary file descriptors
   - Set resource limits (ulimit)
   - Memory cleanup

**Implementation Hints:**
```c
// daemon.h
typedef struct {
    pid_t pid;
    int status;             // 0=idle, 1=busy
    time_t started;
    char task[256];
} worker_t;

int daemon_init(void);
int daemon_create_pidfile(const char *path);
int daemon_setup_signals(void);
void daemon_signal_handler(int sig);
int daemon_spawn_worker(void (*worker_func)(void *), void *arg);
int daemon_monitor_workers(worker_t *workers, int count);
void daemon_shutdown(void);
```

**Testing:**
```bash
# Start daemon
sudo ./storage_daemon

# Check it's running
ps aux | grep storage_daemon
cat /var/run/storage_mgr.pid

# Send reload signal
sudo kill -HUP $(cat /var/run/storage_mgr.pid)

# Graceful shutdown
sudo kill -TERM $(cat /var/run/storage_mgr.pid)

# Verify no zombies
ps aux | grep defunct
```

**Grading Rubric:**
- Proper daemonization
- Signal handling
- Worker management
- Resource cleanup

---

### Part 12: Automation & Scripting (15 points)

**Objective:** Automated maintenance and monitoring scripts.

**Required Scripts:**

1. **Health Check Script** (`health_check.sh`)
   - Check RAID status
   - Check LVM volumes
   - Check filesystem health
   - Check disk space
   - Email alerts on issues

2. **Automated Backup Script** (`auto_backup.sh`)
   - Daily full backup
   - Incremental backups
   - Cleanup old backups
   - Log results

3. **Performance Report** (`perf_report.sh`)
   - Collect system statistics
   - Generate HTML/PDF report
   - Compare with baseline

4. **Systemd Integration** (bonus)
   - Service file for daemon
   - Timer for scheduled tasks
   - Automatic restart on failure

**Implementation Hints:**
```bash
#!/bin/bash
# health_check.sh

check_raid() {
    local array=$1
    if ! mdadm --detail "$array" | grep -q "State : active"; then
        echo "WARNING: RAID array $array is degraded!"
        return 1
    fi
    return 0
}

check_disk_space() {
    local mount=$1
    local threshold=90
    
    usage=$(df -h "$mount" | tail -1 | awk '{print $5}' | sed 's/%//')
    if [ "$usage" -gt "$threshold" ]; then
        echo "WARNING: Disk usage on $mount is ${usage}%"
        return 1
    fi
    return 0
}

# Main health check loop
check_raid /dev/md0
check_disk_space /mnt/data
check_lvm_status
# etc.
```

**Testing:**
```bash
# Run health check
./scripts/health_check.sh

# Schedule with cron
crontab -e
# Add: 0 * * * * /path/to/health_check.sh

# Test backup script
./scripts/auto_backup.sh --dry-run

# Generate performance report
./scripts/perf_report.sh --output=/var/www/html/report.html
```

**Grading Rubric:**
- Health check script
- Backup automation
- Performance reporting
- Systemd integration

---

### Part 13: Documentation (15 points)

**Required Documentation:**

1. **README.md**
   - Project overview
   - Architecture diagram
   - Compilation instructions
   - Usage examples
   - Dependencies

2. **DESIGN.md**
   - System architecture
   - Component interactions
   - Data structures
   - Design decisions
   - Trade-offs

3. **USER_MANUAL.md**
   - Command reference
   - Configuration guide
   - Troubleshooting
   - FAQ

4. **API_REFERENCE.md**
   - All public functions
   - Parameters and return values
   - Usage examples
   - Error codes

**Grading Rubric:**
- README completeness: 4 points
- Design documentation: 4 points
- User manual: 4 points
- Code comments: 3 points

---

---

### Part 14: Presentation (15 points)

**Requirements:**

1. **Live Demo** (15-20 minutes)
   - Show major features working
   - Handle Q&A
   - Demonstrate robustness

2. **Slides** (10-15 slides)
   - Architecture overview
   - Key challenges and solutions
   - Performance results
   - Lessons learned

3. **Demo Video** (5-10 minutes)
   - Screen recording
   - Voice narration
   - Show real usage

**Grading Rubric:**
- Demo quality and completeness
- Presentation clarity
- Q&A handling

---

## Complete Grading Rubric Summary

| Component | Points | Weight |
|-----------|--------|--------|
| 1. RAID Management | 30 | 10% |
| 2. LVM Implementation | 35 | 11.7% |
| 3. Filesystem Operations | 25 | 8.3% |
| 4. Virtual Memory | 20 | 6.7% |
| 5. Security Features | 30 | 10% |
| 6. Monitoring System | 30 | 10% |
| 7. Backup System | 35 | 11.7% |
| 8. Performance Tuning | 20 | 6.7% |
| 9. IPC Architecture | 25 | 8.3% |
| 10. Kernel Module | 20 | 6.7% |
| 11. Process Management | 15 | 5% |
| 12. Automation/Scripting | 15 | 5% |
| 13. Documentation | 15 | 5% |
| 14. Presentation | 15 | 5% |
| **TOTAL** | **300** | **100%** |

### Bonus Points (up to +50)
- Web interface: +20 points
- btrfs support: +15 points
- Thin provisioning: +10 points
- Machine learning predictions: +20 points
- Container integration: +15 points
- **Maximum Total: 350 points**

---

## Project Timeline (6 Weeks)

### Week 1: Foundation & Core Storage (50 points)
**Goals:**
- Set up development environment
- Implement project skeleton
- Complete RAID management (30 pts)
- Start LVM implementation (20 pts)

**Deliverables:**
- Basic project structure
- RAID create/monitor working
- Initial LVM functions

**Checkpoint:** Demo RAID creation and monitoring

---

### Week 2: Storage Management (60 points)
**Goals:**
- Complete LVM implementation (15 pts remaining)
- Filesystem operations (25 pts)
- Virtual memory management (20 pts)

**Deliverables:**
- Full LVM stack working
- Multiple filesystem support
- Swap management

**Checkpoint:** Create LVM volume with filesystem

---

### Week 3: Security & Monitoring (60 points)
**Goals:**
- Security features (30 pts)
- Monitoring system (30 pts)

**Deliverables:**
- ACLs and encryption working
- Real-time monitoring dashboard
- Historical data collection

**Checkpoint:** Demo encrypted volume with monitoring

---

### Week 4: Backup & Performance (55 points)
**Goals:**
- Backup system (35 pts)
- Performance tuning (20 pts)

**Deliverables:**
- Automated backup working
- Scheduler for backups
- Performance benchmarks

**Checkpoint:** Automated backup with restore

---

### Week 5: IPC & System Integration (55 points)
**Goals:**
- IPC architecture (25 pts)
- Kernel module (20 pts)
- Process management (15 pts)

**Deliverables:**
- Client-server working
- Kernel module loaded
- Daemon with workers

**Checkpoint:** Multiple clients communicating

---

### Week 6: Testing, Documentation & Presentation (45 points)
**Goals:**
- Automation scripts (15 pts)
- Complete documentation (15 pts)
- Testing suite (15 pts)
- Presentation prep (15 pts)

**Deliverables:**
- All tests passing
- Complete documentation
- Demo video
- Presentation slides

**Final:** Project presentation and submission

---

## Development Environment

### Required Software
```bash
# Compiler and build tools
sudo apt install build-essential gcc make cmake

# Development tools
sudo apt install git valgrind gdb strace

# Storage tools
sudo apt install mdadm lvm2 xfsprogs btrfs-progs

# Security tools
sudo apt install cryptsetup acl attr

# Monitoring tools
sudo apt install sysstat iotop lsof

# Kernel development
sudo apt install linux-headers-$(uname -r)

# Optional: GUI development
sudo apt install libgtk-3-dev

# Testing framework
sudo apt install libcunit1-dev

# Documentation
sudo apt install doxygen graphviz
```

### Recommended Setup
```bash
# Use a virtual machine!
# VirtualBox or VMware with:
# - Ubuntu 22.04 LTS or newer
# - 4 GB RAM minimum
# - 40 GB disk (for testing)
# - Multiple virtual disks for RAID testing

# Create loop devices for testing
for i in {0..7}; do
    dd if=/dev/zero of=/tmp/disk$i.img bs=1M count=1024
    sudo losetup /dev/loop$i /tmp/disk$i.img
done
```

### Project Structure
```
storage_manager/
├── src/
│   ├── main.c
│   ├── daemon.c
│   ├── raid_manager.c
│   ├── lvm_manager.c
│   ├── filesystem_ops.c
│   ├── memory_manager.c
│   ├── security_manager.c
│   ├── monitor.c
│   ├── backup_engine.c
│   ├── performance_tuner.c
│   ├── ipc_server.c
│   └── utils.c
│
├── include/
│   ├── raid_manager.h
│   ├── lvm_manager.h
│   ├── filesystem_ops.h
│   ├── memory_manager.h
│   ├── security_manager.h
│   ├── monitor.h
│   ├── backup_engine.h
│   ├── performance_tuner.h
│   ├── ipc_server.h
│   └── common.h
│
├── cli/
│   └── storage_cli.c
│
├── kernel_module/
│   ├── storage_stats.c
│   └── Makefile
│
├── scripts/
│   ├── health_check.sh
│   ├── auto_backup.sh
│   └── perf_report.sh
│
├── tests/
│   ├── test_raid.c
│   ├── test_lvm.c
│   ├── test_filesystem.c
│   ├── test_backup.c
│   ├── integration_test.sh
│   └── stress_test.c
│
├── docs/
│   ├── README.md
│   ├── DESIGN.md
│   ├── USER_MANUAL.md
│   └── API_REFERENCE.md
│
├── Makefile
├── .gitignore
└── LICENSE
```

---

## Resources & References

### Man Pages (Essential Reading)
```bash
# RAID
man mdadm
man md

# LVM
man pvcreate vgcreate lvcreate lvextend
man lvm

# Filesystems
man mkfs.ext4 mkfs.xfs
man mount umount
man fsck resize2fs xfs_growfs

# Security
man cryptsetup
man setfacl getfacl
man chattr lsattr

# Monitoring
man iostat iotop vmstat
man lsof

# System calls
man 2 open read write ioctl mmap
man 2 fork exec wait
man 2 socket bind listen accept

# Signals
man 7 signal
man 2 sigaction

# /proc filesystem
man 5 proc
```

### Online Resources
- **Linux Documentation Project**: https://www.tldp.org/
- **Kernel Documentation**: https://www.kernel.org/doc/html/latest/
- **LVM HOWTO**: https://tldp.org/HOWTO/LVM-HOWTO/
- **Linux RAID Wiki**: https://raid.wiki.kernel.org/
- **The Linux Programming Interface** (book website)
- **ArchWiki**: https://wiki.archlinux.org/ (excellent system admin docs)

### Example Projects (for inspiration, NOT copying)
- **Stratis**: Modern Linux storage management
- **ZFS on Linux**: Advanced filesystem
- **Ceph**: Distributed storage
- **LVM2 source code**: Real-world implementation

---

## Important Warnings

### Safety First!
1. **ALWAYS use a virtual machine**
   - RAID operations can destroy data
   - Kernel modules can crash the system
   - LVM commands can wipe disks

2. **Never test on production systems**
   - Use loop devices for testing
   - Create dedicated test VM
   - Back up your work frequently

3. **Verify all commands before execution**
   - Double-check device paths
   - Use `--dry-run` flags when available
   - Test on empty devices first

### Common Pitfalls
1. **Forgetting to check return values**
   - ALL system calls can fail
   - Always check and handle errors

2. **Resource leaks**
   - Close file descriptors
   - Free allocated memory
   - Unmount filesystems
   - Remove LVM volumes

3. **Race conditions**
   - Use proper locking (mutexes/semaphores)
   - Be careful with shared memory
   - Test with multiple clients

4. **Privilege escalation**
   - Most operations require root
   - Use `sudo` carefully
   - Drop privileges when possible

5. **Kernel module bugs**
   - Can crash entire system
   - Always test in VM
   - Check `dmesg` for errors

---

## Team Guidelines

### For Teams of 2-3 Students

**Recommended Division:**

**Student 1: Storage Foundation**
- RAID management
- LVM implementation
- Filesystem operations
- Kernel module

**Student 2: System Integration**
- IPC architecture
- Process management
- Monitoring system
- Performance tuning

**Student 3: High-Level Features** (if team of 3)
- Security features
- Backup system
- Automation scripts
- Documentation & testing

### Collaboration Tools
- **Git/GitHub**: Version control (REQUIRED)
- **Discord/Slack**: Communication
- **Google Docs**: Shared documentation
- **Trello/Jira**: Task tracking

### Meetings
- Weekly team meetings (minimum)
- Daily standups (recommended)
- Code reviews before merging
- Pair programming for complex parts

---

## Submission Requirements

### What to Submit

1. **Source Code** (via Git repository)
   - All `.c` and `.h` files
   - Makefiles
   - Scripts
   - Kernel module code

2. **Documentation** (in `docs/` directory)
   - README.md
   - DESIGN.md
   - USER_MANUAL.md
   - API_REFERENCE.md

3. **Presentation Materials**
   - Slides (PDF)
   - Demo video (MP4, uploaded to YouTube/similar)

4. **Project Report** (15-20 pages, PDF)
   - Introduction
   - Architecture & Design
   - Implementation Details
   - Performance Analysis
   - Challenges & Solutions
   - Conclusion
   - Individual Contributions (for teams)

### Submission Format
```
project_submission/
├── code/                    # Complete source code
├── docs/                    # All documentation
├── tests/                   # Test suite
├── presentation/
│   ├── slides.pdf
│   └── demo_video_link.txt
├── report.pdf
└── README.txt               # Submission checklist
```

### Deadline
- **Week 6, Friday 11:59 PM**
- No late submissions accepted (unless medical emergency)
- Submit via course management system

---

##  Academic Integrity

**We WILL check:**
- Git commit history (should show incremental development)
- Code similarity with classmates
- Code style consistency
- Understanding during demo

---

## ❓ FAQ

### Q: Can I use a different programming language?
**A:** No. This must be done in C to demonstrate systems programming skills.

### Q: Can I use existing libraries like libblkid?
**A:** Yes, you can use standard system libraries, but implement the high-level logic yourself.

### Q: What if I break my VM?
**A:** That's why we use VMs! Take snapshots frequently. You can always restore or create a new one.

### Q: Do I need to implement EVERYTHING?
**A:** Yes for full credit. However, you can prioritize based on the grading rubric. Aim for at least 240/300 points (80%).

### Q: Can I work alone?
**A:** Yes, but the scope expectations are the same. Consider reducing optional/bonus features.

### Q: My RAID array won't create. Help?
**A:** Check:
1. Are devices clean? (`mdadm --zero-superblock /dev/loop0`)
2. Do you have root privileges?
3. Are devices the same size?
4. Check `dmesg` for errors

### Q: How do I debug the kernel module?
**A:** Use `printk()` and check `dmesg`. Be careful with crashes - use VM snapshots!

### Q: Can I use Docker containers?
**A:** No for testing RAID/LVM (needs real block devices), but yes for the web interface (bonus).

### Q: Is there a reference implementation?
**A:** No. This is your original work. We provide specifications, not solutions.

---

## Evaluation Criteria

### Code Quality (implicit in all sections)
- **Correctness**: Does it work as specified?
- **Robustness**: Handles errors gracefully
- **Efficiency**: Reasonable performance
- **Style**: Consistent, readable code
- **Comments**: Well-documented

### What Earns an A (270-300 points)
- All core features work reliably
- Excellent error handling
- Clean, well-documented code
- Comprehensive testing
- Professional presentation
- Some bonus features

### What Earns a B (240-269 points)
- Most features work
- Good error handling
- Decent code quality
- Adequate testing
- Good presentation

### What Earns a C (210-239 points)
- Basic features work
- Some error handling
- Acceptable code quality
- Some testing
- Satisfactory presentation

### What Fails (<210 points)
- Missing major components
- Doesn't compile/run
- Poor code quality
- No testing
- Academic integrity violations

---

## Bonus Opportunities

### Extra Features (up to +50 points)

1. **Web Interface** (+20 points)
   - Dashboard with real-time updates
   - WebSocket or Server-Sent Events
   - Charts and graphs
   - Mobile-responsive

2. **btrfs Support** (+15 points)
   - Subvolumes
   - Snapshots
   - Compression
   - RAID (btrfs native)

3. **Thin Provisioning** (+10 points)
   - LVM thin pools
   - Thin volumes
   - Automatic extension

4. **Machine Learning** (+20 points)
   - Predict disk failures
   - Anomaly detection
   - Performance prediction
   - Auto-tuning based on ML

5. **Container Integration** (+15 points)
   - Docker volume plugin
   - Kubernetes CSI driver
   - Container-aware backups

6. **High Availability** (+15 points)
   - Failover support
   - Replication
   - Distributed storage

**Note:** Bonus points are awarded for fully working, well-tested features only.

---

### Debugging Help
When asking for help, provide:
1. What you're trying to do
2. What you expected
3. What actually happened
4. Error messages (exact text)
5. What you've tried
6. Relevant code snippet (not entire file)

---

## 🚀 Ready to Start?

### Checklist Before Beginning

✅ Read this entire guide  
✅ Set up development VM  
✅ Install all required tools  
✅ Create project Git repository  
✅ Read relevant man pages  
✅ Understand the architecture  
✅ Form team (if applicable)  
✅ Create project plan  
✅ Schedule weekly meetings  
✅ Take VM snapshot!

### First Steps

1. **Week 1, Day 1**: Environment setup
2. **Week 1, Day 2**: Project skeleton
3. **Week 1, Day 3-4**: RAID implementation
4. **Week 1, Day 5-7**: LVM basics
5. **Week 1, Weekend**: Checkpoint review

### Stay on Track

- Review timeline weekly
- Commit code daily
- Test frequently
- Document as you go
- Ask for help early

---

## 🎯 Final Tips for Success

### Do's ✅
- **Start early** - This is a complex project
- **Test continuously** - Don't wait until the end
- **Commit often** - Good Git history helps debugging
- **Read error messages** - They usually tell you what's wrong
- **Use tools** - `strace`, `gdb`, `valgrind` are your friends
- **Take breaks** - Fresh eyes catch bugs
- **Backup everything** - Git + external backup
- **Help teammates** - But don't share code

### Don'ts ❌
- **Don't procrastinate** - 6 weeks seems long but isn't
- **Don't skip error handling** - Will cost you later
- **Don't test on real hardware** - VMs only!
- **Don't ignore warnings** - Fix them immediately
- **Don't work in isolation** - Communicate with team
- **Don't guess** - Read documentation
- **Don't copy code** - You'll get caught

---

## 🌟 Learning Outcomes

By completing this project, you will:

1. **Master Linux Systems Programming**
   - System calls, IPC, process management
   - Signal handling, memory management
   - Kernel module development

2. **Understand Storage Systems**
   - RAID technologies and applications
   - Logical volume management
   - Filesystem internals
   - Backup strategies

3. **Develop Professional Skills**
   - Large-scale C programming
   - Multi-threaded architecture
   - Error handling and robustness
   - Testing and debugging
   - Technical documentation
   - Team collaboration

4. **Gain Real-World Experience**
   - Enterprise-grade system design
   - Performance optimization
   - Security implementation
   - Production debugging

---

## 📄 License & Copyright

This project guide is provided for educational purposes as part of the Linux Systems Programming course.

**For Students:**
- Use this guide for learning
- Complete the project honestly
- Don't share solutions publicly

**For Instructors:**
- Feel free to adapt for your course
- Maintain academic integrity standards
- Attribute original source

---

**Document Version:** 1.0  
**Last Updated:** October 27, 2025  
**Course:** Linux Systems Programming  
**Project Duration:** 6 weeks (300 points)

---
