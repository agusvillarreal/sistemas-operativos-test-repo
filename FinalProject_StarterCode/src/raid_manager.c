#include "raid_manager.h"
#include <sys/wait.h>

/*
 * RAID Manager Implementation
 *
 * This file implements the RAID management functionality.
 * You need to implement all the functions marked with TODO.
 *
 * Key tools to use:
 * - mdadm: for creating and managing RAID arrays
 * - /proc/mdstat: for reading RAID status
 * - execute_command(): utility function for running shell commands
 */

/* =================================================================== */
/* MODULE INITIALIZATION                                               */
/* =================================================================== */

int raid_manager_init(void) {
  LOG_INFO("Initializing RAID manager");

  /* TODO: Check if mdadm is installed */

  /* TODO: Check if /proc/mdstat exists */

  /* TODO: Initialize any global state */

  return SUCCESS;
}

void raid_manager_cleanup(void) {
  LOG_INFO("Cleaning up RAID manager");

  /* TODO: Clean up any resources */
}

/* =================================================================== */
/* RAID CREATION                                                       */
/* =================================================================== */

int raid_create(const char *array_name, int level, char **devices,
                int device_count, int chunk_size_kb) {

  if (!array_name || !devices || device_count == 0) {
    LOG_ERROR("Invalid parameters to raid_create");
    return ERROR_INVALID_PARAM;
  }

  /* Validate RAID level */
  if (!raid_validate_level(level)) {
    LOG_ERROR("Invalid RAID level: %d", level);
    return ERROR_INVALID_PARAM;
  }

  /* Validate device count for this RAID level */
  if (!raid_validate_device_count(level, device_count)) {
    LOG_ERROR("Invalid device count %d for RAID level %d", device_count, level);
    return ERROR_INVALID_PARAM;
  }

  /* Check if array already exists */
  if (raid_exists(array_name)) {
    LOG_ERROR("RAID array %s already exists", array_name);
    return ERROR_BUSY;
  }

  /* TODO: Build mdadm command */
  /* Example: mdadm --create /dev/md0 --level=1 --raid-devices=2 /dev/loop0
   * /dev/loop1 */

  char cmd[1024];
  char device_path[MAX_PATH_LEN];

  /* Build device path */
  snprintf(device_path, sizeof(device_path), "/dev/%s", array_name);

  /* Start building command */
  snprintf(cmd, sizeof(cmd), "mdadm --create %s --level=%d --raid-devices=%d",
           device_path, level, device_count);

  /* TODO: Add chunk size if specified */
  if (chunk_size_kb > 0) {
    /* Add --chunk=SIZE option */
  }

  /* TODO: Add device paths to command */
  for (int i = 0; i < device_count; i++) {
    /* Append each device to the command */
    /* Make sure to check for buffer overflow! */
  }

  /* TODO: Execute the command */
  LOG_INFO("Creating RAID array %s with level %d", array_name, level);
  LOG_DEBUG("Command: %s", cmd);

  /* int ret = execute_command_with_status(cmd, &exit_status); */

  /* TODO: Check result and return appropriate error code */

  LOG_INFO("RAID array %s created successfully", array_name);
  return SUCCESS;
}

/* =================================================================== */
/* RAID INFORMATION                                                    */
/* =================================================================== */

int raid_get_info(const char *array_name, raid_array_t *info) {

  if (!array_name || !info) {
    return ERROR_INVALID_PARAM;
  }

  /* TODO: Check if array exists */

  /* TODO: Run: mdadm --detail /dev/mdX */

  /* TODO: Parse output and fill info structure */
  /*
   * The output looks like:
   * /dev/md0:
   *       Version : 1.2
   *   Raid Level : raid1
   *   Array Size : 1046528 (1022.00 MiB 1071.64 MB)
   *   Used Dev Size : 1046528 (1022.00 MiB 1071.64 MB)
   *    Raid Devices : 2
   *   Total Devices : 2
   *     State : active
   *    Active Devices : 2
   *   Failed Devices : 0
   *    Spare Devices : 0
   *
   * Number   Major   Minor   RaidDevice State
   *    0       7        0        0      active sync   /dev/loop0
   *    1       7        1        1      active sync   /dev/loop1
   */

  /* TODO: Fill in info->name */
  /* TODO: Fill in info->raid_level */
  /* TODO: Fill in info->num_devices */
  /* TODO: Fill in info->status */
  /* TODO: Fill in device list */

  return SUCCESS;
}

int raid_monitor(const char *array_name, raid_array_t *info) {

  /* This is similar to raid_get_info but focuses on status */

  /* TODO: Get current status */

  /* TODO: Check for degraded arrays */

  /* TODO: Check rebuild progress */

  /* TODO: Update info structure */

  return raid_get_info(array_name, info);
}

/* =================================================================== */
/* RAID LISTING                                                        */
/* =================================================================== */

int raid_list(raid_list_t *list) {

  if (!list) {
    return ERROR_INVALID_PARAM;
  }

  /* Initialize list */
  memset(list, 0, sizeof(raid_list_t));

  /* TODO: Parse /proc/mdstat */
  return raid_parse_mdstat(list);
}

