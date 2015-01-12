#include "Renderer.h"
#if !defined( __ANDROID__) && !defined(__APPLE__)
#include <GL/glew.h>
#include <GL/glfw.h>
#elif defined __APPLE__
#include <OpenGLES/ES2/gl.h>
#else
#include <EGL/egl.h>
#include <GLES/gl.h>
#include <GLES/glext.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#endif //#ifndef __ANDROID__
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "common.h"
#include "Utils.h"
#include <stdlib.h>
#include <stdio.h>
#include "LinearAllocator.h"
#include <string.h>
#include <png.h>

typedef struct {
	const int width;
	const int height;
	const int size;
	const GLenum gl_color_format;
	const void* data;
} RawImageData;

/* Returns the decoded image data, or aborts if there's an error during decoding. */
RawImageData get_raw_image_data_from_png(const void* png_data, const int png_data_size);
void release_raw_image_data(const RawImageData* data);
typedef struct {
	const png_byte* data;
	const png_size_t size;
} DataHandle;

typedef struct {
	const DataHandle data;
	png_size_t offset;
} ReadDataHandle;

typedef struct {
	const png_uint_32 width;
	const png_uint_32 height;
	const int color_type;
} PngInfo;

static void read_png_data_callback(
	png_structp png_ptr, png_byte* png_data, png_size_t read_length);
static PngInfo read_and_update_info(const png_structp png_ptr, const png_infop info_ptr);
static DataHandle read_entire_png_image(
	const png_structp png_ptr, const png_infop info_ptr, const png_uint_32 height);
