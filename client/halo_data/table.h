#pragma once

#include <stdint.h>

#define HALO_NAME_LENGTH 12
#define NULL_ID 0xFFFFFFFF

/// This is an array of data from Halo for movement
struct MovementInfo {
    uint8_t jump; //0x0
    uint8_t change_grenades;
    uint8_t action;
    uint8_t change_weapons;
    uint8_t melee; //0x4
    uint8_t flashlight;
    uint8_t secondary_fire;
    uint8_t primary_fire;
    uint8_t unknown_1; //0x8
    uint8_t unknown_2;
    uint8_t crouch;
    uint8_t zoom;
    uint8_t score_screen; //0xC
    uint8_t reload;
    uint8_t exchange_weapons;
    uint8_t chat;
    uint8_t unused_1[4]; //0x10
    uint8_t unused_2[4]; //0x14
    uint8_t unused_3[3]; //0x18
    uint8_t game_info; //0x1B
    uint8_t show_player_names; //0x1C
    uint8_t unused_4[3]; //0x1D
    float move_up; //0x20
    float move_side; //0x24
    float aim_horizontal; //0x28
    float aim_vertical; //0x2C
    float custom_move_up; // 0x30 - custom by chimera
    float custom_move_side; // 0x34 - custom by chimera
    float custom_look_horizontal; // 0x38 - custom by chimera
    float custom_look_vertical; // 0x3C - custom by chimera
};

/// Retrieve the movement info.
MovementInfo &get_movement_info() noexcept;

/// Halo's tables are mapped out like this.
struct GenericTable {
    char name[0x20];
    uint16_t max_count;
    uint16_t index_size;
    uint32_t one;
    uint32_t data_v;
    uint16_t zero;
    uint16_t size;
    uint16_t count;
    uint16_t next_id;
    void *first;
};

/// Return a reference to the object table.
GenericTable &get_object_table() noexcept;

/// Return a reference to the player table.
GenericTable &get_player_table() noexcept;

/// Return a reference to the particle table.
GenericTable &get_particle_table() noexcept;

/// Return a reference to the antenna table.
GenericTable &get_antenna_table() noexcept;

/// Return a reference to the flag cloth table.
GenericTable &get_flag_table() noexcept;

/// Return a reference to the light table.
GenericTable &get_light_table() noexcept;

/// Return a reference to the effect table.
GenericTable &get_effect_table() noexcept;

/// Return a reference to the decal table.
GenericTable &get_decal_table() noexcept;

/// Get the client player index.
uint32_t client_player_index() noexcept;

/// Check if two wide strings are equal, optionally up to a maximum length.
bool unichar_equal(const short *a, const short *b, size_t size = SIZE_MAX) noexcept;

class HaloPlayer {
private:
    uint32_t player_index;
public:
    uint32_t index() noexcept;
    uint32_t object_id() noexcept;
    char *player_data() noexcept;
    HaloPlayer(uint32_t player_index=client_player_index()) noexcept;
    HaloPlayer(const short *name) noexcept;
};

class HaloObject {
private:
    uint32_t object_index;
public:
    uint32_t index() noexcept;
    char *object_data() noexcept;
    HaloObject(uint32_t object_index) noexcept;
};

enum CameraType {
    CAMERA_FIRST_PERSON = 0x0,
    CAMERA_VEHICLE = 0x1,
    CAMERA_CINEMATIC = 0x2,
    CAMERA_DEBUG = 0x3
};
CameraType get_camera_type() noexcept;
