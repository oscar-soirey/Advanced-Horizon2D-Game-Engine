#include "renderer.h"

#include <iostream>
#include <sstream>
#include <algorithm>
#include <filesystem>

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <utility>
#include <external/tinyxml2/tinyxml2.h>

#include <components/light.h>
#include <components/camera.h>
#include <components/colliders/box_collision.h>
#include <core/window.h>
#include <rendering/shader.h>
#include <core/assets_manager.h>
#include <utils/log.h>
#include <actor.h>
#include <core/reflection.h>
#include "physics_namespace.h"

using namespace reactphysics3d;

NAMEPSACE_AH2D_START;

// Créer le vecteur de vertices dans le corps du constructeur
float vertices[] = {
    // Positions        // Coordonnées de texture
    -50.0f,  50.0f, 0.0f,  0.0f, 1.0f,  // Haut gauche
     50.0f,  50.0f, 0.0f,  1.0f, 1.0f,  // Haut droit
     50.0f, -50.0f, 0.0f,  1.0f, 0.0f,  // Bas droit
    -50.0f, -50.0f, 0.0f,  0.0f, 0.0f   // Bas gauche
};

unsigned int indices[] = { 0, 1, 3, 1, 2, 3 };

Render::Render()
{
    //on crée les deux shaders
    shader = std::make_unique<Shader>("wvertex.glsl", "wfragment.glsl");
    ui_shader = std::make_unique<Shader>("uivertex.glsl", "uifragment.glsl");

    shader->use();
    shader->setInt("texture1", 0);  //dire que la variable texture1 dans le shader contiendra la valeur 0 (en reference à l'unité de texture TEXTURE_0)
    ui_shader->setInt("texture1", 0);

    //-----> creation des VAO et autres pour le shader world (pour les sprites) <-----//
    //creation du vao, vbo et ebo
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    //lier le vao
    glBindVertexArray(VAO);

    //lier le vbo et envoyer les données de vertex à ce vbo
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    //lier le ebo
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    //quand on dessinera avec ce VAO, il faudra communiquer 3 entrés au shader
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)nullptr);
    glEnableVertexAttribArray(0);

    //envoyer les coordonnées de texture au vertex shader
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    //on ressort avec le VAO déja bind et pret a etre utilisé

    //reactPhysics3D, collisions physique :
    physics::physics_world = physics::physics_common.createPhysicsWorld();
    physics::physics_world->setGravity(Vector3(0.0f, -9.81f, 0.0f));
}


Render::~Render()
{
/*
    Log::Debug(std::to_string(physics_world->getNbRigidBodies()));
    //detruire le physics world
    if (physics_world != nullptr)
    {
        Log::Debug("physics world not nullptr");
        physics_common.destroyPhysicsWorld(physics_world);
        physics_world = nullptr;
    }*/

    //liberer la memoire allouée aux vao, vbo et ebo
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
}


void Render::update(Window& window, glm::vec4 backgroundColor)
{
    physics::physics_world->update(window.getDeltaTime());

    //on draw les sprites en premier, on bind un vbo qui gere les vertices 3d (entre -50 et 50)
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    shader->use();

    //glRendering
    glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, backgroundColor.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (auto & actor : actors)
    {
        if (actor.second != nullptr)
        {
            actor.second->update(window);
        }
    }

    if (cameraInUse)
    {
        //Initializer les valeurs de projection
        glm::mat4 projection = window.getProjection(cameraInUse->fov);
        //Envoyer les paramètres de projection au shader
        shader->setMat4("projection", projection);

        //draw de tous les sprites
        for (SpriteComponent* obj : sprites)
        {
            //on update meme si il n'est pas visible
            obj->update(window);

            if (obj->isVisibleY(*cameraInUse, window) && obj->isVisibleX(*cameraInUse, window) && obj->visible)
            {
                //on draw uniquement si il est visible a l'écran
                obj->draw(*shader, window);
            }
        }

        cameraInUse->update(*shader);
    }
    else
        Log::Info("Render : no camera is currently used");

    //gestion des erreurs
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR)
    {
        LOG_ERROR("OpenGL Error: " + std::to_string(err));
    }

    for (const auto& id : actorsToRemove)
    {
        actors.erase(id);  //on supprime définitivement l'actor de la liste d'actors
    }
    actorsToRemove.clear();


    //----- puis on draw les widgets -----//
    ui_shader->use();
    float aspect = (float)window.width / (float)window.height;

    //calculer la projection orthographique, top : -0.5, bottom : 0.5
    float orthoSize = 1.0f;
    float halfHeight = orthoSize * 0.5f;
    float halfWidth = halfHeight * aspect;

    float left   = -halfWidth;
    float right  =  halfWidth;
    float bottom =  1.0f;  //on met directement les valeurs pour que les y aillent de 0 à 1
    float top    =  0.0f;

    glm::mat4 orthoProjection = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);

    ui_shader->setMat4("projection", orthoProjection);

    for (const auto& [id, widget] : widgets)
    {
        if (widget != nullptr)
        {
            widget->update(&window);
            if (widget->visible)
            {
                widget->draw(*ui_shader, window);
            }
        }
    }
}

