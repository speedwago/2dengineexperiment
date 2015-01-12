#ifndef Utils_h__
#define Utils_h__

#include "Box2d/Common/b2Math.h"
/********************************************************************
	created:	2013/09/04
	created:	4:9:2013   22:15
	filename: 	D:\maurizio\Documents\Visual Studio 2010\Projects\king_test\king_test\src\Utils.h
	file path:	D:\maurizio\Documents\Visual Studio 2010\Projects\king_test\king_test\src
	file base:	Utils
	file ext:	h
	author:		Maurizio Cerrato
	
	purpose:	utility stuff
*********************************************************************/

class TextureAtlas;

#ifdef __ANDROID__

typedef struct
{
    const long data_length;
    const void* data;
    const void* file_handle;
} FileData;



#endif

namespace speed
{
	namespace io
	{
		/// \brief load a file with an optional null characters
		char* loadFile(const char* pPath, int &sizee,bool addnull = true);

		/// \brief parse a texture packer sheet.
		void  parseSheet(const char* pPath, TextureAtlas *pAtlas);

		void  printText(const char* txt, TextureAtlas *pAtlas, float x, float y);

		/// \brief parse a texture packer sheet.
		void  parseTxtSheet(const char* pPathTxt, const char* pPathImg, TextureAtlas *pAtlas);

		void  parseTxtTextSheet(const char* pPathTxt, const char* pPathImg, TextureAtlas *pAtlas);

		///\ brief return true and the position of the mouse left click
		bool GetMouseLClickPosition(int *x,int *y);

		///\ brief return true and the position of the mouse left release
		bool GetMouseLReleasePosition(int *x,int *y);
		bool GetMouseRReleasePosition(int *x,int *y);

		bool GetMouseRClickPosition(int *x,int *y);

		void GetMousePosition(int *x,int *y);


        bool IsLeftPressed();
        bool IsRightPressed();
	}

	namespace collision
	{
		bool IsPointInsideBox(float x,float y, float minX, float minY, float maxX, float maxY );

		b2Vec2 GetScreenPos(const b2Vec2& box2dPos);
		
		b2Vec2 GetBox2dPos(const b2Vec2& screenPos);
        b2Vec2 GetBox2dWH(const b2Vec2& screenWH);

	}

	namespace audio
	{
		/// \brief initialize the lib
		bool init();

		/// \brief play music
		void playMusic(const char *pFile);

		/// \brief play an SFX
		/// todo: implement a version that uses id
		void playSFX(const char *pFile);

		/// \brief update audio
		void update();

	}

	namespace misc
	{

		/// \brieg get time in seconds
		float GetTime();
	}
}

#ifdef __ANDROID__
void   on_touch_press(float x, float y);
void   on_touch_drag(float x, float y);
#endif

#endif // Utils_h__
