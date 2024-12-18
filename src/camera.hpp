#pragma once
#include <array>

namespace sc {
    class camera {
       public:
        float x;
        float y;
        float center_x;
        float center_y;
        float zoom;

        const std::array<float, 4 * 4> get_view_proj(float screen_w,
                                                     float screen_h) const;
    };
}  // namespace sc