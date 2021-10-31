#pragma once
#include <string>

class Game
{
    public:
    Game();
    explicit Game(std::string windowTitle);
    ~Game();

    void Run();
    protected:
    virtual void PhysicsUpdate(float deltaTime){};
    virtual void Update(float deltaTime) {};

    private:
    std::string mTitle;
    bool mRunning;

    void initializeServices();
    void shutdownServices();
};

extern Game* CreateGame();