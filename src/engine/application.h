#ifndef APPLICATION_H
#define APPLICATION_H

#include "window.h"

class Renderer;
class ChunkManager;
class Camera;

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
    
    // Timing
    double lastTime;
    
    // Debug mode
    bool debugMode;
    bool debugStepMode;
    bool debugStepRequested;
    bool prevF1KeyState;
    bool prevF2KeyState;
    
    void processInput(float deltaTime);
    void logDebugInfo();
};

#endif // APPLICATION_H