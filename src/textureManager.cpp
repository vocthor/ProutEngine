#include "textureManager.hpp"

#include <stdexcept>

TextureHandle TextureManager::load(const std::string &path, ::GLenum pixelType)
{
    auto it = cache_.find(path);
    if (it != cache_.end())
        return it->second;

    auto index = static_cast<std::uint32_t>(textures_.size());
    textures_.emplace_back(path, pixelType);
    auto handle = static_cast<TextureHandle>(index);
    cache_.emplace(path, handle);
    return handle;
}

const Texture &TextureManager::get(TextureHandle handle) const
{
    auto index = static_cast<std::uint32_t>(handle);
    if (index >= textures_.size())
        throw std::out_of_range("TextureManager::get: invalid handle");
    return textures_[index];
}