static GLenum get_gl_color_format(const int png_color_format);
RawImageData get_raw_image_data_from_png(const void* png_data, const int png_data_size) {
	assert(png_data != NULL && png_data_size > 8);
//	assert(png_check_sig((void*)png_data, 8));

	png_structp png_ptr = png_create_read_struct(
		PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	assert(png_ptr != NULL);
	png_infop info_ptr = png_create_info_struct(png_ptr);
	assert(info_ptr != NULL);

	ReadDataHandle png_data_handle =  {(png_byte*)png_data, png_data_size};
	png_set_read_fn(png_ptr, &png_data_handle, read_png_data_callback);

	if (setjmp(png_jmpbuf(png_ptr))) {
		KASSERT(0,"Error reading PNG file!");
	}

	const PngInfo png_info = read_and_update_info(png_ptr, info_ptr);
	const DataHandle raw_image = read_entire_png_image(
		png_ptr, info_ptr, png_info.height);

	png_read_end(png_ptr, info_ptr);
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	RawImageData r =  {
		png_info.width,
		png_info.height,
		raw_image.size,
		get_gl_color_format(png_info.color_type),
		raw_image.data};

	return r;
}
static void read_png_data_callback(
	png_structp png_ptr, png_byte* raw_data, png_size_t read_length) {
		ReadDataHandle* handle = (ReadDataHandle*)png_get_io_ptr(png_ptr);
		const png_byte* png_src = handle->data.data + handle->offset;

		memcpy(raw_data, png_src, read_length);
		handle->offset += read_length;
}
static PngInfo read_and_update_info(const png_structp png_ptr, const png_infop info_ptr)
{
	png_uint_32 width, height;
	int bit_depth, color_type;

	png_read_info(png_ptr, info_ptr);
	png_get_IHDR(
		png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, NULL, NULL, NULL);

	// Convert transparency to full alpha
	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
		png_set_tRNS_to_alpha(png_ptr);

	// Convert grayscale, if needed.
	if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
		png_set_expand_gray_1_2_4_to_8(png_ptr);

	// Convert paletted images, if needed.
	if (color_type == PNG_COLOR_TYPE_PALETTE)
		png_set_palette_to_rgb(png_ptr);

	// Add alpha channel, if there is none.
	// Rationale: GL_RGBA is faster than GL_RGB on many GPUs)
	if (color_type == PNG_COLOR_TYPE_PALETTE || color_type == PNG_COLOR_TYPE_RGB)
		png_set_add_alpha(png_ptr, 0xFF, PNG_FILLER_AFTER);

	// Ensure 8-bit packing
//	if (bit_depth < 8)
//		png_set_packing(png_ptr);
	//else if (bit_depth == 16)
	//	png_set_scale_16(png_ptr);

	png_read_update_info(png_ptr, info_ptr);

	// Read the new color type after updates have been made.
	color_type = png_get_color_type(png_ptr, info_ptr);
	PngInfo in={width, height, color_type};
	return in; 
}
static DataHandle read_entire_png_image(
	const png_structp png_ptr, 
	const png_infop info_ptr, 
	const png_uint_32 height) 
{
	const png_size_t row_size = png_get_rowbytes(png_ptr, info_ptr);
	const int data_length = row_size * height;
	assert(row_size > 0);

	png_byte* raw_image = (png_byte*)malloc(data_length);
	assert(raw_image != NULL);

	png_bytep* row_ptrs = (png_bytep *)malloc(height * sizeof(png_bytep));

	png_uint_32 i;
	for (i = 0; i < height; i++) {
		row_ptrs[i] = raw_image + i * row_size;
	}

	png_read_image(png_ptr, &row_ptrs[0]);
	DataHandle  d={raw_image, data_length};

	return d;
}
static GLenum get_gl_color_format(const int png_color_format) {
	assert(png_color_format == PNG_COLOR_TYPE_GRAY
		|| png_color_format == PNG_COLOR_TYPE_RGB_ALPHA
		|| png_color_format == PNG_COLOR_TYPE_GRAY_ALPHA);

	switch (png_color_format) {
	case PNG_COLOR_TYPE_GRAY:
		return GL_LUMINANCE;
	case PNG_COLOR_TYPE_RGB_ALPHA:
		return GL_RGBA;
	case PNG_COLOR_TYPE_GRAY_ALPHA:
		return GL_LUMINANCE_ALPHA;
	}

	return 0;
}
GLuint png_texture_load(const char * file_name, int * width, int * height)
{
	png_byte header[8];

	FILE *fp = fopen(file_name, "rb");
	if (fp == 0)
	{
		perror(file_name);
		return 0;
	}

	// read the header
	fread(header, 1, 8, fp);

	if (png_sig_cmp(header, 0, 8))
	{
		fprintf(stderr, "error: %s is not a PNG.\n", file_name);
		fclose(fp);
		return 0;
	}

	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr)
	{
		fprintf(stderr, "error: png_create_read_struct returned 0.\n");
		fclose(fp);
		return 0;
	}

	// create png info struct
	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		fprintf(stderr, "error: png_create_info_struct returned 0.\n");
		png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
		fclose(fp);
		return 0;
	}

	// create png info struct
	png_infop end_info = png_create_info_struct(png_ptr);
	if (!end_info)
	{
		fprintf(stderr, "error: png_create_info_struct returned 0.\n");
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
		fclose(fp);
		return 0;
	}

	// the code in this if statement gets called if libpng encounters an error
	if (setjmp(png_jmpbuf(png_ptr))) {
		fprintf(stderr, "error from libpng\n");
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		fclose(fp);
		return 0;
	}

	// init png reading
	png_init_io(png_ptr, fp);

	// let libpng know you already read the first 8 bytes
	png_set_sig_bytes(png_ptr, 8);

	// read all the info up to the image data
	png_read_info(png_ptr, info_ptr);

	// variables to pass to get info
	int bit_depth, color_type;
	png_uint_32 temp_width, temp_height;

	// get info about png
	png_get_IHDR(png_ptr, info_ptr, &temp_width, &temp_height, &bit_depth, &color_type,
		NULL, NULL, NULL);

	if (width){ *width = temp_width; }
	if (height){ *height = temp_height; }

	// Update the png info struct.
	png_read_update_info(png_ptr, info_ptr);
	

	// Row size in bytes.
	int rowbytes = png_get_rowbytes(png_ptr, info_ptr);

	// glTexImage2d requires rows to be 4-byte aligned
	rowbytes += 3 - ((rowbytes-1) % 4);

	// Allocate the image_data as a big block, to be given to opengl
	png_byte * image_data;
	image_data = (png_byte *)malloc(rowbytes * temp_height * sizeof(png_byte)+15);
	if (image_data == NULL)
	{
		fprintf(stderr, "error: could not allocate memory for PNG image data\n");
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		fclose(fp);
		return 0;
	}

	// row_pointers is for pointing to image_data for reading the png with libpng
	png_bytep * row_pointers = (png_bytep *)malloc(temp_height * sizeof(png_bytep));
	if (row_pointers == NULL)
	{
		fprintf(stderr, "error: could not allocate memory for PNG row pointers\n");
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		free(image_data);
		fclose(fp);
		return 0;
	}

	// set the individual row_pointers to point at the correct offsets of image_data
	unsigned int i;
	for (i = 0; i < temp_height; i++)
	{
		row_pointers[temp_height - 1 - i] = image_data + i * rowbytes;
	}

	// read the png into image_data through row_pointers
	png_read_image(png_ptr, row_pointers);

	// Generate the OpenGL texture object
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, color_type==2 ?GL_RGB:GL_RGBA, temp_width, temp_height, 0,color_type==2 ?GL_RGB:GL_RGBA, GL_UNSIGNED_BYTE, image_data);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// clean up
	png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
	free(image_data);
	free(row_pointers);
	fclose(fp);
	return texture;
}


