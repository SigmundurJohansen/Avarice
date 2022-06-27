#pragma once
#include "include/service_locator.h"
#include "core/game.h"


int main(int argc, char **argv)
{

    auto* theGame = Avarice::CreateGame();
    
    theGame->Run();

    delete theGame;

    return 0;
}