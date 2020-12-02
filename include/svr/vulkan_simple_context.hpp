
#pragma once

#include <vulkan/vulkan.hpp>

#include <cstdint>
#include <vector>

namespace svr {

class vulkan_simple_context {
    // Vulkan Handles
    vk::UniqueInstance instance_;

    // Vulkan Configuration
    uint32_t                 min_vulkan_api_version_      = VK_API_VERSION_1_1;
    std::vector<const char*> enabled_layers_              = {};
    std::vector<const char*> enabled_instance_extensions_ = {};

public:
    vulkan_simple_context();

private:
    // Handle Creators
    void create_instance();
};

} // namespace svr