using namespace glm;

glm::mat4 s_ProjMatrix;

LinearAllocator s_VertexBucket(sizeof(speed::renderer::Vert)* 512);
static unsigned int s_VertexBucketID;



bool speed::renderer::init(int width, int hieght)
{
#if !defined( __ANDROID__) && !defined(__APPLE__)
	if( !glfwInit() )
	{
		KLOG( "Failed to initialize GLFW\n" );
		return false;
	}

	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 2);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 1);

	// Open a window and create its OpenGL context
	if( !glfwOpenWindow( width, hieght, 0,0,0,0, 32,0, GLFW_WINDOW ) )
	{
		KLOG( "Failed to initialize GLFW\n" );
		glfwTerminate();
		return false;
	}
	glewInit() ;

#ifndef EMSCRIPTEN
	glfwEnable( GLFW_STICKY_KEYS );
#endif
#endif
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	s_VertexBucketID = createVertexBuffer((Vert*)s_VertexBucket.GetBase(),sizeof(Vert)* 512);
	clear();
	KLOG("Renderer Init OK\n");
	return true;
}

void speed::renderer::swapBuffers()
{
#if !defined( __ANDROID__) && !defined(__APPLE__)

	unsigned int er =glGetError();
	glfwSwapBuffers();
	glfwSleep(1.0/60.f);
	//glEnd()
//#else
	//eglSwapBuffers();
#endif

}

void speed::renderer::clear()
{
	s_VertexBucket.Flush();
	glClear( GL_COLOR_BUFFER_BIT );
}

void speed::renderer::setClearColor(float r, float g, float b, float a)
{
	glClearColor(r , g, b, a);
}

bool speed::renderer::getWindowExit()
{
#if!defined( __ANDROID__) && !defined(__APPLE__)

	return !glfwGetWindowParam( GLFW_OPENED );
#else
	return true;
#endif

}

void speed::renderer::shutdown()
{
	glDeleteBuffers(1, &s_VertexBucketID);
#if !defined( __ANDROID__) && !defined(__APPLE__)
	glfwTerminate();
#endif

}

void speed::renderer::destroyBuffer(unsigned int& buffer)
{
	glDeleteBuffers(1,&buffer);
}

void speed::renderer::destroyTexture(unsigned int& buffer)
{
	glDeleteTextures(1,&buffer);
}


unsigned int speed::renderer::loadShaders(const char * pVertex,const char * pFragment)
{
	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	int s;
	const char* VertexShaderCode = speed::io::loadFile(pVertex,s);
	if (!VertexShaderCode )
	{
		KLOG("unable to open shader file\n");
		return 0;
	}

	const char* FragmentShaderCode = speed::io::loadFile(pFragment,s);
	if (!FragmentShaderCode )
	{
		KLOG("unable to open shader file\n");
		return 0;
	}


	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	KLOG("Compiling shader : %s\n", pVertex);
	glShaderSource(VertexShaderID, 1, &VertexShaderCode, NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	KASSERT(Result != 0, "vertex error\n");
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 )
	{
		char VertexShaderErrorMessage[512];
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		KLOG("%s",VertexShaderErrorMessage);
	}


	KLOG("Compiling shader : %s\n", pFragment);
	glShaderSource(FragmentShaderID, 1, &FragmentShaderCode , NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	KASSERT(Result != 0, "fragment error\n");
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 )
	{
		char FragmentErrorMessage[512];
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentErrorMessage[0]);
		KLOG("%s",FragmentErrorMessage);
	}


	// Link the program
	KLOG("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 )
	{
		char  ProgramErrorMessage[512];
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		KLOG("%s\n", &ProgramErrorMessage[0]);
	}

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}



