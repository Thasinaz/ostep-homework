#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_PATH 1024

void print_usage(char *s) {
  fprintf(stderr, "Usage: %s [directory]\n", s);
}

void recv_list_content(const char *directory, int indentation) {
  DIR *dirfd = opendir(directory);
  if (dirfd == NULL) {
    perror("recursive_search");
    exit(1);
  }

  struct dirent *dir;
  while ((dir = readdir(dirfd)) != NULL) {
    char *name = dir->d_name;
    if (strcmp(name, "..") == 0 || strcmp(name, ".") == 0) {
      continue;
    }
    for (int i = 0; i < indentation; i++) {
      putchar(' ');
      putchar(' ');
    }
    printf("%s\n", name);
    if (dir->d_type == DT_DIR) {
      char sub_dir[MAX_PATH];
      sub_dir[0] = 0;
      strcpy(sub_dir, directory);
      int dir_len = strlen(directory);
      if (sub_dir[dir_len - 1] != '/') {
        sub_dir[dir_len] = '/';
        dir_len++;
        sub_dir[dir_len] = 0;
      }
      strcat(sub_dir, name);
      recv_list_content(sub_dir, indentation + 1);
    }
  }
  closedir(dirfd);
}


int main(int argc, char *argv[]) {
  if (argc > 2) {
    print_usage(argv[0]);
    exit(1);
  }
  char *directory = ".";
  if (argc == 2) {
    directory = argv[1];
  }

  recv_list_content(directory, 0);
  return 0;
}
