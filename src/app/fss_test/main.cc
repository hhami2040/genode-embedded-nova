#include <libc/component.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <dirent.h> 

struct Main
{
    Main(Genode::Env &env)
    {
        Libc::with_libc([&] () {
            printf("--- FSS Test Started ---\n");

            char const *file_name = "test.tst";
            char const *pattern   = "a single line of text";
            size_t const pattern_size = strlen(pattern) + 1;

            if (mkdir("testdir", 0777) == 0 || errno == EEXIST) {
                printf("Directory 'testdir' created or already exists.\n");
            }

            if (chdir("testdir") == 0) {
                int fd = open(file_name, O_CREAT | O_WRONLY, 0666);
                if (fd >= 0) {
                    ssize_t written = write(fd, pattern, pattern_size);
                    close(fd);
                    printf("Wrote %ld bytes to file.\n", (long)written);
                } else {
                    printf("Failed to open file for writing.\n");
                }


                DIR *dir = opendir(".");
                if (dir) {
                    struct dirent *entry;
                    printf("--- Listing contents of 'testdir' ---\n");
                    while ((entry = readdir(dir)) != NULL) {
                        printf("Found in directory -> %s\n", entry->d_name);
                    }
                    closedir(dir);
                } else {
                    printf("Failed to open current directory for listing.\n");
                }
                // ----------------------------------------------------

                fd = open(file_name, O_RDONLY);
                if (fd >= 0) {
                    char buf[128];
                    memset(buf, 0, sizeof(buf));
                    ssize_t bytes_read = read(fd, buf, sizeof(buf));
                    close(fd);
                    
                    printf("Read from file (%ld bytes): \"%s\"\n", (long)bytes_read, buf);
                    if (strcmp(buf, pattern) == 0) {
                        printf("SUCCESS: File content matches perfectly!\n");
                    } else {
                        printf("ERROR: File content mismatch!\n");
                    }
                }
                
                chdir("..");
            }

            printf("--- MY APP FSS Test Finished ---\n");
        });

        env.parent().exit(0);
    }
};

void Libc::Component::construct(Libc::Env &env) { static Main main(env); }
