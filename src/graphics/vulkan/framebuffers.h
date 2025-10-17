#ifndef FRAMEBUFFERS_H
#define FRAMEBUFFERS_H

#include <vulkan/vulkan.h>
#include <vector>

class Framebuffers {
public:
    Framebuffers(VkDevice device);
    ~Framebuffers();

    void createFramebuffers(const std::vector<VkImageView>& imageViews, 
                           VkRenderPass renderPass,
                           VkExtent2D extent);
    void cleanup();

    const std::vector<VkFramebuffer>& getFramebuffers() const { return framebuffers; }

private:
    VkDevice device;
    std::vector<VkFramebuffer> framebuffers;
};

#endif // FRAMEBUFFERS_H
