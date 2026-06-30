#include <base/component.h>
#include <base/log.h>

void Component::construct(Genode::Env &env)
{
    Genode::log("=== [Task 1] Hello Embedded on NOVA Microkernel ===");
}
