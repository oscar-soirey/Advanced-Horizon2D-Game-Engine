#include "sprite.h"

#include <glm/gtx/string_cast.hpp>
#include <algorithm>
#include <utility>
#include <iostream>

#include <rendering/renderer.h>
#include <rendering/texture.h>
#include <rendering/shader.h>
#include <core/window.h>
#include <components/camera.h>
#include <utils/log.h>

NAMEPSACE_AH2D_START;


SpriteComponent::SpriteComponent(Render& renderObject, const std::string& texturePackedPath, float zOrder)
        : render(renderObject),
          texturePackedPath(texturePackedPath),
          zOrder(zOrder)
{

    //on indexe le sprite dans la liste du render
    //ajouter le sprite a la liste de rendu pour etre draw (en respectant l'ordre du zOrder)
    //trouver la position où insérer en gardant le vecteur trié par zOrder
    auto it = std::lower_bound(
        render.sprites.begin(),
        render.sprites.end(),
        this,
        [](const SpriteComponent* leftSprite, const SpriteComponent* rightSprite)
        {
            return leftSprite->zOrder < rightSprite->zOrder;
        }
    );
    //insérer à la bonne position
    render.sprites.insert(it, this);

    auto it_ = render.texturesCache.find(texturePackedPath);

    //comparer l'itérateur retourné avec numbers.end()
    if (it_ != render.texturesCache.end())
    {
        //la texture est dans le cache
#ifdef LOG_INFOS_FULL
        Log::Info(texturePackedPath + " est dans le cache");
#endif
        activeTex = it_->second.get();
    }
    else
    {
        //la texture n'est pas encore en cache et n'est pas chargée
#ifdef LOG_INFOS_FULL
        Log::Info(texturePackedPath + " n'est pas dans le cache");
#endif

        //--- Utilisation de insert pour optimiser ---
        //on insère l'élément. La méthode insert retourne une PAIR :
        //- le premier élément (it_insert.first) est l'itérateur vers le nouvel élément.
        //- le second (it_insert.second) est un booléen (vrai si l'insertion a eu lieu).
        auto unique_tex = std::make_unique<Texture>(texturePackedPath.c_str(), GL_RGBA8);

        render.texturesCache[texturePackedPath] = std::move(unique_tex);

        activeTex = render.texturesCache[texturePackedPath].get();
    }
#ifdef LOG_INFOS_FULL
    Log::Info("Sprite created, texture : " + std::to_string(activeTex->getWidth()) + "x" + std::to_string(activeTex->getHeight()) + ", name : " + texturePackedPath);
#endif

    //changer le scale en fonction de la texture, et plus tard en fonction de l'uv
    float width =  activeTex->getWidth()/1000.0f;
    float height = activeTex->getHeight()/1000.0f;
    defaultTextureScale = glm::vec2(width, height);
}

SpriteComponent::~SpriteComponent()
{
    render.removeSprite(this);
}


