#pragma once
#include "src/engine.h"
#include "src/game.h"
#include "src/service_locator.h"
#include "game.h"


int main(int argc, char **argv)
{

    auto* theGame = CreateGame();
    Engine::Init();

    ServiceLocator::GetWindow()->OpenWindow();

    while(!ServiceLocator::GetWindow()->Update())    {    }
}