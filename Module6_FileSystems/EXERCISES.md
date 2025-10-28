# Module 6: File Systems - Practical Exercises
## Hands-On Labs and Assignments

**Prerequisites:** VM with root access, at least 40GB disk space, additional virtual disks

**Safety Warning:** These exercises involve potentially destructive operations. ALWAYS use a virtual machine with snapshots. Never test on production systems or your main computer.

---

## Lab Setup

### Creating Loop Devices for Testing

Since we'll be working with RAID and LVM, we need multiple block devices. Loop devices are perfect for testing without needing real disks.

```bash
# Create 8 disk image files (1GB each)
for i in {0..7}; do
    dd if=/dev/zero of=~/disk$i.img bs=1M count=1024
done

# Create loop devices
for i in {0..7}; do
    sudo losetup /dev/loop$i ~/disk$i.img
done

# Verify
losetup -a
lsblk
```

**Cleanup when done:**
```bash
# Detach loop devices
for i in {0..7}; do
    sudo losetup -d /dev/loop$i
done

# Remove disk images
rm ~/disk*.img
```

---

## Exercise 1: File System Basics

### Task 1.1: Create and Mount ext4 File System

**Objectives:**
- Create an ext4 file system
- Mount it manually
- Configure automatic mounting
- Verify mounting

**Steps:**

1. Create ext4 filesystem with label:
```bash
sudo mkfs.ext4 -L mydata /dev/loop0
```

2. Create mount point and mount:
```bash
sudo mkdir /mnt/mydata
sudo mount /dev/loop0 /mnt/mydata
```

3. Verify:
```bash
df -h | grep mydata
mount | grep loop0
```

4. Create test files:
```bash
sudo touch /mnt/mydata/test{1..5}.txt
ls -la /mnt/mydata/
```

5. Get UUID:
```bash
sudo blkid /dev/loop0
```

6. Add to /etc/fstab (using UUID):
```bash
# Format: UUID=xxx /mnt/mydata ext4 defaults 0 2
sudo nano /etc/fstab
```

7. Test fstab:
```bash
sudo umount /mnt/mydata
sudo mount -a
df -h | grep mydata
```

**Questions:**
1. What is the advantage of using UUID vs device name in fstab?
2. What does the "0 2" at the end of the fstab entry mean?
3. What happens if you try to mount the device twice?

### Task 1.2: File System with Custom Parameters

**Objectives:**
- Create filesystem with custom inode ratio
- Understand the trade-offs

**Steps:**

1. Check default inode count:
```bash
sudo mkfs.ext4 -n /dev/loop1
```

2. Create filesystem with more inodes (for many small files):
```bash
sudo mkfs.ext4 -i 4096 -L smallfiles /dev/loop1
```

3. Create filesystem with fewer inodes (for large files):
```bash
sudo mkfs.ext4 -i 16384 -L largefiles /dev/loop2
```

4. Compare:
```bash
sudo tune2fs -l /dev/loop1 | grep -i inode
sudo tune2fs -l /dev/loop2 | grep -i inode
```

5. Mount and test:
```bash
sudo mkdir /mnt/smallfiles /mnt/largefiles
sudo mount /dev/loop1 /mnt/smallfiles
sudo mount /dev/loop2 /mnt/largefiles
df -i
```

**Challenge:**
Create a filesystem that reserves only 1% for root (instead of default 5%) and document why you might want this.

### Task 1.3: Mount Options Performance Testing

**Objectives:**
- Compare performance with different mount options
- Understand noatime benefits

**Steps:**

1. Mount with default options:
```bash
sudo mount /dev/loop0 /mnt/test1
```

2. Create test script:
```bash
cat > test_perf.sh << 'EOF'
#!/bin/bash
time for i in {1..1000}; do
    cat $1 > /dev/null
done
EOF
chmod +x test_perf.sh
```

3. Test with atime:
```bash
sudo dd if=/dev/zero of=/mnt/test1/testfile bs=1M count=100
sync
echo 3 | sudo tee /proc/sys/vm/drop_caches
./test_perf.sh /mnt/test1/testfile
```

4. Remount with noatime:
```bash
sudo mount -o remount,noatime /mnt/test1
```

5. Test again:
```bash
echo 3 | sudo tee /proc/sys/vm/drop_caches
./test_perf.sh /mnt/test1/testfile
```

