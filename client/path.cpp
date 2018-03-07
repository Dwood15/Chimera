#include "path.h"
#include "client_signature.h"

char *halo_path() noexcept {
    static auto *path = reinterpret_cast<const char **>(get_signature("path_sig").address() + 1);
    return (char*)*path;
}
