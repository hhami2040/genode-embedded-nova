#include <libc/component.h>
#include <base/log.h>
#include <stdio.h>
#include <string.h>

void Libc::Component::construct(Libc::Env &env)
{
    // استفاده از Genode::log برای اطمینان از نمایش در کنسول
    Genode::log(">>> FSS TEST: APP STARTED SUCCESSFULLY!");

    FILE *file = fopen("/storage/data.log", "w+");

    if (file) {
        Genode::log(">>> FSS TEST: FILE SYSTEM ACCESS OK.");
        const char *msg = "VMS Log: Stream started.";
        fwrite(msg, 1, strlen(msg), file);
        fflush(file); 
        fclose(file);
        Genode::log(">>> FSS TEST: LOG WRITTEN.");
    } else {
        Genode::log(">>> FSS TEST: ERROR OPENING FILE.");
    }
}
