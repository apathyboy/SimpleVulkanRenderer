
#include "color_renderer.hpp"

color_renderer::color_renderer(uint32_t width, uint32_t height, std::array<float, 4> color)
    : context_{width, height}
    , color_{color}
{
    record_command_buffers();
}

void color_renderer::draw()
{
    context_.begin_draw();
    context_.end_draw();
}

void color_renderer::update_color(std::array<float, 4> color)
{
    color_ = color;

    record_command_buffers();
}

void color_renderer::record_command_buffers()
{
    auto& swapchain       = context_.swapchain();
    auto& physical_device = context_.physical_device();
    auto& command_buffers = context_.command_buffers();

    vk::ImageSubresourceRange image_subresource_range =
        {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1};

    for (uint32_t idx = 0; idx < swapchain.images.size(); ++idx) {
        vk::ImageMemoryBarrier barrier_from_present_to_clear = {
            vk::AccessFlagBits::eMemoryRead,
            vk::AccessFlagBits::eTransferWrite,
            vk::ImageLayout::eUndefined,
            vk::ImageLayout::eTransferDstOptimal,
            physical_device.queue_family_idx,
            physical_device.queue_family_idx,
            swapchain.images[idx],
            image_subresource_range};

        vk::ImageMemoryBarrier barrier_from_clear_to_present = {
            vk::AccessFlagBits::eTransferWrite,
            vk::AccessFlagBits::eMemoryRead,
            vk::ImageLayout::eTransferDstOptimal,
            vk::ImageLayout::ePresentSrcKHR,
            physical_device.queue_family_idx,
            physical_device.queue_family_idx,
            swapchain.images[idx],
            image_subresource_range};

        command_buffers[idx]->begin(vk::CommandBufferBeginInfo{});

        command_buffers[idx]->pipelineBarrier(
            vk::PipelineStageFlagBits::eTransfer,
            vk::PipelineStageFlagBits::eTransfer,
            {},
            {},
            {},
            barrier_from_present_to_clear);

        command_buffers[idx]->clearColorImage(
            swapchain.images[idx],
            vk::ImageLayout::eTransferDstOptimal,
            color_,
            image_subresource_range);

        command_buffers[idx]->pipelineBarrier(
            vk::PipelineStageFlagBits::eTransfer,
            vk::PipelineStageFlagBits::eBottomOfPipe,
            {},
            {},
            {},
            barrier_from_clear_to_present);

        command_buffers[idx]->end();
    }
}
