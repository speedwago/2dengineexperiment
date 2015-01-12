#include "TextureAtlas.h"
#include "common.h"
#include "Renderer.h"
#include "LinearAllocator.h"
#include <memory>

/// \brief a region of the texture atlas
struct Region
{
	float u0;
	float v0;
	float u1;
	float v1;
	int w;
	int h;
	char name[64];
};

/// brief pimpl : define internal state here to make the interface more clear and avoid depencies
struct TextureAtlasData
{

	TextureAtlasData()
		:
	          numInstance(0)
	          ,numRegion(0)
                  ,vBuffer(sizeof(speed::renderer::Vert) * TextureAtlas::kMaxInstances * 4)
		  ,iBuffer(sizeof(unsigned int) * TextureAtlas::kMaxInstances * 6)


	{
	}
	
	~TextureAtlasData()
	{
		speed::renderer::destroyBuffer(iBufferId);
		speed::renderer::destroyBuffer(vBufferId);
		speed::renderer::destroyTexture(vTextureId);
	}
	
	unsigned int numInstance;
	unsigned int numRegion;
	LinearAllocator vBuffer;
	LinearAllocator iBuffer;
	unsigned int  iBufferId;
	unsigned int  vBufferId;
	unsigned int  vShaderId;
	unsigned int  vTextureId;
	Region aRegions[TextureAtlas::kMaxSpritePerAtlas];
};

TextureAtlas::TextureAtlas()
{
	m_pData = new TextureAtlasData();
	m_pData->vBufferId = speed::renderer::createVertexBuffer(
		(const speed::renderer::Vert *)m_pData->vBuffer.GetBase(),sizeof(speed::renderer::Vert) * TextureAtlas::kMaxInstances * 4);

	m_pData->iBufferId = speed::renderer::createIndexBuffer(
		(const unsigned short*)m_pData->iBuffer.GetBase(),sizeof(unsigned short) * TextureAtlas::kMaxInstances * 6);
	
}

TextureAtlas::~TextureAtlas()
{

	delete m_pData;
}

int TextureAtlas::GetRegionFromName(char *pName)
{
	int result = -1;
	for (unsigned int i=0 ; i < m_pData->numRegion;++i)
	{
		if (strcmp(pName,m_pData->aRegions[i].name)==0)
		{
			result = i;
			break;
		}
	}

		return result;
}

Sprite TextureAtlas::GetSprite(int id)
{
	Region m = m_pData->aRegions[id];
	Sprite s = { id, .0f, .0f, .0f, .0f, (float)m.w, (float)m.h };
	return s;
}

	int TextureAtlas::GetNumInstances()
	{
		return m_pData->numInstance;
	}

void TextureAtlas::AddInstance(const speed::renderer::Vert *pVert, unsigned int numpsprite)
{
	KASSERT(m_pData, "error");
	KASSERT(numpsprite + m_pData->numInstance < kMaxInstances, "error");
	speed::renderer::Vert* mem = (speed::renderer::Vert*)m_pData->vBuffer.AllocateBlock(sizeof(speed::renderer::Vert) * numpsprite * 4);
	memcpy(mem,pVert, sizeof(speed::renderer::Vert) * numpsprite * 4);
	unsigned short* memIndex = (unsigned short*)m_pData->iBuffer.AllocateBlock(sizeof(unsigned short) * numpsprite * 6);

	for (unsigned int i = 0; i < numpsprite; ++i)
	{
		int index = m_pData->numInstance * 4;
		memIndex[0] = index;
		memIndex[1] = index + 1;
		memIndex[2] = index + 2;
		memIndex[3] = index;
		memIndex[4] = index + 3;
		memIndex[5] = index + 2;
		m_pData->numInstance++;
		memIndex += 6;
	}
	
}

