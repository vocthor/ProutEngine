#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

#include "render/texture.hpp"

enum class TextureHandle : std::uint32_t
{
    Invalid = UINT32_MAX
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
