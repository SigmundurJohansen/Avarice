#include "game.h"
#include "service_locator.h"
#include "../platform/glfw_window.h"
#include "rendering/vulkan/vulkan_renderer.h"

namespace Avarice{
Game::Game() : Game("Super Gamge")
{
    
}

Game::Game(std::string windowTitle) : mTitle(std::move(windowTitle)), mRunning(true)
{
    initializeServices();
}

Game::~Game()
{
    shutdownServices();
}

void Game::Run()
{
    // open window
    ServiceLocator::GetWindow()->OpenWindow({.title = mTitle, .width = 800, .height = 600});

    //run the game
    while(mRunning)
    {
        if(ServiceLocator::GetWindow()->Update())
        {
            mRunning = false;
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

    // initialize input system

    // initialize renderer 
    RendererSettings settings
    {
        .ApplicationName = mTitle
    };
    ServiceLocator::Provide(new VulkanRenderer(), settings);
}

void Game::shutdownServices()
{
    ServiceLocator::ShutdownServices();
}
}