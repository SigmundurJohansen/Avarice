#pragma once

#include <string>

class Game
{
    public:
    Game();
    explicit Game(std::string windowTitle);
    ~Game() = default;

    void Run();

    private:
    void initializeServices();
    void shutdownServices();
};

extern Game* CreateGame();