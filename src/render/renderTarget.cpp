#include "render/renderTarget.hpp"

#include <algorithm>
#include <array>

#include <GLFW/glfw3.h>

#include "core/window.hpp"

namespace
{
    ::GLuint createTexture(unsigned int width, unsigned int height,
                           ::GLint internalFormat, ::GLenum format, ::GLenum type)
    {
        ::GLuint tex = 0;
        ::glGenTextures(1, &tex);
        ::glBindTexture(GL_TEXTURE_2D, tex);
        ::glTexImage2D(GL_TEXTURE_2D, 0, internalFormat,
                       static_cast<::GLsizei>(width), static_cast<::GLsizei>(height),
                       0, format, type, nullptr);
        ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        ::glBindTexture(GL_TEXTURE_2D, 0);
        return tex;
    }

    void deleteTexture(::GLuint t)
    {
        ::glDeleteTextures(1, &t);
    }
}

// ---------------------------------------------------------------------------
ScreenTarget::ScreenTarget(Window &window)
    : window_{window}
{
}

void ScreenTarget::bind()
{
    ::glBindFramebuffer(GL_FRAMEBUFFER, 0u);
    ::glViewport(0, 0, static_cast<::GLsizei>(width()), static_cast<::GLsizei>(height()));
}

void ScreenTarget::unbind()
{
    ::glBindFramebuffer(GL_FRAMEBUFFER, 0u);
}

unsigned int ScreenTarget::width() const
{
    int w = 0, h = 0;
    ::glfwGetFramebufferSize(window_.handle(), &w, &h);
    return static_cast<unsigned int>(std::max(w, 1));
}

unsigned int ScreenTarget::height() const
{
    int w = 0, h = 0;
    ::glfwGetFramebufferSize(window_.handle(), &w, &h);
    return static_cast<unsigned int>(std::max(h, 1));
}

// ---------------------------------------------------------------------------
OffscreenTarget::OffscreenTarget(unsigned int width, unsigned int height)
    : width_{std::max(width, 1u)}, height_{std::max(height, 1u)}
{
    create();
}

void OffscreenTarget::bind()
{
    fbo_->bind();
    ::glViewport(0, 0, static_cast<::GLsizei>(width_), static_cast<::GLsizei>(height_));
}

void OffscreenTarget::unbind()
{
    fbo_->unbind();
}

void OffscreenTarget::resize(unsigned int width, unsigned int height)
{
    width = std::max(width, 1u);
    height = std::max(height, 1u);
    if (width == width_ && height == height_)
        return;

    width_ = width;
    height_ = height;
    create();
}

void OffscreenTarget::create()
{
    // Recreate attachments, then the FBO. The previous ones are released by AutoRelease.
    fbo_.reset();
    color_ = AutoRelease<::GLuint>{createTexture(width_, height_, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE), deleteTexture};
    depthStencil_ = AutoRelease<::GLuint>{createTexture(width_, height_, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8), deleteTexture};

    std::array<::GLuint, 1> colors{color_.get()};
    // Same D24S8 texture used as depth and stencil attachment
    fbo_ = std::make_unique<Framebuffer>(colors, depthStencil_.get(), depthStencil_.get());
}
