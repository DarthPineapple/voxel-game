#ifndef PIPELINE_H
#define PIPELINE_H

#include <vulkan/vulkan.h>
#include <vector>

class Pipeline {
public:
    Pipeline(VkDevice device, VkRenderPass renderPass);
    ~Pipeline();

    void createPipeline();
    void cleanup();

    VkPipeline getPipeline() const { return pipeline; }
    VkPipelineLayout getPipelineLayout() const { return pipelineLayout; }

private:
    VkDevice device;
    VkRenderPass renderPass;
    VkPipeline pipeline;
    VkPipelineLayout pipelineLayout;

    void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);
};

#endif // PIPELINE_H