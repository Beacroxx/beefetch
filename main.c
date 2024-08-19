#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <sys/utsname.h>
#include <pwd.h>

#define MAX_CMD_LEN 256
#define MAX_OUTPUT_LEN 1024

/**
 * Retrieves the value of a specific field from the /proc/meminfo file.
 * @param field The field to retrieve the value of.
 * @return The value of the specified field.
 */
unsigned long get_mem_value(const char *field) {
  FILE *fp; // File pointer
  char line[256]; // Buffer for reading lines
  unsigned long value = 0; // Stores the value of the field

  fp = fopen("/proc/meminfo", "r"); // Open the file
  if (fp == NULL) { // Error checking
    perror("Error opening file");
    exit(1);
  }

  while (fgets(line, sizeof(line), fp)) { // Read lines
    if (strncmp(line, field, strlen(field)) == 0) { // Check if the line starts with the field
      sscanf(line, "%*s %lu", &value); // Extract the value
      break;
    }
  }

  fclose(fp); // Close the file
  return value; // Return the value
}

/**
 * Retrieves the name of the operating system from the /etc/os-release file.
 * @param output The buffer to store the operating system name.
 */
void get_os(char *output) {
  FILE *fp; // File pointer
  char line[256]; // Buffer for reading lines

  fp = fopen("/etc/os-release", "r"); // Open the file
  if (fp == NULL) { // Error checking
    perror("Error opening file");
    exit(1);
  }

  while (fgets(line, sizeof(line), fp)) { // Read lines
    if (strncmp(line, "NAME=", 4) == 0) { // Check if the line starts with NAME=
      sscanf(line, "NAME=\"%[^\"]\"", output); // Extract the name
      break;
    }
  }
}

/**
 * Main function that retrieves and prints system information.
 * @return 0 on success.
 */
int main() {
  struct passwd *pw = getpwuid(getuid()); // Get the current user information
  char hostname[256]; // Buffer for the hostname
  gethostname(hostname, sizeof(hostname)); // Get the hostname

  struct utsname uname_data; // Struct to store kernel information
  uname(&uname_data); // Get the kernel information

  struct sysinfo sys_info; // Struct to store system information
  sysinfo(&sys_info); // Get the system information

  long uptime_days = sys_info.uptime / 86400; // Calculate the uptime in days
  long uptime_hours = (sys_info.uptime % 86400) / 3600; // Calculate the uptime in hours
  long uptime_minutes = (sys_info.uptime % 3600) / 60; // Calculate the uptime in minutes

  char uptime_str[256]; // Buffer for the uptime string
  char *uptime_str_ptr = uptime_str; // Pointer to traverse the buffer
  if (uptime_days) { // If there are days
    sprintf(uptime_str_ptr, "%jdd ", uptime_days); // Append the days to the string
    uptime_str_ptr += strlen(uptime_str_ptr); // Update the pointer
  }
  if (uptime_hours % 24) { // If there are hours
    sprintf(uptime_str_ptr, "%jdh ", uptime_hours % 24); // Append the hours to the string
    uptime_str_ptr += strlen(uptime_str_ptr); // Update the pointer
  }
  if (uptime_minutes % 60) { // If there are minutes
    sprintf(uptime_str_ptr, "%jdm ", uptime_minutes % 60); // Append the minutes to the string
    uptime_str_ptr += strlen(uptime_str_ptr); // Update the pointer
  }
  sprintf(uptime_str_ptr, "%jds", sys_info.uptime % 60); // Append the seconds to the string

  long total_ram = get_mem_value("MemTotal:") / 1024; // Get the total memory in MB
  long used_ram = total_ram - get_mem_value("MemAvailable:") / 1024; // Get the used memory in MB

  char* wm = getenv("DESKTOP_SESSION"); // Get the window manager
  char* display_server = getenv("XDG_SESSION_TYPE"); // Get the display server

  char os[256]; // Buffer for the operating system name
  get_os(os); // Get the operating system name

  printf("\033[1;34m                 \033[4m%s\033[0m\033[1;37m@\033[0m\033[1;34m\033[4m%s\033[0m\033[1;37m\n",
         pw->pw_name, hostname); // Print the username and hostname
  printf("\033[1;33m       __        \033[0m\033[1;35mOS:\033[0m %s\n", os); // Print the operating system name
  printf("\033[1;33m      // \\       \033[0m\033[1;35mKernel:\033[0m %s\n", uname_data.release); // Print the kernel release
  printf("\033[1;33m      \\\\_/ /     \033[0m\033[1;35mUptime:\033[0m %s\n", uptime_str); // Print the uptime
  printf("\033[1;33m    --(||)(')    \033[0m\033[1;35mRAM:\033[0m %ld MB / %ld MB\n",
         used_ram, total_ram); // Print the used and total memory
  printf("\033[1;33m       '''       \033[0m\033[1;35mWindow Manager:\033[0m %s\n", wm); // Print the window manager
  printf("\033[1;33m                 \033[0m\033[1;35mServer:\033[0m %s\n", display_server); // Print the display server

  return 0; // Exit successfully
}

