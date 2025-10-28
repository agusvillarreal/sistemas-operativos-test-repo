# Module 6: File Systems and Storage Administration
## Advanced Linux Storage Management

**Duration:** 2-3 weeks  
**Prerequisites:** Modules 1-5, root access, VM environment  
**Level:** Advanced

---

## Table of Contents

1. [Introduction to File Systems](#introduction-to-file-systems)
2. [File System Types](#file-system-types)
3. [File System Creation and Management](#file-system-creation-and-management)
4. [RAID Configuration](#raid-configuration)
5. [Logical Volume Management (LVM)](#logical-volume-management-lvm)
6. [Virtual Memory and Swap](#virtual-memory-and-swap)
7. [Security and Permissions](#security-and-permissions)
8. [Monitoring and Troubleshooting](#monitoring-and-troubleshooting)
9. [Backup and Recovery](#backup-and-recovery)
10. [Performance Tuning](#performance-tuning)

---

## Introduction to File Systems

### What is a File System?

A file system is a method and data structure that an operating system uses to:
- Organize and store files on storage devices
- Keep track of file metadata (permissions, timestamps, ownership)
- Manage free space
- Provide hierarchy (directories/folders)

### Key Concepts

**1. Block Devices**
- Fixed-size chunks of data (typically 4KB)
- Addressed by block number
- Examples: `/dev/sda`, `/dev/loop0`

**2. Inodes**
- Data structure storing file metadata
- Contains: permissions, ownership, timestamps, pointers to data blocks
- Does NOT contain filename (stored in directory)

**3. Superblock**
- Critical metadata about the file system
- Contains: size, block size, free blocks, inode count
- Multiple copies stored for redundancy

**4. Mount Points**
- Directory where a file system is attached to the directory tree
- Example: `/dev/sda1` mounted at `/mnt/data`

### File System Architecture

```
Physical Device (e.g., /dev/sda1)
    |
    ├── Superblock (FS metadata)
    ├── Inode Table (file metadata)
    ├── Data Blocks (actual file content)
    └── Free Space Bitmap
```

### The Linux Storage Stack

```
Application Layer
    |
System Call Layer (open, read, write)
    |
Virtual File System (VFS) Layer
    |
File System Driver (ext4, xfs, btrfs)
    |
Block Layer (I/O scheduling)
    |
Device Driver
    |
Physical Device
```

---

## File System Types

### 1. ext4 (Fourth Extended File System)

**Characteristics:**
- Default on many Linux distributions
- Maximum file size: 16 TB
- Maximum volume size: 1 EB (exabyte)
- Journaling for crash recovery
- Extent-based allocation (reduces fragmentation)
- Backward compatible with ext2/ext3

**Best For:**
- General-purpose workloads
- Boot partitions
- Traditional file servers

**Limitations:**
- No built-in snapshots
- No data checksumming
- No transparent compression

**Create ext4:**
```bash
mkfs.ext4 /dev/sdb1
mkfs.ext4 -L mydata /dev/sdb1          # With label
mkfs.ext4 -b 4096 -i 8192 /dev/sdb1    # Custom block/inode ratio
```

### 2. XFS

**Characteristics:**
- High-performance, 64-bit journaling file system
- Excellent for large files and parallel I/O
- Maximum file size: 8 EB
- Maximum volume size: 8 EB
- Delayed allocation (improves performance)
- Online defragmentation

**Best For:**
- Large files (video, databases)
- High-performance requirements
- Parallel I/O workloads

**Limitations:**
- Cannot be shrunk (only grown)
- Requires more memory
- Complex repair process

**Create XFS:**
```bash
mkfs.xfs /dev/sdb1
mkfs.xfs -L mydata -b size=4096 /dev/sdb1
```

### 3. btrfs (B-tree File System)

**Characteristics:**
- Copy-on-write (CoW) file system
- Built-in snapshots
- Built-in RAID support
- Data checksumming (detects corruption)
- Transparent compression
- Online resizing (grow and shrink)

**Best For:**
- Systems requiring snapshots
- Data integrity critical systems
- Modern desktop/server deployments

**Limitations:**
- Still maturing (some RAID levels unstable)
- More complex than ext4
- Higher CPU overhead

**Create btrfs:**
```bash
mkfs.btrfs /dev/sdb1
mkfs.btrfs -L mydata -m raid1 -d raid1 /dev/sdb1 /dev/sdc1
```

### Comparison Table

| Feature | ext4 | XFS | btrfs |
|---------|------|-----|-------|
| Max File Size | 16 TB | 8 EB | 16 EB |
| Max Volume Size | 1 EB | 8 EB | 16 EB |
| Journaling | Yes | Yes | CoW |
| Snapshots | No | No | Yes |
| Compression | No | No | Yes |
| Online Resize | Grow only | Grow only | Both |
| Checksums | No | Optional | Yes |
| Defrag | Online | Online | Online |
| Maturity | Very stable | Very stable | Stable |
| Performance | Good | Excellent | Good |

---

## File System Creation and Management

### Creating File Systems

**Basic Syntax:**
```bash
mkfs.<type> [options] <device>
```

**Examples:**
```bash
# Create ext4 with label
mkfs.ext4 -L my_data /dev/sdb1

# Create XFS with specific block size
mkfs.xfs -b size=4096 /dev/sdb1

# Create btrfs with RAID1
mkfs.btrfs -m raid1 -d raid1 /dev/sdb1 /dev/sdc1

# Force creation (overwrites existing)
mkfs.ext4 -F /dev/sdb1
```

**Important Options:**

For ext4:
- `-b <size>` - Block size (1024, 2048, 4096)
- `-i <bytes-per-inode>` - Inode ratio
- `-L <label>` - Volume label
- `-m <percent>` - Reserved blocks percentage
- `-J <options>` - Journal options

For XFS:
- `-b size=<bytes>` - Block size
- `-d agcount=<num>` - Allocation groups
- `-L <label>` - Volume label
- `-f` - Force overwrite

For btrfs:
- `-m <profile>` - Metadata RAID profile
- `-d <profile>` - Data RAID profile
- `-L <label>` - Volume label

### Mounting File Systems

**Manual Mount:**
```bash
mount <device> <mount_point>
mount -t <type> <device> <mount_point>
mount -o <options> <device> <mount_point>
```

**Examples:**
```bash
# Basic mount
mount /dev/sdb1 /mnt/data

# Specify file system type
mount -t ext4 /dev/sdb1 /mnt/data

# Mount with options
mount -o noatime,nodiratime /dev/sdb1 /mnt/data

# Read-only mount
mount -o ro /dev/sdb1 /mnt/data

# Mount by label
mount -L mydata /mnt/data

# Mount by UUID
mount -U 12345678-1234-1234-1234-123456789012 /mnt/data
```

**Common Mount Options:**

- `noatime` - Don't update access times (improves performance)
- `nodiratime` - Don't update directory access times
- `ro` - Read-only
- `rw` - Read-write
- `noexec` - Don't allow program execution
- `nosuid` - Ignore SUID bits
- `nodev` - Don't interpret device files
- `sync` - Synchronous I/O (slower but safer)
- `async` - Asynchronous I/O (default)
- `defaults` - Default options (rw,suid,dev,exec,auto,nouser,async)

### Unmounting

```bash
umount <mount_point>
umount <device>

# Force unmount (use carefully!)
umount -f /mnt/data

# Lazy unmount (detach now, cleanup when possible)
umount -l /mnt/data
```

**Check what's using a mount:**
```bash
lsof /mnt/data
fuser -v /mnt/data
```

### Persistent Mounts (/etc/fstab)

**Format:**
```
<device>  <mount_point>  <type>  <options>  <dump>  <pass>
```

**Examples:**
```bash
# By device
/dev/sdb1  /mnt/data  ext4  defaults  0  2

# By UUID (recommended)
UUID=12345678-1234-1234-1234-123456789012  /mnt/data  ext4  defaults  0  2

# By label
LABEL=mydata  /mnt/data  ext4  defaults  0  2

# With specific options
UUID=12345678-1234-1234-1234-123456789012  /mnt/data  ext4  noatime,nodiratime  0  2

# Swap entry
UUID=87654321-4321-4321-4321-210987654321  none  swap  sw  0  0
```

**Field Explanations:**
- Field 1: Device (UUID recommended)
- Field 2: Mount point
- Field 3: File system type
- Field 4: Mount options
- Field 5: Dump backup (0=no, 1=yes)
- Field 6: fsck order (0=skip, 1=root, 2=other)

**Testing fstab:**
```bash
# Test mount all entries in fstab
mount -a

# Find UUIDs
blkid
lsblk -f
```

### Checking File Systems

**ext4:**
```bash
# Check and repair
fsck.ext4 /dev/sdb1

# Force check even if clean
fsck.ext4 -f /dev/sdb1

# Automatic repair
fsck.ext4 -y /dev/sdb1

# Check without modifying
fsck.ext4 -n /dev/sdb1
```

**XFS:**
```bash
# Check (must be unmounted)
xfs_repair /dev/sdb1

# Dry run
xfs_repair -n /dev/sdb1

# Force repair
xfs_repair -L /dev/sdb1  # Zeroes log (data loss possible)
```

**btrfs:**
```bash
# Check (can be online)
btrfs check /dev/sdb1

# Scrub (verify checksums)
btrfs scrub start /mnt/data
btrfs scrub status /mnt/data
```

### Tuning File Systems

**ext4 tuning:**
```bash
# View current settings
tune2fs -l /dev/sdb1

# Set reserved blocks percentage
tune2fs -m 1 /dev/sdb1

# Set label
tune2fs -L newlabel /dev/sdb1

# Set max mount count before check
tune2fs -c 30 /dev/sdb1

# Enable/disable features
tune2fs -O has_journal /dev/sdb1
```

**XFS tuning:**
```bash
# View settings
xfs_info /dev/sdb1

# Set label
xfs_admin -L newlabel /dev/sdb1

# Get UUID
xfs_admin -u /dev/sdb1
```

### Resizing File Systems

**ext4:**
```bash
# Grow to fill device (must be unmounted for shrink)
resize2fs /dev/sdb1

# Resize to specific size
resize2fs /dev/sdb1 10G

# Shrink (must unmount first!)
umount /mnt/data
e2fsck -f /dev/sdb1
resize2fs /dev/sdb1 5G
mount /mnt/data
```

**XFS:**
```bash
# Grow (can be online, cannot shrink!)
xfs_growfs /mnt/data

# Grow to specific size
xfs_growfs -D 10000000 /mnt/data  # In blocks
```

**btrfs:**
```bash
# Grow (online)
btrfs filesystem resize +5G /mnt/data

# Shrink (online)
btrfs filesystem resize -2G /mnt/data

# Resize to specific size
btrfs filesystem resize 10G /mnt/data

# Grow to max
btrfs filesystem resize max /mnt/data
```

---

## RAID Configuration

### What is RAID?

RAID (Redundant Array of Independent Disks) combines multiple physical disks into a logical unit for:
- **Redundancy** - Protection against disk failure
- **Performance** - Parallel I/O operations
- **Capacity** - Larger logical volumes

### RAID Levels

**RAID 0 - Striping**
```
Disk 1: [A1][B1][C1][D1]
Disk 2: [A2][B2][C2][D2]
```
- Data striped across disks
- No redundancy
- Capacity: N disks
- Performance: Excellent read/write
- Failure tolerance: None
- Use case: Speed-critical, non-critical data

**RAID 1 - Mirroring**
```
Disk 1: [A][B][C][D]
Disk 2: [A][B][C][D]
```
- Complete duplication
- Full redundancy
- Capacity: N/2 disks
- Performance: Good read, normal write
- Failure tolerance: N-1 disks
- Use case: Critical data, boot drives

**RAID 5 - Striping with Parity**
```
Disk 1: [A1][B1][C1][P ]
Disk 2: [A2][B2][P ][D2]
Disk 3: [A3][P ][C3][D3]
```
- Data + distributed parity
- Redundancy with efficiency
- Capacity: N-1 disks
- Performance: Good read, moderate write
- Failure tolerance: 1 disk
- Use case: File servers, balanced performance/redundancy

**RAID 6 - Double Parity**
```
Disk 1: [A1][B1][P ][P ]
Disk 2: [A2][P ][C2][P ]
Disk 3: [P ][B2][C3][D3]
Disk 4: [A3][P ][P ][D4]
```
- Data + two parity blocks
- Higher redundancy
- Capacity: N-2 disks
- Performance: Good read, slower write
- Failure tolerance: 2 disks
- Use case: Large arrays, critical data

**RAID 10 - Mirror + Stripe**
```
Pair 1:          Pair 2:
Disk 1: [A][C]   Disk 3: [A][C]
Disk 2: [B][D]   Disk 4: [B][D]
```
- Mirrored pairs, then striped
- Best performance + redundancy
- Capacity: N/2 disks
- Performance: Excellent
- Failure tolerance: 1 disk per mirror
- Use case: Databases, high-performance systems

### Creating RAID with mdadm

**Install mdadm:**
```bash
sudo apt install mdadm
```

**Create RAID 1:**
```bash
# Create array
sudo mdadm --create /dev/md0 \
  --level=1 \
  --raid-devices=2 \
  /dev/sdb /dev/sdc

# Monitor creation
watch cat /proc/mdstat

# Create file system
sudo mkfs.ext4 /dev/md0

# Mount
sudo mount /dev/md0 /mnt/raid1
```

**Create RAID 5:**
```bash
sudo mdadm --create /dev/md1 \
  --level=5 \
  --raid-devices=3 \
  --chunk=512 \
  /dev/sdb /dev/sdc /dev/sdd
```

**Create RAID 10:**
```bash
sudo mdadm --create /dev/md2 \
  --level=10 \
  --raid-devices=4 \
  /dev/sdb /dev/sdc /dev/sdd /dev/sde
```

### Managing RAID Arrays

**View array status:**
```bash
# Quick status
cat /proc/mdstat

# Detailed information
sudo mdadm --detail /dev/md0

# Examine disk
sudo mdadm --examine /dev/sdb
```

**Add disk to array:**
```bash
sudo mdadm --manage /dev/md0 --add /dev/sde
```

**Mark disk as failed:**
```bash
sudo mdadm --manage /dev/md0 --fail /dev/sdc
```

**Remove disk from array:**
```bash
sudo mdadm --manage /dev/md0 --remove /dev/sdc
```

**Stop array:**
```bash
sudo umount /mnt/raid1
sudo mdadm --stop /dev/md0
```

**Start/assemble array:**
```bash
sudo mdadm --assemble /dev/md0 /dev/sdb /dev/sdc
```

**Save configuration:**
```bash
sudo mdadm --detail --scan >> /etc/mdadm/mdadm.conf
```

### RAID Monitoring

**Email alerts:**
```bash
# Edit /etc/mdadm/mdadm.conf
MAILADDR your-email@example.com

# Start monitoring daemon
sudo systemctl enable mdmonitor
sudo systemctl start mdmonitor
```

**Manual monitoring script:**
```bash
#!/bin/bash
# check_raid.sh

if grep -q 'U_' /proc/mdstat; then
    echo "WARNING: RAID array degraded!"
    cat /proc/mdstat
    exit 1
fi
echo "RAID arrays healthy"
```

---

## Logical Volume Management (LVM)

### What is LVM?

LVM provides flexible disk management by abstracting physical storage into logical volumes that can be:
- Resized dynamically
- Snapshotted
- Moved between disks
- Combined from multiple disks

### LVM Architecture

```
Physical Volumes (PV)
  /dev/sdb  /dev/sdc  /dev/sdd
       |        |        |
       +--------+--------+
              |
       Volume Group (VG)
            vg0
              |
       +------+------+
       |      |      |
      LV1    LV2    LV3
       |      |      |
    /home  /var  /backup
```

**Components:**
- **Physical Volume (PV)** - Physical disk or partition
- **Volume Group (VG)** - Pool of PVs
- **Logical Volume (LV)** - Virtual partition from VG
- **Physical Extent (PE)** - Small chunk of PV (default 4MB)
- **Logical Extent (LE)** - Small chunk of LV

### Creating LVM Setup

**1. Create Physical Volumes:**
```bash
# Initialize disk for LVM
sudo pvcreate /dev/sdb
sudo pvcreate /dev/sdc /dev/sdd

# View PVs
sudo pvdisplay
sudo pvs  # Short format
```

**2. Create Volume Group:**
```bash
# Create VG from PVs
sudo vgcreate vg0 /dev/sdb /dev/sdc

# View VGs
sudo vgdisplay
sudo vgs

# Extend VG (add more PVs)
sudo vgextend vg0 /dev/sdd
```

**3. Create Logical Volumes:**
```bash
# Create LV with size
sudo lvcreate -L 10G -n lv_data vg0

# Create LV with percentage
sudo lvcreate -l 50%FREE -n lv_backup vg0

# Create LV using all free space
sudo lvcreate -l 100%FREE -n lv_large vg0

# View LVs
sudo lvdisplay
sudo lvs
```

**4. Create File System:**
```bash
sudo mkfs.ext4 /dev/vg0/lv_data
sudo mount /dev/vg0/lv_data /mnt/data
```

### Resizing Logical Volumes

**Extend (grow):**
```bash
# Extend LV
sudo lvextend -L +5G /dev/vg0/lv_data

# Extend and resize file system in one command
sudo lvextend -L +5G -r /dev/vg0/lv_data

# Or resize file system separately
sudo resize2fs /dev/vg0/lv_data
```

**Reduce (shrink):**
```bash
# MUST unmount first!
sudo umount /mnt/data

# Check file system
sudo e2fsck -f /dev/vg0/lv_data

# Shrink file system first
sudo resize2fs /dev/vg0/lv_data 5G

# Then shrink LV
sudo lvreduce -L 5G /dev/vg0/lv_data

# Remount
sudo mount /dev/vg0/lv_data /mnt/data
```

### LVM Snapshots

**Create snapshot:**
```bash
# Create snapshot (reserve space for changes)
sudo lvcreate -L 2G -s -n lv_data_snap /dev/vg0/lv_data

# Mount snapshot (read-only usually)
sudo mkdir /mnt/snapshot
sudo mount -o ro /dev/vg0/lv_data_snap /mnt/snapshot
```

**Use cases:**
- Backup while system is live
- Testing changes (revert if needed)
- Development/testing environments

**Remove snapshot:**
```bash
sudo umount /mnt/snapshot
sudo lvremove /dev/vg0/lv_data_snap
```

**Merge snapshot (revert changes):**
```bash
# Unmount both original and snapshot
sudo umount /mnt/data
sudo umount /mnt/snapshot

# Merge
sudo lvconvert --merge /dev/vg0/lv_data_snap

# Remount original (changes reverted)
sudo mount /dev/vg0/lv_data /mnt/data
```

### Advanced LVM

**Thin Provisioning:**
```bash
# Create thin pool
sudo lvcreate -L 50G --thinpool thinpool vg0

# Create thin volumes (can overcommit)
sudo lvcreate -V 20G --thin -n thin1 vg0/thinpool
sudo lvcreate -V 20G --thin -n thin2 vg0/thinpool
sudo lvcreate -V 20G --thin -n thin3 vg0/thinpool
# Total: 60G allocated from 50G pool!
```

**Moving data between PVs:**
```bash
# Move all data from /dev/sdb to other PVs in VG
sudo pvmove /dev/sdb

# Then remove from VG
sudo vgreduce vg0 /dev/sdb
sudo pvremove /dev/sdb
```

**Remove LVM components:**
```bash
# Remove LV
sudo umount /mnt/data
sudo lvremove /dev/vg0/lv_data

# Remove VG
sudo vgremove vg0

# Remove PV
sudo pvremove /dev/sdb
```

---

## Virtual Memory and Swap

### What is Swap?

Swap space is disk space used as "overflow" for RAM:
- Pages not recently used moved to swap
- Frees RAM for active processes
- Prevents out-of-memory kills
- Performance cost (disk slower than RAM)

### Swap Types

**1. Swap Partition**
- Dedicated partition
- Slightly faster
- Fixed size

**2. Swap File**
- Regular file used as swap
- More flexible
- Can be resized

### Creating Swap

**Swap Partition:**
```bash
# Create partition (use fdisk/parted)
sudo mkswap /dev/sdb1
sudo swapon /dev/sdb1

# Make permanent
echo "/dev/sdb1 none swap sw 0 0" | sudo tee -a /etc/fstab
```

**Swap File:**
```bash
# Create file (4GB example)
sudo fallocate -l 4G /swapfile
# Or: sudo dd if=/dev/zero of=/swapfile bs=1M count=4096

# Set permissions
sudo chmod 600 /swapfile

# Format as swap
sudo mkswap /swapfile

# Enable
sudo swapon /swapfile

# Make permanent
echo "/swapfile none swap sw 0 0" | sudo tee -a /etc/fstab
```

### Managing Swap

**View swap:**
```bash
swapon --show
free -h
cat /proc/swaps
```

**Disable swap:**
```bash
sudo swapoff /swapfile
sudo swapoff -a  # All swap
```

**Set swap priority:**
```bash
sudo swapon --priority 10 /swapfile
```

**Remove swap:**
```bash
sudo swapoff /swapfile
sudo rm /swapfile
# Remove from /etc/fstab
```

### Swap Tuning

**Swappiness** - How aggressively to use swap (0-100):
```bash
# View current
cat /proc/sys/vm/swappiness

# Set temporarily
sudo sysctl vm.swappiness=10

# Set permanently (add to /etc/sysctl.conf)
vm.swappiness=10
```

- 0 = Avoid swap unless absolutely necessary
- 10 = Recommended for servers
- 60 = Default
- 100 = Aggressive swapping

**How much swap?**
- RAM < 2GB: 2x RAM
- RAM 2-8GB: Same as RAM
- RAM > 8GB: 8GB or 0.5x RAM
- Servers with enough RAM: May not need swap

---

## Security and Permissions

### Standard Permissions Review

**Permission bits:**
```
rwxrwxrwx
|||
||+--- Execute
|+---- Write
+----- Read

Owner Group Other
```

**Commands:**
```bash
chmod 755 file    # rwxr-xr-x
chmod u+x file    # Add execute for user
chown user file   # Change owner
chgrp group file  # Change group
```

### Access Control Lists (ACLs)

ACLs provide fine-grained permissions beyond standard Unix permissions.

**Check if filesystem supports ACLs:**
```bash
tune2fs -l /dev/sdb1 | grep acl
mount | grep acl
```

**Enable ACLs:**
```bash
# Temporary
sudo mount -o remount,acl /mnt/data

# Permanent (add acl to options in /etc/fstab)
UUID=xxx /mnt/data ext4 defaults,acl 0 2
```

**Set ACLs:**
```bash
# Give user alice read+write
setfacl -m u:alice:rw file.txt

# Give group developers read+execute
setfacl -m g:developers:rx /var/www

# Recursive
setfacl -R -m u:bob:rwx /data

# Default ACL (inherited by new files)
setfacl -d -m g:developers:rw /shared

# Multiple entries
setfacl -m u:alice:rw,u:bob:r,g:admin:rwx file.txt
```

**View ACLs:**
```bash
getfacl file.txt
```

**Remove ACLs:**
```bash
# Remove specific entry
setfacl -x u:alice file.txt

# Remove all ACLs
setfacl -b file.txt
```

**ACL Permissions:**
- r (read) - 4
- w (write) - 2
- x (execute) - 1
- \- (none) - 0

### File Attributes

Extended attributes for special behaviors.

**Set attributes:**
```bash
# Immutable (cannot modify, delete, rename)
sudo chattr +i important.conf

# Append-only (can only append, not modify/delete)
sudo chattr +a /var/log/app.log

# No dump (excluded from backup)
sudo chattr +d temp.dat

# Compressed (transparent compression)
sudo chattr +c largefile

# No atime update (performance)
sudo chattr +A datafile
```

**View attributes:**
```bash
lsattr file.txt
```

**Remove attributes:**
```bash
sudo chattr -i important.conf
```

**Common attributes:**
- i = Immutable
- a = Append only
- d = No dump
- c = Compressed
- A = No atime updates
- s = Secure deletion (overwrite with zeros)
- u = Undeletable (save for recovery)

### Disk Encryption with LUKS

**Create encrypted volume:**
```bash
# Format device with LUKS
sudo cryptsetup luksFormat /dev/sdb1

# Open encrypted device
sudo cryptsetup open /dev/sdb1 secure_data

# Create file system
sudo mkfs.ext4 /dev/mapper/secure_data

# Mount
sudo mount /dev/mapper/secure_data /mnt/secure
```

**Close encrypted volume:**
```bash
sudo umount /mnt/secure
sudo cryptsetup close secure_data
```

**Automatic unlock at boot:**
```bash
# Add key file
sudo dd if=/dev/urandom of=/root/keyfile bs=1024 count=4
sudo chmod 0400 /root/keyfile
sudo cryptsetup luksAddKey /dev/sdb1 /root/keyfile

# Add to /etc/crypttab
secure_data /dev/sdb1 /root/keyfile luks

# Add to /etc/fstab
/dev/mapper/secure_data /mnt/secure ext4 defaults 0 2
```

**Manage keys:**
```bash
# List key slots
sudo cryptsetup luksDump /dev/sdb1

# Add key
sudo cryptsetup luksAddKey /dev/sdb1

# Remove key
sudo cryptsetup luksRemoveKey /dev/sdb1

# Change passphrase
sudo cryptsetup luksChangeKey /dev/sdb1
```

---

## Monitoring and Troubleshooting

### Disk Space Monitoring

**Check disk usage:**
```bash
df -h              # Human readable
df -i              # Inode usage
df -T              # Show file system type
```

**Directory usage:**
```bash
du -sh /var/log    # Summary
du -h --max-depth=1 /var    # One level deep
ncdu /home         # Interactive (install ncdu first)
```

**Find large files:**
```bash
find / -type f -size +100M 2>/dev/null
```

### I/O Monitoring

**iostat - I/O statistics:**
```bash
iostat            # Basic stats
iostat -x 1       # Extended stats, 1 second updates
iostat -xz 1      # Exclude zero-activity devices
```

**Key metrics:**
- %util - Device utilization (close to 100% = bottleneck)
- await - Average wait time (ms)
- svctm - Service time
- r/s, w/s - Read/write operations per second

**iotop - Top for I/O:**
```bash
sudo iotop        # Interactive
sudo iotop -o     # Only show active processes
sudo iotop -a     # Accumulated I/O
```

**vmstat - Virtual memory statistics:**
```bash
vmstat 1          # 1 second updates
```

**Key columns:**
- swpd - Swap used
- free - Free RAM
- si, so - Swap in/out
- bi, bo - Blocks in/out

### File System Check

**Check mounted filesystems:**
```bash
mount | column -t
findmnt           # Tree view
```

**Find files using disk:**
```bash
lsof /mnt/data           # List open files
fuser -v /mnt/data       # Show processes using mountpoint
```

**Identify what's using space:**
```bash
# After deleting files, space not freed?
# (Process still has file open)
lsof | grep deleted
```

### SMART Monitoring

**Check disk health:**
```bash
# Install smartmontools
sudo apt install smartmontools

# View disk info
sudo smartctl -i /dev/sda

# Run short test
sudo smartctl -t short /dev/sda

# View test results
sudo smartctl -a /dev/sda

# Enable automatic testing
sudo smartctl -s on /dev/sda
```

**Key SMART attributes:**
- Reallocated_Sector_Count - Bad sectors moved
- Current_Pending_Sector - Sectors waiting for reallocation
- Offline_Uncorrectable - Cannot be corrected
- Temperature_Celsius - Drive temperature

### Common Issues

**Issue 1: Filesystem is read-only**
```bash
# Check for errors
sudo fsck.ext4 -f /dev/sdb1

# Remount as read-write
sudo mount -o remount,rw /mnt/data
```

**Issue 2: Cannot unmount (device busy)**
```bash
# Find what's using it
lsof /mnt/data
fuser -vm /mnt/data

# Kill processes (careful!)
fuser -km /mnt/data

# Lazy unmount (last resort)
sudo umount -l /mnt/data
```

**Issue 3: Out of inodes**
```bash
# Check inode usage
df -i

# Find directories with many files
sudo find / -xdev -printf '%h\n' | sort | uniq -c | sort -rn | head
```

**Issue 4: Disk full but df shows space**
```bash
# Files deleted but still open
lsof | grep deleted

# Kill or restart processes holding deleted files
```

---

## Backup and Recovery

### Backup Strategies

**3-2-1 Rule:**
- 3 copies of data
- 2 different media types
- 1 off-site copy

**Backup Types:**
- **Full** - Complete copy (slow, large, easy restore)
- **Incremental** - Changes since last backup (fast, small, slower restore)
- **Differential** - Changes since last full (medium speed/size)

### Using rsync

**Basic backup:**
```bash
# Local backup
rsync -av /source/ /backup/

# Remote backup
rsync -av /data/ user@server:/backup/

# With compression
rsync -avz /data/ user@server:/backup/

# Show progress
rsync -av --progress /data/ /backup/

# Dry run (test without doing)
rsync -avn /data/ /backup/
```

**Incremental backup:**
```bash
# First full backup
rsync -av /data/ /backup/full/

# Incremental (hard links to unchanged files)
rsync -av --link-dest=/backup/full /data/ /backup/incr1/
rsync -av --link-dest=/backup/incr1 /data/ /backup/incr2/
```

**Useful options:**
- -a = Archive mode (preserve permissions, times, etc.)
- -v = Verbose
- -z = Compress during transfer
- -h = Human-readable sizes
- --delete = Delete files in dest not in source
- --exclude = Exclude patterns
- --progress = Show progress

### Using tar

**Create archive:**
```bash
# Basic tar
tar -cf backup.tar /data

# Compressed with gzip
tar -czf backup.tar.gz /data

# Compressed with bzip2 (better compression)
tar -cjf backup.tar.bz2 /data

# Compressed with xz (best compression)
tar -cJf backup.tar.xz /data

# With verbose and preserve permissions
tar -czvf backup.tar.gz /data

# Exclude directories
tar -czf backup.tar.gz --exclude=/data/cache /data
```

**Extract archive:**
```bash
tar -xzf backup.tar.gz
tar -xzf backup.tar.gz -C /restore/path
```

**List contents:**
```bash
tar -tzf backup.tar.gz
```

### LVM Snapshots for Backup

**Backup using snapshot:**
```bash
# Create snapshot
sudo lvcreate -L 5G -s -n backup_snap /dev/vg0/lv_data

# Mount snapshot
sudo mkdir /mnt/snapshot
sudo mount -o ro /dev/vg0/backup_snap /mnt/snapshot

# Backup from snapshot (data is consistent)
sudo rsync -av /mnt/snapshot/ /backup/

# Or tar
sudo tar -czf backup.tar.gz /mnt/snapshot

# Cleanup
sudo umount /mnt/snapshot
sudo lvremove /dev/vg0/backup_snap
```

### Automated Backups

**Backup script:**
```bash
#!/bin/bash
# backup.sh

BACKUP_DIR="/backup"
DATE=$(date +%Y-%m-%d)
LOG_FILE="$BACKUP_DIR/backup-$DATE.log"

echo "Backup started: $(date)" > "$LOG_FILE"

# Create LVM snapshot
lvcreate -L 2G -s -n temp_snap /dev/vg0/lv_data

# Mount snapshot
mkdir -p /mnt/temp_snap
mount -o ro /dev/vg0/temp_snap /mnt/temp_snap

# Backup
rsync -av --delete /mnt/temp_snap/ "$BACKUP_DIR/$DATE/" >> "$LOG_FILE" 2>&1

# Cleanup
umount /mnt/temp_snap
lvremove -y /dev/vg0/temp_snap

# Keep only last 7 days
find "$BACKUP_DIR" -maxdepth 1 -type d -mtime +7 -exec rm -rf {} \;

echo "Backup completed: $(date)" >> "$LOG_FILE"
```

**Cron scheduling:**
```bash
# Daily backup at 2 AM
0 2 * * * /usr/local/bin/backup.sh

# Weekly full, daily incremental
0 2 * * 0 /usr/local/bin/full_backup.sh
0 2 * * 1-6 /usr/local/bin/incremental_backup.sh
```

### Recovery

**Restore from rsync backup:**
```bash
rsync -av /backup/2024-10-27/ /data/
```

**Restore from tar:**
```bash
tar -xzf backup.tar.gz -C /data/
```

**Restore specific files:**
```bash
# From tar
tar -xzf backup.tar.gz path/to/file

# From rsync
rsync -av /backup/2024-10-27/path/to/file /data/path/to/
```

---

## Performance Tuning

### I/O Schedulers

Linux provides different I/O schedulers for different workloads.

**Available schedulers:**
- **none** - No scheduling (NVMe default)
- **mq-deadline** - Deadline-based (good for most workloads)
- **bfq** - Budget Fair Queueing (good for desktops)
- **kyber** - Token-based (low latency)

**View current scheduler:**
```bash
cat /sys/block/sda/queue/scheduler
```

**Change scheduler:**
```bash
# Temporary
echo mq-deadline | sudo tee /sys/block/sda/queue/scheduler

# Permanent (add to /etc/rc.local or systemd)
echo 'ACTION=="add|change", KERNEL=="sd*", ATTR{queue/scheduler}="mq-deadline"' | \
  sudo tee /etc/udev/rules.d/60-scheduler.rules
```

**Recommendations:**
- SSDs: none or mq-deadline
- HDDs: mq-deadline or bfq
- Database servers: mq-deadline
- Desktops: bfq
- Real-time: kyber

### Read-Ahead Tuning

**View read-ahead:**
```bash
sudo blockdev --getra /dev/sda
```

**Set read-ahead:**
```bash
# In 512-byte sectors (4096 = 2MB)
sudo blockdev --setra 4096 /dev/sda
```

**Recommendations:**
- Default: 256 (128KB)
- Sequential workloads: 2048-8192 (1-4MB)
- Random workloads: 128-256 (64-128KB)
- SSDs: 256 (128KB)

### File System Mount Options

**Performance options:**
```bash
# No access time updates (big performance gain)
mount -o noatime /dev/sdb1 /mnt/data

# No directory access time
mount -o nodiratime /dev/sdb1 /mnt/data

# Both
mount -o noatime,nodiratime /dev/sdb1 /mnt/data

# Disable barriers (unsafe but fast)
mount -o nobarrier /dev/sdb1 /mnt/data

# Commit interval (seconds between syncs)
mount -o commit=60 /dev/sdb1 /mnt/data
```

### Kernel Tuning

**View all parameters:**
```bash
sysctl -a | grep vm
```

**Key parameters:**

**vm.swappiness**
```bash
# Current value
cat /proc/sys/vm/swappiness

# Set (0-100, default 60)
sudo sysctl vm.swappiness=10
```

**vm.dirty_ratio**
```bash
# Percentage of RAM that can be dirty before sync
sudo sysctl vm.dirty_ratio=15
```

**vm.dirty_background_ratio**
```bash
# When background writeback starts
sudo sysctl vm.dirty_background_ratio=5
```

**vm.vfs_cache_pressure**
```bash
# Tendency to reclaim inode/dentry cache
# Lower = keep more (default 100)
sudo sysctl vm.vfs_cache_pressure=50
```

**Make permanent:**
```bash
# Add to /etc/sysctl.conf
vm.swappiness=10
vm.dirty_ratio=15
vm.dirty_background_ratio=5
vm.vfs_cache_pressure=50

# Apply
sudo sysctl -p
```

### Benchmarking

**dd benchmark:**
```bash
# Write test
dd if=/dev/zero of=/mnt/data/test bs=1M count=1000 oflag=direct

# Read test
dd if=/mnt/data/test of=/dev/null bs=1M iflag=direct

# Cached read
dd if=/mnt/data/test of=/dev/null bs=1M
```

**hdparm:**
```bash
# Cached reads
sudo hdparm -t /dev/sda

# Direct disk reads
sudo hdparm -T /dev/sda
```

**fio (flexible I/O tester):**
```bash
# Install
sudo apt install fio

# Random read test
fio --name=randread --ioengine=libaio --iodepth=16 --rw=randread \
    --bs=4k --size=1G --numjobs=4 --runtime=60 --group_reporting

# Sequential write test
fio --name=seqwrite --ioengine=libaio --iodepth=1 --rw=write \
    --bs=128k --size=1G --numjobs=1 --runtime=60 --group_reporting
```

---

## Summary

### Key Commands Reference

**File Systems:**
- `mkfs.ext4`, `mkfs.xfs`, `mkfs.btrfs` - Create file systems
- `mount`, `umount` - Mount/unmount
- `fsck`, `xfs_repair` - Check/repair
- `tune2fs`, `xfs_admin` - Tune parameters
- `resize2fs`, `xfs_growfs` - Resize

**RAID:**
- `mdadm --create` - Create array
- `mdadm --detail` - View array info
- `mdadm --manage` - Manage disks
- `cat /proc/mdstat` - Quick status

**LVM:**
- `pvcreate`, `vgcreate`, `lvcreate` - Create
- `pvdisplay`, `vgdisplay`, `lvdisplay` - View
- `lvextend`, `lvreduce` - Resize
- `lvcreate -s` - Snapshot

**Swap:**
- `mkswap` - Format swap
- `swapon`, `swapoff` - Enable/disable
- `swapon --show` - View swap

**Security:**
- `setfacl`, `getfacl` - ACLs
- `cryptsetup` - LUKS encryption
- `chattr`, `lsattr` - File attributes

**Monitoring:**
- `df`, `du` - Disk usage
- `iostat`, `iotop` - I/O stats
- `vmstat` - Virtual memory stats
- `lsof`, `fuser` - Open files

**Backup:**
- `rsync` - Sync files
- `tar` - Archive files

### Best Practices

1. **Always backup before major operations**
2. **Test in VMs first**
3. **Use UUIDs in /etc/fstab**
4. **Monitor RAID arrays regularly**
5. **Use LVM for flexibility**
6. **Enable SMART monitoring**
7. **Automate backups**
8. **Test restore procedures**
9. **Document your setup**
10. **Keep kernel and tools updated**

---

End of Lecture Notes


