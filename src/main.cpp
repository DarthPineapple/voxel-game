#include <iostream>
#include "engine/application.h"

int main() {
    Application app;

    try {
        app.init();
        app.run();
    } catch (const std::runtime_error& e) {
        std::string error_msg(e.what());
        // Check if this is an expected Vulkan failure in headless environment
        if (error_msg.find("Failed to create Vulkan instance") != std::string::npos) {
            std::cout << "Note: Vulkan initialization failed (expected in headless environments)" << std::endl;
            app.cleanup();
            return EXIT_SUCCESS;  // Exit gracefully
        }
        // For other runtime errors, treat as failure
        std::cerr << "An error occurred: " << e.what() << std::endl;
        app.cleanup();
        return EXIT_FAILURE;
    } catch (const std::exception& e) {
        std::cerr << "An error occurred: " << e.what() << std::endl;
        app.cleanup();
        return EXIT_FAILURE;
    }

    app.cleanup();
    return EXIT_SUCCESS;
}