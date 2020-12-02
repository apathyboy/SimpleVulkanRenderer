
#include "vulkan_simple_context.hpp"

namespace svr {

vulkan_simple_context::vulkan_simple_context()
{
    if (vk::enumerateInstanceVersion() < min_vulkan_api_version_) {
        throw std::runtime_error{"Min Vulkan API version not found"};
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
