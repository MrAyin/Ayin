#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <string.h>
#include <time.h>

void print_file_info(const char *filename) {
    struct stat fileStat;
    if (stat(filename, &fileStat) < 0) {
        perror("Error getting file info");
        return;
    }

    // Print file type and permissions
    printf("%c", S_ISDIR(fileStat.st_mode) ? 'd' : '-');
    printf("%c", (fileStat.st_mode & S_IRUSR) ? 'r' : '-');
    printf("%c", (fileStat.st_mode & S_IWUSR) ? 'w' : '-');
    printf("%c", (fileStat.st_mode & S_IXUSR) ? 'x' : '-');
    printf("%c", (fileStat.st_mode & S_IRGRP) ? 'r' : '-');
    printf("%c", (fileStat.st_mode & S_IWGRP) ? 'w' : '-');
    printf("%c", (fileStat.st_mode & S_IXGRP) ? 'x' : '-');
    printf("%c", (fileStat.st_mode & S_IROTH) ? 'r' : '-');
    printf("%c", (fileStat.st_mode & S_IWOTH) ? 'w' : '-');
    printf("%c ", (fileStat.st_mode & S_IXOTH) ? 'x' : '-');

    // Print number of links
    printf("%ld ", fileStat.st_nlink);

    // Print owner and group
    printf("%s ", getpwuid(fileStat.st_uid)->pw_name);
    printf("%s ", getgrgid(fileStat.st_gid)->gr_name);

    // Print file size
    printf("%ld ", fileStat.st_size);

    // Print last modification time
    char timebuf[80];
    struct tm *tm_info = localtime(&fileStat.st_mtime);
    strftime(timebuf, sizeof(timebuf), "%a %b %d %H:%M:%S %Y", tm_info);
    printf("%s ", timebuf);

    // Print filename
    printf("%s\n", filename);
}

int main() {
    DIR *dir;
    struct dirent *ent;

    if ((dir = opendir(".")) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            // Ignore "." and ".."
            if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
                print_file_info(ent->d_name);
            }
        }
        closedir(dir);
    } else {
        perror("Could not open directory");
    }

    return 0;
}