**Questions:**
1. What performance difference did you observe?
2. When might you NOT want to use noatime?
3. What's the difference between noatime and relatime?

---

## Exercise 2: XFS File System

### Task 2.1: Create and Manage XFS

**Objectives:**
- Create XFS filesystem
- Understand XFS-specific tools
- Test online resizing

**Steps:**

1. Create XFS:
```bash
sudo mkfs.xfs -L myxfs /dev/loop3
```

2. Mount:
```bash
sudo mkdir /mnt/myxfs
sudo mount /dev/loop3 /mnt/myxfs
```

3. View XFS info:
```bash
sudo xfs_info /dev/loop3
```

4. Create test data:
```bash
sudo dd if=/dev/urandom of=/mnt/myxfs/bigfile bs=1M count=100
```

5. Get filesystem statistics:
```bash
sudo xfs_db -c "sb 0" -c "print" -r /dev/loop3
```

6. Test online resize (grow only!):
```bash
# First, grow the underlying device (for demo, we'll skip)
# Then grow the filesystem
sudo xfs_growfs /mnt/myxfs
```

**Important:** XFS cannot be shrunk! Only grown.

### Task 2.2: XFS Defragmentation

**Objectives:**
- Check fragmentation
- Defragment XFS filesystem

**Steps:**

1. Check fragmentation:
```bash
sudo xfs_db -c frag -r /dev/loop3
```

2. Defragment specific file:
```bash
sudo xfs_fsr -v /mnt/myxfs/bigfile
```

3. Defragment entire filesystem:
```bash
sudo xfs_fsr /mnt/myxfs
```

---

## Exercise 3: RAID Configuration

### Task 3.1: Create RAID 1 Array

**Objectives:**
- Create mirrored RAID array
- Test redundancy
- Handle disk failure

**Steps:**

1. Create RAID 1:
```bash
sudo mdadm --create /dev/md0 \
  --level=1 \
  --raid-devices=2 \
  /dev/loop0 /dev/loop1
```

2. Monitor creation:
```bash
watch -n 1 cat /proc/mdstat
```

3. View details:
```bash
sudo mdadm --detail /dev/md0
```

4. Create filesystem and mount:
```bash
sudo mkfs.ext4 /dev/md0
sudo mkdir /mnt/raid1
sudo mount /dev/md0 /mnt/raid1
```

5. Create test data:
```bash
sudo dd if=/dev/urandom of=/mnt/raid1/testfile bs=1M count=100
md5sum /mnt/raid1/testfile > /tmp/original.md5
```

6. Simulate disk failure:
```bash
sudo mdadm --manage /dev/md0 --fail /dev/loop0
```

7. Check array status:
```bash
sudo mdadm --detail /dev/md0
cat /proc/mdstat
```

8. Verify data integrity:
```bash
md5sum -c /tmp/original.md5
```

9. Remove failed disk:
```bash
sudo mdadm --manage /dev/md0 --remove /dev/loop0
```

10. Add replacement disk:
```bash
sudo mdadm --manage /dev/md0 --add /dev/loop2
```

11. Watch rebuild:
```bash
watch -n 1 cat /proc/mdstat
```

**Questions:**
1. What happened to the data when a disk failed?
2. How long did the rebuild take?
3. What is the write performance penalty of RAID 1?

### Task 3.2: Create RAID 5 Array

**Objectives:**
- Create RAID 5 array
- Understand parity
- Calculate usable space

**Steps:**

1. Stop previous RAID (if running):
```bash
sudo umount /mnt/raid1
sudo mdadm --stop /dev/md0
```

2. Zero superblocks:
```bash
sudo mdadm --zero-superblock /dev/loop{0..2}
```

3. Create RAID 5:
```bash
sudo mdadm --create /dev/md1 \
  --level=5 \
  --raid-devices=3 \
  --chunk=512 \
  /dev/loop0 /dev/loop1 /dev/loop2
```

4. Check capacity:
```bash
sudo mdadm --detail /dev/md1
lsblk
```

5. Create filesystem:
```bash
sudo mkfs.ext4 /dev/md1
sudo mkdir /mnt/raid5
sudo mount /dev/md1 /mnt/raid5
```

6. Fill with data:
```bash
sudo dd if=/dev/urandom of=/mnt/raid5/file1 bs=1M count=100
sudo dd if=/dev/urandom of=/mnt/raid5/file2 bs=1M count=100
```

7. Test disk failure and recovery (similar to RAID 1)

