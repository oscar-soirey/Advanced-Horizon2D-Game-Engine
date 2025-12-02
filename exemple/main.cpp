#include <iostream>

#include <ah2d/rendering/renderer.h>
#include <ah2d/core/window.h>
#include <ah2d/components/camera.h>
#include <ah2d/core/assets_manager.h>
#include <ah2d/actor.h>
#include <ah2d/controller.h>
#include <ah2d/components/colliders/capsule_collision.h>
#include <ah2d/core/maths.h>
#include <ah2d/core/h2d_namespace.h>

#include "platform.hpp"
#include "player.hpp"
#include "test_emitter.hpp"
#include "sprite.h"

USING_NAMESPACE_AH2D;

int main(int argc, char* argv[])
{
    Window window("GLFW", 1280, 720, "Test h2D Framework");
    glfwSwapInterval(0);  //desactiver la v sync

    Render render;

    AssetsManager::init(argv[0]);

    CameraComponent camera(70.0f);
    render.useCamera(&camera);

    Controller playerController{};

    camera.setRelativeLocation(glm::vec3(0, 0, 100));
    camera.setCameraRotation(maths::Rotator(-90, 0, 0));

    render.loadLevel("level.hscene");

    Actor* player0_actor = render.getActorByID("Player_0");
    Player* player0 = dynamic_cast<Player*>(player0_actor);

    auto skeletal = dynamic_cast<SkeletalSpriteComponent*>(player0->getComponentByID("sprite_component"));
    skeletal->getState()->setAnimation(0, "idle", true);

    player0->setActorScale(glm::vec2(0.2));

    playerController.possess(player0);

    auto* p = dynamic_cast<SpriteTest*>(render.getActorByID("sprite_test"));



    //player0->drawDebugTraces();
    auto comp = dynamic_cast<CapsuleCollisionComponent*>(player0->getComponentByID("capsule_component"));
    //if (comp != nullptr)
        //comp->drawDebugCollisions();

    //bug spawn actor from class marche pas
    /*
    auto* p = dynamic_cast<Platform*>(render.spawnActorFromClass("Platform"));
    p->setActorLocation(glm::vec3(0.0));
    p->setActorRotation(0.0);
    p->setActorScale(glm::vec2(1.0));*/

    //render.loadUIScreen("ui.hui");

    bool isFirstFrame = true;

    //inputs de la souris
    double lastX = 400, lastY = 300; //position initiale
    bool firstMouse = true;
    float sensitivity = 0.1f;

    while (!window.shouldClose())
    {
        window.processDebugInput();

        //std::cout << 1/window.getDeltaTime() << std::endl;

        window.update();
        if (!isFirstFrame)
            render.update(window);

        isFirstFrame = false;


        float cameraSpeed = 0.0f * static_cast<float>(window.getDeltaTime());
        glm::vec3 pos = camera.getWorldLocation();

        p->setActorRotation(p->getActorRotation() + 1*window.getDeltaTime());


        if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_D) == GLFW_PRESS)
            pos += cameraSpeed * camera.getCameraRotator().getRightVector();
        if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_A) == GLFW_PRESS)
            pos -= cameraSpeed * camera.getCameraRotator().getRightVector();

        if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_Q) == GLFW_PRESS)
            pos -= cameraSpeed * camera.getCameraRotator().getUpVector();
        if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_E) == GLFW_PRESS)
            pos += cameraSpeed * camera.getCameraRotator().getUpVector();

        if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_W) == GLFW_PRESS)
            pos += cameraSpeed * camera.getCameraRotator().getForwardVector();
        if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_S) == GLFW_PRESS)
            pos -= cameraSpeed * camera.getCameraRotator().getForwardVector();

        camera.setRelativeLocation(pos);

        bool mouseCapture;
        if (glfwGetMouseButton(window.getGLFWwindow(), GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
        {
            if (!mouseCapture) // on vient juste de capturer la souris
            {
                mouseCapture = true;
                glfwSetInputMode(window.getGLFWwindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

                // Réinitialiser la dernière position au moment de la capture
                glfwGetCursorPos(window.getGLFWwindow(), &lastX, &lastY);
                firstMouse = false;
            }
        }
        else if (glfwGetMouseButton(window.getGLFWwindow(), GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE)
        {
            mouseCapture = false;
            glfwSetInputMode(window.getGLFWwindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }


        double xpos, ypos;
        if (mouseCapture)
            glfwGetCursorPos(window.getGLFWwindow(), &xpos, &ypos);

        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos;  //inversé car y va vers le bas
        lastX = xpos;
        lastY = ypos;

        xoffset *= sensitivity;
        yoffset *= sensitivity;

        //Modifie le Rotator directement
        camera.setCameraRotation(maths::Rotator(camera.getCameraRotator().yaw + xoffset, camera.getCameraRotator().pitch + yoffset, camera.getCameraRotator().roll));
    }
    AssetsManager::deinit();

    return 0;
}



/* mouvement libre de camera
 *
 *
 *avant la boucle :
*   //inputs de la souris
    double lastX = 400, lastY = 300; //position initiale
    bool firstMouse = true;
    float sensitivity = 0.1f;


float cameraSpeed = 100.0f * window.getDeltaTime();
glm::vec3 pos = camera.getWorldLocation();


if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_D) == GLFW_PRESS)
    pos += cameraSpeed * camera.getCameraRotator().getRightVector();
if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_A) == GLFW_PRESS)
    pos -= cameraSpeed * camera.getCameraRotator().getRightVector();

if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_Q) == GLFW_PRESS)
    pos -= cameraSpeed * camera.getCameraRotator().getUpVector();
if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_E) == GLFW_PRESS)
    pos += cameraSpeed * camera.getCameraRotator().getUpVector();

if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_W) == GLFW_PRESS)
    pos += cameraSpeed * camera.getCameraRotator().getForwardVector();
if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_S) == GLFW_PRESS)
    pos -= cameraSpeed * camera.getCameraRotator().getForwardVector();

camera.setRelativeLocation(pos);

bool mouseCapture;
if (glfwGetMouseButton(window.getGLFWwindow(), GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
{
    if (!mouseCapture) // on vient juste de capturer la souris
    {
        mouseCapture = true;
        glfwSetInputMode(window.getGLFWwindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        // Réinitialiser la dernière position au moment de la capture
        glfwGetCursorPos(window.getGLFWwindow(), &lastX, &lastY);
        firstMouse = false;
    }
}
else if (glfwGetMouseButton(window.getGLFWwindow(), GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE)
{
    mouseCapture = false;
    glfwSetInputMode(window.getGLFWwindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}


double xpos, ypos;
if (mouseCapture)
    glfwGetCursorPos(window.getGLFWwindow(), &xpos, &ypos);

if (firstMouse)
{
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
}

float xoffset = xpos - lastX;
float yoffset = lastY - ypos;  //inversé car y va vers le bas
lastX = xpos;
lastY = ypos;

xoffset *= sensitivity;
yoffset *= sensitivity;

//Modifie le Rotator directement
camera.setCameraRotation(Utils::Rotator(camera.getCameraRotator().yaw + xoffset, camera.getCameraRotator().pitch + yoffset, camera.getCameraRotator().roll));
*/