#include "camera.hpp"

#include <array>

namespace sc {
    const std::array<float, 4 * 4> camera::get_view_proj(float screen_w,
                                                         float screen_h) const {
        float half_width = (float)screen_w / 2.0f / zoom;
        float half_height = (float)screen_h / 2.0f / zoom;

        float offset_x = (center_x - screen_w / 2.0f) / zoom;
        float offset_y = (center_y - screen_h / 2.0f) / zoom;

        float left = x - half_width + offset_x;
        float right = x + half_width + offset_x;
        float bottom = -y - half_height + offset_y;
        float top = -y + half_height + offset_y;

        std::array<float, 4 * 4> matrix{};
        matrix[0] = 2.0f / (right - left);
        matrix[5] = 2.0f / (top - bottom);
        matrix[10] = -1.0f;
        matrix[12] = -(right + left) / (right - left);
        matrix[13] = -(top + bottom) / (top - bottom);
        matrix[15] = 1.0f;

        return matrix;
    }
}  // namespace sc
