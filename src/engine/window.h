class Window {
public:
    Window();
    ~Window();

    bool create(int width, int height, const char* title);
    void destroy();
    void processEvents();

    bool isOpen() const;

private:
    // Platform-specific window handle
    void* windowHandle;
    bool open;
};