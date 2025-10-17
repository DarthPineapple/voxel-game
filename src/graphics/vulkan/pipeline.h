#ifndef PIPELINE_H
#define PIPELINE_H

#include <vulkan/vulkan.h>
#include <vector>
#include <string>

class Pipeline {
public:
    Pipeline(VkDevice device, VkRenderPass renderPass, VkExtent2D extent);
    ~Pipeline();

    void createPipeline(const std::string& vertShaderPath, const std::string& fragShaderPath);
    void cleanup();

    VkPipeline getPipeline() const { return pipeline; }
    VkPipelineLayout getPipelineLayout() const { return pipelineLayout; }
    VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }

private:
    VkDevice device;
    VkRenderPass renderPass;
    VkExtent2D extent;
    VkPipeline pipeline;
    VkPipelineLayout pipelineLayout;
    VkDescriptorSetLayout descriptorSetLayout;

    void createDescriptorSetLayout();
    std::vector<char> readFile(const std::string& filename);
    void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);
};

#endif // PIPELINE_H