int raid_parse_mdstat(raid_list_t *list) {

  /* TODO: Open /proc/mdstat */
  FILE *fp = fopen("/proc/mdstat", "r");
  if (!fp) {
    LOG_ERROR("Failed to open /proc/mdstat: %s", strerror(errno));
    return ERROR_GENERAL;
  }

  /* TODO: Parse the file line by line */
  /*
   * Example /proc/mdstat content:
   *
   * Personalities : [raid1]
   * md0 : active raid1 loop1[1] loop0[0]
   *       1046528 blocks super 1.2 [2/2] [UU]
   *
   * unused devices: <none>
   */

  char line[256];
  int array_count = 0;

  while (fgets(line, sizeof(line), fp)) {
    /* TODO: Parse each line */

    /* Look for lines like: md0 : active raid1 loop1[1] loop0[0] */

    /* Extract: array name, status, RAID level, devices */

    /* Fill in list->arrays[array_count] */

    /* Increment array_count */
  }

  fclose(fp);

  list->num_arrays = array_count;
  return array_count;
}

/* =================================================================== */
/* DEVICE MANAGEMENT                                                   */
/* =================================================================== */

int raid_add_device(const char *array_name, const char *device) {

  if (!array_name || !device) {
    return ERROR_INVALID_PARAM;
  }

  /* TODO: Check if array exists */

  /* TODO: Check if device is valid */

  /* TODO: Run: mdadm --manage /dev/mdX --add /dev/deviceY */

  LOG_INFO("Adding device %s to RAID array %s", device, array_name);

  /* TODO: Build and execute command */

  return SUCCESS;
}

int raid_fail_device(const char *array_name, const char *device) {

  if (!array_name || !device) {
    return ERROR_INVALID_PARAM;
  }

  /* TODO: Run: mdadm --manage /dev/mdX --fail /dev/deviceY */

  LOG_WARNING("Marking device %s as failed in array %s", device, array_name);

  /* TODO: Build and execute command */

  return SUCCESS;
}

int raid_remove_device(const char *array_name, const char *device) {

  if (!array_name || !device) {
    return ERROR_INVALID_PARAM;
  }

  /* TODO: Run: mdadm --manage /dev/mdX --remove /dev/deviceY */

  LOG_INFO("Removing device %s from array %s", device, array_name);

  /* TODO: Build and execute command */

  return SUCCESS;
}

/* =================================================================== */
/* ARRAY CONTROL                                                       */
/* =================================================================== */

int raid_stop(const char *array_name) {

  if (!array_name) {
    return ERROR_INVALID_PARAM;
  }

  /* TODO: Check if array is mounted (should not be) */

  /* TODO: Run: mdadm --stop /dev/mdX */

  LOG_INFO("Stopping RAID array %s", array_name);

  /* TODO: Build and execute command */

  return SUCCESS;
}

int raid_start(const char *array_name) {

  if (!array_name) {
    return ERROR_INVALID_PARAM;
  }

  /* TODO: Run: mdadm --assemble /dev/mdX */

  LOG_INFO("Starting RAID array %s", array_name);

  /* TODO: Build and execute command */

  return SUCCESS;
}

/* =================================================================== */
/* STATUS FUNCTIONS                                                    */
/* =================================================================== */

int raid_exists(const char *array_name) {

  /* TODO: Check if /dev/mdX exists */

  char device_path[MAX_PATH_LEN];
  snprintf(device_path, sizeof(device_path), "/dev/%s", array_name);

  return file_exists(device_path);
}

int raid_get_status(const char *array_name, char *status, size_t status_size) {

  if (!array_name || !status) {
    return ERROR_INVALID_PARAM;
  }

  raid_array_t info;
  int ret = raid_get_info(array_name, &info);
  if (ret != SUCCESS) {
    return ret;
  }

  SAFE_STRNCPY(status, info.status, status_size);
  return SUCCESS;
}

int raid_is_healthy(const char *array_name) {

  char status[32];
  int ret = raid_get_status(array_name, status, sizeof(status));
  if (ret != SUCCESS) {
    return ret;
  }

  /* Check if status is "active" */
  return (strcmp(status, RAID_STATUS_ACTIVE) == 0) ? 1 : 0;
}

int raid_get_resync_progress(const char *array_name) {

  /* TODO: Parse /proc/mdstat for resync progress */
  /*
   * Look for lines like:
   * [>....................]  resync =  5.4% (55808/1046528) finish=0.1min
   * speed=55808K/sec
   */

  /* TODO: Extract percentage */

  return 0; /* Return 0-100, or -1 if not resyncing */
}

/* =================================================================== */
/* HELPER FUNCTIONS                                                    */
/* =================================================================== */

int raid_exec_mdadm(const char *args) {

  char cmd[1024];
  snprintf(cmd, sizeof(cmd), "mdadm %s", args);

  LOG_DEBUG("Executing: %s", cmd);

  int exit_status;
  int ret = execute_command_with_status(cmd, &exit_status);

  if (ret != 0 || exit_status != 0) {
    LOG_ERROR("mdadm command failed: %s (exit: %d)", cmd, exit_status);
    return ERROR_RAID;
  }

  return SUCCESS;
}

int raid_validate_level(int level) {

  /* Check if level is one of: 0, 1, 5, 10 */
  return (level == 0 || level == 1 || level == 5 || level == 10);
}

int raid_validate_device_count(int level, int device_count) {

  /* Minimum devices required for each RAID level */
  switch (level) {
  case 0: /* RAID 0: minimum 2 devices */
    return device_count >= 2;

  case 1: /* RAID 1: minimum 2 devices */
    return device_count >= 2;

  case 5: /* RAID 5: minimum 3 devices */
    return device_count >= 3;

  case 10: /* RAID 10: minimum 4 devices (and even number) */
    return device_count >= 4 && (device_count % 2 == 0);

  default:
    return 0;
  }
}

