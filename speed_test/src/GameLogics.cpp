#include "GameLogic.h"
#include "Utils.h"
#include <math.h>
#include <stdlib.h>
#include "common.h"
#include <time.h>

#include <string.h>
#include  "Box2d/Box2d.h"
#include "GameRenderer.h"

#ifdef EMSCRIPTEN
#include "html5.h"
#endif
/// \brief structors
using namespace speed;



static float fUpforce =200.0;
/// initialize animations constant
const float GameLogic::kfFallingAnimationTime = 1.0f;
const InterpolatorFunction GameLogic::kpfFallingInterpolation = ElasticIn;

const float GameLogic::kfSwapAnimationTime = 0.5f;
const InterpolatorFunction GameLogic::kpfSwapInterpolation = QuadInOut;

const float GameLogic::kfFadingAnimationTime = 0.5f;
const InterpolatorFunction GameLogic::kpfFadingInterpolation = CubeInOut;

const float GameLogic::kGameTime = 60.0f;

const int GameLogic::laneYPos[3] = { GameLogic::kScreenH *0.1, GameLogic::kScreenH *0.5, GameLogic::kScreenH *0.8 };
static GameLogic* s_pGame;
static int touchidnew;

#ifdef EMSCRIPTEN
EM_BOOL touch_callback(int eventType, const EmscriptenTouchEvent *e, void *userData)
{
	if (s_pGame->m_State == GameLogic::INTRO)
	{

		s_pGame->Init();
		s_pGame->m_State = GameLogic::PLAY;
	    return 0;
	}
	const EmscriptenTouchPoint *t = &e->touches[0];
	s_pGame->LastMouseX =  t->canvasX;
	s_pGame->LastMouseY =  t->canvasY;
	touchidnew = t->identifier;
	return 0;
}

EM_BOOL touch_callback_move(int eventType, const EmscriptenTouchEvent *e, void *userData)
{
	int xSwipe,ySwipe;
	const EmscriptenTouchPoint *t = &e->touches[0];
	xSwipe = t->canvasX;
	ySwipe = t->canvasY;
	int xoffset, yoffset;
	if (s_pGame->m_State != GameLogic::INTRO)
	{

		//check if the swipe is long enough by checking the leng of the fidtance vectors
		if (abs(s_pGame->LastMouseX - xSwipe) *abs(s_pGame->LastMouseX - xSwipe) + abs(s_pGame->LastMouseY - ySwipe) *abs(s_pGame->LastMouseY - ySwipe) > 100 * 100 && touchidnew>=0)
		{
			//find the direction
			if (abs(xSwipe - s_pGame->LastMouseX) > abs(ySwipe - s_pGame->LastMouseY))
			{
				xoffset = sgn<int>(xSwipe - s_pGame->LastMouseX);
				if (xoffset < 0)
				{


					if (s_pGame->player.m_PlayerState == Player::SWIM)
					{

						s_pGame->m_GiftsObjects.push();
						GiftData& d = s_pGame->m_GiftsObjects.getAt(s_pGame->m_GiftsObjects.getSize() - 1);
						d.pos = s_pGame->player.pos;
						d.spriteId = CRATE0;
						d.active = true;
						s_pGame->m_InterpolatorObjects.push();
						Interpolator &inter = s_pGame->m_InterpolatorObjects.getAt(s_pGame->m_InterpolatorObjects.getSize() - 1);
						inter.Init(speed::misc::GetTime(), 0.5f, GameLogic::kpfSwapInterpolation, 0, &d.pos.x, GameLogic::DeactivateGift, &d);
						inter.Launch();


						//change the state to launching
						s_pGame->player.m_PlayerState = Player::LAUNCHING;
						//set timed callback to make it go back to swim
						s_pGame->m_CallbackTimer.Init(0.25, NORMAL, GameLogic::SetStateIdle);
					}
				}
			}
			else
			{
				yoffset = sgn<int>(ySwipe - s_pGame->LastMouseY);

				//	if (player.curlane + yoffset >= 0 && player.curlane + yoffset < 3)
				{
					s_pGame->player.nextlane = s_pGame->player.curlane + yoffset;
					s_pGame->m_PlayerInterpolator.Init(speed::misc::GetTime(), GameLogic::kfSwapAnimationTime, GameLogic::kpfSwapInterpolation,s_pGame->player.pos.y + (yoffset * 200), &s_pGame->player.pos.y, 0, 0);
					s_pGame->m_PlayerInterpolator.Launch();
					//switch player pos


				}
			}

			s_pGame->LastMouseX =  t->canvasX;
			s_pGame->LastMouseY =  t->canvasY;
			touchidnew = -1;
		}
	}



	

	return 0;
}
#endif

GameLogic::GameLogic()
	:
