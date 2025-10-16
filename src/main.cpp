#include <iostream>
#include "engine/application.h"

int main() {
    Application app;

    try {
        app.init();
        app.run();
    } catch (const std::exception& e) {
        std::cerr << "An error occurred: " << e.what() << std::endl;
        app.cleanup();
        return EXIT_FAILURE;
    }

    app.cleanup();
    return EXIT_SUCCESS;
}