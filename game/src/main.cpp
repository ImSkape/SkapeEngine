// game/src/main.cpp
#include "engine/core/Application.h"
#include <string>

int main(int argc, char** argv)
{
    for (int i = 1; i < argc; i++) {
        if (std::string(argv[i]) == "--headless") {
            Application app;
            app.RunHeadless("game", 10);
            return 0;
        }
    }
    Application app;
    app.Run("projects/MyGame/project.yaml");
    return 0;
}