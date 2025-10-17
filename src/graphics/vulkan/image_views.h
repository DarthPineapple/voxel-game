#ifndef IMAGE_VIEWS_H
#define IMAGE_VIEWS_H

#include <vulkan/vulkan.h>
#include <vector>

class ImageViews {
public:
    ImageViews(VkDevice device);
    ~ImageViews();

    void createImageViews(const std::vector<VkImage>& swapchainImages, VkFormat swapchainImageFormat);
    void cleanup();

    const std::vector<VkImageView>& getImageViews() const { return imageViews; }

private:
    VkDevice device;
    std::vector<VkImageView> imageViews;
};

#endif // IMAGE_VIEWS_H
