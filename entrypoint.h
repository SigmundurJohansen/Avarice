#pragma once
#include "include/engine.h"
#include "include/game.h"
#include "include/service_locator.h"


int main(int argc, char **argv)
{

    //auto* theGame = CreateGame();
    Engine::printHello();

    ServiceLocator::GetWindow()->OpenWindow();

    //while(!ServiceLocator::GetWIndow()->Update())    {    }
}