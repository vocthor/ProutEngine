#include <memory>

#include "core/application.hpp"
#include "game/gameLayer.hpp"

int main()
{
    Application app;
    app.pushLayer(std::make_unique<GameLayer>());
    app.run();
    return 0;
}
