#pragma once

#include <functional>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <core/h2d_namespace.h>

class ALCdevice;
class ALCcontext;

NAMEPSACE_AH2D_START;

class Window {
public:
    int width;
    int height;

    Window(std::string type, int width, int height, const std::string& title);
    ~Window();

    void update();
    [[nodiscard]] bool shouldClose() const;
    [[nodiscard]] GLFWwindow* getGLFWwindow() const;
    void processDebugInput();
    void setFullscreen(bool newFullscreen);

    [[nodiscard]] double getDeltaTime() const;

    [[nodiscard]] glm::mat4 getProjection(float cameraFov) const;

    glm::vec2 getNormalizedCursorPos();

private:
    std::string type;

    GLFWwindow* window;
    double lastFrame;
    double deltaTime;

    //sons
    ALCdevice* alDevice;
    ALCcontext* alContext;

    bool fullscreen;
    int cachedWidth, cachedHeight;
    int cachedX, cachedY;

    bool f11pressedLastFrame;

};

NAMEPSACE_AH2D_END;