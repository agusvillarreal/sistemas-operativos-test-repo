# Final Project Quick Reference Card
## Enterprise Storage Manager - Command Cheat Sheet

---

## 🚀 Quick Start Commands

```bash
# Start the daemon
sudo ./storage_daemon

# Check status
./storage_cli status

# View help
./storage_cli --help
```

---

## 📦 RAID Commands

```bash
# Create RAID arrays
./storage_cli raid create /dev/md0 --level=1 --devices=/dev/loop0,/dev/loop1
./storage_cli raid create /dev/md1 --level=5 --devices=/dev/loop{2..5}

# Monitor RAID
./storage_cli raid status /dev/md0
./storage_cli raid list

# Manage disks
./storage_cli raid add /dev/md0 /dev/loop6
./storage_cli raid fail /dev/md0 /dev/loop0
./storage_cli raid remove /dev/md0 /dev/loop0

# Stop RAID
./storage_cli raid stop /dev/md0

# Low-level commands
mdadm --create /dev/md0 --level=1 --raid-devices=2 /dev/loop0 /dev/loop1
mdadm --detail /dev/md0
cat /proc/mdstat
mdadm --manage /dev/md0 --add /dev/loop2
mdadm --manage /dev/md0 --fail /dev/loop0
mdadm --stop /dev/md0
```

---

## 💾 LVM Commands

```bash
# Create Physical Volumes
./storage_cli lvm pv-create /dev/loop0
./storage_cli lvm pv-create /dev/loop1

# Create Volume Group
./storage_cli lvm vg-create vg0 /dev/loop0 /dev/loop1

# Create Logical Volume
./storage_cli lvm lv-create vg0 data 5G

# Extend Logical Volume
./storage_cli lvm lv-extend vg0 data +2G

# Create Snapshot
./storage_cli lvm snapshot vg0 data snap1 1G

# List everything
./storage_cli lvm pv-list
./storage_cli lvm vg-list
./storage_cli lvm lv-list

# Low-level commands
pvcreate /dev/loop0
pvdisplay
vgcreate vg0 /dev/loop0 /dev/loop1
vgdisplay
lvcreate -L 5G -n data vg0
lvdisplay
lvextend -L +2G /dev/vg0/data
lvcreate -s -L 1G -n snap1 /dev/vg0/data
```

---

## 📁 Filesystem Commands

```bash
# Create filesystems
./storage_cli fs create /dev/vg0/data ext4 --label=mydata
./storage_cli fs create /dev/vg0/logs xfs --label=logs

# Mount filesystems
./storage_cli fs mount /dev/vg0/data /mnt/data
./storage_cli fs mount /dev/vg0/logs /mnt/logs --options=noatime

# Unmount
./storage_cli fs unmount /mnt/data

# Check filesystem
./storage_cli fs check /dev/vg0/data

# Resize filesystem
./storage_cli fs resize /dev/vg0/data

# Get info
./storage_cli fs info /mnt/data
./storage_cli fs list

# Low-level commands
mkfs.ext4 -L mydata /dev/vg0/data
mkfs.xfs -L logs /dev/vg0/logs
mount /dev/vg0/data /mnt/data
mount -o noatime /dev/vg0/logs /mnt/logs
umount /mnt/data
fsck.ext4 -f /dev/vg0/data
resize2fs /dev/vg0/data
xfs_growfs /mnt/logs
df -h /mnt/data
tune2fs -l /dev/vg0/data
```

---

## 💿 Swap Commands

```bash
# Create swap
./storage_cli swap create /swapfile 2G
./storage_cli swap create /dev/vg0/swap 4G

# Enable swap
./storage_cli swap enable /swapfile --priority=10

# Disable swap
./storage_cli swap disable /swapfile

# List swap
./storage_cli swap list

# Low-level commands
dd if=/dev/zero of=/swapfile bs=1M count=2048
chmod 600 /swapfile
mkswap /swapfile
swapon -p 10 /swapfile
swapoff /swapfile
swapon --show
```

---

## 🔒 Security Commands

```bash
# ACLs
./storage_cli security acl-set /mnt/data user:alice:rwx
./storage_cli security acl-set /mnt/data group:developers:rx
./storage_cli security acl-get /mnt/data
./storage_cli security acl-remove /mnt/data user:alice

# Encryption
./storage_cli security encrypt /dev/vg0/secure --name=secure_vol
./storage_cli security open secure_vol
./storage_cli security close secure_vol

# File attributes
./storage_cli security chattr /mnt/data/important.txt +i
./storage_cli security lsattr /mnt/data/important.txt

# Low-level commands
setfacl -m u:alice:rwx /mnt/data
setfacl -m g:developers:rx /mnt/data
getfacl /mnt/data
cryptsetup luksFormat /dev/vg0/secure
cryptsetup open /dev/vg0/secure secure_vol
cryptsetup close secure_vol
chattr +i /mnt/data/important.txt
lsattr /mnt/data/
```

