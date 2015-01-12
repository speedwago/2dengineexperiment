#ifndef Game_h__
#define Game_h__

/********************************************************************
	created:	2013/09/10
	created:	10:9:2013   0:19
	filename: 	D:\maurizio\Documents\Visual Studio 2010\Projects\king_test\king_test\src\Game.h
	file path:	D:\maurizio\Documents\Visual Studio 2010\Projects\king_test\king_test\src
	file base:	Game
	file ext:	h
	author:		Maurizio Cerrato
	
	purpose:	Game application
*********************************************************************/

#include "GameTimer.h"
#include "GameLogic.h"
#include "GameRenderer.h"
/// \brief Game is the main entry point of the game. It contains the main loop and handles gameplay and rendering
class Game
{
public:

	/// \brief default constructor
	Game();

	/// \brief deconstructor
	~Game();

	/// \brief initialize the app
	void Init(int w , int h);
	
	/// \brief the main loop
	void Run();

	 static void Step();

	/// \brief tenrminate the app
	void Shutdown();

	/// \brief callback called by the timer
	static void UpdateStep(void *pData);

public:
	
	/// \brief pointer to the game logic class
	GameLogic m_GameLogic;

	/// \brief pointer to the renderclass
	GameRenderer m_Renderer;


};

void on_surface_created(int w , int h);
void on_surface_changed(int w , int h);
void on_draw_frame();
#endif // Game_h__
