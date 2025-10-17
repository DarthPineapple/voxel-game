#ifndef APPLICATION_H
#define APPLICATION_H

#include "window.h"

class Renderer;
class ChunkManager;

class Application {
public:
    Application();
    ~Application();

    void init();
    void run();
    void cleanup();

private:
    Window* window;
    Renderer* renderer;
    ChunkManager* chunkManager;
    bool isRunning;
};

#endif // APPLICATION_H