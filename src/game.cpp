#include "game.h"
#include "service_locator.h"
#include "../platform/glfw_window.h"

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
    ServiceLocator::GetWindow()->OpenWindow();

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

    }
}

void Game::initializeServices()
{
    // provide window
    ServiceLocator::Provide(new CustomWindow());

    // initialize input system

    // initialize renderer 

}

void Game::shutdownServices()
{
    ServiceLocator::ShutdownServices();
}