**Questions:**
1. With 3 x 1GB disks, how much usable space do you have?
2. What is the minimum number of disks for RAID 5?
3. Can RAID 5 survive two disk failures?

### Task 3.3: Save RAID Configuration

**Objectives:**
- Make RAID persistent across reboots
- Understand mdadm.conf

**Steps:**

1. Scan and save configuration:
```bash
sudo mdadm --detail --scan | sudo tee -a /etc/mdadm/mdadm.conf
```

2. Update initramfs:
```bash
sudo update-initramfs -u
```

3. Add to fstab:
```bash
echo "/dev/md1 /mnt/raid5 ext4 defaults 0 2" | sudo tee -a /etc/fstab
```

4. Test reboot persistence:
```bash
sudo reboot
# After reboot:
cat /proc/mdstat
df -h | grep raid5
```

---

## Exercise 4: LVM Management

### Task 4.1: Create LVM Setup

**Objectives:**
- Create Physical Volumes
- Create Volume Group
- Create Logical Volumes
- Understand LVM hierarchy

**Steps:**

1. Create Physical Volumes:
```bash
sudo pvcreate /dev/loop3 /dev/loop4 /dev/loop5
sudo pvdisplay
sudo pvs
```

2. Create Volume Group:
```bash
sudo vgcreate vg_data /dev/loop3 /dev/loop4
sudo vgdisplay vg_data
sudo vgs
```

3. Create Logical Volumes:
```bash
# Fixed size
sudo lvcreate -L 500M -n lv_web vg_data

# Percentage
sudo lvcreate -l 50%FREE -n lv_db vg_data

# Remaining space
sudo lvcreate -l 100%FREE -n lv_backup vg_data
```

4. View LVs:
```bash
sudo lvdisplay
sudo lvs
ls -l /dev/vg_data/
ls -l /dev/mapper/
```

5. Create filesystems:
```bash
sudo mkfs.ext4 /dev/vg_data/lv_web
sudo mkfs.ext4 /dev/vg_data/lv_db
sudo mkfs.ext4 /dev/vg_data/lv_backup
```

6. Mount:
```bash
sudo mkdir /mnt/{web,db,backup}
sudo mount /dev/vg_data/lv_web /mnt/web
sudo mount /dev/vg_data/lv_db /mnt/db
sudo mount /dev/vg_data/lv_backup /mnt/backup
```

7. Verify:
```bash
df -h | grep vg_data
lsblk
```

**Questions:**
1. Draw the hierarchy: PV -> VG -> LV
2. What is a Physical Extent?
3. Why might you want multiple Logical Volumes in one Volume Group?

### Task 4.2: Resize Logical Volumes

**Objectives:**
- Extend LV (grow)
- Reduce LV (shrink)
- Understand the process

**Steps:**

1. Check current size:
```bash
df -h /mnt/web
sudo lvs
```

2. Extend LV and filesystem in one command:
```bash
sudo lvextend -L +200M -r /dev/vg_data/lv_web
```

3. Verify:
```bash
df -h /mnt/web
sudo lvs
```

4. Extend to use all free space:
```bash
# First, extend VG with another PV
sudo vgextend vg_data /dev/loop5

# Then extend LV
sudo lvextend -l +100%FREE -r /dev/vg_data/lv_web
```

5. Reduce LV (requires unmounting):
```bash
# Unmount
sudo umount /mnt/web

# Check filesystem
sudo e2fsck -f /dev/vg_data/lv_web

# Reduce filesystem first
sudo resize2fs /dev/vg_data/lv_web 400M

# Then reduce LV
sudo lvreduce -L 400M /dev/vg_data/lv_web

# Remount
sudo mount /dev/vg_data/lv_web /mnt/web
df -h /mnt/web
```

**Important:** Always reduce filesystem before LV, extend LV before filesystem.

### Task 4.3: LVM Snapshots

**Objectives:**
- Create snapshot
- Test snapshot for backup
- Revert using snapshot
- Remove snapshot

**Steps:**

1. Create test data:
```bash
sudo mkdir /mnt/db/data
for i in {1..10}; do
    sudo touch /mnt/db/data/file$i.txt
    echo "Original content $i" | sudo tee /mnt/db/data/file$i.txt
done
```

2. Create snapshot:
```bash
sudo lvcreate -L 100M -s -n lv_db_snap /dev/vg_data/lv_db
```

