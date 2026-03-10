#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

#include "texture.hpp"

enum class TextureHandle : std::uint32_t
{
    Invalid = UINT32_MAX
};

// Used in other classes to reference a Texture and its role in the material (diffuse, specular, etc.)
struct TextureRef
{
    TextureHandle handle;
    std::string type; // rôle : "texture_diffuse", "texture_specular", etc.
};

class TextureManager
{
public:
    TextureHandle load(const std::string &path, ::GLenum pixelType = GL_UNSIGNED_BYTE);
    const Texture &get(TextureHandle handle) const;

private:
    std::vector<Texture> textures_;
    std::unordered_map<std::string, TextureHandle> cache_;
};
