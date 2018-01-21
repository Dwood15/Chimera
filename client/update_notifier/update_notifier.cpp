#include "update_notifier.h"
#include "../../version.h"
#include "../messaging/messaging.h"
#include "../hooks/tick.h"

struct CheckStatus;
CheckStatus *status;

long latest_build = 0;

extern "C" {
    const char *latest_update(CheckStatus *);
    CheckStatus *check_for_updates();
}

static bool block_update_notifier = false;

ChimeraCommandError block_update_notifier_command(size_t argc, const char **argv) {
    if(argc == 1) {
        block_update_notifier = bool_value(argv[0]);
    }
    char x[256] = {};
    sprintf(x, "chimera_block_update_notifier: %s", block_update_notifier ? "true" : "false");
    console_out(x);
    return CHIMERA_COMMAND_ERROR_SUCCESS;
}

void check_updater() {
    static size_t i = 0;
    if(i++ == 0) {
        status = check_for_updates();
    }
    else if(i == 2 * 60 * 30) {
        remove_tick_event(check_updater);
    }
    else {
        auto *update_status = latest_update(status);
        if(update_status) {
            remove_tick_event(check_updater);
            latest_build = atol(update_status);

            if
            #ifdef CHIMERA_ALPHA_VERSION
            (latest_build >= CHIMERA_BUILD_NUMBER)
            #else
            (latest_build > CHIMERA_BUILD_NUMBER)
            #endif
            {
                if(!block_update_notifier) {
                    char f[256] = {};
                    sprintf(f,"Your build of Chimera [build " CHIMERA_BUILD_STRING "] is outdated. The latest build is %li.", latest_build);
                    console_out_warning(f);
                    console_out_warning("Download the latest build at Chimera.OpenCarnage.net");
                    console_out_warning("If you do not wish to be notified of updates, use \"chimera_block_update_notifier 1\"");
                }
            }
            else {
                latest_build = 0;
            }
        }
    }
}
