
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

struct vulkan_swapchain {
    vk::UniqueSwapchainKHR handle;
    std::vector<vk::Image> images;
    vk::SurfaceKHR         surface;
    uint32_t               min_image_count;
    vk::SurfaceFormatKHR   format;
    vk::Extent2D           extent;
    vk::ImageUsageFlags    image_usage;
    vk::PresentModeKHR     present_mode;
    uint32_t               layer_count = 1;
    vk::Bool32             clipped     = VK_TRUE;
};

class vulkan_simple_context {
    // Vulkan Handles
    vk::UniqueInstance                   instance_;
    vk::UniqueSurfaceKHR                 surface_;
    vulkan_physical_device               physical_device_ = {};
    vk::UniqueDevice                     device_;
    vk::Queue                            device_queue_;
    vulkan_swapchain                     swapchain_ = {};
    vk::UniqueCommandPool                command_pool_;
    std::vector<vk::UniqueCommandBuffer> command_buffers_;

    // Vulkan Configuration
    uint32_t                   min_vulkan_api_version_      = VK_API_VERSION_1_1;
    std::vector<const char*>   enabled_layers_              = {};
    std::vector<const char*>   enabled_instance_extensions_ = {};
    std::vector<const char*>   enabled_device_extensions_   = {};
    vk::PhysicalDeviceFeatures device_features_             = {};
    uint32_t                   client_area_width_;
    uint32_t                   client_area_height_;

public:
    vulkan_simple_context(uint32_t width, uint32_t height);

private:
    // Handle Creators
    void create_instance();
    void create_surface();
    void create_logical_device();
    void create_swapchain();
    void create_command_pool();
    void create_command_buffers();

    void select_physical_device();
};

} // namespace svr
