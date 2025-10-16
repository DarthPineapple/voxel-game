class Device {
public:
    Device();
    ~Device();

    bool createDevice(VkInstance instance);
    void cleanup();

private:
    VkDevice device;
    VkPhysicalDevice physicalDevice;
    VkQueue graphicsQueue;
    VkQueue presentQueue;

    void pickPhysicalDevice(VkInstance instance);
    void createLogicalDevice();
};