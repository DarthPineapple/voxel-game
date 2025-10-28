#ifndef OVERLAY_PIPELINE_H
#define OVERLAY_PIPELINE_H

#include <vulkan/vulkan.h>
#include <vector>
#include <string>

class OverlayPipeline {
public:
    OverlayPipeline(VkDevice device, VkRenderPass renderPass, VkExtent2D extent);
    ~OverlayPipeline();

    void createPipeline(const std::string& vertShaderPath, const std::string& fragShaderPath);
    void cleanup();

    VkPipeline getPipeline() const { return pipeline; }
    VkPipelineLayout getPipelineLayout() const { return pipelineLayout; }

private:
    VkDevice device;
    VkRenderPass renderPass;
    VkExtent2D extent;
    VkPipeline pipeline;
    VkPipelineLayout pipelineLayout;

    std::vector<char> readFile(const std::string& filename);
    void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);
};

#endif // OVERLAY_PIPELINE_H