void SpriteComponent::draw(Shader& shader, Window& window)
{
    updateModelMatrix();

    //lier la texture
    activeTex->bind(GL_TEXTURE0);

    bindGLUV();

    shader.setMat4("model", model);
    shader.setVec4("tintColor", tintColor);

    //dessiner le sprite
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

void SpriteComponent::update(Window& window)
{
}

bool SpriteComponent::isVisibleY(CameraComponent& camera, Window& window)
{
    return true;
    glm::mat4 mvp = camera.getView() * model; //matrice complète (en prennant en compte la matrice view de la camera)

    //définit les coordonées des points de chaque corner de l'image
    glm::vec4 corners[4] = {
        mvp * glm::vec4(-50.0f, -50.0f, 0.0f, 1.0f), // coin bas-gauche transformé
        mvp * glm::vec4( 50.0f, -50.0f, 0.0f, 1.0f), // coin bas-droit transformé
        mvp * glm::vec4( 50.0f,  50.0f, 0.0f, 1.0f), // coin haut-droit transformé
        mvp * glm::vec4(-50.0f,  50.0f, 0.0f, 1.0f), // coin haut-gauche transformé
    };

    // Initialisation de minY et maxY avec la composante Y du premier coin (ces valeurs seront modifées plus tard dans la boucle)
    float minY = corners[0].y;
    float maxY = corners[0].y;

    //trouver la minY et la maxY.
    for (int i = 1; i < 4; ++i) {
        if (corners[i].y < minY) minY = corners[i].y;
        if (corners[i].y > maxY) maxY = corners[i].y;
    }

    // Vérifie si l'intervalle vertical [minY, maxY] du sprite intersecte
    // la fenêtre d'affichage verticale fixée entre -100 et +100.
    // Si le sprite est entièrement en dessous (-∞, -100) ou au-dessus (100, +∞),
    // il n'est pas visible, donc retourne false.
    // Sinon, il est au moins partiellement visible, retourne true.
    return !(maxY < -100.0f+camera.getWorldLocation().y || minY > 100.0f+camera.getWorldLocation().y);
}

//meme chose pour les X
bool SpriteComponent::isVisibleX(CameraComponent& camera, Window& window)
{
    return true;
    glm::mat4 mvp = camera.getView() * model; //matrice complète (en prennant en compte la matrice view de la camera)

    //définit les coordonées des points de chaque corner de l'image
    glm::vec4 corners[4] = {
        mvp * glm::vec4(-50.0f, -50.0f, 0.0f, 1.0f), // coin bas-gauche transformé
        mvp * glm::vec4( 50.0f, -50.0f, 0.0f, 1.0f), // coin bas-droit transformé
        mvp * glm::vec4( 50.0f,  50.0f, 0.0f, 1.0f), // coin haut-droit transformé
        mvp * glm::vec4(-50.0f,  50.0f, 0.0f, 1.0f), // coin haut-gauche transformé
    };

    float minX = corners[0].x;
    float maxX = corners[0].x;

    for (int i = 1; i < 4; ++i) {
        if (corners[i].x < minX) minX = corners[i].x;
        if (corners[i].x > maxX) maxX = corners[i].x;
    }

    //on regarde le ratio de window sur les x (variable selon les types d'écran).
    float ratioWidth = (window.width*100.0f)/window.height;
    return !(maxX < -ratioWidth+camera.getWorldLocation().x || minX > ratioWidth+camera.getWorldLocation().x);
}

void SpriteComponent::updateModelMatrix()
{
    //glm::vec3 pivotWorld{pivot.x + getWorldLocation().x, pivot.y + getWorldLocation().y, getWorldLocation().z};

    //on deplace l'objet au point de pivot, on applique la rotation, puis on remet l'objet a sa position
    glm::mat4 m = glm::mat4(1.0f);

    //position world
    m = glm::translate(m, getWorldLocation());

    //translation jusqu’au pivot
    m = glm::translate(m, glm::vec3(pivot, 0.0f));

    //rotation autour de Z
    m = glm::rotate(m, getWorldRotation(), glm::vec3(0, 0, 1));

    //retour en arrière du pivot
    m = glm::translate(m, glm::vec3(-pivot, 0.0f));

    //scale
    m = glm::scale(m, glm::vec3(getWorldScale(), 1.0f));
    m = glm::scale(m, glm::vec3(defaultTextureScale, 1.0f));

    model = m;
}

void SpriteComponent::bindGLUV()
{
    float newVertices[] = {
        // Positions        // UV
        -50.0f,  50.0f, 0.0f,  uv.uMin, uv.vMax,  // top-left
         50.0f,  50.0f, 0.0f,  uv.uMax, uv.vMax,  // top-right
         50.0f, -50.0f, 0.0f,  uv.uMax, uv.vMin,  // bottom-right
        -50.0f, -50.0f, 0.0f,  uv.uMin, uv.vMin   // bottom-left
    };

    //changer la taille par defaut de la texture pour que ca prenne la taille en tenant compte de l'uv
    float width =  activeTex->getWidth()/1000.0f;
    float height = activeTex->getHeight()/1000.0f;
    float uvsize_u = uv.uMax - uv.uMin;
    float uvsize_v = uv.vMax - uv.vMin;
    defaultTextureScale = glm::vec2(width*uvsize_u, height*uvsize_v);

    //pas sur que c'est utile ca
    glBindBuffer(GL_ARRAY_BUFFER, render.getVBO());
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(newVertices), newVertices);
}

void SpriteComponent::setZOrder(float newZOrder)
{
    //changer le zOrder equivaut a supprimer le sprite de la liste de rendu, puis le rajouter au bon endroit

    render.removeSprite(this);

    zOrder = newZOrder;

    //on indexe le sprite dans la liste du render
    //ajouter le sprite a la liste de rendu pour etre draw (en respectant l'ordre du zOrder)
    //trouver la position où insérer en gardant le vecteur trié par zOrder
    auto it = std::lower_bound(
        render.sprites.begin(),
        render.sprites.end(),
        this,
        [](const SpriteComponent* leftSprite, const SpriteComponent* rightSprite)
        {
            return leftSprite->zOrder < rightSprite->zOrder;
        }
    );
    //insérer à la bonne position
    render.sprites.insert(it, this);
}

void SpriteComponent::setPivot(glm::vec2 newPivot)
{
    //on inverse le y pour que les coordonnées bas soit > au haut de la scene
    pivot = glm::vec2(newPivot.x, -newPivot.y);
}



NAMEPSACE_AH2D_END;