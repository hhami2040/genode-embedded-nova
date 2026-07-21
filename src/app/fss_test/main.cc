#include <libc/component.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

struct Main
{
    Main(Genode::Env &env)
    {
        Libc::with_libc([&] () {
            printf("--- Host File Read Test Started ---\n");


            int fd = open("host_data/my_config.txt", O_RDONLY);
            if (fd >= 0) {
                char buf[128];
                memset(buf, 0, sizeof(buf));
                read(fd, buf, sizeof(buf)); 
                close(fd);
                
                printf("Read from Host File -> \"%s\"\n", buf);
            } else {
                printf("Failed to open host file!\n");
            }

            printf("--- Host File Read Test Finished ---\n");
        });

        env.parent().exit(0);
    }
};

void Libc::Component::construct(Libc::Env &env) { static Main main(env); }
