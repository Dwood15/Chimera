#include <mutex>
#include <math.h>
#include <iostream>
#include "../messaging/messaging.h"
#include "../client_signature.h"
#include "../hooks/camera.h"
#include "../hooks/frame.h"
#include "../hooks/tick.h"
#include "../halo_data/tag_data.h"
#include "../halo_data/table.h"
#include "light.h"
#include "interpolation.h"
#include "particle.h"
#include "fp.h"
#include "rotation.h"
#include "widget.h"
#include "camera.h"

static float *camera_tick_rate = nullptr;
size_t chimera_interpolate_setting = 0;
static bool chimera_interpolate_predict = false;

static bool nuked = true;

struct ModelNode {
    float scale = 1.0;
    RotationMatrix rotation;
    Vector3D position;
};

#define MAX_NODES 64

enum InterpolationType {
    INTERPOLATION_NONE = 0,
    INTERPOLATION_POSITION,
    INTERPOLATION_POSITION_ROTATION
};

struct BufferedObject {
    uint32_t tag_id;
    InterpolationType interpolation_type;
    Vector3D position;
    Vector3D position_center;
    ModelNode nodes[MAX_NODES];
};

static BufferedObject objects_buffer_0[2048];
static BufferedObject objects_buffer_1[2048];

const char ilevels[10][9] {
//   B V W E G P S M C
    {},                  // 0
    {1,1,1,0,0,0,0,0,0}, // 1
    {2,2,1,0,0,0,0,1,1}, // 2
    {2,2,1,1,1,0,0,1,1}, // low (first person)
    {2,2,2,2,1,1,0,1,1}, // 4
    {2,2,2,2,1,1,0,2,2}, // 5
    {2,2,2,2,2,1,1,2,2}, // medium (particles)
    {2,2,2,2,2,1,2,2,2}, // 7
    {2,2,2,2,2,2,2,2,2}, // high
    {3,3,3,3,3,3,3,3,3}, // ultra
};

const uint32_t model_node_offset[16] = {
    0x550,
    0x5C0,
    0x340,
    0x294,
    0x244,
    0x2B0,
    0x1F8,
    0x228,
    0x21C
};

float interpolation_tick_progress = 0.0;
extern interpolate_vector_fn interpolate_vector_x;

