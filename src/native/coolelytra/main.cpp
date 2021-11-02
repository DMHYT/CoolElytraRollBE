#include <hook.h>
#include <mod.h>
#include <logger.h>
#include <symbol.h>
#include <nativejs.h>


class CoolElytraModule : public Module {
	public:
	CoolElytraModule(): Module("coolelytra") {};

	virtual void initialize() {

    }
};

MAIN {
	Module* main_module = new CoolElytraModule();
}