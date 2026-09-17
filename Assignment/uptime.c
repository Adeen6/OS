#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
  int ticks = uptime();
  if (ticks < 0) {
    fprintf(2, "uptime failed\n");
    exit(1);
  }
  printf("up %d ticks\n", ticks);
  exit(0);
}