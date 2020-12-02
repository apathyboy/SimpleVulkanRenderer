
#include <vulkan/vulkan.hpp>
#include <fmt/core.h>

int main()
{
    auto vulkan_api_version = vk::enumerateInstanceVersion();

    fmt::print("Vulkan Info\n");
    fmt::print(
        "Version: {}.{}.{}\n",
        VK_VERSION_MAJOR(vulkan_api_version),
        VK_VERSION_MINOR(vulkan_api_version),
        VK_VERSION_PATCH(vulkan_api_version));

    for (const auto& extension : vk::enumerateInstanceExtensionProperties()) {
        fmt::print("Instance Extension: {}\n", extension.extensionName);
    }

    for (const auto& layer : vk::enumerateInstanceLayerProperties()) {
        fmt::print("Instance Layer: {} - {}\n", layer.layerName, layer.description);
    }

    return 0;
}
