
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
    vk::UniqueSemaphore                  image_available_semaphore_;
    vk::UniqueSemaphore                  rendering_complete_semaphore_;

    uint32_t current_swapchain_idx_;

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

    void begin_draw();
    void end_draw();

    vulkan_physical_device&               physical_device() { return physical_device_; }
    vulkan_swapchain&                     swapchain() { return swapchain_; }
    std::vector<vk::UniqueCommandBuffer>& command_buffers()
    {
        device_->resetCommandPool(command_pool_.get(), {});
        return command_buffers_;
    }

private:
    // Handle Creators
    void create_instance();
    void create_surface();
    void create_logical_device();
    void create_swapchain();
    void create_command_pool();
    void create_command_buffers();
    void create_synchronization();

    void select_physical_device();
};

} // namespace svr
