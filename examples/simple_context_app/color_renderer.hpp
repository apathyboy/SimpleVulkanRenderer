
#pragma once

#include <svr/vulkan_simple_context.hpp>

#include <array>
#include <cstdint>

class color_renderer {
    svr::vulkan_simple_context context_;

    std::array<float, 4> color_;

public:
    color_renderer(uint32_t width, uint32_t height, std::array<float, 4> color);

    void draw();

    void update_color(std::array<float, 4> color);

private:
    void record_command_buffers();
};
