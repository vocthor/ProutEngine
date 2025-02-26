#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <stb_image/stb_image.h>

#include "shader.hpp"

class Texture
{
public:
    GLuint ID;
    const char *type;
    GLuint unit;
    /**
     * @brief Format of the image. `GL_RGB` for jpg, `GL_RGBA` for png.
     */
    GLenum format;

    /**
     * @brief Construct a new Texture object
     *
     * @param image Path to the image file
     * @param texType Is it a 1D, 2D or 3D texture ?
     * @param slot Which unit slot to use (in a case of multiple textures)
     * @param pixelType Usually GL_UNSIGNED_BYTE
     */
    Texture(const char *image, const char *texType, GLenum slot, GLenum pixelType);

    /**
     * @brief Assigns a texture unit to a texture
     *
     * @param shader Reference to the main shader program object.
     * @param uniform Name of the sampler2D uniform to use in the fragment shader.
     * @param unit Texture unit slot to link this Texture to. Usually similar as the slot used (cf constructor).
     */
    void texUnit(Shader &shader, const char *uniform, GLuint unit);

    // Binds a texture
    void bind();

    // Unbinds a texture
    void unbind();

    // Deletes a texture
    void remove();
};
#endif