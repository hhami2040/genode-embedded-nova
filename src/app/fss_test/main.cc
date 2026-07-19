#include <base/component.h>
#include <base/log.h>
#include <vfs/file_system.h>
#include <libc/component.h> // این هدر را اضافه کنید

// به جای ارث‌بری مستقیم از Component، از Libc::Component استفاده می‌کنیم
void Libc::Component::construct(Libc::Env &env)
{
    (void)env;
    Genode::log("--- [fss_test] Component started successfully with VFS and Libc ---");
}s