3. View snapshot:
```bash
sudo lvs
sudo lvdisplay /dev/vg_data/lv_db_snap
```

4. Mount snapshot:
```bash
sudo mkdir /mnt/db_snap
sudo mount -o ro /dev/vg_data/lv_db_snap /mnt/db_snap
```

5. Compare:
```bash
ls /mnt/db/data/
ls /mnt/db_snap/data/
```

6. Make changes to original:
```bash
sudo rm /mnt/db/data/file{5..10}.txt
echo "Modified" | sudo tee /mnt/db/data/file1.txt
ls /mnt/db/data/
```

7. Verify snapshot unchanged:
```bash
ls /mnt/db_snap/data/
cat /mnt/db_snap/data/file1.txt
```

8. Backup from snapshot:
```bash
sudo tar -czf ~/db_backup.tar.gz -C /mnt/db_snap .
```

9. Remove snapshot:
```bash
sudo umount /mnt/db_snap
sudo lvremove /dev/vg_data/lv_db_snap
```

10. Test snapshot merge (revert):
```bash
# Create snapshot
sudo lvcreate -L 100M -s -n lv_db_snap2 /dev/vg_data/lv_db

# Make more changes
echo "More changes" | sudo tee /mnt/db/data/newfile.txt

# Unmount to merge
sudo umount /mnt/db

# Merge (revert)
sudo lvconvert --merge /dev/vg_data/lv_db_snap2

# Remount and verify
sudo mount /dev/vg_data/lv_db /mnt/db
ls /mnt/db/data/
# newfile.txt should be gone!
```

**Questions:**
1. Why must you allocate space for a snapshot?
2. What happens if the snapshot fills up?
3. How can snapshots be used for testing?

---

## Exercise 5: Swap Management

### Task 5.1: Create and Use Swap File

**Objectives:**
- Create swap file
- Enable and test swap
- Monitor swap usage

**Steps:**

1. Create swap file:
```bash
sudo fallocate -l 2G /swapfile
# Or: sudo dd if=/dev/zero of=/swapfile bs=1M count=2048
```

2. Set permissions:
```bash
sudo chmod 600 /swapfile
ls -lh /swapfile
```

3. Format as swap:
```bash
sudo mkswap /swapfile
```

4. Enable swap:
```bash
sudo swapon /swapfile
```

5. Verify:
```bash
swapon --show
free -h
cat /proc/swaps
```

6. Make permanent:
```bash
echo "/swapfile none swap sw 0 0" | sudo tee -a /etc/fstab
```

7. Test swap usage with stress:
```bash
# Install stress tool
sudo apt install stress

# Create memory pressure
stress --vm 2 --vm-bytes 1G --timeout 60s &

# Monitor in another terminal
watch -n 1 free -h
```

8. Tune swappiness:
```bash
# View current
cat /proc/sys/vm/swappiness

# Set to 10 (less aggressive)
sudo sysctl vm.swappiness=10

# Make permanent
echo "vm.swappiness=10" | sudo tee -a /etc/sysctl.conf
```

**Questions:**
1. What is the default swappiness value?
2. When would you set swappiness to 0?
3. How much swap should a server with 16GB RAM have?

### Task 5.2: Create Swap on LVM

**Objectives:**
- Create swap logical volume
- Compare with swap file

**Steps:**

1. Create LV for swap:
```bash
sudo lvcreate -L 1G -n lv_swap vg_data
```

2. Format as swap:
```bash
sudo mkswap /dev/vg_data/lv_swap
```

3. Enable:
```bash
sudo swapon /dev/vg_data/lv_swap
```

4. Check priority:
```bash
swapon --show
cat /proc/swaps
```

5. Set priority:
```bash
sudo swapoff /dev/vg_data/lv_swap
sudo swapon -p 10 /dev/vg_data/lv_swap
```

6. Add to fstab:
```bash
# Get UUID
sudo blkid /dev/vg_data/lv_swap

# Add to fstab
UUID=xxx none swap sw,pri=10 0 0
```

**Challenge:**
Create a script that automatically creates swap if system has high memory pressure.

---

## Exercise 6: Access Control Lists

### Task 6.1: Basic ACLs

**Objectives:**
- Set ACLs for users and groups
- Understand ACL precedence
- View and manage ACLs

**Steps:**

1. Create test directory:
```bash
sudo mkdir /mnt/web/shared
sudo mkdir /mnt/web/shared/docs
```

