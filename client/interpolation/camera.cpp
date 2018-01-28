#include <string.h>
#include "../halo_data/table.h"
#include "../client_signature.h"
#include "../hooks/tick.h"
#include "../halo_data/tag_data.h"
#include "camera.h"

static CameraData camera_coords_buffer_0 = {};
static CameraData camera_coords_buffer_1 = {};
static bool cam_interped = false;

CameraData &camera_data() noexcept {
    static auto *camera_coord_addr = *reinterpret_cast<CameraData **>(get_signature("camera_coord_sig").address() + 2);
    return *camera_coord_addr;
}

void interpolate_all_cam_before() noexcept {
    extern float interpolation_tick_progress;
    static int32_t tick_before = 0;
    auto tick_now = tick_count();
    static bool skip_interpolation = false;

    auto &data = camera_data();

    if(tick_now != tick_before) {
        tick_before = tick_now;
        memcpy(&camera_coords_buffer_1,&camera_coords_buffer_0,sizeof(camera_coords_buffer_1));
        memcpy(&camera_coords_buffer_0,&data,sizeof(camera_coords_buffer_0));
        skip_interpolation = distance_squared(camera_coords_buffer_0.position, camera_coords_buffer_1.position) > 10;
    }
    cam_interped = false;
    auto ct = get_camera_type();
    float etr = effective_tick_rate();

    bool cannot_interpolate_camera = tick_now < 2 || (etr < 20 && ct != CAMERA_FIRST_PERSON) || skip_interpolation;
    if(cannot_interpolate_camera) return;
    cam_interped = true;
    if(ct != CAMERA_FIRST_PERSON) {
        for(int i=0;i<2;i++) {
            interpolate_vector_rotation(camera_coords_buffer_1.orientation[i],camera_coords_buffer_0.orientation[i],data.orientation[i],interpolation_tick_progress);
        }
    }

    interpolate_vector(camera_coords_buffer_1.position, camera_coords_buffer_0.position, data.position, interpolation_tick_progress);
}

void interpolate_all_cam_after() noexcept {
    if(cam_interped) {
        auto &camera = camera_data();
        camera.position = camera_coords_buffer_0.position;
        memcpy(&camera.orientation, &camera_coords_buffer_0.orientation, sizeof(camera_coords_buffer_0.orientation));
    }
}

static char zoom_level() noexcept {
    static char *z = (**reinterpret_cast<char ***>(get_signature("zoomed_in_sig").address() + 2) + 0x5E8);
    if(*z > 0) return *z;
    else return 0;
}

float stored_zoom_scale = 0.0;

float zoom_scale() noexcept {
    if(stored_zoom_scale != 0.0) return stored_zoom_scale;
    stored_zoom_scale = 1.0;
    auto zoom = zoom_level();
    if(zoom) {
        HaloObject po(HaloPlayer().object_id());
        auto *pd = po.object_data();
        if(pd) {
            HaloObject pw(*reinterpret_cast<uint32_t *>(pd + 0x118));
            auto *pwd = pw.object_data();
            if(pwd) {
                if(pwd[0xB4] == 2) {
                    auto *&tag_data = HaloTag::from_id(*reinterpret_cast<HaloTagID *>(pwd)).data;
                    auto &max_zoom = *reinterpret_cast<float *>(tag_data + 0x3E0);
                    auto &zoom_levels = *reinterpret_cast<char *>(tag_data + 0x3DA);
                    if(zoom_levels == 1) return max_zoom;
                    auto &min_zoom = *reinterpret_cast<float *>(tag_data + 0x3DC);
                    stored_zoom_scale = min_zoom + (static_cast<float>(zoom - 1) / static_cast<float>(zoom_levels - 1)) * (max_zoom - min_zoom);
                }
            }
        }
    }
    return stored_zoom_scale;
}
