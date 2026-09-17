#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

void memdump(char *fmt, char *data, int len);

int
main(int argc, char *argv[])
{
  if (argc == 1) {
    printf("Example 1:\n");
    int a[2] = {61810, 2026};
    memdump("ii", (char *)a, sizeof(a));

    printf("Example 2:\n");
    memdump("S", "a string", sizeof("a string"));

    printf("Example 3:\n");
    char *s = "another";
    memdump("s", (char *)&s, sizeof(s));

    struct sss {
      char *ptr;
      int num1;
      short num2;
      char byte;
      char bytes[8];
    } example;

    example.ptr = "hello";
    example.num1 = 1819438967;
    example.num2 = 100;
    example.byte = 'z';
    strcpy(example.bytes, "xyzzy");

    printf("Example 4:\n");
    memdump("pihcS", (char *)&example, sizeof(example));

    printf("Example 5:\n");
    memdump("sccccc", (char *)&example, sizeof(example));
  } else if (argc == 2) {
    // format in argv[1], up to 512 bytes of data from standard input.
    char data[512];
    int n = 0;
    memset(data, '\0', sizeof(data));
    while (n < sizeof(data)) {
      int nn = read(0, data + n, sizeof(data) - n);
      if (nn <= 0)
        break;
      n += nn;
    }
    memdump(argv[1], data, n);
  } else {
    printf("Usage: memdump [format]\n");
    exit(1);
  }
  exit(0);
}

void
memdump(char *fmt, char *data, int len)
{
  int pos = 0; // how far into data we've consumed

  for(int i = 0; fmt[i] != '\0'; i++){
    char f = fmt[i];

    if(f == 'i'){
      if(len - pos < 4){
        printf("memdump: not enough data for 'i'\n");
        return;
      }
      int val = *(int*)(data + pos);
      printf("%d\n", val);
      pos += 4;
    }
    else if(f == 'h'){
      if(len - pos < 2){
        printf("memdump: not enough data for 'h'\n");
        return;
      }
      short val = *(short*)(data + pos);
      printf("%d\n", val);
      pos += 2;
    }
    else if(f == 'c'){
      if(len - pos < 1){
        printf("memdump: not enough data for 'c'\n");
        return;
      }
      printf("%c\n", data[pos]);
      pos += 1;
    }
    else if(f == 'p'){
      if(len - pos < 8){
        printf("memdump: not enough data for 'p'\n");
        return;
      }
      uint64 val = *(uint64*)(data + pos);
      printf("%lx\n", val); 
      pos += 8;
    }
    else if(f == 's'){
      if(len - pos < 8){
        printf("memdump: not enough data for 's'\n");
        return;
      }
      char *str = *(char**)(data + pos);
      printf("%s\n", str);
      pos += 8;
    }
    else if(f == 'S'){
      for(int j = pos; j < len && data[j] != '\0'; j++){
        printf("%c", data[j]);
      }
      printf("\n");
    }
  }
}
