#include "hac2.h"

#include <stdio.h>

bool hac2_present() noexcept {
    static int hac2p = -1;
    if(hac2p == -1) {
        if (FILE *hac = fopen("controls/loader.dll", "r")) {
            fclose(hac);
            hac2p = 1;
        }
        else hac2p = 0;
    }
    return hac2p == 1;
}
