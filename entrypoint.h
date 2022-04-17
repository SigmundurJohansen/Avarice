#pragma once
#include "include/service_locator.h"
#include "game.h"


int main(int argc, char **argv)
{

    auto* theGame = Avarice::CreateGame();
    theGame->Run();

    delete theGame;
}