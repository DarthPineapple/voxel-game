class Renderer {
public:
    Renderer();
    ~Renderer();

    void init();
    void render();
    void cleanup();

private:
    void createCommandBuffers();
    void drawFrame();
    void updateUniformBuffer();
    
    // Vulkan related members
    VkDevice device;
    VkCommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers;
    VkPipeline graphicsPipeline;
    VkRenderPass renderPass;
    VkFramebuffer framebuffer;
};