static void do_interpolation(uint32_t i) noexcept {
    HaloObject o(i);
    auto *data = o.object_data();
    if(nuked) {
        objects_buffer_0[i].interpolation_type = INTERPOLATION_NONE;
        if(data) objects_buffer_0[i].tag_id = *reinterpret_cast<uint32_t *>(data);
        if(objects_buffer_0[i].tag_id != objects_buffer_1[i].tag_id) {
            objects_buffer_1[i].interpolation_type = INTERPOLATION_NONE;
        }
    }
    if(data) {
        const uint16_t &type = data[0xB4];
        if(type > 8 || ((*reinterpret_cast<uint32_t *>(data + 0x10) & 1) && type != 5)) {
            return;
        };
        char ld = ilevels[chimera_interpolate_setting][type];
        if(ld == 0) return;
        uint32_t node_count = 0;
        auto &model_id = *reinterpret_cast<HaloTagID *>(HaloTag::from_id(*reinterpret_cast<HaloTagID *>(data)).data + 0x28 + 0xC);
        if(!model_id.is_null()) node_count = *reinterpret_cast<uint32_t *>(HaloTag::from_id(model_id).data + 0xB8);
        if(type == 0x4 || type == 0x5) node_count = 1;
        if(node_count == 0 || node_count > MAX_NODES) return;

        const uint32_t &offset = model_node_offset[type];
        ModelNode *nodes = reinterpret_cast<ModelNode *>(data + offset);

        objects_buffer_0[i].position = *reinterpret_cast<Vector3D *>(data + 0x5C);
        auto &position_center = *reinterpret_cast<Vector3D *>(data + 0xA0);
        objects_buffer_0[i].position_center = position_center;

        if(nuked) {
            if(type <= 3) {
                if(distance_squared(objects_buffer_0[i].position_center, objects_buffer_1[i].position_center) > 1.5) {
                    objects_buffer_0[i].interpolation_type = INTERPOLATION_NONE;
                    return;
                }
            }

            auto r = INTERPOLATION_NONE;

            if(chimera_interpolate_setting == 9) {
                r = INTERPOLATION_POSITION_ROTATION;
            }
            else {
                float adjusted_scale = distance(objects_buffer_0[i].position_center, camera_data().position)/zoom_scale();
                if(ld == 1) {
                    if(adjusted_scale < 20.0) r = INTERPOLATION_POSITION;
                }
                else if(ld == 2) {
                    if(adjusted_scale < 15.0) r = INTERPOLATION_POSITION_ROTATION;
                    else if(adjusted_scale < 70.0) r = INTERPOLATION_POSITION;
                }
                else if(ld == 3) r = INTERPOLATION_POSITION_ROTATION;
            }

            objects_buffer_0[i].interpolation_type = r;
        }

        if(objects_buffer_0[i].interpolation_type == INTERPOLATION_NONE) return;

        if(objects_buffer_1[i].interpolation_type != INTERPOLATION_NONE)
            interpolate_vector_x(objects_buffer_1[i].position_center, objects_buffer_0[i].position_center, position_center, interpolation_tick_progress);

        for(uint32_t x=0;x<node_count;x++) {
            objects_buffer_0[i].nodes[x] = nodes[x];
            if(objects_buffer_1[i].interpolation_type != INTERPOLATION_NONE) {
                interpolate_vector_x(objects_buffer_1[i].nodes[x].position, objects_buffer_0[i].nodes[x].position, nodes[x].position, interpolation_tick_progress);
                nodes[x].scale = objects_buffer_1[i].nodes[x].scale + (objects_buffer_0[i].nodes[x].scale - objects_buffer_1[i].nodes[x].scale) * interpolation_tick_progress;

                if(objects_buffer_0[i].interpolation_type == INTERPOLATION_POSITION_ROTATION) {
                    Quaternion before = objects_buffer_1[i].nodes[x].rotation;
                    Quaternion after = objects_buffer_0[i].nodes[x].rotation;
                    Quaternion out;
                    interpolate_quat(before,after,out,interpolation_tick_progress);
                    nodes[x].rotation = out;
                }
            }
        }
    }
}

static void interpolate_objects() noexcept {
    if(tick_count() == 0) return;
    interpolation_tick_progress = tick_progress_inaccurate();
    for(uint32_t t=0;t<2048;t++) {
        do_interpolation(t);
    }

    if(nuked) {
        buffer_widgets_l();
    }

    if(chimera_interpolate_setting >= 6) do_particle_interpolation();
    if(chimera_interpolate_setting >= 1) do_antenna_interpolation();
    if(chimera_interpolate_setting >= 3) do_flag_interpolation();
    if(chimera_interpolate_setting >= 8) light_before();

    nuked = false;
}


static void reset() noexcept {
    extern float stored_zoom_scale;
    memcpy(objects_buffer_1,objects_buffer_0,sizeof(objects_buffer_0));
    buffer_widgets();
    stored_zoom_scale = 0;
    nuked = true;
}

static void rollback_interpolation() noexcept {
    if(tick_count() == 0) return;
    if(chimera_interpolate_setting >= 1) rollback_widget_interpolation();
    for(uint32_t i=0;i<2048;i++) {
        HaloObject o(i);
        char *data = o.object_data();
        if(objects_buffer_0[i].interpolation_type != INTERPOLATION_NONE && objects_buffer_1[i].interpolation_type != INTERPOLATION_NONE && data) {
            const uint16_t &type = data[0xB4];
            if((*reinterpret_cast<uint32_t *>(data + 0x10) & 1) && type != 5) continue;

            uint32_t node_count = 0;

            auto &model_id = *reinterpret_cast<HaloTagID *>(HaloTag::from_id(*reinterpret_cast<HaloTagID *>(data)).data + 0x28 + 0xC);
            if(!model_id.is_null()) node_count = *reinterpret_cast<uint32_t *>(HaloTag::from_id(model_id).data + 0xB8);

            const auto &offset = model_node_offset[type];
            ModelNode *nodes = (ModelNode *)(data + offset);
            if(type == 0x4 || type == 0x5) node_count = 1;

            *reinterpret_cast<Vector3D *>(data + 0xA0) = objects_buffer_0[i].position_center;

            for(uint32_t x=0;x<node_count;x++) {
                nodes[x] = objects_buffer_0[i].nodes[x];
            }
        }
    }
}

