#pragma once

#include <memory>

#include <glad/glad.h>

#include "render/frameBuffer.hpp"
#include "utils/autoRelease.hpp"

class Window;

// ---------------------------------------------------------------------------
// RenderTarget — where to draw the scene
// ---------------------------------------------------------------------------
class RenderTarget
{
public:
    virtual ~RenderTarget() = default;

    // Binds the target and sets the GL viewport to its size.
    virtual void bind() = 0;
    // Rebinds the default framebuffer (the window).
    virtual void unbind() = 0;

    virtual unsigned int width() const = 0;
    virtual unsigned int height() const = 0;
    virtual void resize(unsigned int width, unsigned int height) = 0;
};

// ---------------------------------------------------------------------------
// ScreenTarget — the window's default framebuffer (FBO 0).
// Its size is driven by the window, resize() is a no-op.
// ---------------------------------------------------------------------------
class ScreenTarget final : public RenderTarget
{
public:
    explicit ScreenTarget(Window &window);

    void bind() override;
    void unbind() override;

    unsigned int width() const override;
    unsigned int height() const override;
    void resize(unsigned int, unsigned int) override {}

private:
    Window &window_;
};

// ---------------------------------------------------------------------------
// OffscreenTarget — draws into textures (RGBA8 color + D24S8 depth/stencil).
// colorTexture() can be displayed afterwards, e.g. with ImGui::Image.
// ---------------------------------------------------------------------------
class OffscreenTarget final : public RenderTarget
{
public:
    OffscreenTarget(unsigned int width, unsigned int height);

    void bind() override;
    void unbind() override;

    unsigned int width() const override { return width_; }
    unsigned int height() const override { return height_; }
    void resize(unsigned int width, unsigned int height) override;

    ::GLuint colorTexture() const { return color_.get(); }

private:
    void create();

    unsigned int width_;
    unsigned int height_;
    AutoRelease<::GLuint> color_;
    AutoRelease<::GLuint> depthStencil_;
    std::unique_ptr<Framebuffer> fbo_;
};
