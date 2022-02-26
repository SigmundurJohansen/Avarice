#pragma once
#include <string>

namespace Avarice{
class Game
{
    public:
    Game();
    explicit Game(std::string _windowTitle);
    ~Game();

    void Run();
    protected:
    virtual void PhysicsUpdate(float _deltaTime){};
    virtual void Update(float _deltaTime) {};

    private:
    std::string m_Title;
    bool m_Running;

    void Initialize();
    void GUI();
    void Shutdown();
};

extern Game* CreateGame();

}