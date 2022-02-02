#include "game.h"
#include <service_locator.h>
#include "rendering/vulkan/vulkan_renderer.h"
#include "../platforms/custom_window.h"

namespace Avarice{
Game::Game() : Game("Super Gamge"){  }

Game::Game(std::string _windowTitle) : m_Title(std::move(_windowTitle)), m_Running(true)
{
    Initialize();
}

Game::~Game()
{
    Shutdown();
}

void Game::Run()
{
    //run the game
    while(m_Running)
    {
        if(ServiceLocator::GetWindow()->Update())
        {
            m_Running = false;
            continue;
        }

        if (ServiceLocator::GetInputManager()) {
            ServiceLocator::GetInputManager()->processInput();
        }
        // calculate delta time

        // update game state
        Update(0.0f); 
        // update pyhsics

        // draw
        ServiceLocator::GetRenderer()->RenderFrame();
    }
}

void Game::Initialize()
{
        // provide input manager
    ServiceLocator::Provide(new InputManager());
    // provide window
    ServiceLocator::Provide(new CustomWindow());

    // open window
    ServiceLocator::GetWindow()->OpenWindow({.title = m_Title, .width = 800, .height = 600});
    
    // initialize input system

    // initialize renderer 
    RendererSettings settings
    {
        .ApplicationName = m_Title
    };
    ServiceLocator::Provide(new VulkanRenderer(), settings);
}

void Game::Shutdown()
{
    ServiceLocator::ShutdownServices();
}
}