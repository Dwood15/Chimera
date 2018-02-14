#include "input.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include "../messaging/messaging.h"
#include "../client_signature.h"
#include "../hooks/frame.h"
#include "../halo_data/table.h"
#include "../interpolation/camera.h"

static double sensitivity = 0;

typedef LRESULT (*window_proc_type)(_In_ HWND hwnd,_In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);

window_proc_type old_proc;

LRESULT CALLBACK wproc(
  _In_ HWND   hWnd,
  _In_ UINT   uMsg,
  _In_ WPARAM wParam,
  _In_ LPARAM lParam
) {
    switch(uMsg) {
        case(WM_INPUT): {
            UINT dwsize;
            GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, NULL, &dwsize, sizeof(RAWINPUTHEADER));
            BYTE *input = new BYTE[dwsize]();
            if(GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, input, &dwsize, sizeof(RAWINPUTHEADER)) != dwsize) {
                return 0;
            }
            RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(input);
            float &yaw = *reinterpret_cast<float *>(0x402AD4B8);
            float &pitch = *reinterpret_cast<float *>(0x402AD4BC);

            auto adjusted_sens = sensitivity / 1000.0 / zoom_scale();
            yaw -= raw->data.mouse.lLastX * adjusted_sens;
            pitch -= raw->data.mouse.lLastY * adjusted_sens;

            while(yaw < 0.0) {
                yaw += M_PI * 2;
            }
            while(yaw > M_PI * 2) {
                yaw -= M_PI * 2;
            }
            while(pitch > M_PI / 2) {
                pitch -= M_PI / 2;
            }
            while(pitch < -M_PI / 2) {
                pitch += M_PI / 2;
            }

            static bool left = false;
            if(raw->data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN) {
                left = true;
            }
            else if(raw->data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_UP) {
                left = false;
            }
            static bool right = false;
            if(raw->data.mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN) {
                right = true;
            }
            else if(raw->data.mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_UP) {
                right = false;
            }
            get_movement_info().primary_fire = left ? 64 : 0;
            get_movement_info().secondary_fire = right ? 64 : 0;

            delete[] input;

            return 0;
        }
        default: return CallWindowProc((WNDPROC)old_proc, hWnd, uMsg, wParam, lParam);
    }
}

ChimeraCommandError test_input_command(size_t argc, const char **argv) noexcept {
    if(argc == 1) {
        auto new_value = strtof(argv[0], nullptr);
        auto &sig_1 = get_signature("input_yaw_sig");
        auto &sig_2 = get_signature("input_pitch_1_sig");
        auto &sig_3 = get_signature("input_pitch_2_sig");
        auto &sig_4 = get_signature("reset_input_sig");

        if((new_value == 0) != (sensitivity == 0)) {
            if(new_value) {
                RAWINPUTDEVICE x = {};
                x.usUsagePage = 1;
                x.usUsage = 2;
                x.dwFlags = RIDEV_NOLEGACY;
                x.hwndTarget = 0;
                RegisterRawInputDevices(&x, 1, sizeof(x));

                POINT p;
                GetCursorPos(&p);
                RECT r;
                r.left = p.x;
                r.right = p.x;
                r.top = p.y;
                r.bottom = p.y;

                ClipCursor(&r);

                const short new_code[] = { 0x90, 0x90, 0x90 };
                const short new_code2[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
                write_code_s(sig_1.address(), new_code);
                write_code_s(sig_2.address(), new_code);
                write_code_s(sig_3.address(), new_code);
                write_code_s(sig_4.address(), new_code2);

                console_out(std::to_string((unsigned long)&get_movement_info().secondary_fire));

                old_proc = reinterpret_cast<window_proc_type>(GetWindowLong(GetActiveWindow(), GWL_WNDPROC));
                SetWindowLong(GetActiveWindow(), GWL_WNDPROC, reinterpret_cast<LONG>(wproc));
            }
            else {
                RAWINPUTDEVICE x = {};
                x.usUsagePage = 1;
                x.usUsage = 2;
                x.dwFlags = RIDEV_REMOVE;
                x.hwndTarget = NULL;
                RegisterRawInputDevices(&x, 1, sizeof(x));
                sig_1.undo();
                sig_2.undo();
                sig_3.undo();
                SetWindowLong(GetActiveWindow(), GWL_WNDPROC, reinterpret_cast<LONG>(old_proc));
                ClipCursor(nullptr);
            }
        }
        sensitivity = new_value;
    }
    console_out(std::to_string(sensitivity));
    return CHIMERA_COMMAND_ERROR_SUCCESS;
}
