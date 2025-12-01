#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <utility>

#include "texture.h"

#include <utils/log.h>
#include <core/assets_manager.h>

NAMEPSACE_AH2D_START;

//texture class
Texture::Texture(char const* packedPath, GLenum format)
{
    //bind de la nouvelle texture
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    //configuration des parametres de la texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //Charger les données brutes de l'image en mémoire via AssetsManager
    std::vector<char> image_buffer = AssetsManager::load_asset(packedPath);
    if (image_buffer.empty())
    {
        std::string packed_path_str = packedPath;
        Log::Error("Erreur: Impossible de charger l'asset " + packed_path_str + " via AssetsManager");
        return; // Échec du chargement de l'actif
    }

    stbi_set_flip_vertically_on_load(true);
    // Convertir std::vector<char>::data() en unsigned char* et sa taille en int
    unsigned char *data = stbi_load_from_memory(
        (stbi_uc const*)image_buffer.data(), //Pointeur vers les données
        (int)image_buffer.size(),            //Taille totale du tampon
        &width,
        &height,
        &nrChannels,
        STBI_rgb_alpha
    );

    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
#ifdef LOG_INFOS_FULL
        Log::Info("Texture Loaded");
#endif
    }
    else
    {
        Log::Error("Texture loading Error");
    }
    stbi_image_free(data);
}

Texture::~Texture()
{
    glDeleteTextures(1, &texture);
}

void Texture::bind(GLenum textureUnit) const
{
    glActiveTexture(textureUnit);
    glBindTexture(GL_TEXTURE_2D, texture);
}

int Texture::getWidth() const
{
    return width;
}

int Texture::getHeight() const
{
    return height;
}

unsigned int Texture::getID() const
{
    return texture;
}

NAMEPSACE_AH2D_END;