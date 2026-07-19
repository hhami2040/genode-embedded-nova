#include <base/component.h>
#include <base/log.h>
#include <base/heap.h>
#include <base/allocator_avl.h> //requited
#include <file_system_session/connection.h>

void Component::construct(Genode::Env &env) {
    Genode::log("--- [fss_test] Initializing via Official Pattern ---");

    // ۱. ساخت Heap برای مدیریت کلی حافظه
    static Genode::Heap heap(env.ram(), env.rm());

    // ۲. ساخت Allocator اختصاصی برای سشن فایل‌سیستم
    static Genode::Allocator_avl fs_tx_block_alloc(&heap);

    // ۳. ایجاد کانکشن (فقط با پاس دادن allocator)
    File_system::Connection fs(env, fs_tx_block_alloc);

    Genode::log("--- [fss_test] Connection Created Successfully! ---");
}
