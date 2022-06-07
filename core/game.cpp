#include "game.h"

namespace Avarice
{
    Game::Game() : Game("Super Gamge") {}

    Game::Game(std::string _windowTitle) : m_Title(std::move(_windowTitle)), m_Running(true)
    {
    }

    Game::~Game()
    {
        Shutdown();
    }

    void Game::Run()
    {
        Initialize();
        /*
        // Create Framebuffers
        int w, h;
        glfwGetFramebufferSize(window, &w, &h);
        ImGui_ImplVulkanH_Window *wd = &g_MainWindowData;
        SetupVulkanWindow(wd, surface, w, h);
        */

        // run the game
        while (m_Running)
        {
            if (ServiceLocator::GetWindow()->Update())
            {
                m_Running = false;
                continue;
            }

            if (ServiceLocator::GetInputManager())
            {
                ServiceLocator::GetInputManager()->processInput();
            }
            // calculate delta time

            // update game state
            Update(0.0f);
            // update pyhsics

            // draw
            ServiceLocator::GetRenderer()->BeginFrame();
        }
    }

    void Game::Initialize()
    {
        // provide input manager
        ServiceLocator::Provide(new InputManager());
        // provide window
         ServiceLocator::Provide(new CustomWindow());
        //ServiceLocator::Provide(new SDLWindow());

        // open window
        ServiceLocator::GetWindow()->OpenWindow({.title = m_Title, .width = 800, .height = 600});

        // initialize input system

        // initialize renderer
        RendererSettings settings{
            .ApplicationName = m_Title};
        ServiceLocator::Provide(new VulkanRenderer(), settings);
        //ServiceLocator::Provide(new SDL2_Renderer(), settings);
    }

    void Game::Shutdown()
    {
        ServiceLocator::ShutdownServices();
    }

}