2. Create test users:
```bash
sudo useradd -m alice
sudo useradd -m bob
sudo useradd -m charlie
```

3. Set standard permissions:
```bash
sudo chown root:root /mnt/web/shared
sudo chmod 755 /mnt/web/shared
```

4. Set ACL for specific user:
```bash
# Give alice read+write
sudo setfacl -m u:alice:rw /mnt/web/shared/docs

# Verify
getfacl /mnt/web/shared/docs
ls -l /mnt/web/shared/docs  # Notice the + sign
```

5. Set ACL for group:
```bash
# Create group
sudo groupadd developers

# Add users to group
sudo usermod -a -G developers bob
sudo usermod -a -G developers charlie

# Set group ACL
sudo setfacl -m g:developers:rx /mnt/web/shared/docs
```

6. View all ACLs:
```bash
getfacl /mnt/web/shared/docs
```

7. Test permissions:
```bash
# As alice (should have read-write)
sudo -u alice touch /mnt/web/shared/docs/alice_file.txt

# As bob (should have read-execute only)
sudo -u bob touch /mnt/web/shared/docs/bob_file.txt  # Should fail
sudo -u bob cat /mnt/web/shared/docs/alice_file.txt  # Should work
```

8. Remove specific ACL:
```bash
sudo setfacl -x u:alice /mnt/web/shared/docs
```

9. Remove all ACLs:
```bash
sudo setfacl -b /mnt/web/shared/docs
```

### Task 6.2: Default ACLs

**Objectives:**
- Set default ACLs for inheritance
- Understand how new files inherit ACLs

**Steps:**

1. Set default ACL on directory:
```bash
sudo setfacl -d -m g:developers:rw /mnt/web/shared
```

2. View:
```bash
getfacl /mnt/web/shared
```

3. Create new file:
```bash
sudo touch /mnt/web/shared/newfile.txt
```

4. Check inherited ACLs:
```bash
getfacl /mnt/web/shared/newfile.txt
```

5. Recursive ACLs:
```bash
sudo mkdir -p /mnt/web/shared/project/{src,docs,tests}
sudo setfacl -R -m g:developers:rwx /mnt/web/shared/project
sudo setfacl -R -d -m g:developers:rwx /mnt/web/shared/project
```

**Challenge:**
Create a shared directory where:
- Alice has full control
- Bob can only read
- New files automatically inherit these permissions

---

## Exercise 7: Monitoring and Troubleshooting

### Task 7.1: I/O Performance Analysis

**Objectives:**
- Use iostat to monitor I/O
- Identify bottlenecks
- Understand key metrics

**Steps:**

1. Install sysstat:
```bash
sudo apt install sysstat
```

2. Baseline monitoring:
```bash
iostat -x 1 5
```

3. Create I/O load:
```bash
# In one terminal:
dd if=/dev/zero of=/mnt/web/testfile bs=1M count=1000 oflag=direct

# In another terminal:
iostat -x 1
```

4. Analyze output:
```
Device   r/s    w/s   rkB/s   wkB/s  %util
loop0   0.00  150.00   0.00  153600.00  95.00
```

5. Use iotop:
```bash
sudo iotop -o
```

6. Monitor specific mount point:
```bash
watch -n 1 'df -h | grep web'
```

**Questions:**
1. What does %util close to 100% mean?
2. What is the difference between await and svctm?
3. When would you see high r/s but low rkB/s?

### Task 7.2: Find Disk Space Issues

**Objectives:**
- Identify what's using disk space
- Find deleted but open files
- Manage inode exhaustion

**Steps:**

1. Find largest directories:
```bash
sudo du -h /mnt/web | sort -rh | head -20
```

2. Find large files:
```bash
sudo find /mnt/web -type f -size +10M -exec ls -lh {} \;
```

3. Interactive disk usage:
```bash
sudo apt install ncdu
sudo ncdu /mnt/web
```

4. Simulate deleted but open file:
```bash
# Create and open file
dd if=/dev/zero of=/tmp/bigfile bs=1M count=100
tail -f /tmp/bigfile &
TAIL_PID=$!

# Delete file
rm /tmp/bigfile

# Check disk space (still used)
df -h /tmp

# Find deleted but open files
lsof | grep deleted

# Kill process to free space
kill $TAIL_PID
```

5. Check inode usage:
```bash
df -i
```