void TextureAtlas::AddInstance(unsigned int region, float x, float y, float faAlpha, float ftheta,float sx,float sy)
{
	KASSERT(region < m_pData->numRegion, "invalid index");
	Region& r = m_pData->aRegions[region];
	speed::renderer::Vert v[4];
	float x2;
	float y2;
	float x1;
	float y1;
	float s;
	float c;
	x2 = - r.w/2.0f;
	y2 =r.h/2.0f;
	x1=x2;
	y1= y2;
	s = sin(ftheta);
	c = cos(ftheta);
	x2 = 	x1 * c - y1*s;
	y2 = 	y1 * c + x1*s;
	
	v[0].vPos[0] = x +x2*sx;
	v[0].vPos[1] = y +y2*sy;
	v[0].vPos[2] =faAlpha;
	//v[0].vPos[3] =ftheta;
	v[0].uv[0] = r.u0;
	v[0].uv[1] = -r.v1;


	x2 = - r.w/2.0f;
	y2 = -r.h / 2.0f;
	x1=x2;
	y1= y2;

	x2 = 	x1 * c - y1*s;
	y2 = 	y1 * c + x1*s;

	v[1].vPos[0] = x+x2*sx;
	v[1].vPos[1] = y +y2*sy;
	v[1].vPos[2] = faAlpha;
	//v[1].vPos[3] =ftheta;
	v[1].uv[0] = r.u0;
	v[1].uv[1] = -r.v0;


	x2 = r.w / 2.0f;
	y2 =-r.h / 2.0f;
	x1=x2;
	y1= y2;

	x2 = 	x1 * c - y1*s;
	y2 = 	y1 * c + x1*s;

	v[2].vPos[0] = x +x2*sx;
	v[2].vPos[1] = y +y2*sy;
	v[2].vPos[2] = faAlpha;
	//v[2].vPos[3] =ftheta;
	v[2].uv[0] = r.u1;
	v[2].uv[1] = -r.v0;

	x2 = r.w / 2.0f;
	y2 = r.h / 2.0f;
	x1=x2;
	y1= y2;

	x2 = 	x1 * c - y1*s;
	y2 = 	y1 * c + x1*s;

	v[3].vPos[0] = x +x2*sx;
	v[3].vPos[1] = y + y2*sy;
	v[3].vPos[2] = faAlpha;
	//v[3].vPos[3] =ftheta;
	v[3].uv[0] = r.u1;
	v[3].uv[1] = -r.v1;
	AddInstance(v, 1);

}

void TextureAtlas::LoadTexture(const char *ptex,int &w , int &h)
{
	m_pData->vTextureId = speed::renderer::loadTexture(ptex, w ,  h);
}

void TextureAtlas::Draw(unsigned int shader)
{
	speed::renderer::bindIndexBuffer(m_pData->iBufferId ,
		(const unsigned short *)m_pData->iBuffer.GetBase(),
		sizeof(unsigned short) * m_pData->numInstance *6 );

	speed::renderer::bindVertexBuffer(m_pData->vBufferId ,
		(const speed::renderer::Vert*)m_pData->vBuffer.GetBase(), 
		sizeof(const speed::renderer::Vert) * m_pData->numInstance *4 );
		
	speed::renderer::bindTexture(shader,m_pData->vTextureId);

	speed::renderer::draw(shader,m_pData->vBufferId, m_pData->iBufferId, m_pData->numInstance *6,m_pData->iBuffer.GetBase());
}

void TextureAtlas::Flush()
{
	m_pData->numInstance = 0;
	m_pData->iBuffer.Flush();
	m_pData->vBuffer.Flush();
}

void TextureAtlas::AddRegion(float u0, float v0, float u1, float v1, int w, int h, const char *pName)
{
	KASSERT(m_pData->numRegion + 1 < TextureAtlas::kMaxSpritePerAtlas, "maximum number of sprite per atlas reached\n");
	Region r = {u0, v0, u1, v1, w, h,0};
	KASSERT(strlen(pName) < 64, "nhjame too long");
	strcpy(r.name,pName);
	m_pData->aRegions[m_pData->numRegion++] = r;
}