m_GameOVerTriggered(false)
{
#ifdef EMSCRIPTEN
	emscripten_set_touchstart_callback(0, 0, 1, touch_callback);

	//emscripten_set_touchend_callback(0, 0, 1, touch_callback);
	 emscripten_set_touchmove_callback(0, 0, 1, touch_callback_move);
	
	 //emscripten_set_touchcancel_callback(0, 0, 1, touch_callback);
#endif
}

void GameLogic::Init()
{
    frameid=0;
	hitcount =0;
	s_pGame = this;

	player.m_PlayerState = Player::SWIM;
	// random seed for release builds
#ifndef _DEBUG
	srand (time(NULL));
#endif


	// init sprites assets ans positionning in the grid
	// inittiliaze the time over counter
	m_Timer.Init(kGameTime,NORMAL,GameLogic::TimeIsOver, NULL);
    m_ObjectGenerator.Init(5.0,LOOP,GameLogic::CreateRandom,NULL);
    m_animTimer.Init(0.15,LOOP,GameLogic::IncrementFrame,NULL);

	m_GameOVerTriggered = false;
	
	LastMouseY= 0;
	LastMouseX = 0;

	player.pos = collision::vec2(GameLogic::kScreenW-50, GameLogic::kScreenH / 2);
	//m_GiftsObjects.DeleteAll();
	m_GiftsObjects.reset();
	m_InterpolatorObjects.reset();

	// Define the gravity vector.




    cameraOffSet = 0;
	groundOffSet =0;
	gameoverOffset = 0;

	bodyYpos = .0;
	lastbodyYpos = .0;

	m_State = INTRO;
	player.curlane = 0;
	player.nextlane = 0;
}



void GameLogic::Shutdown()
{
}

GameLogic::~GameLogic()
{
}

void GameLogic::Update(float dt)
{
	UpdateUserInput();
   if (m_State == GameLogic::INTRO)
	   return;
    UpdateInterpolators(dt);

	UpdateTimers(dt);

}


void GameLogic::LaneSwapFinished(void * pGameLogicInstance)
{
	swap(s_pGame->player.nextlane, s_pGame->player.curlane);
}

void GameLogic::UpdateUserInput()
{
	int xSwipe, ySwipe;
	int xoffset = 0;
	int yoffset = 0;
	int xMouse, yMouse;
   if (m_State == INTRO)
   {
	 
		bool bClicked = io::GetMouseLClickPosition(&xMouse, &yMouse);
		if (bClicked)
		{
			Init();
			m_State = PLAY;
			return;
		}
	   return;
   }

  
	if (cameraOffSet >=GameLogic::kScreenH)
		cameraOffSet =0;

    cameraOffSet += 0.55;//bodyYpos-lastbodyYpos;
	groundOffSet+= 0.55;
	int x3,y3;
	io::GetMousePosition(&x3,&y3);


	// check if we swipe on block
	if (io::GetMouseLReleasePosition(&xSwipe, &ySwipe))
	{
		//check if the swipe is long enough by checking the leng of the fidtance vectors
		if (abs(LastMouseX - xSwipe) *abs(LastMouseX - xSwipe) + abs(LastMouseY - ySwipe) *abs(LastMouseY - ySwipe) > 20 * 20)
		{
			//find the direction
			if (abs(xSwipe - LastMouseX) > abs(ySwipe - LastMouseY))
			{
				xoffset = sgn<int>(xSwipe - LastMouseX);
				if (xoffset < 0)
				{


					if (player.m_PlayerState == Player::SWIM)
					{

						m_GiftsObjects.push();
						GiftData& d = m_GiftsObjects.getAt(m_GiftsObjects.getSize() - 1);
						d.pos = player.pos;
						d.spriteId = CRATE0;
						d.active = true;
						m_InterpolatorObjects.push();
						Interpolator &inter = m_InterpolatorObjects.getAt(m_InterpolatorObjects.getSize() - 1);
						inter.Init(speed::misc::GetTime(), 0.5f, kpfSwapInterpolation, 0, &d.pos.x, GameLogic::DeactivateGift, &d);
						inter.Launch();
						

						//change the state to launching
						player.m_PlayerState = Player::LAUNCHING;
						//set timed callback to make it go back to swim
						m_CallbackTimer.Init(0.25, NORMAL, GameLogic::SetStateIdle);
					}
				}
			}
			else
			{
				yoffset = sgn<int>(ySwipe - LastMouseY);

				if (player.curlane + yoffset >= 0 && player.curlane + yoffset < 3)
				{
					player.nextlane = player.curlane + yoffset;
					m_PlayerInterpolator.Init(speed::misc::GetTime(), kfSwapAnimationTime, kpfSwapInterpolation, laneYPos[player.nextlane], &player.pos.y, GameLogic::LaneSwapFinished, 0);
					m_PlayerInterpolator.Launch();
					//switch player pos


				}
			}
		}
	}

	LastMouseX =x3;
	LastMouseY = y3;


    int xMouse2, yMouse2;

	if (m_State == GAME_OVER)
	{
		 if (-200+gameoverOffset>= GameLogic::kScreenH/2)
		 {
		// if game over restart when click
		int xMouse, yMouse;
		bool bClicked = io::GetMouseLClickPosition(&xMouse, &yMouse);
		if (bClicked)
		{
			Init();
			m_State = PLAY;
			return;
		}
		}
	}

	//if (m_State != WAIT_FOR_USER)
		//return;

	if (m_GameOVerTriggered)
	{
		m_State = GAME_OVER;
		player.m_PlayerState = Player::DEAD;
		audio::playSFX("data/human_die.wav");
		s_pGame->frameid =0;
		// if game over restart when click
		int xMouse, yMouse;
		bool bClicked = io::GetMouseLClickPosition(&xMouse, &yMouse);
		if (bClicked)
		{
//			Init();
			return;
		}
	}



//	bool bClicked = io::GetMouseLClickPosition(&xMouse, &yMouse);
	// check first if we clicked in a valid game area
	//update last state
	//io::GetMouseLClickPosition(&xMouse, &yMouse);
}