---

## 📊 Monitoring Commands

```bash
# Real-time monitoring
./storage_cli monitor start --interval=1
./storage_cli monitor stop

# Device stats
./storage_cli monitor stats /dev/vg0/data

# Historical data
./storage_cli monitor history /dev/vg0/data --last=1h
./storage_cli monitor history /dev/vg0/data --from="2025-10-27 10:00"

# Generate report
./storage_cli monitor report --output=report.txt

# Low-level commands
iostat -x 1
iotop -o
vmstat 1
lsof /mnt/data
watch -n 1 'df -h && free -h'
```

---

## 💾 Backup Commands

```bash
# Create backup
./storage_cli backup create /mnt/data /backup/data --type=full
./storage_cli backup create /mnt/data /backup/data --type=incremental

# List backups
./storage_cli backup list

# Verify backup
./storage_cli backup verify backup-2025-10-27-001

# Restore backup
./storage_cli backup restore backup-2025-10-27-001 /restore/test

# Schedule backups
./storage_cli backup schedule --cron="0 2 * * *" --keep=7

# Cleanup old backups
./storage_cli backup cleanup --keep=5

# Low-level commands
rsync -av /mnt/data/ /backup/data/
rsync -av --link-dest=/backup/prev /mnt/data/ /backup/current/
tar czf /backup/data.tar.gz /mnt/data
lvcreate -s -L 1G -n backup_snap /dev/vg0/data
mount /dev/vg0/backup_snap /mnt/snapshot
rsync -av /mnt/snapshot/ /backup/
umount /mnt/snapshot
lvremove /dev/vg0/backup_snap
```

---

## ⚡ Performance Commands

```bash
# Benchmark
./storage_cli perf benchmark /dev/vg0/data --output=results.txt

# Get recommendations
./storage_cli perf recommend /dev/vg0/data --workload=database

# Tune performance
./storage_cli perf tune /dev/vg0/data --scheduler=deadline --readahead=2048

# Compare results
./storage_cli perf compare before.txt after.txt

# Low-level commands
# I/O Scheduler
cat /sys/block/sda/queue/scheduler
echo deadline > /sys/block/sda/queue/scheduler

# Read-ahead
blockdev --getra /dev/sda
blockdev --setra 2048 /dev/sda

# Kernel parameters
sysctl vm.swappiness=10
sysctl vm.dirty_ratio=15
sysctl vm.vfs_cache_pressure=50

# Benchmarking
hdparm -t /dev/sda
dd if=/dev/zero of=/mnt/data/test bs=1M count=1000 oflag=direct
fio --name=randread --ioengine=libaio --rw=randread --bs=4k --size=1G
```

---

## 🔧 Kernel Module Commands

```bash
# Load module
sudo insmod kernel_module/storage_stats.ko

# Check module loaded
lsmod | grep storage_stats

# View statistics
cat /proc/storage_stats

# Unload module
sudo rmmod storage_stats

# Check kernel log
dmesg | tail -20

# Module info
modinfo storage_stats.ko
```

---

## 🔄 Daemon Management

```bash
# Start daemon
sudo ./storage_daemon

# Check daemon status
./storage_cli status
ps aux | grep storage_daemon
cat /var/run/storage_mgr.pid

# Reload configuration
sudo kill -HUP $(cat /var/run/storage_mgr.pid)

# Stop daemon
sudo kill -TERM $(cat /var/run/storage_mgr.pid)
# or
./storage_cli shutdown
```

---

## 🛠️ Development & Testing

```bash
# Compile everything
make clean
make all

# Run tests
make test

# Run specific tests
./tests/test_raid
./tests/test_lvm
./tests/test_filesystem

# Stress test
./tests/stress_test --duration=300 --clients=50

# Code coverage
make coverage
firefox coverage/index.html

# Memory check
valgrind --leak-check=full ./storage_daemon

# Debugging
gdb ./storage_daemon
strace -f ./storage_daemon
```

---

## 🔍 Troubleshooting Commands

```bash
# Check system logs
journalctl -xe
tail -f /var/log/syslog
dmesg | grep -i error

# Check RAID issues
cat /proc/mdstat
mdadm --detail /dev/md0
mdadm --examine /dev/loop0

# Check LVM issues
pvs
vgs
lvs
vgck vg0

# Check filesystem issues
dmesg | grep -i "ext4\|xfs"
mount | grep /mnt/data

# Check disk space
df -h
du -sh /mnt/data/*
ncdu /mnt/data

# Check processes
ps aux | grep storage
pstree -p $(cat /var/run/storage_mgr.pid)
lsof | grep storage

# Network/IPC
lsof -U
netstat -anp | grep storage
ss -x | grep storage

# Memory
free -h
cat /proc/meminfo
vmstat 1

# I/O
iostat -x 1
iotop
lsof /mnt/data
```