ChimeraCommandError interpolate_command(size_t argc, const char **argv) noexcept {
    static bool use_text = true;
    if(argc != 0) {
        auto &camera_coord_s = get_signature("camera_coord_sig");
        auto &camera_tick_rate_s = get_signature("camera_tick_rate_sig");
        auto &fp_interp_s = get_signature("fp_interp_sig");
        auto &do_reset_particle_s = get_signature("do_reset_particle_sig");

        static bool initialized = false;

        std::string arg = argv[0];
        for(size_t i=0;i<arg.size();i++) {
            arg[i] = tolower(arg[i]);
        }

        size_t new_setting = atol(argv[0]);

        if(arg == "off" || arg == "0" || arg == "false") {
            new_setting = 0;
            use_text = true;
        }
        else if(arg == "low") {
            new_setting = 3;
            use_text = true;
        }
        else if(arg == "medium" || arg == "med" || arg == "true" || arg == "on") {
            new_setting = 6;
            use_text = true;
        }
        else if(arg == "high") {
            new_setting = 8;
            use_text = true;
        }
        else if(arg == "ultra") {
            new_setting = 9;
            use_text = true;
        }
        else if(new_setting > 9 || new_setting == 0) {
            char z[256] = {};
            sprintf(z,"chimera_interpolate: Invalid setting \"%s\". Use \"chimera interpolate\" for help.",argv[0]);
            console_out_error(z);
            return CHIMERA_COMMAND_ERROR_FAILURE;
        }
        else {
            use_text = false;
        }
        if(initialized && new_setting == 0) {
            camera_coord_s.undo();
            camera_tick_rate_s.undo();
            fp_interp_s.undo();
            do_reset_particle_s.undo();
            remove_tick_event(reset);
            remove_precamera_event(interpolate_all_cam_before);
            remove_camera_event(interpolate_all_cam_after);
            remove_preframe_event(interpolate_objects);
            remove_frame_event(rollback_interpolation);
            initialized = false;
        }
        else if(!initialized && new_setting != 0) {
            memset(objects_buffer_0,0,sizeof(objects_buffer_0));
            memset(objects_buffer_1,0,sizeof(objects_buffer_1));

            camera_tick_rate = *reinterpret_cast<float **>(camera_tick_rate_s.address() + 2);

            static BasicCodecave do_reset_particle_code;
            write_jmp_call(do_reset_particle_s.address(), reinterpret_cast<void *>(on_particle_physics_before), reinterpret_cast<void *>(on_particle_physics_after), do_reset_particle_code);

            static BasicCodecave fp_code;
            write_jmp_call(fp_interp_s.address(), reinterpret_cast<void *>(fp_before), reinterpret_cast<void *>(fp_after), fp_code);

            add_tick_event(reset);
            add_precamera_event(interpolate_all_cam_before);
            add_camera_event(interpolate_all_cam_after);
            add_preframe_event(interpolate_objects);
            add_frame_event(rollback_interpolation);
            initialized = true;
        }
        chimera_interpolate_setting = new_setting;
    }
    char s[256] = {};
    if(use_text) {
        const char *what;
        switch(chimera_interpolate_setting) {
        case 0:
            what = "off";
            break;
        case 3:
            what = "low";
            break;
        case 6:
            what = "medium";
            break;
        case 8:
            what = "high";
            break;
        case 9:
            what = "ultra";
            break;
        default:
            what = "custom";
            break;
        }
        sprintf(s,"chimera_interpolate: %s", what);
    }
    else {
        sprintf(s,"chimera_interpolate: %u", chimera_interpolate_setting);
    }
    console_out(s);
    return CHIMERA_COMMAND_ERROR_SUCCESS;
}

ChimeraCommandError interpolate_predict_command(size_t argc, const char **argv) noexcept {
    if(argc != 0) {
        chimera_interpolate_predict = bool_value(argv[0]);
        interpolate_vector_x = chimera_interpolate_predict ? interpolate_vector_predict : interpolate_vector;
    }
    char s[256] = {};
    sprintf(s,"chimera_interpolate_predict: %s", chimera_interpolate_predict ? "true" : "false");
    console_out(s);
    return CHIMERA_COMMAND_ERROR_SUCCESS;
}
