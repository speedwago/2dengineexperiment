#include "Game.h"
#include "GameLogic.h"
#include "GameRenderer.h"
#include "Renderer.h"
#include "Utils.h"
#include "Interpolator.h"
#include "Box2d/Box2d.h"

using namespace speed;


static double s_fLasttime;
static double s_fCurrDelta;
static Game* s_game;

Game::Game()
	:
        m_GameLogic()
	,m_Renderer()

{
}

Game::~Game()
{

}

void Game::Init(int w, int h)
{

	m_GameLogic.Init();
	m_Renderer.Init(&m_GameLogic,w,h);
	KLOG("w and h %d, %d", w , h);
	
	if (audio::init())
	{
        audio::playSFX("data/sfx_gamestart.wav");
		audio::playMusic("data/underwater_music_turbulance.ogg");
	}
	s_fLasttime = misc::GetTime() ;
	s_game = this;
}

void Game::Step()
{
	static double lastTimeP =0;
	static double s_fCurrDeltaP = 0;
	double fTime = (misc::GetTime()) ;
	s_fCurrDelta =  fTime - s_fLasttime ;
	s_fCurrDeltaP = fTime - lastTimeP;
	s_game->m_GameLogic.Update(1.0f/60.0f);
	//if (s_fCurrDeltaP >= 0.016 )
	{
		s_game->m_GameLogic.UpdateBox2d(1.0/60.0f);
		lastTimeP = fTime;
	}

	renderer::clear();
	s_game->m_Renderer.Render();

	renderer::swapBuffers();

	s_fLasttime = fTime ;
}
void Game::Run()
{
	do
	{
		Step();
	} // Check if the window was closed
	while( !renderer::getWindowExit() );

	Shutdown();
}

void Game::Shutdown()
{
	renderer::shutdown();
}


void on_surface_created(int w, int h)
{
	s_game = new Game();
	s_game->Init(w,h);
}

void on_surface_changed(int w, int h)
{
  s_game->m_Renderer.Init(&s_game->m_GameLogic,w,h);
}
void on_draw_frame()
{
	Game::Step();
}
