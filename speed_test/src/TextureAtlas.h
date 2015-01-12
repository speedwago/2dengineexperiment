#ifndef TextureAtlas_h__
#define TextureAtlas_h__

/********************************************************************
	created:	2013/09/05
	created:	5:9:2013   20:36
	filename: 	D:\maurizio\Documents\Visual Studio 2010\Projects\king_test\king_test\src\TextureAtlas.h
	file path:	D:\maurizio\Documents\Visual Studio 2010\Projects\king_test\king_test\src
	file base:	TextureAtlas
	file ext:	h
	author:		Maurizio Cerrato
	
	purpose:	Texture atlases instancing
*********************************************************************/

#include "Renderer.h"
struct TextureAtlasData;

/// \holds data for a sprite
struct Sprite
{
	unsigned int assedID;
	float x;
	float y;
	float theta;
	float fAlpha;
	float w;
	float h;
};
/// \brief TextureAtlas handle the textures and their instances 
/// , keeping track of them and upload them to the GPU.
class TextureAtlas
{

public:
	/// brief max numer of instance supported
	static const int kMaxInstances = 1024;

	/// \biref define the maximon number of sprite per atlas
	static const int kMaxSpritePerAtlas = 256;

	/// \biref structor
	TextureAtlas();
	
	/// brief\destructor
	~TextureAtlas();

	/// \brief add a texture instance to the render queue.
	void AddInstance(const speed::renderer::Vert *pVert, unsigned int numpsprite);

	int GetRegionFromName(char *pName);

	Sprite GetSprite(int id);

	/// \brief add one istance taken from the given region
	void AddInstance(unsigned int region, float x, float y, float faAlpha,float ftheta= 0.0f,float sx= 1.0f,float sy= 1.0f);

	/// \brief add a texture region to atlas
	void AddRegion(float u0, float v0, float u1, float v1, int w, int h,const char *pName);

	/// \brief load textture
	void LoadTexture(const char *pData, int &w, int &h);

	/// \brief issue draw
	void Draw(unsigned int shader);

	/// \flush the data
	void Flush();

	int GetNumInstances();

private:

	// pimpl 
	TextureAtlasData* m_pData;

};
#endif // TextureAtlas_h__