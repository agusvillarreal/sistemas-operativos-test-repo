# Module 6: File Systems and Storage Administration

**Duration:** 2-3 weeks  
**Prerequisites:** Modules 1-5, root access on Linux system  
**Level:** Advanced

---

## Module Overview

This module covers advanced Linux storage administration, including file systems, RAID, LVM, security, and performance tuning. Students will learn to manage enterprise-grade storage systems from the command line.

### Topics Covered

1. File Systems (ext4, XFS, btrfs)
2. RAID Configuration (levels 0, 1, 5, 10)
3. Logical Volume Management (LVM)
4. Virtual Memory and Swap
5. Security (ACLs, LUKS encryption, file attributes)
6. Monitoring and Troubleshooting
7. Backup and Recovery
8. Performance Tuning

---

## Learning Objectives

By the end of this module, students will be able to:

- Create and manage multiple file system types
- Configure and maintain RAID arrays
- Implement LVM with physical volumes, volume groups, and logical volumes
- Create and manage LVM snapshots
- Configure ACLs for fine-grained permissions
- Encrypt volumes with LUKS
- Monitor I/O and disk performance
- Implement automated backup strategies
- Tune file system and kernel parameters for optimal performance

---

## Module Structure

### 1. Lecture Notes

**File:** `LECTURE_NOTES.md` (100+ pages)

Comprehensive theory covering:
- File system concepts and architecture
- Detailed coverage of ext4, XFS, and btrfs
- RAID theory and implementation
- LVM architecture and usage
- Swap management
- ACLs and encryption
- Monitoring tools
- Backup strategies
- Performance optimization

### 2. Practical Exercises

**File:** `EXERCISES.md`

Hands-on labs including:
- 9 detailed exercises with step-by-step instructions
- Lab setup with loop devices
- Real-world scenarios
- Troubleshooting tasks
- Final project integrating all concepts

### 3. Command Reference

Quick reference for all commands covered in the module.

---

## Getting Started

### Prerequisites Check

Ensure you have:
- Linux system (Ubuntu 22.04+ recommended)
- Root/sudo access
- At least 40GB free disk space
- Virtual machine (REQUIRED for safety)

### Lab Setup

**Step 1: Take VM Snapshot**
```bash
# Always have a recovery point!
```

**Step 2: Install Required Tools**
```bash
sudo apt update
sudo apt install -y \
  mdadm \
  lvm2 \
  xfsprogs \
  btrfs-progs \
  cryptsetup \
  acl \
  attr \
  sysstat \
  iotop \
  ncdu \
  smartmontools \
  rsync
```

**Step 3: Create Loop Devices**
```bash
# Create disk images for testing
for i in {0..7}; do
    dd if=/dev/zero of=~/disk$i.img bs=1M count=1024
    sudo losetup /dev/loop$i ~/disk$i.img
done

# Verify
losetup -a
```

**Step 4: Start with Exercise 1**
Open `EXERCISES.md` and begin with Exercise 1: File System Basics.

---

## Module Timeline

### Week 1: File Systems and RAID

**Days 1-2: File System Basics**
- Read LECTURE_NOTES.md sections 1-3
- Complete Exercise 1: File System Basics
- Complete Exercise 2: XFS File System

**Days 3-5: RAID Configuration**
- Read LECTURE_NOTES.md section 4
- Complete Exercise 3: RAID Configuration
- Practice RAID failure scenarios

**Weekend:**
- Review RAID levels
- Practice RAID commands

### Week 2: LVM and Security

**Days 1-3: Logical Volume Management**
- Read LECTURE_NOTES.md section 5
- Complete Exercise 4: LVM Management
- Practice resizing and snapshots

**Days 4-5: Security**
- Read LECTURE_NOTES.md sections 6-7
- Complete Exercise 5: Swap Management
- Complete Exercise 6: Access Control Lists

**Weekend:**
- Review LVM hierarchy
- Practice ACLs and encryption

### Week 3: Advanced Topics and Project

**Days 1-2: Monitoring and Backup**
- Read LECTURE_NOTES.md sections 8-9
- Complete Exercise 7: Monitoring
- Complete Exercise 8: Backup and Recovery

**Days 3-4: Performance Tuning**
- Read LECTURE_NOTES.md section 10
- Complete Exercise 9: Performance Tuning

**Day 5 - End of Week:**
- Work on Final Project
- Documentation and testing

---

## Key Commands by Topic

### File Systems
```bash
mkfs.ext4        # Create ext4 filesystem
mkfs.xfs         # Create XFS filesystem
mount            # Mount filesystem
umount           # Unmount filesystem
fsck.ext4        # Check ext4 filesystem
xfs_repair       # Repair XFS filesystem
resize2fs        # Resize ext4
xfs_growfs       # Grow XFS
tune2fs          # Tune ext4 parameters
```

### RAID
```bash
mdadm --create   # Create RAID array
mdadm --detail   # View array details
mdadm --manage   # Manage array
cat /proc/mdstat # Quick status
mdadm --stop     # Stop array
mdadm --assemble # Start array
```

### LVM
```bash
pvcreate         # Create physical volume
vgcreate         # Create volume group
lvcreate         # Create logical volume
lvextend         # Extend logical volume
lvreduce         # Reduce logical volume
lvcreate -s      # Create snapshot
pvs, vgs, lvs    # View PVs, VGs, LVs
```

### Security
```bash
setfacl          # Set ACLs
getfacl          # View ACLs
cryptsetup       # LUKS encryption
chattr           # Set file attributes
lsattr           # View file attributes
```

