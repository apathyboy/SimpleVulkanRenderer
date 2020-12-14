
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

vk::SurfaceFormatKHR select_optimal_surface_format(std::vector<vk::SurfaceFormatKHR> available)
{
    vk::SurfaceFormatKHR optimal_format = {
        vk::Format::eB8G8R8A8Srgb,
        vk::ColorSpaceKHR::eSrgbNonlinear};

    if (std::ranges::find(available, optimal_format) == available.end()) {
        throw std::runtime_error{"Required surface format not supported"};
    }

    return optimal_format;
}

vk::PresentModeKHR select_optimal_present_mode(std::vector<vk::PresentModeKHR> available)
{
    vk::PresentModeKHR optimal_mode = vk::PresentModeKHR::eFifo;

    if (std::ranges::find(available, optimal_mode) == available.end()) {
        throw std::runtime_error{"Required presentation mode not found"};
    }

    return optimal_mode;
}

vk::Extent2D select_optimal_surface_extent(
    vk::SurfaceCapabilitiesKHR capabilities,
    uint32_t                   client_area_width,
    uint32_t                   client_area_height)
{
    return {
        std::clamp(
            client_area_width,
            capabilities.minImageExtent.width,
            capabilities.maxImageExtent.width),
        std::clamp(
            client_area_height,
            capabilities.minImageExtent.height,
            capabilities.maxImageExtent.height)};
}

uint32_t select_swapchain_image_count(vk::SurfaceCapabilitiesKHR capabilities)
{
    return std::clamp(
        capabilities.minImageCount + 1,
        capabilities.minImageCount,
        capabilities.maxImageCount);
}

vulkan_simple_context::vulkan_simple_context(uint32_t width, uint32_t height)
    : client_area_width_{width}
    , client_area_height_{height}
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

    // Enable device extensions
    enabled_device_extensions_.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

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
    create_logical_device();
    create_swapchain();
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

void vulkan_simple_context::create_logical_device()
{
    vk::DeviceQueueCreateInfo queue_create_info = {
        {},
        physical_device_.queue_family_idx,
        std::array{1.f}};

    vk::DeviceCreateInfo create_info =
        {{}, queue_create_info, {}, enabled_device_extensions_, &device_features_};

    device_ = physical_device_.handle.createDeviceUnique(create_info);

    device_queue_ = device_->getQueue(physical_device_.queue_family_idx, 0);
}

void vulkan_simple_context::create_swapchain()
{
    auto surface_capabilities = physical_device_.handle.getSurfaceCapabilitiesKHR(
        surface_.get());

    swapchain_.surface = surface_.get();

    swapchain_.min_image_count = select_swapchain_image_count(surface_capabilities);

    swapchain_.format = select_optimal_surface_format(
        physical_device_.handle.getSurfaceFormatsKHR(swapchain_.surface));

    swapchain_.extent = select_optimal_surface_extent(
        surface_capabilities,
        client_area_width_,
        client_area_height_);

    swapchain_.image_usage = vk::ImageUsageFlagBits::eColorAttachment
                             | vk::ImageUsageFlagBits::eTransferDst;

    swapchain_.present_mode = select_optimal_present_mode(
        physical_device_.handle.getSurfacePresentModesKHR(swapchain_.surface));

    vk::SwapchainCreateInfoKHR create_info = vk::SwapchainCreateInfoKHR(
        {},
        swapchain_.surface,
        swapchain_.min_image_count,
        swapchain_.format.format,
        swapchain_.format.colorSpace,
        swapchain_.extent,
        swapchain_.layer_count,
        swapchain_.image_usage,
        vk::SharingMode::eExclusive,
        physical_device_.queue_family_idx,
        surface_capabilities.currentTransform,
        vk::CompositeAlphaFlagBitsKHR::eOpaque,
        swapchain_.present_mode,
        swapchain_.clipped,
        swapchain_.handle.get());

    swapchain_.handle = device_->createSwapchainKHRUnique(create_info);

    swapchain_.images = device_->getSwapchainImagesKHR(swapchain_.handle.get());
}

void vulkan_simple_context::select_physical_device()
{
    auto device = instance_->enumeratePhysicalDevices().front();

    physical_device_ = {device, find_queue_family_idx(device, surface_.get())};
}

} // namespace svr
