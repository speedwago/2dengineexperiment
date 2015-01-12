#ifndef GameLogic_h__
#define GameLogic_h__

#include "GameTimer.h"
#include "TextureAtlas.h"
#include "Interpolator.h"

/********************************************************************
	created:	2013/09/08
	created:	8:9:2013   3:07
	filename: 	d:\maurizio\Documents\Visual Studio 2010\Projects\king_test\king_test\src\GameLogic.h
	file path:	d:\maurizio\Documents\Visual Studio 2010\Projects\king_test\king_test\src
	file base:	GameLogic
	file ext:	h
	author:		Maurizio Cerrato
	
	purpose:	Game logi function
*********************************************************************/

/// \brief this class handles the game play logic : swap piece if valid, handle input, check of grid matches. 

#include "Interpolator.h"
#include <stddef.h>
#include  "Box2d/Box2d.h"
#include "List.h"
#include "GameRenderer.h"
class GameTimer;


class UserData
{
public:
    bool m_contacting;
    int m_id;
	float m_fInitialRad;

    //in Ball class constructor
    UserData(int id)
		: m_id(id),
		m_contacting(false),
		m_fInitialRad(0.0f)

    {
    }

    //Ball class functions
    void startContact() 
    { 
        
        //if ( m_id != BORDER1 && m_id != BORDER2 && m_id != BORDER3 && m_id != CHAR_SWIM)
        {
            m_contacting = true; 
        }
    }
    void endContact() { m_contacting = false; }
};

struct PhyObject
{
	b2Body *pBody;
	LIST_LINK(PhyObject) m_listByObject;
	LIST_LINK(PhyObject) m_listByRopeObject;
};

class GameLogic
{
public :

   LIST_DECLARE(PhyObject, m_listByObject) m_PhysicsObjects;
   LIST_DECLARE(PhyObject, m_listByRopeObject) m_PhysicsRopeObjects;


   static const unsigned short GROUND_GROUP = 1;
   static const unsigned short PLAYER_GROUP = 1 << 1;
   static const unsigned short OBJECTS_GROUP = 1 << 2;
   static const unsigned short NONE_GROUP = 1 << 3;

   static const unsigned short MASK_GRUND = PLAYER_GROUP;
   
   b2Body* m_pLastCreated;

	/// \brief the game grid row
	static const int kBoardRow = 8;

	/// \brief the game grid Col
	static const int kBoardCol = 8;

	/// \brief horizontal spacing between pieces
	static const int kgridHSpacing = 38;

	/// \brief vertical spacing between pieces
	static const int kgridVSpacing = 38;

	/// \brief this is the point in witch the diamonds have to be drawn at first
	static const int kxOffetBoard = 340;

	/// \brief this is the point in witch the diamonds have to be drawn at first
	static const int kyOffetBoard = 120;

	/// \brief screen width
	static const  int kScreenW =640 ;
	
	/// \brief screen Height
	static const  int kScreenH = 960;

	/// \brief how long the falling animation have to last
	static const float kfFallingAnimationTime;

	/// \brief what kind of interpolation should have to use.
	static const InterpolatorFunction kpfFallingInterpolation;

	/// \brief time and funcion to use when we want to swap
	static const float kfSwapAnimationTime ;

	/// \brief time and funcion to use when we want to swap
	static const InterpolatorFunction kpfSwapInterpolation;

	/// \brief animation time for fading.
	static const float kfFadingAnimationTime ;

	/// \brief time over 
	static const float kGameTime;

	/// \brief callback for the timer. Fire a game over when done.

	/// \brief interpolato for fading
	static const InterpolatorFunction kpfFadingInterpolation;


	/// \brief callback for the timer. Fire a game over when done.
	static void TimeIsOver(void * pGameLogicInstance);
    static void  CreateRandom(void * pGameLogicInstance);
    static void IncrementFrame(void * pGameLogicInstance);

	void PickObject(b2Vec2 &wordpos);

    float cameraOffSet;
	float groundOffSet;
	float gameoverOffset;
    float bodyYpos;
    float lastbodyYpos;
	 int hitcount ;
	enum GameState
	{
		INTRO,
		PLAY,
		GAME_OVER
	};

	/// \brief structors
	GameLogic();
	
	/// brief deconstructor
	~GameLogic();

	/// \bief init the game logic
	void Init();

	/// \brief update 
	void Update(float dt);
	void UpdateBox2d(float dt);
	void CreateBox(float xScreen, float yScreen);
	b2Body* CreateCircle(float xScreen, float yScreen);
	b2Body* CreateCircleStatic(float xScreen, float yScreen);
	void CreateRope(b2Body &a, b2Body &b);
    b2Body* CreateBarrel(float xScreen, float yScreen);
    b2Body* CreateAnchor(float xScreen, float yScreen);
    b2Body* CreateBody(float xScreen, float yScreen);
    b2Body* CreatePropeller(float xScreen, float yScreen);
     b2Body* CreateCrate(float xScreen, float yScreen);
      b2Body* CreatePanel(float xScreen, float yScreen);
	/// \brief read user input
	void UpdateUserInput();


	/// \brief update the interpolator list
	void UpdateInterpolators(float dt);

	/// \brief update the timer list
	void UpdateTimers(float dt);

	/// \brief shutdown system
	void Shutdown();

	/// \brief gets the current state
	GameState GetCurrentState() const;


	/// \brief return the game tim
	float GetGameTime() const;


	enum PlayerState
	{
		SWIM,
		HIT,
		DEAD, 
	};

    PlayerState m_PlayerState;
	b2World world;
	UserData m_Char;
	UserData m_Ground;
	int frameid;
	GameTimer m_animTimer;
private:
	

	/// \brief gaming timer
	GameTimer m_Timer;

    GameTimer m_ObjectGenerator;

	/// \brief scheduled timer
	GameTimer m_CallbackTimer;

	public:
	/// \brief  current game state
	GameState m_State;


	/// \brief  next game state
	GameState m_NextState;

	/// \brief last clicked position
	int LastMouseX;
	
	/// \brief last clicked position
	int LastMouseY;

	bool m_GameOVerTriggered;

	int m_currentIndex;

	b2Vec2 gravity;

	// Construct a world object, which will hold and simulate the rigid bodies.
    public:


	


};
#endif // GameLogic_h__