### Monitoring
```bash
df -h            # Disk usage
du -sh           # Directory usage
iostat           # I/O statistics
iotop            # Top for I/O
vmstat           # Virtual memory stats
lsof             # List open files
```

---

## Assessment

### Lab Work (40%)
- Completion of Exercises 1-9
- Correct implementation
- Understanding demonstrated

### Final Project (40%)
- Complete storage solution
- All components working
- Documentation quality

### Quiz (20%)
- File system concepts
- RAID levels
- LVM architecture
- Security mechanisms
- Performance tuning

---

## Common Issues and Solutions

### Issue: Cannot create loop device
**Solution:**
```bash
# Load loop module
sudo modprobe loop

# Check available loop devices
ls -l /dev/loop*
```

### Issue: mdadm says device busy
**Solution:**
```bash
# Check if mounted
mount | grep mdX

# Stop array first
sudo mdadm --stop /dev/mdX

# Clear superblock
sudo mdadm --zero-superblock /dev/loopX
```

### Issue: Cannot unmount filesystem
**Solution:**
```bash
# Find what's using it
lsof /mnt/point
fuser -v /mnt/point

# Lazy unmount (last resort)
sudo umount -l /mnt/point
```

### Issue: LVM snapshot full
**Solution:**
```bash
# Extend snapshot
sudo lvextend -L +500M /dev/vg/snap

# Or remove and recreate with more space
sudo lvremove /dev/vg/snap
sudo lvcreate -L 2G -s -n snap /dev/vg/lv
```

---

## Best Practices

1. **Always use VMs for learning**
   - Never test on production systems
   - Take snapshots before major operations

2. **Use UUIDs in /etc/fstab**
   - More reliable than device names
   - Use `blkid` to find UUIDs

3. **Label your file systems**
   - Makes identification easier
   - Use meaningful names

4. **Monitor RAID regularly**
   - Set up email alerts
   - Check `/proc/mdstat` daily

5. **Test your backups**
   - Backup is useless if restore doesn't work
   - Practice restore procedures

6. **Document everything**
   - Keep records of your setup
   - Document recovery procedures

7. **Plan before implementing**
   - Decide on partition layout
   - Plan for growth
   - Consider redundancy needs

---

## Additional Resources

### Documentation
- Linux Documentation Project: https://www.tldp.org/
- Red Hat System Administration Guide
- ArchWiki Storage pages
- Ubuntu Server Guide

### Man Pages
Essential man pages to read:
```bash
man mkfs.ext4
man mount
man mdadm
man lvm
man setfacl
man cryptsetup
man iostat
```

### Online Tutorials
- DigitalOcean Community Tutorials
- Linux Academy / A Cloud Guru
- YouTube: Learn Linux TV channel

### Books
- "Linux System Administration" - Tom Adelstein & Bill Lubanovic
- "UNIX and Linux System Administration Handbook" - Nemeth et al.
- "Linux Bible" - Christopher Negus

---

## Next Steps After This Module

### For the Course
- Proceed to Final Project (integrates all modules)
- Complete project requirements
- Prepare presentation

### For Further Learning
- Study advanced topics:
  - Cluster file systems (GFS2, OCFS2)
  - Distributed file systems (Ceph, GlusterFS)
  - ZFS on Linux
  - Container storage (Docker volumes, Kubernetes PVs)

- Get certified:
  - RHCSA (Red Hat Certified System Administrator)
  - LFCS (Linux Foundation Certified System Administrator)

- Real-world practice:
  - Set up home NAS
  - Configure backup server
  - Build storage cluster

---

## Support and Questions

### During Class
- Office hours: [Schedule]
- Discussion forum: [Link]
- Email: [Contact]

### Self-Study
- Stack Overflow (tag: linux, storage, lvm, raid)
- Reddit: /r/linux, /r/linuxadmin
- IRC: #linux on Libera.Chat

---

## Quick Start Guide

**For Students:**

1. Read this README
2. Set up your lab environment
3. Read relevant sections of LECTURE_NOTES.md
4. Work through exercises in order
5. Take notes and document your work
6. Ask questions early
7. Start the final project early

**For Instructors:**

1. Review all materials
2. Prepare lab environment (VM templates)
3. Demonstrate setup in class
4. Emphasize safety (VM usage)
5. Schedule regular checkpoints
6. Provide feedback on exercises
7. Grade final projects thoroughly

---

## Files in This Module

```
Module6_FileSystems/
├── README.md               # This file
├── LECTURE_NOTES.md        # Complete theory (100+ pages)
├── EXERCISES.md            # Hands-on labs
└── QUICK_REFERENCE.md      # Command cheat sheet (to be created)
```

---

## Warning

These exercises involve potentially destructive operations:
- Creating file systems (destroys existing data)
- RAID operations (can destroy data)
- LVM operations (can destroy data)
- File system resizing (can cause data loss if done incorrectly)

**ALWAYS:**
- Use virtual machines
- Take snapshots before major operations
- Have backups
- Test in isolated environment
- Never use production systems for learning

---

## Changelog

**Version 1.0** (October 2025)
- Initial release
- Complete lecture notes
- 9 practical exercises
- Final project specification

---

## License

This educational material is provided for learning purposes as part of the Linux Systems Programming course.

---

## Acknowledgments

Based on real-world storage administration practices and industry best practices from Red Hat, Ubuntu, and SUSE documentation.

---

**Ready to begin? Start with the Lab Setup section above, then open LECTURE_NOTES.md!**

Good luck with your storage administration journey!


