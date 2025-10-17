#include "framebuffers.h"
#include <stdexcept>

Framebuffers::Framebuffers(VkDevice device)
    : device(device) {
}

Framebuffers::~Framebuffers() {
    cleanup();
}

void Framebuffers::createFramebuffers(const std::vector<VkImageView>& imageViews, 
                                     VkRenderPass renderPass,
                                     VkExtent2D extent) {
    framebuffers.resize(imageViews.size());

    for (size_t i = 0; i < imageViews.size(); i++) {
        VkImageView attachments[] = { imageViews[i] };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = extent.width;
        framebufferInfo.height = extent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &framebuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create framebuffer!");
        }
    }
}

void Framebuffers::cleanup() {
    for (auto framebuffer : framebuffers) {
        if (framebuffer != VK_NULL_HANDLE) {
            vkDestroyFramebuffer(device, framebuffer, nullptr);
        }
    }
    framebuffers.clear();
}
