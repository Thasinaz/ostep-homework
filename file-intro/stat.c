#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define PERMISSION_MASK 07777

void print_stat(struct stat *statbuf) {
  printf("Size: %ld\tBlocks: %ld\tIO Block: %ld\t", statbuf->st_size,
         statbuf->st_blocks, statbuf->st_blksize);
  mode_t mode = statbuf->st_mode;
  mode_t type = mode & S_IFMT;
  if (type == S_IFSOCK) {
    printf("socket\n");
  } else if (type == S_IFLNK) {
    printf("symbolic link\n");
  } else if (type == S_IFREG) {
    printf("regular file\n");
  } else if (type == S_IFDIR) {
    printf("directory\n");
  } else if (type == S_IFCHR) {
    printf("character device\n");
  } else if (type == S_IFIFO) {
    printf("FIFO\n");
  }
  printf("Device: %ld\tInode: %lu\tLinks: %lu\n", statbuf->st_dev,
         statbuf->st_ino, statbuf->st_nlink);
  uid_t uid = statbuf->st_uid;
  gid_t gid = statbuf->st_gid;
  printf("Access: %04o\tUid: (%ld/%s)\tGid: (%ld/%s)\n", mode & PERMISSION_MASK,
         uid, getpwuid(uid)->pw_name, gid, getgrgid(gid)->gr_name);
  printf("Access: %s", ctime(&statbuf->st_atime));
  printf("Modify: %s", ctime(&statbuf->st_mtime));
  printf("Change: %s", ctime(&statbuf->st_ctime));
}

int main(int argc, char *argv[]) {
  for (int i = 1; i < argc; i++) {
    struct stat statbuf;
    if (stat(argv[i], &statbuf) == 0) {
      printf("File: %s\n", argv[i]);
      print_stat(&statbuf);
    } else {
      perror("stat");
      exit(1);
    }
  }
  return 0;
}
