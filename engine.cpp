#include "engine.h"
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

#include "platform/glfw_window.h"
#include "service_locator.h"

void Engine::printHello()
{    
    std::cout << "hello from engine" << std::endl;
}


void Engine::Init()
{
    printHello(); 
    ServiceLocator::Provide(new CustomWindow());
}