void GameLogic::SetStateIdle(void * pGameLogicInstance)
{
	s_pGame->player.m_PlayerState = Player::SWIM;
}

void GameLogic::DeactivateGift(void * pGift)
{
	GiftData * p = (GiftData *)pGift;
	p->active = false;
}
void GameLogic::IncrementFrame(void * pGameLogicInstance)
{
	switch (s_pGame->player.m_PlayerState)
	{
	case Player::SWIM :
    s_pGame->frameid =(s_pGame->frameid+1) % 8;
	break;
	case Player::HIT:
		s_pGame->frameid=0;
		s_pGame->hitcount++;
		if (s_pGame->hitcount > 4)
		{
			s_pGame->frameid =1;
			//s_pGame->m_PlayerState = SWIM;
		}

		if (s_pGame->hitcount > 8)
		{
			s_pGame->hitcount =0;
			s_pGame->player.m_PlayerState = Player::SWIM;
		}


		break;

	case Player::DEAD:
		s_pGame->frameid=0;
		s_pGame->hitcount++;
		if (s_pGame->hitcount > 4)
		{
			s_pGame->frameid =1;
			//s_pGame->m_PlayerState = SWIM;
		}

		if (s_pGame->hitcount > 8)
		{
			s_pGame->frameid =1;
		}

		if (s_pGame->hitcount > 12)
		{
			s_pGame->frameid =2;
		}
		if (s_pGame->hitcount > 16)
		{
			s_pGame->frameid =3;
		}


		break;

	}
}
void GameLogic::CreateRandom(void * pGameLogicInstance)
{
	int lane = rand() % 3;
	collision::vec2 posElf(0, laneYPos[lane]);
	s_pGame->m_ElfObjects.push();
	ElfData& elf = s_pGame->m_ElfObjects.getAt(s_pGame->m_ElfObjects.getSize() -1);
	elf.pos = posElf;
	elf.active = true;
	elf.spriteId = BODY;
	//put elf generator here
	s_pGame->m_InterpolatorObjects.push();
	Interpolator &inter = s_pGame->m_InterpolatorObjects.getAt(s_pGame->m_InterpolatorObjects.getSize() - 1);
	inter.Init(speed::misc::GetTime(), 15.0 + rand() % 5, Linear, GameLogic::kScreenW, &elf.pos.x, GameLogic::DeactivateGift, &elf);
	inter.Launch();
    
}




void GameLogic::UpdateInterpolators(float dt)
{
	m_PlayerInterpolator.Update(dt);

	for (int i = 0; i < m_InterpolatorObjects.getSize(); ++i)
	{
		m_InterpolatorObjects.getAt(i).Update(dt);
	}

	// pop unused
	if (!m_InterpolatorObjects.isEmpty() && m_InterpolatorObjects.getAt(0).IsTerminated())
	{
		m_InterpolatorObjects.pop();
	}
}

void GameLogic::UpdateTimers(float dt)
{
		m_CallbackTimer.Update(dt);
		m_Timer.Update(dt);
        m_ObjectGenerator.Update(dt);
        m_animTimer.Update(dt);
}

float GameLogic::GetGameTime() const
{
	return m_Timer.GetRemainingTime();
}


GameLogic::GameState GameLogic::GetCurrentState() const
{
	return m_State;
}


void GameLogic::TimeIsOver(void * pGameLogicInstance)
{

	s_pGame->m_GameOVerTriggered = true;

	KLOG("time over called!\n");
}