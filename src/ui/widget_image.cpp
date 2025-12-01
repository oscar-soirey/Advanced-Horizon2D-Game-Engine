#include <glad/glad.h>

#include "widget_image.h"

#include <rendering/shader.h>
#include <rendering/texture.h>
#include <rendering/renderer.h>
#include <core/reflection.h>
#include <utils/log.h>

NAMEPSACE_AH2D_START;

widget::Image::Image()
{
    HPROPERTY(texture_path, Access::Editable);
    HPROPERTY(tint_color, Access::Editable);
}

void widget::Image::init(Render &render)
{
    BaseWidget::init(render);

    auto it_ = render.texturesCache.find(texture_path);

    //Comparer l'itérateur retourné avec numbers.end()
    if (it_ != render.texturesCache.end())
    {
        //la texture est dans le cache
#ifdef LOG_INFOS_FULL
        Log::Info(texture_path + " UI : est dans le cache");
#endif
        texture_object = it_->second.get();
    }
    else
    {
        //la texture n'est pas encore en cache et n'est pas chargée
#ifdef LOG_INFOS_FULL
        Log::Info(texture_path + " UI : n'est pas dans le cache");
#endif

        // --- Utilisation de insert pour optimiser ---
        // On insère l'élément. La méthode insert retourne une PAIR :
        // - le premier élément (it_insert.first) est l'itérateur vers le nouvel élément.
        // - le second (it_insert.second) est un booléen (vrai si l'insertion a eu lieu).
        auto unique_tex = std::make_unique<Texture>(texture_path.c_str(), GL_RGBA8);

        render.texturesCache[texture_path] = std::move(unique_tex);

        texture_object = render.texturesCache[texture_path].get();

        //changer le scale en fonction de la texture
        float width =  texture_object->getWidth()/1000.0f;
        float height = texture_object->getHeight()/1000.0f;
        default_texture_scale = glm::vec2(width, height);
    }
}

void widget::Image::draw(Shader &shader, Window& window)
{
    //bien sur tres important !! -->
    BaseWidget::draw(shader, window);

    shader.setVec4("tintColor", tint_color);

    texture_object->bind();

    //dessiner le sprite
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}


NAMEPSACE_AH2D_END;