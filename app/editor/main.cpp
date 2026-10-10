#include <memory>

#include "core/application.hpp"
#include "editor/editorLayer.hpp"
#include "game/gameLayer.hpp"

int main()
{
    Application app;
    app.pushLayer(std::make_unique<GameLayer>());
    app.pushLayer(std::make_unique<EditorLayer>());
    app.run();
    return 0;
}
