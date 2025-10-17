#ifndef RENDER_PASS_H
#define RENDER_PASS_H

#include <vulkan/vulkan.h>

class RenderPass {
public:
    RenderPass(VkDevice device);
    ~RenderPass();

    void createRenderPass(VkFormat swapchainImageFormat);
    void cleanup();

    VkRenderPass getRenderPass() const { return renderPass; }

private:
    VkDevice device;
    VkRenderPass renderPass;
};

#endif // RENDER_PASS_H
