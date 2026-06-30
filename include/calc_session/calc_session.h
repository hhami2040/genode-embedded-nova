#pragma once

#include <session/session.h>
#include <base/rpc.h>

namespace Calc { struct Session; }

struct Calc::Session : Genode::Session
{
    // تعریف استاتیک سهمیه برای استفاده کلاینت و سرور
    enum { RAM_QUOTA = 2048, CAP_QUOTA = 2 };

    static const char *service_name() { return "Calc"; }

    virtual int add(int a, int b) = 0;
    virtual int sub(int a, int b) = 0;

    GENODE_RPC(Rpc_add, int, add, int, int);
    GENODE_RPC(Rpc_sub, int, sub, int, int);
    GENODE_RPC_INTERFACE(Rpc_add, Rpc_sub);
};
