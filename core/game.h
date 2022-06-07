#pragma once
#include "engine.h"

namespace Avarice
{
    class Game
    {
    public:
        Game();
        explicit Game(std::string _windowTitle);
        ~Game();

        void Run();

    protected:
        virtual void PhysicsUpdate(float _deltaTime){};
        virtual void Update(float _deltaTime){};
        virtual void Render(){};
        virtual void OnExit(){};

    private:
        std::string m_Title;
        bool m_Running;

        void Initialize();
        void GUI();
        void Shutdown();
    };

    extern Game *CreateGame();

}