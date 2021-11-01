#pragma once
#include "src/game.h"
#include "src/service_locator.h"
#include "game.h"


int main(int argc, char **argv)
{

    auto* theGame = Avarice::CreateGame();
    
    theGame->Run();

    delete theGame;
}