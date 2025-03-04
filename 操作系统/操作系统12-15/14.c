#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <time.h>

int main(int args, char *argv[]) 
{
    struct stat statbuf;
    time_t t;

    if (args < 2) 
    {
        printf("please input a file\n");
        exit(1);
    }

    if (stat(argv[1], &statbuf) == 0)
    {
        t = statbuf.st_atime;
        printf("%s\n", ctime(&t));
        printf("Inode number: %ld\n", (long)statbuf.st_ino);
        printf("Number of links: %ld\n", (long)statbuf.st_nlink);
        printf("Owner UID: %d\n", statbuf.st_uid);
        printf("Group GID: %d\n", statbuf.st_gid);
        printf("File size: %ld bytes\n", (long)statbuf.st_size);
        printf("Filesystem block size: %ld bytes\n", (long)statbuf.st_blksize);
        printf("Number of blocks: %ld\n", (long)statbuf.st_blocks);
    } 
    else {
        perror(argv[1]);
        exit(1);
    }

    if (S_ISREG(statbuf.st_mode)) {
        printf("File type: Regular file\n");
    } else if (S_ISDIR(statbuf.st_mode)) {
        printf("File type: Directory\n");
    } else if (S_ISCHR(statbuf.st_mode)) {
        printf("File type: Character device\n");
    } else if (S_ISBLK(statbuf.st_mode)) {
        printf("File type: Block device\n");
    } else if (S_ISFIFO(statbuf.st_mode)) {
        printf("File type: FIFO (pipe)\n");
    } else if (S_ISLNK(statbuf.st_mode)) {
        printf("File type: Symbolic link\n");
    } else if (S_ISSOCK(statbuf.st_mode)) {
        printf("File type: Socket\n");
    }


    printf("Permissions: ");
    printf((statbuf.st_mode & S_IRUSR) ? "r" : "-");
    printf((statbuf.st_mode & S_IWUSR) ? "w" : "-");
    printf((statbuf.st_mode & S_IXUSR) ? "x" : "-");
    printf((statbuf.st_mode & S_IRGRP) ? "r" : "-");
    printf((statbuf.st_mode & S_IWGRP) ? "w" : "-");
    printf((statbuf.st_mode & S_IXGRP) ? "x" : "-");
    printf((statbuf.st_mode & S_IROTH) ? "r" : "-");
    printf((statbuf.st_mode & S_IWOTH) ? "w" : "-");
    printf((statbuf.st_mode & S_IXOTH) ? "x" : "-");
    printf("\n");

    return 0;
}
