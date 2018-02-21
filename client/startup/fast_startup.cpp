#include "fast_startup.h"

#include "../halo_data/map.h"
#include "../messaging/messaging.h"
#include "../client_signature.h"
#include "../settings.h"
#include "../halo_data/tag_data.h"

#include "crc32.h"

// CRC of map = CRC of BSPs, model data, and tag data
static uint32_t calculate_crc32_of_map_file(FILE *f) noexcept {
    uint32_t crc = 0;
    MapHeader header;
    fread(&header, sizeof(header), 1, f);

    auto *tag_data = new char[header.tag_data_size];
    fseek(f, header.tag_data_offset, SEEK_SET);
    fread(tag_data, header.tag_data_size, 1, f);

    auto *scenario_tag = tag_data + (*reinterpret_cast<uint32_t *>(tag_data) - 0x40440000) + (*reinterpret_cast<uint32_t *>(tag_data + 4) & 0xFFFF) * 0x20;
    auto *scenario_tag_data = tag_data + (*reinterpret_cast<uint32_t *>(scenario_tag + 0x14) - 0x40440000);

    // First, the BSP(s)
    auto &structure_bsp_count = *reinterpret_cast<uint32_t *>(scenario_tag_data + 0x5A4);
    auto *structure_bsps = tag_data + (*reinterpret_cast<uint32_t *>(scenario_tag_data + 0x5A4 + 4) - 0x40440000);
    for(size_t b=0;b<structure_bsp_count;b++) {
        char *bsp = structure_bsps + b * 0x20;
        auto &bsp_offset = *reinterpret_cast<uint32_t *>(bsp);
        auto &bsp_size = *reinterpret_cast<uint32_t *>(bsp + 4);

        char *bsp_data = new char[bsp_size];
        fseek(f, bsp_offset, SEEK_SET);
        fread(bsp_data, bsp_size, 1, f);
        crc = crc32(crc, bsp_data, bsp_size);
        delete[] bsp_data;
    }

    // Next, model data
    auto &model_vertices_offset = *reinterpret_cast<uint32_t *>(tag_data + 0x14);
    auto &vertices_size = *reinterpret_cast<uint32_t *>(tag_data + 0x20);

    auto *model_vertices = new char[vertices_size];
    fseek(f, model_vertices_offset, SEEK_SET);
    fread(model_vertices, vertices_size, 1, f);
    crc = crc32(crc, model_vertices, vertices_size);
    delete[] model_vertices;

    // Lastly, tag data
    crc = crc32(crc, tag_data, header.tag_data_size);
    delete[] tag_data;

    return crc;
}

static void do_crc_things() noexcept {
    static char *loading_map = *reinterpret_cast<char **>(get_signature("loading_map_sig").address() + 1);
    static char crc_already[65536] = {};
    auto *indices = map_indices();
    for(size_t i=0;i<maps_count();i++) {
        if(strcmp(indices[i].file_name, loading_map) == 0) {
            if(!crc_already[i]) {
                char map_path[MAX_PATH];
                sprintf(map_path, "maps\\%s.map", indices[i].file_name);
                FILE *f = fopen(map_path, "rb");
                if(f) {
                    indices[i].crc32 = ~calculate_crc32_of_map_file(f);
                    crc_already[i] = 1;
                    fclose(f);
                    break;
                }
            }
        }
    }
}

ChimeraCommandError fast_startup_command(size_t argc, const char **argv) noexcept {
    static bool active = false;
    extern bool first_tick;
    if(argc == 1) {
        bool new_value = bool_value(argv[0]);
        if(new_value != active) {
            if(!first_tick) {
                auto &fast_startup_sig = get_signature("crc32_call_sig");
                auto &get_crc_sig = get_signature("get_crc_sig");
                if(new_value) {
                    static unsigned char nop5[5] = { 0x90, 0x90, 0x90, 0x90, 0x90 };
                    write_code_c(fast_startup_sig.address(), nop5);

                    unsigned char code[] = {
                        // shl eax, 0x04
                        0xC1, 0xE0, 0x04,

                        // pushad
                        0x60,

                        // call do_crc_things
                        0xE8, 0xFF, 0xFF, 0xFF, 0xFF,

                        // popad
                        0x61,

                        // mov ecx, [eax+edx+0C]
                        0x8B, 0x4C, 0x10, 0x0C,

                        // jmp back
                        0xE9, 0xFF, 0xFF, 0xFF, 0xFF
                    };

                    static BasicCodecave on_get_crc(code, sizeof(code));
                    static unsigned char nop7[7] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
                    write_code_c(get_crc_sig.address(), nop7);
                    write_code_any_value(get_crc_sig.address(), static_cast<unsigned char>(0xE9));
                    write_code_any_value(get_crc_sig.address() + 1, reinterpret_cast<int>(on_get_crc.data) - reinterpret_cast<int>(get_crc_sig.address() + 5));
                    write_code_any_value(on_get_crc.data + 4 + 1, reinterpret_cast<int>(do_crc_things) - reinterpret_cast<int>(on_get_crc.data + 4 + 5));
                    write_code_any_value(on_get_crc.data + 0xE + 1, reinterpret_cast<int>(get_crc_sig.address() + 5) - reinterpret_cast<int>(on_get_crc.data + 0xE + 5));
                }
            }
            startup_parameters().fast_startup = active;
            save_all_changes();
            active = new_value;
        }
    }
    console_out(active ? "true" : "false");
    return CHIMERA_COMMAND_ERROR_SUCCESS;
}