---

## 📚 Setup Commands (Development Environment)

```bash
# Install dependencies
sudo apt update
sudo apt install build-essential gcc make cmake
sudo apt install mdadm lvm2 xfsprogs btrfs-progs
sudo apt install cryptsetup acl attr
sudo apt install sysstat iotop lsof
sudo apt install linux-headers-$(uname -r)
sudo apt install libcunit1-dev valgrind

# Create loop devices for testing
for i in {0..7}; do
    dd if=/dev/zero of=/tmp/disk$i.img bs=1M count=1024
    sudo losetup /dev/loop$i /tmp/disk$i.img
done

# List loop devices
losetup -a

# Remove loop devices
for i in {0..7}; do
    sudo losetup -d /dev/loop$i
done

# Clean up test files
rm -f /tmp/disk*.img
```

---

## 🧹 Cleanup Commands

```bash
# Full cleanup (DANGER!)
./scripts/cleanup_all.sh

# Individual cleanup
umount /mnt/data
lvremove /dev/vg0/data
vgremove vg0
pvremove /dev/loop0
mdadm --stop /dev/md0
mdadm --zero-superblock /dev/loop0
losetup -d /dev/loop0

# Clean build artifacts
make clean

# Remove IPC resources
ipcrm -a

# Kill daemon
sudo killall storage_daemon
```

---

## 📝 Useful File Paths

```
/proc/mdstat                    - RAID status
/proc/meminfo                   - Memory information
/proc/[pid]/io                  - Process I/O stats
/sys/block/sda/queue/scheduler  - I/O scheduler
/sys/block/sda/queue/read_ahead_kb - Read-ahead setting
/dev/mapper/                    - Device mapper (LVM, LUKS)
/var/run/storage_mgr.pid        - Daemon PID file
/var/run/storage_mgr.sock       - IPC socket
/var/log/storage_mgr.log        - Daemon log file
```

---

## 🎯 Common Workflows

### Workflow 1: Create RAID + LVM + Filesystem
```bash
# 1. Create RAID array
./storage_cli raid create /dev/md0 --level=5 --devices=/dev/loop{0..3}

# 2. Create LVM on RAID
./storage_cli lvm pv-create /dev/md0
./storage_cli lvm vg-create vg_raid /dev/md0
./storage_cli lvm lv-create vg_raid data 10G

# 3. Create and mount filesystem
./storage_cli fs create /dev/vg_raid/data ext4 --label=raid_data
./storage_cli fs mount /dev/vg_raid/data /mnt/raid_data
```

### Workflow 2: Encrypted Volume
```bash
# 1. Create LV
./storage_cli lvm lv-create vg0 secure 5G

# 2. Encrypt
./storage_cli security encrypt /dev/vg0/secure --name=secure_vol

# 3. Open and use
./storage_cli security open secure_vol
./storage_cli fs create /dev/mapper/secure_vol ext4
./storage_cli fs mount /dev/mapper/secure_vol /mnt/secure
```

### Workflow 3: Snapshot Backup
```bash
# 1. Create snapshot
./storage_cli lvm snapshot vg0 data backup_snap 2G

# 2. Mount snapshot
mkdir -p /mnt/snapshot
./storage_cli fs mount /dev/vg0/backup_snap /mnt/snapshot

# 3. Backup
./storage_cli backup create /mnt/snapshot /backup/data --type=full

# 4. Cleanup
./storage_cli fs unmount /mnt/snapshot
./storage_cli lvm lv-remove vg0 backup_snap
```

---

## 🔢 Error Codes

| Code | Meaning |
|------|---------|
| 0    | Success |
| -1   | General error |
| -2   | Permission denied |
| -3   | Device not found |
| -4   | Device busy |
| -5   | Invalid parameter |
| -10  | RAID error |
| -20  | LVM error |
| -30  | Filesystem error |
| -40  | Encryption error |
| -50  | Backup error |

---

## 📞 Getting Help

```bash
# Command help
./storage_cli --help
./storage_cli raid --help
./storage_cli lvm --help

# Check logs
tail -f /var/log/storage_mgr.log

# Debug mode
./storage_daemon --debug --foreground

# Verbose client
./storage_cli -v status
```

---

## ⚠️ Important Notes

1. **Always use loop devices for testing** (never real disks!)
2. **Run in a VM** (some operations can crash the system)
3. **Backup before experimenting** (take VM snapshots)
4. **Most commands require root** (use `sudo`)
5. **Check return values** (all operations can fail)
6. **Read error messages** (they usually tell you what's wrong)

---

**Print this reference and keep it handy while working on your project!**



