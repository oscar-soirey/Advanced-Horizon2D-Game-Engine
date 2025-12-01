#pragma once

#include <glad/glad.h>
#include <iostream>
#include <core/h2d_namespace.h>

NAMEPSACE_AH2D_START;

class Texture {
public:
    Texture(char const* packedPath, GLenum format);
    ~Texture();
    void bind(GLenum textureUnit = GL_TEXTURE0) const;
    [[nodiscard]] int getWidth() const;
    [[nodiscard]] int getHeight() const;

    [[nodiscard]] unsigned int getID() const;

private:
    unsigned int texture;
    int width, height, nrChannels;
};

NAMEPSACE_AH2D_END;