Shader Render::getShader()
{
    return *shader;
}

//-------------------------Lights----------------------------------
void Render::addLight(LightComponent* newLight)
{
    //on ajoute la light dans le tableau
    lights.push_back(newLight);
    lightNum++;

    //on crée 3 tableaux de valeurs de light
    std::vector<glm::vec3> locations;
    std::vector<glm::vec3> colors;
    std::vector<float> attenuations;
    for (LightComponent* light : lights)
    {
        locations.push_back(light->getWorldLocation());
        colors.push_back(light->color);
        attenuations.push_back(light->attenuation);
    }

    //on envoie les informations de lumiere au shader
    glUniform1i(glGetUniformLocation(shader->ID, "numLights"), lightNum);
    glUniform2fv(glGetUniformLocation(shader->ID, "lightLoc"), lightNum, &locations[0].x);  //c'est un pointeur vers le premier float de la structure utilisé pour envoyer tout le tableau
    glUniform3fv(glGetUniformLocation(shader->ID, "lightColor"), lightNum, &colors[0].x);
    glUniform1fv(glGetUniformLocation(shader->ID, "lightRadius"), lightNum, &attenuations[0]);
}

void Render::delLight(LightComponent* oldLight)
{
    lights.erase(
        std::remove(lights.begin(), lights.end(), oldLight), lights.end()
    );
    lightNum--;
}


//crée une fonction qui permet de cast avec un unique_ptr
template <typename Derived, typename Base>
std::unique_ptr<Derived> dynamic_unique_cast(std::unique_ptr<Base>&& base)
{
    if (auto* casted = dynamic_cast<Derived*>(base.get()))
    {
        base.release();
        return std::unique_ptr<Derived>(casted);
    }
    return nullptr;
}

//-------------------------Factory----------------------------------
// Construct crée un objet du type demandé via la factory
Actor* Render::construct(const std::string& instanceID, const std::string& className, bool init)
{
    //récupère la factory globale
    auto& factory = GlobalFactory();
    //on recherche la classe
    auto it = factory.find(className);
    if(it != factory.end())
    {
        auto [iter, inserted] = actors.emplace(instanceID, dynamic_unique_cast<Actor>(it->second()));
        //creer l'actor
        Actor* actor_ptr = iter->second.get();
        if (init)
            actor_ptr->init(*this);
        return actor_ptr;
    }
    else
    {
        LOG_ERROR("Failed to find class in factory: " + className + ", make sure you registered the class by adding in the file the macro HCLASS('classname')");
        return nullptr;
    }
}

Actor* Render::spawnActorFromClass(const std::string &className)
{
    std::string id = maths::generateRandomID(className + "_");
    auto actor = construct(id, className);
    return actor;
}

void Render::destroyActorFromPointer(Actor* actorptr)
{
    // rechercher la clé correspondant au pointeur
    auto it = std::find_if(actors.begin(), actors.end(),
        [actorptr](const auto& pair)
        {
            return pair.second.get() == actorptr; // compare les pointeurs bruts
        });

    if (it != actors.end())
    {
        actorsToRemove.push_back(it->first);
    }
}

void Render::destroyActorFromID(const std::string& id)
{
    actorsToRemove.push_back(id);
}

void Render::removeSprite(SpriteComponent* spriteptr)
{
    std::erase(sprites, spriteptr);
}


void Render::useCamera(CameraComponent* camera)
{
    cameraInUse = camera;
}

GLuint Render::getVBO() const
{
    return VBO;
}

bool Render::loadLevel(const std::string &levelPackedPath)
{
    auto levelData = AssetsManager::load_asset(levelPackedPath);
    std::string levelStr(levelData.begin(), levelData.end());

    tinyxml2::XMLDocument doc;
    if (doc.Parse(levelStr.c_str()) != tinyxml2::XML_SUCCESS)
    {
        Log::Error("Failed to load .hscene xml file : " + levelPackedPath);
        return false;
    }
    tinyxml2::XMLElement* levelElem = doc.FirstChildElement("Level");
    currentLevelName = levelElem->Attribute("name");

    //-----Outliner-----
    tinyxml2::XMLElement* actorsElem = levelElem->FirstChildElement("Outliner");
    if (!actorsElem)
    {
        Log::Error("Failed to load level : No xml tag <Outliner> : " + levelPackedPath);
        return false;
    }

    //parcourir tous les actors dans <Outliner>
    for (tinyxml2::XMLElement* actorElem = actorsElem->FirstChildElement(); actorElem != nullptr; actorElem = actorElem->NextSiblingElement())
    {
        std::string actorType = actorElem->Name(); //charger le nom de la balise, ce sera utile pour construire la bonne classe
        const char* objectID = actorElem->Attribute("objectID");

        auto actor = construct(objectID, actorType, false);

        //boucle sur tous les attributs de l'élément
        for (const tinyxml2::XMLAttribute* attr = actorElem->FirstAttribute(); attr != nullptr; attr = attr->Next())
        {
            std::string attribName = attr->Name();
            std::string attribValue = attr->Value();
            if (actor->propertyExists(attribName))
            {
                PropertyVariantType valueVariantType = getValueByString(attribValue);
                std::visit([actor, attribName](auto&& val)
                {
                    actor->setPropertyValue(attribName, val);
                }, valueVariantType);
            }
            else
            {
                //"Property pos doest exists in class Character", par exemple
                LOG_ERROR("Property " + attribName + " doesnt exists in class : " + actorElem->Name());
            }
        }
        actor->init(*this);
    }
    return true;
}