void speed::renderer::draw(unsigned int shader, unsigned int vertexId,unsigned int IndexID,  unsigned int num, void *pInd)
{
	// Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// Use our shader
	glUseProgram(shader);

	unsigned int vertexPosition_modelspaceID = glGetAttribLocation(shader, "vertexPosition_modelspace");
	unsigned int uv = glGetAttribLocation(shader, "vertexUV");

	unsigned int uniform = getUniformByName(shader, "Proj");

	glUniformMatrix4fv(uniform, 1, GL_FALSE, (const float *)&s_ProjMatrix[0][0]);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(vertexPosition_modelspaceID);
	glEnableVertexAttribArray(uv);
	glBindBuffer(GL_ARRAY_BUFFER, vertexId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,IndexID );
	glVertexAttribPointer(
		vertexPosition_modelspaceID, // The attribute we want to configure
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		sizeof(Vert),                  // stride
		(void*)0            // array buffer offset
		);

	glVertexAttribPointer(
		uv, // The attribute we want to configure
		2,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		sizeof(Vert),                  // stride
		(void*)12            // array buffer offset
		);

	// Draw the triangle !
	glDrawElements(
		GL_TRIANGLES,      // mode
		num,    // count
		GL_UNSIGNED_SHORT,   // type
		(void*)0  );

	glDisableVertexAttribArray(vertexPosition_modelspaceID);
	glDisableVertexAttribArray(uv);
}

unsigned int speed::renderer::createIndexBuffer(const unsigned short * pIndex, int size)
{
	GLuint elementbuffer;

	glGenBuffers(1, &elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, pIndex, GL_STATIC_DRAW);
	return elementbuffer;
}


unsigned int speed::renderer::createVertexBuffer(const Vert* pVert, int size)
{
	GLuint vertexbuffer;

	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, size, pVert, GL_STATIC_DRAW);
	return vertexbuffer;
}

unsigned int speed::renderer::loadTexture(const char *pTex, int& w, int& h)
{

	GLuint textureID = png_texture_load(pTex,&w,&h);
	//glGenTextures(1, &textureID);
	//glBindTexture(GL_TEXTURE_2D, textureID);


	//char *pTexture = king::io::loadFile(pTex,a,false);

	//const RawImageData raw_image_data = 
		//get_raw_image_data_from_png(pTexture, a);
	// Invert texture
	//char* data=  read_png_file(pTex);
	//memset(data, 0, 4*w*h*sizeof(GLubyte));
/*	for (int i = 0; i < h; ++i)
	{
		memcpy(&data[4*w*(h-i-1)], &pTexture[4*w*i], 4*w);
	}*/

	//glfwLoadTexture2D(pTex, 0);


	/*glTexImage2D(GL_TEXTURE_2D, 0, raw_image_data.gl_color_format, raw_image_data.w, raw_image_data.h, 0, raw_image_data.gl_color_format, GL_UNSIGNED_BYTE, raw_image_data.data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); 
	glGenerateMipmap(GL_TEXTURE_2D);

	release_raw_image_data(&raw_image_data);
	release_asset_data(&pTexture);*/
	return textureID;
}

unsigned int speed::renderer::getUniformByName(unsigned int id, const char *pName)
{
	return glGetUniformLocation(id, pName);
}

void speed::renderer::bindTexture(unsigned int shader,  unsigned int tex)
{
	unsigned int uniform = getUniformByName(shader, "myTextureSampler");

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);
	glUniform1i(uniform, 0);

}

void speed::renderer::setOrtho(float left, float right, float top, float bottom, float zNear, float zfar )
{
	s_ProjMatrix = glm::ortho(left, right, bottom, top, zNear , zfar);
}

