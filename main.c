#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <sys/utsname.h>
#include <unistd.h>

#define MAX_CMD_LEN 256
#define MAX_OUTPUT_LEN 1024

/**
 * Retrieves the value of a specific field from the /proc/meminfo file.
 * @param field The field to retrieve the value of.
 * @return The value of the specified field.
 */
unsigned long get_mem_value(const char *field) {
  FILE *fp;
  char line[256];
  unsigned long value = 0;

  fp = fopen("/proc/meminfo", "r");
  if (fp == NULL) {
    perror("Error opening file");
    exit(1);
  }

  while (fgets(line, sizeof(line), fp)) {
    if (strncmp(line, field, strlen(field)) == 0) {
      sscanf(line, "%*s %lu", &value);
      break;
    }
  }

  fclose(fp);
  return value;
}

/**
 * Retrieves the name of the operating system from the /etc/os-release file.
 * @param output The buffer to store the operating system name.
 */
void get_os(char *output) {
  FILE *fp;
  char line[256];

  fp = fopen("/etc/os-release", "r");
  if (fp == NULL) {
    perror("Error opening file");
    exit(1);
  }

  while (fgets(line, sizeof(line), fp)) {
    if (strncmp(line, "NAME=", 4) == 0) {
      sscanf(line, "NAME=\"%[^\"]\"", output);
      break;
    }
  }
}

/**
 * Main function that retrieves and prints system information.
 * @return 0 on success.
 */
int main() {
  struct passwd *pw = getpwuid(getuid());
  char hostname[256];
  gethostname(hostname, sizeof(hostname));

  struct utsname uname_data;
  uname(&uname_data);

  struct sysinfo sys_info;
  sysinfo(&sys_info);

  long uptime_days = sys_info.uptime / 86400;
  long uptime_hours = (sys_info.uptime % 86400) / 3600;
  long uptime_minutes = (sys_info.uptime % 3600) / 60;

  char uptime_str[256];
  char *uptime_str_ptr = uptime_str;
  if (uptime_days) {
    sprintf(uptime_str_ptr, "%jdd ", uptime_days);
    uptime_str_ptr += strlen(uptime_str_ptr);
  }
  if (uptime_hours % 24) {
    sprintf(uptime_str_ptr, "%jdh ", uptime_hours % 24);
    uptime_str_ptr += strlen(uptime_str_ptr);
  }
  if (uptime_minutes % 60) {
    sprintf(uptime_str_ptr, "%jdm ", uptime_minutes % 60);
    uptime_str_ptr += strlen(uptime_str_ptr);
  }
  sprintf(uptime_str_ptr, "%jds", sys_info.uptime % 60);

  long total_ram = get_mem_value("MemTotal:") / 1024;
  long used_ram = total_ram - get_mem_value("MemAvailable:") / 1024;

  char *wm = getenv("DESKTOP_SESSION");
  char *display_server = getenv("XDG_SESSION_TYPE");

  char os[256];
  get_os(os);

  printf("\033[1;34m                 "
         "\033[4m%s\033[0m\033[1;37m@\033[0m\033[1;34m\033[4m%s\033[0m\033[1;"
         "37m\n",
         pw->pw_name, hostname);
  printf("\033[1;33m       __        \033[0m\033[1;35mOS:\033[0m %s\n", os);
  printf("\033[1;33m      // \\       \033[0m\033[1;35mKernel:\033[0m %s\n",
         uname_data.release);
  printf("\033[1;33m      \\\\_/ /     \033[0m\033[1;35mUptime:\033[0m %s\n",
         uptime_str);
  printf("\033[1;33m    --(||)(')    \033[0m\033[1;35mRAM:\033[0m %ld MB / %ld "
         "MB\n",
         used_ram, total_ram);
  printf(
      "\033[1;33m       '''       \033[0m\033[1;35mWindow Manager:\033[0m %s\n",
      wm);
  printf("\033[1;33m                 \033[0m\033[1;35mServer:\033[0m %s\n",
         display_server);

  return 0;
}
