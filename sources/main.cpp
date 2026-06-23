#include <MiniFB.h>
#include <MiniFB_keylist.h>

#include <cstdint>
#include <cstdlib>
#include <format>
#include <print>
#include <string>
#include <vector>

inline constexpr unsigned width = 800;
inline constexpr unsigned height = 600;

void modern_print()
{
    const std::string message = std::format("RayTracingRenderer blank window: {}x{}", width, height);
    std::print("{}\n", message);
    system("pause");
}

int main() {
    modern_print();

    std::vector<uint32_t> framebuffer(width * height, MFB_RGB(0, 0, 0));

    mfb_window *window = mfb_open("RayTracingRenderer", width, height);
    if (window == nullptr) {
        return 1;
    }

    mfb_set_target_fps(60);

    while (true) {
        const mfb_update_state state = mfb_update(window, framebuffer.data());
        if (state != MFB_STATE_OK) {
            break;
        }

        const uint8_t *keys = mfb_get_key_buffer(window);
        if (keys != nullptr && keys[MFB_KB_KEY_ESCAPE]) {
            break;
        }

        if (!mfb_wait_sync(window)) {
            break;
        }
    }

    mfb_close(window);
    return 0;
}
