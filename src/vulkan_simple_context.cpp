
#include "vulkan_simple_context.hpp"

namespace svr {

bool has_required_layers(
    const std::vector<vk::LayerProperties>& available,
    const std::vector<const char*>&         required)
{
    return std::ranges::all_of(required, [&available](const auto& name) {
        return std::ranges::any_of(available, [&name](const auto& p) {
            return strcmp(p.layerName, name) == 0;
        });
    });
}

bool has_required_extensions(
    const std::vector<vk::ExtensionProperties>& available,
    const std::vector<const char*>&             required)
{
    return std::ranges::all_of(required, [&available](const auto& name) {
        return std::ranges::any_of(available, [&name](const auto& p) {
            return strcmp(p.extensionName, name) == 0;
        });
    });
}

uint32_t find_queue_family_idx(vk::PhysicalDevice device, vk::SurfaceKHR surface)
{
    std::optional<uint32_t> queue_family_idx;

    auto families = device.getQueueFamilyProperties();

    for (uint32_t idx = 0; idx < families.size(); ++idx) {
        const auto& family = families[idx];

        if (!(family.queueFlags & vk::QueueFlagBits::eGraphics))
            continue;

        if (!device.getSurfaceSupportKHR(idx, surface))
            continue;

        queue_family_idx = idx;
    }

    return queue_family_idx.value();
}

vulkan_simple_context::vulkan_simple_context()
{
    if (vk::enumerateInstanceVersion() < min_vulkan_api_version_) {
        throw std::runtime_error{"Min Vulkan API version not found"};
    }

#ifndef NDEBUG
    enabled_layers_.push_back("VK_LAYER_KHRONOS_validation");
#endif

    // Enable instance extensions
    enabled_instance_extensions_.push_back(VK_KHR_SURFACE_EXTENSION_NAME);

#ifdef WIN32
    enabled_instance_extensions_.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#else
#error Unsupported Vulkan platform.
#endif

    // Verify required layers are available
    if (!has_required_layers(vk::enumerateInstanceLayerProperties(), enabled_layers_)) {
        throw std::runtime_error{"Required layers not found"};
    }

    // Verify required extensions are available
    if (!has_required_extensions(
            vk::enumerateInstanceExtensionProperties(),
            enabled_instance_extensions_)) {
        throw std::runtime_error{"Required extensions not found"};
    }

    // Create context
    create_instance();
    create_surface();
    select_physical_device();
}

void vulkan_simple_context::create_instance()
{
    vk::ApplicationInfo application_info = {nullptr, 0, nullptr, 0, min_vulkan_api_version_};
    vk::InstanceCreateInfo create_info =
        {{}, &application_info, enabled_layers_, enabled_instance_extensions_};

    instance_ = vk::createInstanceUnique(create_info);
}

void vulkan_simple_context::create_surface()
{
#ifdef WIN32
    vk::Win32SurfaceCreateInfoKHR create_info = {
        {},
        GetModuleHandle(nullptr),
        GetActiveWindow()};

    surface_ = instance_->createWin32SurfaceKHRUnique(create_info);
#endif
}

void vulkan_simple_context::select_physical_device()
{
    auto device = instance_->enumeratePhysicalDevices().front();

    physical_device_ = {device, find_queue_family_idx(device, surface_.get())};
}

} // namespace svr