6. Simulate inode exhaustion:
```bash
# Create many small files
sudo mkdir /mnt/web/many_files
for i in {1..10000}; do
    sudo touch /mnt/web/many_files/file$i
done

# Check inodes
df -i /mnt/web
```

---

## Exercise 8: Backup and Recovery

### Task 8.1: Implement Backup Strategy

**Objectives:**
- Create full backup
- Create incremental backup
- Test restore

**Steps:**

1. Create backup directory structure:
```bash
sudo mkdir -p /backup/{full,incremental}
```

2. Create test data:
```bash
sudo mkdir -p /mnt/db/production
for i in {1..50}; do
    sudo dd if=/dev/urandom of=/mnt/db/production/data$i.bin bs=1M count=10
done
```

3. Full backup with rsync:
```bash
sudo rsync -av --delete /mnt/db/production/ /backup/full/
```

4. Record time:
```bash
date > /backup/full/.backup_time
```

5. Make changes:
```bash
# Add files
sudo dd if=/dev/urandom of=/mnt/db/production/new_data.bin bs=1M count=10

# Modify files
sudo dd if=/dev/urandom of=/mnt/db/production/data1.bin bs=1M count=5 conv=notrunc

# Delete files
sudo rm /mnt/db/production/data{40..50}.bin
```

6. Incremental backup:
```bash
sudo rsync -av --delete \
  --link-dest=/backup/full \
  /mnt/db/production/ \
  /backup/incremental/$(date +%Y%m%d)/
```

7. Check space usage:
```bash
du -sh /backup/*
du -sh /backup/incremental/*
```

8. Restore test:
```bash
# Simulate disaster
sudo rm -rf /mnt/db/production/*

# Restore from latest incremental
sudo rsync -av /backup/incremental/$(date +%Y%m%d)/ /mnt/db/production/

# Verify
ls /mnt/db/production/
```

### Task 8.2: Snapshot-Based Backup

**Objectives:**
- Use LVM snapshots for consistent backups
- Create automated backup script

**Steps:**

1. Create backup script:
```bash
cat > backup.sh << 'EOF'
#!/bin/bash

BACKUP_DIR="/backup/snapshots"
DATE=$(date +%Y%m%d_%H%M%S)
LV_NAME="lv_db"
VG_NAME="vg_data"
SNAP_NAME="${LV_NAME}_snap"
MOUNT_POINT="/mnt/db"
SNAP_MOUNT="/mnt/snapshot_temp"

# Create snapshot
echo "Creating snapshot..."
lvcreate -L 500M -s -n $SNAP_NAME /dev/$VG_NAME/$LV_NAME

# Mount snapshot
mkdir -p $SNAP_MOUNT
mount -o ro /dev/$VG_NAME/$SNAP_NAME $SNAP_MOUNT

# Backup from snapshot
echo "Backing up..."
mkdir -p $BACKUP_DIR
rsync -av $SNAP_MOUNT/ $BACKUP_DIR/$DATE/

# Cleanup
umount $SNAP_MOUNT
lvremove -y /dev/$VG_NAME/$SNAP_NAME

echo "Backup complete: $BACKUP_DIR/$DATE"
EOF

chmod +x backup.sh
```

2. Test script:
```bash
sudo ./backup.sh
```

3. Schedule with cron:
```bash
# Edit crontab
sudo crontab -e

# Add daily backup at 2 AM
0 2 * * * /path/to/backup.sh > /var/log/backup.log 2>&1
```

4. Add retention policy:
```bash
# Keep only last 7 days
find /backup/snapshots -maxdepth 1 -type d -mtime +7 -exec rm -rf {} \;
```

---

## Exercise 9: Performance Tuning

### Task 9.1: I/O Scheduler Tuning

**Objectives:**
- Test different I/O schedulers
- Measure performance impact
- Choose optimal scheduler

**Steps:**

1. Check current scheduler:
```bash
cat /sys/block/loop0/queue/scheduler
```

2. Create benchmark script:
```bash
cat > benchmark.sh << 'EOF'
#!/bin/bash

DEVICE=$1
MOUNT=$2
SCHEDULER=$3

echo $SCHEDULER > /sys/block/$DEVICE/queue/scheduler
echo "Testing $SCHEDULER..."

# Drop caches
echo 3 > /proc/sys/vm/drop_caches

# Sequential write
dd if=/dev/zero of=$MOUNT/testfile bs=1M count=500 oflag=direct 2>&1 | grep copied

# Sequential read
dd if=$MOUNT/testfile of=/dev/null bs=1M iflag=direct 2>&1 | grep copied

# Cleanup
rm $MOUNT/testfile
EOF

chmod +x benchmark.sh
```

