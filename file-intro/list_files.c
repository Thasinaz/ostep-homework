#include <dirent.h>
#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define PERMISSION_MASK 07777
#define MAX_PATH 1024
#define TIME_BUF 128

extern char *optarg;
extern int optind;

static int print_stat = 0;

void list_file(const char *directory) {
  DIR *dirfd = opendir(directory);
  if (dirfd == NULL) {
    perror("list_files");
    exit(1);
  }

  char path[MAX_PATH];
  strcpy(path, directory);
  int dir_len = strlen(directory);
  if (path[dir_len - 1] != '/') {
    path[dir_len] = '/';
    dir_len++;
  }
  struct dirent *dir;
  while ((dir = readdir(dirfd)) != NULL) {
    char *name = dir->d_name;
    path[dir_len] = 0;
    strcat(path, name);
    if (print_stat != 0) {
      struct stat statbuf;
      stat(path, &statbuf);
      char time_buf[TIME_BUF];
      ctime_r(&statbuf.st_mtime, time_buf);
      *strchr(time_buf, '\n') = 0;
      mode_t mode = statbuf.st_mode;
      mode_t type = mode & S_IFMT;
      if (type == S_IFSOCK) {
        printf("s");
      } else if (type == S_IFLNK) {
        printf("l");
      } else if (type == S_IFREG) {
        printf("-");
      } else if (type == S_IFDIR) {
        printf("d");
      } else if (type == S_IFCHR) {
        printf("c");
      } else if (type == S_IFIFO) {
        printf("p");
      }
      printf("%03o %s\t%s\t%ld\t%s ", statbuf.st_mode & PERMISSION_MASK,
             getpwuid(statbuf.st_uid)->pw_name,
             getgrgid(statbuf.st_gid)->gr_name, statbuf.st_size, time_buf);
    }
    printf("%s\n", name);
  }
}

int main(int argc, char *argv[]) {
  int c;
  char *directory = ".";
  while ((c = getopt(argc, argv, "l")) != -1) {
    switch (c) {
    case 'l':
      print_stat = 1;
      break;
    default:
      fprintf(stderr, "Usage: %s [-l] [directory]\n", argv[0]);
      break;
    }
  }
  if (argv[optind] != NULL) {
    directory = argv[optind];
  }

  list_file(directory);
  return 0;
}
