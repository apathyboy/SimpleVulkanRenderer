
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

vulkan_simple_context::vulkan_simple_context()
{
    if (vk::enumerateInstanceVersion() < min_vulkan_api_version_) {
        throw std::runtime_error{"Min Vulkan API version not found"};
    }

#ifndef NDEBUG
    enabled_layers_.push_back("VK_LAYER_KHRONOS_validation");
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
}

void vulkan_simple_context::create_instance()
{
    vk::ApplicationInfo application_info = {nullptr, 0, nullptr, 0, min_vulkan_api_version_};
    vk::InstanceCreateInfo create_info =
        {{}, &application_info, enabled_layers_, enabled_instance_extensions_};

    instance_ = vk::createInstanceUnique(create_info);
}

} // namespace svr
