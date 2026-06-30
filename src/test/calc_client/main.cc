#include <base/component.h>
#include <base/log.h>
#include <base/rpc_client.h>
#include <calc_session/calc_session.h>

struct Local_calc_client : Genode::Rpc_client<Calc::Session>
{
    explicit Local_calc_client(Genode::Capability<Calc::Session> cap)
    : Genode::Rpc_client<Calc::Session>(cap) { }

    int add(int a, int b) override { return call<Calc::Session::Rpc_add>(a, b); }
    int sub(int a, int b) override { return call<Calc::Session::Rpc_sub>(a, b); }
};

void Component::construct(Genode::Env &env)
{
    Genode::log("=== Calc Client Started ===");

    try {
        // تغییر شناسه به ۹۹ برای جلوگیری از تداخل با سشن‌های پیش‌فرض مثل PD
        Genode::Parent::Client::Id session_id { 99 };
        
        Genode::Capability<Calc::Session> cap = 
            env.session<Calc::Session>(session_id, "Calc", Genode::Affinity());

        Local_calc_client calc(cap);

        int result_add = calc.add(15, 10);
        Genode::log("Client: RPC add result = ", result_add);

        int result_sub = calc.sub(50, 20);
        Genode::log("Client: RPC sub result = ", result_sub);

    } catch (...) {
        Genode::error("Client: Connection or RPC failed!");
    }

    Genode::log("=== Calc Client Finished ===");
}
