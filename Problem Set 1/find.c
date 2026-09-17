#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "user/user.h"
#include "kernel/fcntl.h"
#include "kernel/param.h"

// Forward declarations
void run_exec(char **cmdv, int cmdc, char *file);
int match(char *re, char *text);
int matchhere(char *re, char *text);
int matchstar(int c, char *re, char *text);

void find(char *path, char *target, char **cmdv, int cmdc)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(path, 0)) < 0){
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }
  if(fstat(fd, &st) < 0){
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  if(st.type != T_DIR){
    close(fd);
    return;
  }

  if(strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)){
    fprintf(2, "find: path too long\n");
    close(fd);
    return;
  }

  strcpy(buf, path);
  p = buf + strlen(buf);
  *p++ = '/';

  while(read(fd, &de, sizeof(de)) == sizeof(de)){
    if(de.inum == 0)
      continue;

    if(strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
      continue;

    memmove(p, de.name, DIRSIZ);
    p[DIRSIZ] = 0;

    if(stat(buf, &st) < 0){
      fprintf(2, "find: cannot stat %s\n", buf);
      continue;
    }

    // Replace direct strcmp with regex match check
    if(match(target, de.name)){
      if(cmdv == 0)
        printf("%s\n", buf);
      else
        run_exec(cmdv, cmdc, buf);
    }

    if(st.type == T_DIR){
      find(buf, target, cmdv, cmdc);
    }
  }
  close(fd);
}

void run_exec(char **cmdv, int cmdc, char *file)
{
  int pid = fork();
  if(pid < 0){
    fprintf(2, "find: fork failed\n");
    return;
  }
  if(pid == 0){
    char *argv[MAXARG];
    int i;
    for(i = 0; i < cmdc; i++)
      argv[i] = cmdv[i];
    argv[i] = file;
    argv[i+1] = 0;
    exec(cmdv[0], argv);
    fprintf(2, "find: exec %s failed\n", cmdv[0]);
    exit(1);
  } else {
    wait(0);
  }
}

// --- Primitive Regex Engine (adapted from user/grep.c) ---

int matchstar(int c, char *re, char *text)
{
  do {
    if(matchhere(re, text))
      return 1;
  } while(*text != '\0' && (*text++ == c || c == '.'));
  return 0;
}

int matchhere(char *re, char *text)
{
  if(re[0] == '\0')
    return 1;
  if(re[1] == '*')
    return matchstar(re[0], re + 2, text);
  if(re[0] == '$' && re[1] == '\0')
    return *text == '\0';
  if(*text != '\0' && (re[0] == '.' || re[0] == *text))
    return matchhere(re + 1, text + 1);
  return 0;
}

int match(char *re, char *text)
{
  if(re[0] == '^')
    return matchhere(re + 1, text);
  do {
    if(matchhere(re, text))
      return 1;
  } while(*text++ != '\0');
  return 0;
}

int main(int argc, char *argv[])
{
  if(argc < 3){
    fprintf(2, "usage: find dir regex_target [-exec command...]\n");
    exit(1);
  }

  char **cmdv = 0;
  int cmdc = 0;

  if(argc > 4 && strcmp(argv[3], "-exec") == 0){
    cmdv = &argv[4];
    cmdc = argc - 4;
  }

  find(argv[1], argv[2], cmdv, cmdc);
  exit(0);
}