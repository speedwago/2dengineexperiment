// Include standard headers
//#define EMSCRIPTEN
#ifndef __ANDROID
#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "Utils.h"
#include "Game.h"
#ifdef EMSCRIPTEN
#include "emscripten/emscripten.h"
#endif
using namespace speed;

int main( void )
{
	
	Game game;
	game.Init(GameLogic::kScreenW,GameLogic::kScreenH);
#ifndef EMSCRIPTEN
	game.Run();
#else
	emscripten_set_main_loop(Game::Step, 0, 1);
#endif 
	return 0;
}
#endif//#ifndef __ANDROID
