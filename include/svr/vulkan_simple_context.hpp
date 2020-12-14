
#pragma once

#include <vulkan/vulkan.hpp>

#include <cstdint>
#include <optional>
#include <vector>

namespace svr {

struct vulkan_physical_device {
    vk::PhysicalDevice handle = nullptr;
    uint32_t           queue_family_idx;
};

class vulkan_simple_context {
    // Vulkan Handles
    vk::UniqueInstance     instance_;
    vk::UniqueSurfaceKHR   surface_;
    vulkan_physical_device physical_device_ = {};

    // Vulkan Configuration
    uint32_t                 min_vulkan_api_version_      = VK_API_VERSION_1_1;
    std::vector<const char*> enabled_layers_              = {};
    std::vector<const char*> enabled_instance_extensions_ = {};

public:
    vulkan_simple_context();

private:
    // Handle Creators
    void create_instance();
    void create_surface();
    void select_physical_device();
};

} // namespace svr
