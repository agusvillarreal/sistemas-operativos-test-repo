# Introduction to Bash Scripting

## Professional Sysadmin Lecture Notes

---

## Table of Contents

1. [What is Bash Scripting?](https://claude.ai/chat/6dfa667b-70d6-466e-9be0-5c24628fcd88#what-is-bash-scripting)
2. [Setting Up Your Environment](https://claude.ai/chat/6dfa667b-70d6-466e-9be0-5c24628fcd88#setting-up-your-environment)
3. [Basic Script Structure](https://claude.ai/chat/6dfa667b-70d6-466e-9be0-5c24628fcd88#basic-script-structure)
4. [Variables and Data Types](https://claude.ai/chat/6dfa667b-70d6-466e-9be0-5c24628fcd88#variables-and-data-types)
5. [Input/Output Operations](https://claude.ai/chat/6dfa667b-70d6-466e-9be0-5c24628fcd88#inputoutput-operations)
6. [Control Structures](https://claude.ai/chat/6dfa667b-70d6-466e-9be0-5c24628fcd88#control-structures)
7. [Functions](https://claude.ai/chat/6dfa667b-70d6-466e-9be0-5c24628fcd88#functions)
8. [File Operations](https://claude.ai/chat/6dfa667b-70d6-466e-9be0-5c24628fcd88#file-operations)
9. [Error Handling](https://claude.ai/chat/6dfa667b-70d6-466e-9be0-5c24628fcd88#error-handling)
10. [Best Practices](https://claude.ai/chat/6dfa667b-70d6-466e-9be0-5c24628fcd88#best-practices)
11. [Common Sysadmin Use Cases](https://claude.ai/chat/6dfa667b-70d6-466e-9be0-5c24628fcd88#common-sysadmin-use-cases)
12. [Debugging Techniques](https://claude.ai/chat/6dfa667b-70d6-466e-9be0-5c24628fcd88#debugging-techniques)

---

## What is Bash Scripting?

**Bash** (Bourne Again Shell) is a command-line interpreter and scripting language that allows system administrators to automate repetitive tasks, manage system configurations, and create powerful tools for system maintenance.


---

## Setting Up Your Environment

### Choosing an Editor

- **nano**: Beginner-friendly, simple interface
- **vim/vi**: Powerful, available everywhere
- **emacs**: Feature-rich, extensible
- **VS Code**: Modern IDE with bash extensions

### Script Location Best Practices

```bash
# Personal scripts
~/bin/              # User-specific scripts
~/.local/bin/       # XDG-compliant user scripts

# System-wide scripts
/usr/local/bin/     # Custom system scripts
/opt/scripts/       # Organization-specific scripts
```

### Making Scripts Executable

```bash
chmod +x script.sh
chmod 755 script.sh  # rwxr-xr-x (owner: rwx, group: r-x, others: r-x)
```

---

## Basic Script Structure

### Shebang Line

```bash
#!/bin/bash
# Always start with shebang to specify interpreter
```

### Basic Template

```bash
#!/bin/bash
#
# Script Name: example.sh
# Description: Brief description of what this script does
# Author: Your Name
# Date: YYYY-MM-DD
# Version: 1.0
#

# Exit on any error
set -e

# Your code here
echo "Hello, World!"
```

### Essential Set Options

```bash
set -e          # Exit immediately if command fails
set -u          # Exit if undefined variable is used
set -o pipefail # Pipeline fails if any command fails
set -x          # Debug mode - print commands as executed
```

---

## Variables and Data Types

### Variable Declaration

```bash
# String variables
name="John Doe"
server_name="web01"

# Integer variables
port=8080
max_connections=100

# Arrays
servers=("web01" "web02" "db01")
log_levels=("INFO" "WARN" "ERROR")
```

### Variable Usage

```bash
# Basic usage
echo $name
echo ${name}  # Preferred for clarity

# String manipulation
echo ${name,,}        # Convert to lowercase
echo ${name^^}        # Convert to uppercase
echo ${name:0:4}      # Substring (first 4 chars)
echo ${#name}         # String length
```

### Environment Variables

```bash
# Common system variables
echo $HOME           # User home directory
echo $USER           # Current username
echo $PATH           # Command search path
echo $PWD            # Current directory
echo $HOSTNAME       # System hostname

# Setting environment variables
export LOG_LEVEL="DEBUG"
export BACKUP_DIR="/opt/backups"
```

### Command Substitution

```bash
# Modern syntax (preferred)
current_date=$(date +%Y-%m-%d)
file_count=$(ls -1 | wc -l)
disk_usage=$(df -h / | tail -1 | awk '{print $5}')

# Legacy syntax (avoid in new scripts)
old_date=`date`
```

---

## Input/Output Operations

### Reading User Input

```bash
# Simple input
echo -n "Enter your username: "
read username

# Secure password input
echo -n "Enter password: "
read -s password
echo  # New line after hidden input

# Input with prompt
read -p "Enter server name: " server

# Reading multiple values
read -p "Enter name and age: " name age
```

### Command Line Arguments

```bash
#!/bin/bash
# Script arguments are accessed via $1, $2, etc.

script_name=$0
first_arg=$1
second_arg=$2
all_args=$@
arg_count=$#

echo "Script: $script_name"
echo "First argument: $first_arg"
echo "All arguments: $all_args"
echo "Argument count: $arg_count"

# Check if arguments provided
if [ $# -eq 0 ]; then
    echo "Usage: $0 <server_name> <port>"
    exit 1
fi
```

### Output Redirection

```bash
# Redirect stdout to file
echo "Log entry" > logfile.txt

# Append to file
echo "Another entry" >> logfile.txt

# Redirect stderr
command_that_fails 2> error.log

# Redirect both stdout and stderr
command > output.log 2>&1

# Discard output
command > /dev/null 2>&1
```

---

## Control Structures

### Conditional Statements

#### Basic If Statement

```bash
if [ condition ]; then
    # Commands
elif [ another_condition ]; then
    # Commands
else
    # Commands
fi
```

#### File Testing

```bash
file="/etc/passwd"

if [ -f "$file" ]; then
    echo "File exists"
fi

if [ -d "/var/log" ]; then
    echo "Directory exists"
fi

if [ -r "$file" ]; then
    echo "File is readable"
fi

if [ -w "$file" ]; then
    echo "File is writable"
fi

if [ -x "$file" ]; then
    echo "File is executable"
fi
```

#### String Comparisons

```bash
if [ "$string1" = "$string2" ]; then
    echo "Strings are equal"
fi

if [ "$string1" != "$string2" ]; then
    echo "Strings are different"
fi

if [ -z "$string" ]; then
    echo "String is empty"
fi

if [ -n "$string" ]; then
    echo "String is not empty"
fi
```

#### Numeric Comparisons

```bash
if [ "$num1" -eq "$num2" ]; then
    echo "Numbers are equal"
fi

if [ "$num1" -gt "$num2" ]; then
    echo "num1 is greater than num2"
fi

if [ "$num1" -lt "$num2" ]; then
    echo "num1 is less than num2"
fi
```

### Loops

#### For Loops

```bash
# Loop through list
for server in web01 web02 db01; do
    echo "Processing $server"
done

# Loop through array
servers=("web01" "web02" "db01")
for server in "${servers[@]}"; do
    echo "Checking $server"
done

# Loop through files
for file in /var/log/*.log; do
    echo "Processing $file"
done

# C-style loop
for ((i=1; i<=10; i++)); do
    echo "Iteration $i"
done
```

#### While Loops

```bash
# Basic while loop
counter=1
while [ $counter -le 5 ]; do
    echo "Count: $counter"
    ((counter++))
done

# Reading file line by line
while IFS= read -r line; do
    echo "Processing: $line"
done < input.txt

# Infinite loop with break condition
while true; do
    read -p "Enter command (quit to exit): " cmd
    if [ "$cmd" = "quit" ]; then
        break
    fi
    echo "You entered: $cmd"
done
```

### Case Statements

```bash
read -p "Enter environment (dev/staging/prod): " env

case $env in
    dev|development)
        echo "Development environment selected"
        config_file="dev.conf"
        ;;
    staging|stage)
        echo "Staging environment selected"
        config_file="staging.conf"
        ;;
    prod|production)
        echo "Production environment selected"
        config_file="prod.conf"
        ;;
    *)
        echo "Invalid environment: $env"
        exit 1
        ;;
esac
```

---

## Functions

### Basic Function Syntax

```bash
# Function definition
function backup_database() {
    echo "Starting database backup..."
    # Function code here
}

# Alternative syntax (preferred)
backup_database() {
    echo "Starting database backup..."
}

# Calling the function
backup_database
```

### Functions with Parameters

```bash
create_user() {
    local username=$1
    local user_home=$2
    
    if [ $# -ne 2 ]; then
        echo "Usage: create_user <username> <home_directory>"
        return 1
    fi
    
    echo "Creating user: $username"
    echo "Home directory: $user_home"
    
    # Actual user creation would go here
    # useradd -m -d "$user_home" "$username"
}

# Call with arguments
create_user "jdoe" "/home/jdoe"
```

### Return Values

```bash
check_service() {
    local service_name=$1
    
    if systemctl is-active --quiet "$service_name"; then
        return 0  # Service is running
    else
        return 1  # Service is not running
    fi
}

# Using return value
if check_service "nginx"; then
    echo "Nginx is running"
else
    echo "Nginx is not running"
fi
```

---

## File Operations

### Reading Files

```bash
# Read entire file into variable
content=$(cat /etc/hostname)

# Read file line by line
while IFS= read -r line; do
    echo "Line: $line"
done < /etc/passwd

# Check if file exists before reading
if [ -f "/etc/nginx/nginx.conf" ]; then
    nginx_config=$(cat /etc/nginx/nginx.conf)
fi
```

### Writing Files

```bash
# Write to file
echo "Log entry: $(date)" > /var/log/script.log

# Append to file
echo "Another entry" >> /var/log/script.log

# Create file with multiple lines
cat > config.txt << EOF
server_name=web01
port=8080
debug=true
EOF
```

### File Operations

```bash
# Copy with backup
cp important.conf important.conf.bak

# Move/rename
mv old_name.txt new_name.txt

# Create directory structure
mkdir -p /opt/myapp/{bin,config,logs}

# Find and process files
find /var/log -name "*.log" -mtime +7 -exec rm {} \;
```

---

## Error Handling

### Exit Codes

```bash
# Check exit code of last command
if [ $? -eq 0 ]; then
    echo "Command succeeded"
else
    echo "Command failed"
fi

# More concise
if command_here; then
    echo "Success"
else
    echo "Failed"
fi
```

### Error Handling Patterns

```bash
# Function with proper error handling
backup_config() {
    local source=$1
    local dest=$2
    
    # Validate arguments
    if [ $# -ne 2 ]; then
        echo "Error: Invalid number of arguments" >&2
        return 1
    fi
    
    # Check if source exists
    if [ ! -f "$source" ]; then
        echo "Error: Source file '$source' not found" >&2
        return 1
    fi
    
    # Perform backup
    if cp "$source" "$dest"; then
        echo "Backup successful: $source -> $dest"
        return 0
    else
        echo "Error: Backup failed" >&2
        return 1
    fi
}

# Trap for cleanup
cleanup() {
    echo "Cleaning up temporary files..."
    rm -f /tmp/script_temp.*
}

# Set trap for script exit
trap cleanup EXIT

# Set trap for interruption
trap 'echo "Script interrupted!"; exit 1' INT TERM
```

---

## Best Practices

### Script Security

```bash
# Use absolute paths
/bin/cp source dest

# Quote all variables
rm "$filename"          # Good
rm $filename           # Bad - word splitting issues

# Validate input
if [[ ! "$port" =~ ^[0-9]+$ ]]; then
    echo "Error: Port must be numeric"
    exit 1
fi
```

### Code Organization

```bash
#!/bin/bash
#
# Well-organized script structure
#

# Global variables
readonly SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
readonly LOG_FILE="/var/log/$(basename "$0" .sh).log"

# Configuration
readonly MAX_RETRIES=3
readonly TIMEOUT=30

# Functions
log_message() {
    echo "[$(date '+%Y-%m-%d %H:%M:%S')] $1" | tee -a "$LOG_FILE"
}

main() {
    log_message "Script started"
    
    # Main script logic here
    
    log_message "Script completed"
}

# Script execution
main "$@"
```

### Variable Naming Conventions

```bash
# Constants (readonly)
readonly MAX_CONNECTIONS=100
readonly CONFIG_DIR="/etc/myapp"

# Global variables
script_name=$(basename "$0")
temp_dir="/tmp"

# Local variables (in functions)
process_file() {
    local filename=$1
    local line_count
    local file_size
    
    line_count=$(wc -l < "$filename")
    file_size=$(stat -f%z "$filename" 2>/dev/null || stat -c%s "$filename")
}
```

---

## Common Sysadmin Use Cases

### System Monitoring Script

```bash
#!/bin/bash
# system_check.sh - Basic system health check

check_disk_space() {
    echo "=== Disk Usage ==="
    df -h | grep -E "^/dev" | while read line; do
        usage=$(echo "$line" | awk '{print $5}' | sed 's/%//')
        if [ "$usage" -gt 80 ]; then
            echo "WARNING: $(echo "$line" | awk '{print $1}') is ${usage}% full"
        fi
    done
}

check_memory() {
    echo "=== Memory Usage ==="
    free -h
    
    # Check if memory usage is high
    mem_percent=$(free | grep Mem | awk '{printf("%.0f", $3/$2 * 100)}')
    if [ "$mem_percent" -gt 80 ]; then
        echo "WARNING: Memory usage is ${mem_percent}%"
    fi
}

check_services() {
    echo "=== Critical Services ==="
    services=("ssh" "nginx" "mysql")
    
    for service in "${services[@]}"; do
        if systemctl is-active --quiet "$service"; then
            echo "✓ $service is running"
        else
            echo "✗ $service is NOT running"
        fi
    done
}

# Main execution
echo "System Health Check - $(date)"
echo "================================"
check_disk_space
echo
check_memory
echo
check_services
```

### Log Rotation Script

```bash
#!/bin/bash
# log_rotate.sh - Custom log rotation

rotate_log() {
    local log_file=$1
    local max_size=$2  # in MB
    
    if [ ! -f "$log_file" ]; then
        echo "Log file $log_file not found"
        return 1
    fi
    
    # Get file size in MB
    file_size=$(du -m "$log_file" | cut -f1)
    
    if [ "$file_size" -gt "$max_size" ]; then
        echo "Rotating $log_file (${file_size}MB > ${max_size}MB)"
        
        # Create backup with timestamp
        backup_name="${log_file}.$(date +%Y%m%d_%H%M%S)"
        mv "$log_file" "$backup_name"
        
        # Create new empty log file
        touch "$log_file"
        
        # Compress old log
        gzip "$backup_name"
        
        echo "Log rotated: $backup_name.gz"
    fi
}

# Rotate application logs
rotate_log "/var/log/myapp/application.log" 100
rotate_log "/var/log/myapp/error.log" 50
```

### User Management Script

```bash
#!/bin/bash
# user_manager.sh - Bulk user operations

create_users_from_file() {
    local user_file=$1
    
    if [ ! -f "$user_file" ]; then
        echo "Error: User file not found: $user_file"
        return 1
    fi
    
    while IFS=':' read -r username fullname department; do
        # Skip empty lines and comments
        [[ "$username" =~ ^#.*$ ]] && continue
        [[ -z "$username" ]] && continue
        
        echo "Creating user: $username ($fullname) - $department"
        
        # Create user with home directory
        if useradd -m -c "$fullname" -s /bin/bash "$username"; then
            echo "✓ User $username created successfully"
            
            # Set initial password (user must change on first login)
            echo "$username:TempPass123!" | chpasswd
            passwd -e "$username"  # Force password change
            
        else
            echo "✗ Failed to create user: $username"
        fi
        
    done < "$user_file"
}

# Example usage:
# create_users_from_file "users.txt"
# Format of users.txt:
# jdoe:John Doe:IT
# asmith:Alice Smith:HR
```

### Backup Script

```bash
#!/bin/bash
# backup.sh - System backup script

# Configuration
readonly BACKUP_DIR="/opt/backups"
readonly SOURCE_DIRS=("/etc" "/home" "/var/www")
readonly RETENTION_DAYS=30

# Logging function
log() {
    echo "[$(date '+%Y-%m-%d %H:%M:%S')] $1" | tee -a "$LOG_FILE"
}

# Create backup
create_backup() {
    local timestamp=$(date +%Y%m%d_%H%M%S)
    local backup_name="system_backup_$timestamp"
    local backup_path="$BACKUP_DIR/$backup_name"
    
    log "Starting backup: $backup_name"
    
    # Create backup directory
    mkdir -p "$backup_path"
    
    # Backup each source directory
    for source in "${SOURCE_DIRS[@]}"; do
        if [ -d "$source" ]; then
            log "Backing up $source"
            tar -czf "$backup_path/$(basename "$source").tar.gz" -C "$(dirname "$source")" "$(basename "$source")"
        else
            log "WARNING: Source directory not found: $source"
        fi
    done
    
    # Create backup manifest
    {
        echo "Backup created: $(date)"
        echo "Hostname: $(hostname)"
        echo "Sources:"
        printf '%s\n' "${SOURCE_DIRS[@]}"
    } > "$backup_path/manifest.txt"
    
    log "Backup completed: $backup_path"
}

# Cleanup old backups
cleanup_old_backups() {
    log "Cleaning up backups older than $RETENTION_DAYS days"
    find "$BACKUP_DIR" -name "system_backup_*" -type d -mtime +$RETENTION_DAYS -exec rm -rf {} \;
}

# Main execution
LOG_FILE="$BACKUP_DIR/backup.log"
mkdir -p "$BACKUP_DIR"

create_backup
cleanup_old_backups

log "Backup script finished"
```

---

## Debugging Techniques

### Debug Mode

```bash
# Enable debug mode
set -x

# Disable debug mode
set +x

# Debug specific section
{
    set -x
    # Commands to debug
    ls -la
    cat file.txt
    set +x
}
```

### Logging for Debugging

```bash
debug_log() {
    if [ "$DEBUG" = "true" ]; then
        echo "[DEBUG] $1" >&2
    fi
}

# Usage
DEBUG=true  # Set via environment or script parameter
debug_log "Processing file: $filename"
```

### Testing Scripts

```bash
# Dry run option
DRY_RUN=false

execute_command() {
    local cmd=$1
    
    if [ "$DRY_RUN" = "true" ]; then
        echo "DRY RUN: Would execute: $cmd"
    else
        eval "$cmd"
    fi
}

# Usage: script.sh --dry-run
if [ "$1" = "--dry-run" ]; then
    DRY_RUN=true
fi
```

---

## Advanced Topics for Further Study

### Signal Handling

- Understanding and handling system signals (SIGTERM, SIGINT, etc.)
- Graceful script shutdown
- Process management

### Regular Expressions

- Pattern matching with grep, sed, awk
- Text processing and log analysis
- Configuration file manipulation

### Advanced I/O

- Named pipes (FIFOs)
- Process substitution
- File descriptors

### Integration with System Tools

- Systemd service integration
- Cron job automation
- Log aggregation systems

---

## Summary

Bash scripting is an essential skill for system administrators, enabling automation of routine tasks, system monitoring, and efficient resource management. Key takeaways:

1. **Start simple**: Begin with basic scripts and gradually add complexity
2. **Follow best practices**: Use proper error handling, variable quoting, and code organization
3. **Test thoroughly**: Always test scripts in non-production environments first
4. **Document well**: Clear comments and documentation save time later
5. **Security first**: Validate input, use absolute paths, and follow the principle of least privilege