void speed::renderer::bindVertexBuffer(unsigned int id, const Vert* pVert, int size)
{
	glBindBuffer(GL_ARRAY_BUFFER, id);
	glBufferData(GL_ARRAY_BUFFER, size, pVert, GL_STATIC_DRAW);
}

void speed::renderer::bindIndexBuffer(unsigned int id, const unsigned short * pInd,int size)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, pInd, GL_STATIC_DRAW);
}

void speed::renderer::drawText(const char* pTex, int x , int y , int size, unsigned int shader)
{

		unsigned int length = strlen(pTex);

		// Fill buffers
		Vert *pVert = (Vert *)s_VertexBucket.AllocateBlock(sizeof(Vert)*6*length);
		Vert *pStart = pVert;
		for ( unsigned int i=0 ; i<length ; i++ )
		{

			pVert[i].vPos[0] = (float)x+i*size;
			pVert[i].vPos[1] = (float)y - size;
			pVert[i].vPos[2] = 	1.0f;

			pVert[i + 1].vPos[0] = (float)x + i*size;
			pVert[i + 1].vPos[1] = (float)y;
			pVert[i+1].vPos[2] = 1.0f;
		
			pVert[i + 2].vPos[0] = (float)x + i*size + size;
			pVert[i + 2].vPos[1] = (float)y - size;
			pVert[i+2].vPos[2] = 1.0f;

			pVert[i + 3].vPos[0] = (float)x + i*size + size;
			pVert[i + 3].vPos[1] = (float)y;
			pVert[i+3].vPos[2] = 1.0f;

			pVert[i + 4].vPos[0] = (float)x + i*size + size;
			pVert[i + 4].vPos[1] = (float)y - size;
			pVert[i+4].vPos[2] = 	1.0f;
			
			
			pVert[i + 5].vPos[0] = (float)x + i*size;
			pVert[i + 5].vPos[1] = (float)y;
			pVert[i+5].vPos[2] =1.0f;

			char character = pTex[i];
			float uv_x = (character%16)/16.0f;
			float uv_y = (character/16)/16.0f;


			pVert[i].uv[0] =uv_x;
			pVert[i].uv[1]= 1.0f - uv_y;

			pVert[i+1].uv[0]= uv_x;
			pVert[i+1].uv[1] =  1.0f - (uv_y + 1.0f/16.0f);
			
			pVert[i+2].uv[0] =  uv_x+1.0f/16.0f;
			pVert[i+2].uv[1] = 1.0f - uv_y ;
	
			pVert[i+3].uv[0] =  uv_x+1.0f/16.0f;
			pVert[i+3].uv[1] =  1.0f - (uv_y + 1.0f/16.0f);

			pVert[i+4].uv[0] =   uv_x+1.0f/16.0f, 
			pVert[i+4].uv[1]= 	1.0f - uv_y ;

			pVert[i+5].uv[0] = uv_x;
			pVert[i+5].uv[1] = 1.0f - (uv_y + 1.0f/16.0f);

			pVert+=5;

		}
		glBindBuffer(GL_ARRAY_BUFFER, s_VertexBucketID );
		glBufferData(GL_ARRAY_BUFFER,  length * 6* sizeof(Vert), pStart, GL_STATIC_DRAW);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		// Use our shader
		glUseProgram(shader);

		unsigned int vertexPosition_modelspaceID = glGetAttribLocation(shader, "vertexPosition_modelspace");
		unsigned int uv = glGetAttribLocation(shader, "vertexUV");

		unsigned int uniform = getUniformByName(shader, "Proj");

		glUniformMatrix4fv(uniform, 1, GL_FALSE, (const float *)&s_ProjMatrix[0][0]);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(vertexPosition_modelspaceID);
		glEnableVertexAttribArray(uv);
		glBindBuffer(GL_ARRAY_BUFFER, s_VertexBucketID);
		glVertexAttribPointer(
			vertexPosition_modelspaceID, // The attribute we want to configure
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			sizeof(Vert),                  // stride
			(void*)0            // array buffer offset
			);

		glVertexAttribPointer(
			uv, // The attribute we want to configure
			2,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			sizeof(Vert),                  // stride
			(void*)12            // array buffer offset
			);

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0,  length * 6 );

		glDisableVertexAttribArray(vertexPosition_modelspaceID);
		glDisableVertexAttribArray(uv);

}
