#ifndef APPLICATION_H
#define APPLICATION_H

#include "window.h"

class Application {
public:
    Application();
    ~Application();

    void init();
    void run();
    void cleanup();

private:
    Window window;
    bool isRunning;
};

#endif // APPLICATION_H