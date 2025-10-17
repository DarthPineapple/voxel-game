#ifndef SYNC_OBJECTS_H
#define SYNC_OBJECTS_H

#include <vulkan/vulkan.h>
#include <vector>

class SyncObjects {
public:
    SyncObjects(VkDevice device);
    ~SyncObjects();

    void createSyncObjects(size_t maxFramesInFlight);
    void cleanup();

    const std::vector<VkSemaphore>& getImageAvailableSemaphores() const { return imageAvailableSemaphores; }
    const std::vector<VkSemaphore>& getRenderFinishedSemaphores() const { return renderFinishedSemaphores; }
    const std::vector<VkFence>& getInFlightFences() const { return inFlightFences; }

private:
    VkDevice device;
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
};

#endif // SYNC_OBJECTS_H
