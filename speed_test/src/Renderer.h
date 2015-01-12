#ifndef Renderer_h__
#define Renderer_h__

/********************************************************************
	created:	2013/09/03
	created:	3:9:2013   22:32
	filename: 	D:\maurizio\Documents\Visual Studio 2010\Projects\king_test\king_test\src\Renderer.h
	file path:	D:\maurizio\Documents\Visual Studio 2010\Projects\king_test\king_test\src
	file base:	Renderer
	file ext:	h
	author:		Maurizio Cerrato
	
	purpose:	Renderer System for king test
*********************************************************************/
//#define size_t int
namespace speed
{
	namespace renderer
	{
		struct Vert
		{
			float vPos[3];
			float uv[2];
		};
		/// \brief Initialize the main OpenGL windows
		/// \param width window width
		/// \param width window height
		/// \return true if success
		bool init(int width, int hieght);

		// \brief swap the buffer chaing
		void swapBuffers();

		/// \brief clear the gl buffer with the given colour
		void clear();

		/// \brief set the clear colour
		void setClearColor(float r, float g, float b, float a);

		/// \brief catch the window exit event
		bool getWindowExit();

		/// \brief shutdown the system
		void shutdown();

		/// \brief load the given fragment and vertex shader
		unsigned int loadShaders(const char * pVertex, const char * pFragment);

		//\brief issue a rendering call
		void draw(unsigned int shader, unsigned int vertexId,unsigned int IndexID,  unsigned int num, void *pInd);

		void drawText(const char* pTex, int x , int y , int size, unsigned int shader);

		/// \brief generate a index buffer in gpu
		unsigned int createIndexBuffer(const unsigned short * pIndex, int size);

		/// \brief generate a vertex buffer in gpu
		unsigned int createVertexBuffer(const Vert* pVert, int size);

		/// brief release buffer on the gpu
		void  destroyBuffer(unsigned int& buffer);

		/// \brief release texture on the gpu
		void destroyTexture(unsigned int& tex);

		/// \brief upload texture with thw given filename to the gpu
		unsigned int loadTexture(const char *pTex, int &w, int &h);

		/// \brief get a uniform from name
		unsigned int getUniformByName(unsigned int id, const char *pName);

		/// \brief get a uniform from name
		unsigned int getUniformByName(unsigned int id, const char *pName);

		//\ brief bind the given texure
		void bindTexture(unsigned int id, unsigned int tex);

	    //\ brief bind vBO
		void bindVertexBuffer(unsigned int id, const Vert* pVert, int size);

		//\ brief bind index
		void bindIndexBuffer(unsigned int id, const unsigned short * pIndex, int size);

		/// \brief set the the ortho matrix
		void setOrtho(float left, float right, float top, float bottom, float nea, float far );
	}
}


#endif // Renderer_h__