3. Test schedulers:
```bash
sudo ./benchmark.sh loop0 /mnt/web mq-deadline
sudo ./benchmark.sh loop0 /mnt/web none
sudo ./benchmark.sh loop0 /mnt/web bfq
```

4. Record results and choose best.

### Task 9.2: Mount Options Optimization

**Objectives:**
- Test various mount options
- Measure impact on performance

**Steps:**

1. Baseline (default options):
```bash
sudo mount /dev/loop0 /mnt/test
time (for i in {1..1000}; do touch /mnt/test/file$i; done)
```

2. With noatime:
```bash
sudo mount -o remount,noatime /mnt/test
time (for i in {1..1000}; do touch /mnt/test/file$i; done)
```

3. With noatime,nodiratime:
```bash
sudo mount -o remount,noatime,nodiratime /mnt/test
time (for i in {1..1000}; do touch /mnt/test/file$i; done)
```

4. Compare results.

---

## Final Project: Complete Storage Solution

**Objectives:**
Design and implement a complete storage solution incorporating all concepts learned.

**Requirements:**

1. **RAID Configuration:**
   - Create RAID 5 array with 3 disks
   - Monitor array health
   - Test failure and recovery

2. **LVM Setup:**
   - Use RAID as PV
   - Create VG
   - Create 3 LVs: web (40%), database (40%), backup (20%)

3. **File Systems:**
   - ext4 for web
   - XFS for database
   - btrfs for backup

4. **Security:**
   - Encrypt backup LV with LUKS
   - Set ACLs on web directory
   - Make important config files immutable

5. **Swap:**
   - Create 2GB swap on LVM
   - Configure swappiness for server use

6. **Monitoring:**
   - Script to check RAID status
   - Script to monitor disk usage
   - Alert if disk > 90% full

7. **Backup:**
   - Automated daily backup using snapshots
   - Keep 7 days of backups
   - Test restore procedure

8. **Performance:**
   - Tune I/O scheduler
   - Optimize mount options
   - Benchmark before/after

9. **Documentation:**
   - Architecture diagram
   - Setup procedures
   - Recovery procedures
   - Performance test results

**Deliverables:**
- All scripts
- Configuration files
- Documentation
- Test results
- Presentation

---

## Additional Challenges

1. **Multi-Tier Storage:**
   - Implement hot/warm/cold storage tiers
   - Automatic data migration based on access patterns

2. **High Availability:**
   - Set up shared storage with cluster file system
   - Implement automatic failover

3. **Compression Testing:**
   - Compare compressed vs uncompressed filesystems
   - Measure space savings and performance impact

4. **Recovery Scenarios:**
   - Recover from failed RAID disk
   - Recover from corrupted filesystem
   - Recover from deleted LVM metadata

---

## Assessment Criteria

For the final project, you will be evaluated on:

1. **Functionality (40%)**
   - All components work correctly
   - RAID, LVM, file systems properly configured
   - Backup and recovery procedures work

2. **Performance (20%)**
   - Proper tuning applied
   - Benchmarks show improvements
   - Optimal choices for workload

3. **Security (15%)**
   - Encryption properly configured
   - ACLs correctly set
   - Permissions appropriate

4. **Automation (15%)**
   - Scripts are robust
   - Proper error handling
   - Logging implemented

5. **Documentation (10%)**
   - Clear and complete
   - Easy to follow
   - Includes troubleshooting

---

## Resources

**Man Pages:**
- `man mkfs.ext4`, `man mkfs.xfs`, `man mkfs.btrfs`
- `man mount`, `man fstab`
- `man mdadm`, `man md`
- `man pvcreate`, `man vgcreate`, `man lvcreate`
- `man setfacl`, `man getfacl`
- `man cryptsetup`
- `man iostat`, `man iotop`

**Online Resources:**
- Linux Documentation Project (TLDP)
- Red Hat System Administration Guide
- ArchWiki (excellent storage documentation)
- Ubuntu Server Guide

**Tools to Install:**
- sysstat (iostat, mpstat)
- iotop
- ncdu
- smartmontools
- rsync
- stress

---

End of Exercises


