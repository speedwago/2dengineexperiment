#ifndef GameRenderer_h__
#define GameRenderer_h__

/********************************************************************
	created:	2013/09/08
	created:	8:9:2013   1:45
	filename: 	d:\maurizio\Documents\Visual Studio 2010\Projects\king_test\king_test\src\GameRenderer.h
	file path:	d:\maurizio\Documents\Visual Studio 2010\Projects\king_test\king_test\src
	file base:	GameRenderer
	file ext:	h
	author:		Maurizio Cerrato
	
	purpose:	Rendering stuff for game
*********************************************************************/

class TextureAtlas;
class GameLogic;



/// \brief this class handle the game grid from a rendering point of view
enum AssetId
{
	BACKGROUND ,
    BACKGROUND2,

    BODY,
    BODY1,
    BORDER1,
    BORDER2,
    BORDER3,
    BTNRESTART,
    BTNSTART,
    CHAINM,
    CHAINR,
    DIE0,
    DIE1,
    DIE2,
    DIE3,
    DIE4,
    HIT0,
    HIT1,
    STRUGGLE0,
    STRUGGLE1,
    STRUGGLE2,
    CHAR_SWIM,
    CHAR_SWIM1,
    CHAR_SWIM2,
    CHAR_SWIM3,
    CHAR_SWIM4,
    CHAR_SWIM5,
    CHAR_SWIM6,
    CHAR_SWIM7,
    AIR_BG,
	AIR_FILL,
    AIR_TEXT,
    AIR_END,
    AIR_LOGO,
    ANCHOR,
    BARELL0,
    BARELL1,
    CASE0,
    CASE1,
	CASE2,
    CRATE0,
    CRATE1,
    PANEL0,
    POPELLER,



    NUM ,
};

class GameRenderer
{
public:

	//static const int kxOffetBoard =
	// \brief structor
	GameRenderer();
	
	/// \brief destructor
	~GameRenderer();

	/// \brief initialize the app;
	void Init( GameLogic* pLogic,  int screenw,  int screenh);

	void Shutdown();

	/// \ brief render the grid
	void Render();

	void RenderPhyObjects();


	void RenderHud();




	///\ brief shader variable
	unsigned int m_shader;

public:

	/// \brief non copyable
	GameRenderer(const GameRenderer &d){};

	/// \brief non copyable
	GameRenderer& operator=(GameRenderer &){return *this;};

	/// \brief get piece asset
	/// \brief texture atlas that contains the board asstes

	/// \brief texture atlas
	TextureAtlas* m_pBoardAtlas;

	/// \brief the game boars
	 GameLogic* m_pBoard;


	/// \brief timer string
	unsigned int m_TextTexture;

	/// \brief timer x pos
	float  m_TimerXPos;

	/// \brief timer y pos
	float  m_TimerYPos;

	/// \brief timer size.
	float  m_TimerSize;

        /// \brief string that contain the timer to render
        char m_szTimerStr[5];

};
#endif // GameRenderer_h__
