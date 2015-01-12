#include "TextureAtlas.h"
#include "GameRenderer.h"
#include "common.h"
#include "GameLogic.h"
#include "Utils.h"
#include "Renderer.h"
#include "Box2d/box2d.h"
using namespace speed;
extern b2Body* bodyChar;
static const float kBackGroundOfffset = 377.0f;
static const float kTimerX = 100.0f;
static const float kTimerY = 450.0f;
static const float kTimerSize = 30.0f;


GameRenderer::GameRenderer()
	:
         m_shader(0)
        ,m_pBoardAtlas(NULL)
	,m_pBoard(NULL)
        ,m_TextTexture(0)
	,m_TimerXPos(kTimerX)
	,m_TimerYPos(kTimerY)
        ,m_TimerSize(kTimerSize)
 {
 }

GameRenderer::~GameRenderer()
{
	delete m_pBoardAtlas;
}

void GameRenderer::Init( GameLogic* pLogic,  int screenw,  int screenh)
{
	m_pBoard = pLogic;
	if (!renderer::init(screenw, screenh))
	{
		KLOG("an error has occurred during Renderer init\n");
		return ;
	}

	m_pBoardAtlas = new TextureAtlas();

	m_shader  = speed::renderer::loadShaders("data/SimpleVertexShader.v","data/SimpleFragmentShader.f");
	io::parseSheet("data/the_only_way_is_up_0.xml",m_pBoardAtlas);

	renderer::setOrtho(0, static_cast<float>(screenw), 0, static_cast<float>(screenh), -10.0f,10.0f);
	renderer::setClearColor(.8f, .3f, .4f, .0f);

	
	int w, h;


	m_TextTexture =  speed::renderer::loadTexture("data/exportedfont3.png",w,h);


}

void GameRenderer::RenderPhyObjects()
{
	PhyObject *p = m_pBoard->m_PhysicsObjects.Head();
	while (p)
	{

		b2Vec2 pixels = collision::GetScreenPos(p->pBody->GetPosition());
		UserData *pUser = (UserData*)p->pBody->GetUserData();
		// if (!(pUser->m_contacting))
		{
			m_pBoardAtlas->AddInstance(pUser->m_id, pixels.x, pixels.y/*+m_pBoard->cameraOffSet*/, 1.0f, -p->pBody->GetAngle() + pUser->m_fInitialRad);


		}
		if (p)
			p = p->m_listByObject.Next();
	}
}


void GameRenderer::RenderHud()
{
	float fTime =m_pBoard->GetGameTime()/GameLogic::kGameTime;
	float fTimeI =1.0f-fTime;
	Sprite s = m_pBoardAtlas->GetSprite(AIR_BG);
	m_pBoardAtlas->AddInstance(AIR_BG, GameLogic::kScreenW/2 , 100 + s.h/2, 1.0f);
	s = m_pBoardAtlas->GetSprite(AIR_TEXT);
	m_pBoardAtlas->AddInstance(AIR_TEXT, GameLogic::kScreenW/2 , 100 - s.h/2, 1.0f);
	s = m_pBoardAtlas->GetSprite(AIR_FILL);
	m_pBoardAtlas->AddInstance(AIR_FILL, GameLogic::kScreenW/2 -s.w/2 *(1.0f-fTime), 100 + s.h/2, 1.0f,0.0f,fTime,1.0f);
}

