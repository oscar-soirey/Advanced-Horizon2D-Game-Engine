#include "shader.h"
#include <fstream>  //pour lire et ecrire dans les fichiers
#include <sstream>  //fournit des classes pour manipuler des chaînes de caractères comme des flux
#include <iostream>
#include <filesystem>

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include <utils/log.h>

NAMEPSACE_AH2D_START;

Shader::Shader(const char *vertexPath, const char *fragmentPath)
{
    if (!std::filesystem::exists(vertexPath))
    {
        LOG_ERROR("shader : vertexPath doesnt exists");
    }
    if (!std::filesystem::exists(fragmentPath))
    {
        LOG_ERROR("shader : fragmentPath doesnt exists");
    }

    std::ifstream vShaderFile(vertexPath); //on ouvre les fichiers
    std::ifstream fShaderFile(fragmentPath);

    std::stringstream vShaderStream, fShaderStream;  //variables pour stocker les valeurs des fichiers
    vShaderStream << vShaderFile.rdbuf();  //on lit tout le contenu des fichiers et on les insère dans les flux
    fShaderStream << fShaderFile.rdbuf();

    //on convertit ces flux en chaine c++ puis on les convertit en char* pour OpenGL
    std::string vertexCode = vShaderStream.str();
    std::string fragmentCode = fShaderStream.str();
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    //compilation
    unsigned int vertex, fragment;

    //creation et compilation du vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, nullptr);
    glCompileShader(vertex);

    GLint success;
    char infoLog[512];
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex, 512, nullptr, infoLog);
        std::cerr << "Vertex Compilation Failed : " << infoLog << std::endl;
    }

    //creation et compilation du fragment shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, nullptr);
    glCompileShader(fragment);

    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment, 512, nullptr, infoLog);
        std::cerr << "Fragment Compilation Failed : " << infoLog << std::endl;
    }

    //Lier les shaders dans un programme
    //(pour vérifier que les shaders peuvent coopérer (ex. : le vertex shader produit bien des sorties que le fragment shader sait utiliser).
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);

    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(ID, 512, nullptr, infoLog);
        std::cerr << "Shader Program Failed : " << infoLog << std::endl;
    }

    //nettoyage des shaders intermediates (une fois liées dans un programme, elles ne sont plus nécéssaires individuellement)
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Shader::use() const
{
    //verification de la validité du programme
    if (ID == 0)
    {
        LOG_ERROR("Shader::use() called but program ID == 0 (shader not created)");
        return;
    }
    GLint success;
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetProgramInfoLog(ID, 512, nullptr, infoLog);
        LOG_ERROR("Shader linking failed: " + std::string(infoLog));
    }

    glUseProgram(ID);
    GLenum shader_err = glGetError();
    if (shader_err != GL_NO_ERROR)
    {
        LOG_ERROR("OpenGL error after shader use: " + std::to_string(shader_err));
    }
}

Shader::~Shader()
{
    glDeleteProgram(ID);
}

void Shader::setBool(const std::string &name, bool value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string &name, int value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value) const
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setMat4(const std::string &name, const glm::mat4 &value) const
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setVec4(const std::string &name, const glm::vec4 &value) const
{
    glUniform4f(glGetUniformLocation(ID, name.c_str()), value.x, value.y, value.z, value.w);
}

void Shader::setVec3(const std::string &name, const glm::vec3 &value) const
{
    glUniform3f(glGetUniformLocation(ID, name.c_str()), value.x, value.y, value.z);
}

void Shader::setVec2(const std::string &name, const glm::vec2 &value) const
{
    glUniform2f(glGetUniformLocation(ID, name.c_str()), value.x, value.y);
}


NAMEPSACE_AH2D_END;