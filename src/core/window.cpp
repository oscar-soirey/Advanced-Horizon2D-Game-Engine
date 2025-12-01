#include "window.h"

#include <iostream>
#include <algorithm>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <AL/alc.h>

#include <utils/log.h>

NAMEPSACE_AH2D_START;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // Récupère le pointeur vers la classe Window à partir de l'utilisateur de la fenêtre
    Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));

    win->width = width;
    win->height = height;

    glViewport(0, 0, width, height);
}


Window::Window(std::string type, int width, int height, const std::string& title) : type(type), width(width), height(height), alDevice(alcOpenDevice(nullptr)), alContext(alcCreateContext(alDevice, nullptr))
{
    if (type == "GLFW")
    {
        if (!glfwInit())
        {
            std::cerr << "Échec de l'initialisation de GLFW" << std::endl;
            std::exit(EXIT_FAILURE);
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

        //fullscreen mode
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);

        window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
        if (!window)
        {
            std::cerr << "Échec de la création de la fenêtre GLFW" << std::endl;
            glfwTerminate();
            std::exit(EXIT_FAILURE);
        }
        glfwSetWindowUserPointer(window, this);
        glfwMakeContextCurrent(window);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cerr << "Échec de l'initialisation de GLAD" << std::endl;
            std::exit(EXIT_FAILURE);
        }


        //activer la transparence des images
        glEnable(GL_BLEND);
        // Définir le mode de blending pour gérer la transparence
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


        //-----Initialiser openAL pour le son----->
        alcMakeContextCurrent(alContext);
    }
}

Window::~Window()
{
    alcMakeContextCurrent(nullptr);
    alcDestroyContext(alContext);
    alcCloseDevice(alDevice);

    if (type == "GLFW")
    {
        glfwDestroyWindow(window);
        glfwTerminate();
    }
}

void Window::update()
{
    if (type == "GLFW")
    {
        // Stocke le temps entre deux frames
        double currentTime = glfwGetTime();
        deltaTime = currentTime - lastFrame;
        lastFrame = currentTime;

        if (glfwGetKey(window, GLFW_KEY_F11) == GLFW_PRESS)
        {
            if (!f11pressedLastFrame)
            {
                f11pressedLastFrame = true;
                if (fullscreen)
                    setFullscreen(false);
                else
                    setFullscreen(true);
            }
        }
        if (glfwGetKey(window, GLFW_KEY_F11) == GLFW_RELEASE)
            f11pressedLastFrame = false;

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

bool Window::shouldClose() const
{
    if (type == "GLFW")
    {
        return glfwWindowShouldClose(window);
    }
    return true;
}

GLFWwindow* Window::getGLFWwindow() const
{
    return window;
}

bool drawCollisionsPressed = false;
void Window::processDebugInput()
{
    if (type == "GLFW")
    {
        //escape
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        //polygon mode
        if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        if (glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS)
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        //draw collisions
        if (glfwGetKey(window, GLFW_KEY_F3) == GLFW_PRESS)
        {
            if (!drawCollisionsPressed)
            {
                drawCollisionsPressed = true;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_F3) == GLFW_RELEASE)
            drawCollisionsPressed = false;
    }
}

double Window::getDeltaTime() const
{
    return std::clamp(deltaTime, 0.0001, 0.05);  //on bloque le delta-time pour ne pas générer des énormes delta times qui pourraient influencer le gameplay
}

glm::mat4 Window::getProjection(float cameraFov) const
{
    float aspect = (float)width / (float)height;  // largeur / hauteur de la fenêtre
    float nearPlane = 0.1f;
    float farPlane = 2000.0f;

    return glm::perspective(glm::radians(cameraFov), aspect, nearPlane, farPlane);
}

void Window::setFullscreen(bool newFullscreen)
{
    if (newFullscreen)
    {
        //mettre les valeurs de taille et position en cache pour pouvoir les retrouver quand on enlève le fullscreen
        glfwGetWindowSize(window, &cachedWidth, &cachedHeight);
        glfwGetWindowPos(window, &cachedX, &cachedY);
        //recuperer les attributs de l'ecran
        const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        //supprimer les bordures
        glfwSetWindowAttrib(window, GLFW_DECORATED, GLFW_FALSE);
        //mettre la fenêtre à la taille du moniteur
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
        fullscreen = true;
    }
    else
    {
        //revenir en mode fenêtré normal
        glfwSetWindowAttrib(window, GLFW_DECORATED, GLFW_TRUE);
        glfwSetWindowMonitor(window, nullptr, cachedX, cachedY, cachedWidth, cachedHeight, 0);
        glfwShowWindow(window);
        fullscreen = false;
    }
}

glm::vec2 Window::getNormalizedCursorPos()
{
    double mouseX, mouseY;
    glfwGetCursorPos(getGLFWwindow(), &mouseX, &mouseY);
    return {mouseX/width, mouseY/height};
}

NAMEPSACE_AH2D_END;