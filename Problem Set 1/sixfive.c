#include "kernel/types.h"
#include "user/user.h"

void process(int fd)
{
  char buf[1];
  int num = 0;
  int has_digits = 0;

  while(read(fd, buf, 1) > 0){
    if(buf[0] >= '0' && buf[0] <= '9'){
      // it's a digit: fold it into the running number
      num = num * 10 + (buf[0] - '0');
      has_digits = 1;
    } else {
      // it's a separator (space, dash, newline, etc.)
      if(has_digits && (num % 5 == 0 || num % 6 == 0)){
        printf("%d\n", num);
      }
      num = 0;
      has_digits = 0;
    }
  }
  // don't forget: check the last number if the file doesn't end in a separator
  if(has_digits && (num % 5 == 0 || num % 6 == 0)){
    printf("%d\n", num);
  }
}

int main(int argc, char *argv[])
{
  if(argc <= 1){
    process(0);  // read from stdin
  } else {
    for(int i = 1; i < argc; i++){
      int fd = open(argv[i], 0);
      if(fd < 0){
        fprintf(2, "sixfive: cannot open %s\n", argv[i]);
        continue;
      }
      process(fd);
      close(fd);
    }
  }
  exit(0);
}