void GameRenderer::Render()
{

	m_pBoardAtlas->Flush();

	float fTime =m_pBoard->GetGameTime()/GameLogic::kGameTime;
	float fTimeI =1.0f-fTime;
	Sprite s = m_pBoardAtlas->GetSprite(BACKGROUND);
	m_pBoardAtlas->AddInstance(BACKGROUND, s.w/2 , GameLogic::kScreenH - s.h/2, 1.0f);
    s = m_pBoardAtlas->GetSprite(BACKGROUND2);
    m_pBoardAtlas->AddInstance(BACKGROUND2, s.w/2 , GameLogic::kScreenH -s.h/2 ,1.0f-fTime);
      s = m_pBoardAtlas->GetSprite(BORDER1);
    m_pBoardAtlas->AddInstance(BORDER1, s.w/2 ,  GameLogic::kScreenH - s.h/2 +m_pBoard->groundOffSet, 1.0f);

    s = m_pBoardAtlas->GetSprite(BORDER2);
    m_pBoardAtlas->AddInstance(BORDER2, s.w/2 -s.w *fTimeI,  GameLogic::kScreenH - s.h/2 +m_pBoard->cameraOffSet, 1.0f);
	s = m_pBoardAtlas->GetSprite(BORDER2);
	m_pBoardAtlas->AddInstance(BORDER2, s.w/2 -s.w *fTimeI,  GameLogic::kScreenH - s.h/2 -GameLogic::kScreenH+m_pBoard->cameraOffSet, 1.0f);
	s = m_pBoardAtlas->GetSprite(BORDER2);
	m_pBoardAtlas->AddInstance(BORDER2, s.w/2 -s.w *fTimeI,  GameLogic::kScreenH - s.h/2 +GameLogic::kScreenH+m_pBoard->cameraOffSet, 1.0f);

    s = m_pBoardAtlas->GetSprite(BORDER3);
    m_pBoardAtlas->AddInstance(BORDER3,GameLogic::kScreenW-  s.w/2+s.w *fTimeI ,  GameLogic::kScreenH - s.h/2 +m_pBoard->cameraOffSet, 1.0f);
	s = m_pBoardAtlas->GetSprite(BORDER3);
	m_pBoardAtlas->AddInstance(BORDER3,GameLogic::kScreenW-  s.w/2 +s.w *fTimeI,  GameLogic::kScreenH - s.h/2 -GameLogic::kScreenH+m_pBoard->cameraOffSet, 1.0f);
	s = m_pBoardAtlas->GetSprite(BORDER3);
	m_pBoardAtlas->AddInstance(BORDER3,GameLogic::kScreenW-  s.w/2 +s.w *fTimeI,  GameLogic::kScreenH - s.h/2 +GameLogic::kScreenH +m_pBoard->cameraOffSet, 1.0f);

	const Sprite &spr = m_pBoardAtlas->GetSprite(CHAR_SWIM);  
	b2Vec2 pixels = collision::GetScreenPos(bodyChar->GetPosition()); 
    float fAngle = 0; 
        b2Vec2 Vel = bodyChar->GetLinearVelocity();
        Vel.Normalize();
        b2Vec2 up =b2Vec2(0,1);
        float dot = sqrt(up.x*Vel.x+up.y*Vel.y); 
            fAngle = acos(fAngle);

	int startFrame =0;
	switch (m_pBoard->m_PlayerState)
	{
		case GameLogic::SWIM:
			startFrame  =CHAR_SWIM;
			break;
		case GameLogic::HIT:
			startFrame = HIT0;
			break;
		case GameLogic::DEAD:
			startFrame = DIE0;
			break;
		default:

			startFrame  =CHAR_SWIM;
			break;


	}

	KASSERT(m_pBoard->frameid>=0,"frame error");
	if (m_pBoard->m_State != GameLogic::INTRO)
	{
	m_pBoardAtlas->AddInstance(startFrame+ m_pBoard->frameid,  pixels.x, pixels.y/*+m_pBoard->cameraOffSet*/, 1.0,0.0);
	RenderPhyObjects();
	}
	//draw all the obhect in the list.

    if (m_pBoard->GetCurrentState() == GameLogic::GAME_OVER)
    {
		m_pBoard->gameoverOffset+=4.00;
		float pos;
		Sprite s = m_pBoardAtlas->GetSprite(AIR_END);
        if (-200+m_pBoard->gameoverOffset< GameLogic::kScreenH/2)
			pos = -200 +m_pBoard->gameoverOffset;
		else
			pos =GameLogic::kScreenH/2;
		m_pBoardAtlas->AddInstance(AIR_END,  GameLogic::kScreenW*0.5,pos, 1.0,0.0);
    }


	 if (m_pBoard->GetCurrentState() == GameLogic::INTRO)
    {

		m_pBoardAtlas->AddInstance(AIR_LOGO,  GameLogic::kScreenW*0.5,GameLogic::kScreenH/2, 1.0,0.0);
    }

	RenderHud();
	m_pBoardAtlas->Draw(m_shader);
	// draw the timer



	if (m_pBoard->GetCurrentState() == GameLogic::GAME_OVER)
	{
	//	speed::renderer::bindTexture(m_shader,m_TextTexture);
		//speed::renderer::drawText("GAME OVER", GameLogic::kScreenW*0.5-200.0,GameLogic::kScreenH*0.5, 60,m_shader);
	}
	else
	{
		//sprintf(m_szTimerStr,"%2.0f",m_pBoard->GetGameTime());
	//	speed::renderer::bindTexture(m_shader,m_TextTexture);
	//	speed::renderer::drawText("2", m_TimerXPos,m_TimerYPos,m_TimerSize,m_shader);
	}
	//DebugRenderBoard();

}

