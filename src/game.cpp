#include "game.h"
#include "service_locator.h"
#include "../platform/custom_window.h"
#include "rendering/vulkan/vulkan_renderer.h"

namespace Avarice{
Game::Game() : Game("Super Gamge")
{
    
}

Game::Game(std::string windowTitle) : m_Title(std::move(windowTitle)), m_Running(true)
{
    initializeServices();
}

Game::~Game()
{
    shutdownServices();
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

        // calculate delta time

        // update game state
        Update(0.0f); 
        // update pyhsics

        // draw
        ServiceLocator::GetRenderer()->RenderFrame();
    }
}

void Game::initializeServices()
{
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

void Game::shutdownServices()
{
    ServiceLocator::ShutdownServices();
}
}