const std::string &Render::getCurrentLevelName()
{
    return currentLevelName;
}

Actor *Render::getActorByID(const std::string &ID) const
{
    //on vérifie d'abord que l'id est bien présent dans la liste d'actors
    if (actors.contains(ID))
    {
        return actors.at(ID).get();
    }
    else
    {
        Log::Debug("getActorByID : " + ID + " not found in outliner");
        return nullptr;
    }
}

PhysicsCommon* Render::getPhysicsCommon()
{
    return &physics::physics_common;
}

PhysicsWorld* Render::getPhysicsWorld() const
{
    return physics::physics_world;
}

widget::BaseWidget *Render::constructWidget(const std::string &instanceID, const std::string &className, bool init)
{
    //récupère la factory globale
    auto& factory = GlobalFactory();
    //on recherche la classe
    auto it = factory.find(className);
    if(it != factory.end())
    {
        auto [iter, inserted] = widgets.emplace(instanceID, dynamic_unique_cast<widget::BaseWidget>(it->second()));
        //creer le widget
        widget::BaseWidget* widget_ptr = iter->second.get();
        if (init)
            widget_ptr->init(*this);
        return widget_ptr;
    }
    else
    {
        LOG_ERROR("Failed to find class in factory: " + className + ", make sure you registered the class by adding in the file the macro HCLASS('classname')");
        return nullptr;
    }
}


//fonction récursive pour parcourir un élément et tous ses enfants
using namespace tinyxml2;
void iterateElements(Render* render, XMLElement* element, int depth = 0)
{
    if (!element) return;

    //Indentation selon la profondeur pour affichage
    std::string indent(depth * 2, ' ');

    //Affiche le nom de l'élément
    std::cout << indent << "Element: " << element->Name() << "\n";

    //si l'élément a un nom et un id valide, sinon on skip et on passe au prochain element
    if (element->Name() && element->Attribute("objectID"))
    {
        auto widget = render->constructWidget(element->Attribute("objectID"), element->Name(), false);

        //boucle sur tous les attributs de l'élément
        for (const XMLAttribute* attr = element->FirstAttribute(); attr != nullptr; attr = attr->Next())
        {
            std::string attribName = attr->Name();
            std::string attribValue = attr->Value();
            if (widget->propertyExists(attribName))
            {
                PropertyVariantType valueVariantType = getValueByString(attribValue);
                std::visit([widget, attribName](auto&& val)
                {
                    widget->setPropertyValue(attribName, val);
                }, valueVariantType);
            }
            else
            {
                //"Property pos doest exists in class Button", par exemple
                LOG_ERROR("Property " + attribName + " doesnt exists in class : " + element->Name());
            }
        }
        widget->init(*render);
    }

    // Appel récursif sur tous les enfants
    XMLElement* child = element->FirstChildElement();
    while (child)
    {
        iterateElements(render, child, depth+1);
        //passer a l'autre element pricipal
        child = child->NextSiblingElement();
    }
}

bool Render::loadUIScreen(const std::string &packedPath)
{
    auto uiData= AssetsManager::load_asset(packedPath);
    std::string uiStr(uiData.begin(), uiData.end());

    tinyxml2::XMLDocument doc;
    if (doc.Parse(uiStr.c_str()) != XML_SUCCESS)
    {
        Log::Error("Failed to load .hui xml file : " + packedPath);
        return false;
    }
    //on cherche les elements et on renvoie une erreur quand l'element n'est pas trouvé
    XMLElement* uiElem = doc.FirstChildElement("ui");
    if (!uiElem)
    {
        LOG_ERROR("Missing <ui> root element in " + packedPath);
        return false;
    }

    const char* nameAttr = uiElem->Attribute("name");
    const char* styleAttr = uiElem->Attribute("style");

    if (!nameAttr)
    {
        LOG_INFO("Missing name attribute in " + packedPath);
    }
    if (styleAttr)
    {
        //réécrire cette partie du code et mettre une fonction dans utils
        //combiner le chemin absolu du ui et le chemin relatif du style
        std::filesystem::path ui_path = packedPath;
        std::filesystem::path style_relative_path = styleAttr;
        std::filesystem::path style_absolute_path = (ui_path / style_relative_path).lexically_normal();
        std::string stylePackedPath = style_absolute_path.string();
    }
    else
        LOG_INFO("Missing style attribute in " + packedPath);

    //fonction récursive qui va créer tous les elements au fur et a mesure
    iterateElements(this, uiElem);

    return true;
}


NAMEPSACE_AH2D_END;