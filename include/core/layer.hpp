#pragma once

#include "core/engineContext.hpp"

// ---------------------------------------------------------------------------
// Layer — one self-contained piece of behaviour (game logic, editor, ...).
//
// Lifecycle: attach() -> (onUpdate(), onRender())* -> detach()
// Layers are updated, then rendered, from the bottom of the stack to the top.
// ---------------------------------------------------------------------------
class Layer
{
public:
    virtual ~Layer() = default;

    // Called by Application when the layer is pushed (GL context is ready).
    void attach(EngineContext &ctx)
    {
        ctx_ = &ctx;
        onAttach();
    }

    void detach()
    {
        onDetach();
        ctx_ = nullptr;
    }

    virtual void onAttach() {}
    virtual void onDetach() {}
    virtual void onUpdate() = 0;
    virtual void onRender() = 0;

protected:
    EngineContext &ctx() { return *ctx_; }

private:
    EngineContext *ctx_ = nullptr;
};
