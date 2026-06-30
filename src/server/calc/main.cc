#include <base/component.h>
#include <base/rpc_server.h>
#include <root/component.h>
#include <base/heap.h>          // هدر حیاتی برای فعال شدن ابزار مدیریت حافظه Heap
#include <calc_session/calc_session.h>

namespace Calc {

    // ۱. پیاده‌سازی متدهای اینترفیس RPC به همراه مشخص کردن سهمیه منابع لازم
    struct Session_component : Genode::Rpc_object<Session, Session_component>
    {
        enum { RAM_QUOTA = 2048, CAP_QUOTA = 2 };

        int add(int a, int b) override {
            Genode::log("Server: received add(", a, ", ", b, ")");
            return a + b;
        }

        int sub(int a, int b) override {
            Genode::log("Server: received sub(", a, ", ", b, ")");
            return a - b;
        }
    };

    // ۲. مدیریت روت کامپوننت برای ساخت سشن‌های کلاینت
    struct Root_component : Genode::Root_component<Session_component, Genode::Single_client>
    {
        Root_component(Genode::Entrypoint &ep, Genode::Allocator &allocator)
        : Genode::Root_component<Session_component, Genode::Single_client>(ep, allocator) { }

        Session_component *_create_session(const char *) override {
            return new (md_alloc()) Session_component();
        }
    };
}

// ۳. نقطه شروع کامپوننت سرور (Main)
void Component::construct(Genode::Env &env)
{
    Genode::log("=== Calc Server Started ===");

    // ساخت مدیریت حافظه هیپ با استفاده از نشست‌های رم و مدیریت منطقه حافظه کامپوننت
    static Genode::Heap heap(env.ram(), env.rm());

    // ثبت روت کامپوننت با استفاده از انتری‌پوینت پیش‌فرض و هیپ ساخته شده
    static Calc::Root_component root(env.ep(), heap);
    env.parent().announce(env.ep().manage(root));
}
