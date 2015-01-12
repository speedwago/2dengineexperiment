#include "Utils.h"
#include "common.h"
#include "tinyxml2.h"
#include <stdio.h>
#include <stdlib.h>
#include "TextureAtlas.h"
#include "GameLogic.h"
#ifndef EMSCRIPTEN
#include "audiere.h"
#endif

#if !defined (__ANDROID__) && !defined(__APPLE__)
#include <GL/glfw.h>
#elif defined (__ANDROID__)
#include <time.h>
#include <stdio.h>
#include <android/asset_manager_jni.h>
#endif
//#include <audiere.h>
#include <time.h>
#ifdef __ANDROID__
extern AAssetManager* asset_manager;

static timespec now;
static timespec last;

#endif
#ifndef EMSCRIPTEN
using namespace audiere;
AudioDevicePtr audioDevice;
#endif
static int  s_lastMouseState;
static int  s_lastMouseRState;
static int  s_lastLeftState;
static int  s_lastRightState;
static float meterToPixel = 30.0;
static float HalfScreenHPixel = GameLogic::kScreenH /2.0f ;
static float HalfScreenWPixel = GameLogic::kScreenW /2.0f ;
static float HalfScreenWMeter = GameLogic::kScreenW /2.0f /meterToPixel;
static float HalfScreenHMeter = GameLogic::kScreenH /2.0f /meterToPixel;


//static AudioDevicePtr audioDevice;
#ifdef __ANDROID__
FileData get_asset_data(const char* relative_path)
{
   // assert(relative_path != NULL);
    AAsset* asset =
        AAssetManager_open(asset_manager, relative_path, AASSET_MODE_STREAMING);
    assert(asset != NULL);

    return (FileData) { AAsset_getLength(asset), AAsset_getBuffer(asset), asset };
}

void release_asset_data(const FileData* file_data)
{
    assert(file_data != NULL);
    assert(file_data->file_handle != NULL);
    AAsset_close((AAsset*)file_data->file_handle);
}
#endif

char* speed::io::loadFile(const char* pPath ,int &sizee,bool addnull )
{
	char* ret(NULL);
	KASSERT(pPath, "path is null\n");
#ifdef __APPLE__
    NSString* path= [NSString stringWithUTF8String:pPath];
    path= [path lastPathComponent];
  NSString* fragShaderPathname = [[NSBundle mainBundle] pathForResource:@"SimpleVertexShader"  ofType:@"v"];
    
    NSBundle *b = [NSBundle mainBundle];
    NSString *dir = [b resourcePath];
    dir =[dir stringByAppendingString:@"/"];
     NSString* manifest_string2 = [path stringByDeletingLastPathComponent];
    NSString* manifest_string = [dir stringByAppendingPathComponent: path];
    
    const char *cpath = [manifest_string UTF8String];
#endif
	FILE *fp(NULL);
#ifndef __ANDROID__ 
#ifdef __APPLE__
    fp = fopen(cpath, "r");
#else
	fp = fopen(pPath, "rb");
#endif
    
	KASSERT(fp, "Unable to open file %s\n", pPath);
    fseek(fp, 0L, SEEK_END);
	int size = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
	if (size > 0)
	{
		addnull ? size++: size;
		ret =  new char[size];
		KASSERT(ret, "unable to allocate %d bytes \n", size);
		int res = fread(ret, size, 1, fp);
		//KASSERT(res == size, "error during file reading\n");
		addnull ? ret[size -1] = '\0': size;
		sizee = size;
	}

	if (fp)
	{
		fclose(fp);
	}
#else
	//char path2[64];
	//sprintf(path2,"assets/%s",pPath);
	//KLOG("%s",path2);
//	fp = fopen(path2, "rb");
	FileData  file = get_asset_data(pPath);
	int size = file.data_length;
	KASSERT(size > 0, "eoor");
	if (size > 0)
	{
		addnull ? size++: size;
		ret =  new char[size];
		KASSERT(ret, "unable to allocate %d bytes \n", size);
		memcpy(ret, file.data,size);
		//KASSERT(res == size, "error during file reading\n");
		addnull ? ret[size -1] = '\0': size;
	}

	release_asset_data(&file);
#endif
	return ret;
}

void  speed::io::printText(const char* txt,  TextureAtlas *pAtlas, float x, float y)
{
	float xx = x;
	while (*txt)
	{
		pAtlas->AddInstance(*txt -32,xx,y,1.0,0.0);
		xx += pAtlas->GetSprite(*txt - 32).w;
		txt++;

	}
}

void  speed::io::parseTxtSheet(const char* pPathTxt, const char* pPathImg, TextureAtlas *pAtlas)
{
	int s;
	char* pData(loadFile(pPathTxt, s));
	if (pData)
	{
		if (pPathImg)
		{
			int wi, hi;
			pAtlas->LoadTexture(pPathImg, wi, hi);
			int x, y, w, h, a, b, c, d;
			char name[64];
			int read;
			while (sscanf(pData, "%s%d, %d, %d, %d, %d, %d, %d, %d\r\n%n", name, &x, &y, &w, &h, &a, &b, &c, &d, &read) != EOF)
			{

				pData += read;

				pAtlas->AddRegion(
					(float)x / wi,
					(float)y / hi,
					(float)(x + w) / wi,
					(float)(y + h) / hi,
					w,
					h,
					name);
			}

		}
	}
}

void  speed::io::parseTxtTextSheet(const char* pPathTxt, const char* pPathImg, TextureAtlas *pAtlas)
{
	int s;
	char* pData(loadFile(pPathTxt, s));
	if (pData)
	{
		if (pPathImg)
		{
			int wi, hi;
			pAtlas->LoadTexture(pPathImg, wi, hi);
			int x, y, w, h, a, b, c, d,e;
			int read;

			char* pos = strstr(pData, "chars count="); //find how many chars are in the file
			if (pos == 0)
				return ;

			int num;
			sscanf(pos, "chars count=%d\n%n", &num, &read);
			pos += read;
			int id;
			while (num--)
			{
				sscanf(pos, "char id=%d   x=%d     y=%d     width=%d     height=%d     xoffset=%d     yoffset=%d    xadvance=%d    page=%d  chnl=%d\n%n",&id, &x, &y, &w, &h, &a, &b, &c, &d, &e, &read);
				pos += read;

				pAtlas->AddRegion(
					(float)x / wi,
					(float)y / hi,
					(float)(x + w) / wi,
					(float)(y + h) / hi,
					w,
					h,
					"chars");
			}

		}
	}
}

void  speed::io::parseSheet(const char* pPath, TextureAtlas *pAtlas)
{
	tinyxml2::XMLDocument		 doc;
	tinyxml2::XMLNode			*pRoot;
	tinyxml2::XMLNode			*pNode;
	int s;
	char* pData(loadFile(pPath,s));
	if (pData)
	{
		doc.Parse(pData);

		pRoot = doc.FirstChildElement("TextureAtlas");
		if (pRoot)
		{
			const char *pImage = pRoot->ToElement()->Attribute("imagePath");

			const char *pWidth = pRoot->ToElement()->Attribute("width");
			const char *pHeight = pRoot->ToElement()->Attribute("height");
			int width, height;
			if (pWidth && pHeight)
			{
				width = atoi(pWidth);
				height = atoi(pHeight);
			}

			if (pImage)
			{
				pAtlas->LoadTexture(pImage,width, height);

			}
			for (pNode = pRoot->FirstChild(); pNode; pNode = pNode->NextSibling())
			{
				if (strcmp(pNode->Value(), "sprite") == 0)
				{
					const char *pX = pNode->ToElement()->Attribute("x");
					const char *pY = pNode->ToElement()->Attribute("y");
					const char *pW = pNode->ToElement()->Attribute("w");
					const char *pH = pNode->ToElement()->Attribute("h");
					const char *pN = pNode->ToElement()->Attribute("n");

					int x = atoi(pX);
					int y = atoi(pY);
					int w = atoi(pW);
					int h = atoi(pH);
					pAtlas->AddRegion(
						(float) x / width, 
						(float) y / height,
						(float) (x + w) / width,
						(float) (y + h) / height,
						w,
						h,
						pN);
				}
			}

		}


		delete[] pData;
	}
}

#ifdef __ANDROID__
static int s_x, s_y;
void   on_touch_press(float x, float y)
{
	s_lastMouseState = 1;
	s_x = x;
	s_y = y;
}
void   on_touch_drag(float x, float y)
{
	s_lastMouseState = 0;
	s_x = x;
	s_y = y;
}
#endif

void speed::io::GetMousePosition(int *x,int *y)
{
	glfwGetMousePos(x,y);
}
bool speed::io::GetMouseRClickPosition(int *x,int *y)
{
#if !defined(__ANDROID__) && !defined(__APPLE__)

	KASSERT(x, "null pointer");
	KASSERT(y, "null pointer");
	int button = glfwGetMouseButton(GLFW_MOUSE_BUTTON_2);
	bool res = (button & (~s_lastMouseRState))? true : false;
	s_lastMouseRState = button;
	if (res)
	{
		glfwGetMousePos(x,y);
	}
	return res;
#elif defined (__ANDROID__)
	if (s_lastMouseState)
	{
		*x = s_x;
		*y = s_y;
	}
	return s_lastMouseState;
#endif
	return 0;
}

bool speed::io::GetMouseLClickPosition(int *x,int *y)
{
#if !defined(__ANDROID__) && !defined(__APPLE__)

	KASSERT(x, "null pointer");
	KASSERT(y, "null pointer");
	int button = glfwGetMouseButton(GLFW_MOUSE_BUTTON_1);
	bool res = button & ~s_lastMouseState? true : false;
	s_lastMouseState = button;
	if (res)
	{
		glfwGetMousePos(x,y);
	}
	return res;
#elif defined (__ANDROID__)
	if (s_lastMouseState)
	{
		*x = s_x;
		*y = s_y;
	}
	return s_lastMouseState;
#endif
    return 0;
}

bool speed::io::IsLeftPressed()
{
    int i =glfwGetKey(GLFW_KEY_LEFT);
    bool res = i & ~s_lastLeftState? true : false;
    s_lastLeftState = i;
    return res;
}

bool speed::io::IsRightPressed()
{
    int i =glfwGetKey(GLFW_KEY_RIGHT);
    bool res = i & ~s_lastRightState? true : false;
    s_lastRightState = i;
    return res;
}

bool speed::io::GetMouseLReleasePosition(int *x,int *y)
{
#if !defined(__ANDROID__) && !defined(__APPLE__)

	KASSERT(x, "null pointer");
	KASSERT(y, "null pointer");
	int button = glfwGetMouseButton(GLFW_MOUSE_BUTTON_1);
	bool res = button>0;

	if (res)
	{
		glfwGetMousePos(x,y);
	}
	return res;
#elif defined (__ANDROID__)
	if (s_lastMouseState ==0)
	{
		*x = s_x;
		*y = s_y;
	}
	return s_lastMouseState ==0;
#endif
    return 0;
}

bool speed::io::GetMouseRReleasePosition(int *x,int *y)
{
#if !defined(__ANDROID__) && !defined(__APPLE__)

	KASSERT(x, "null pointer");
	KASSERT(y, "null pointer");
	int button = glfwGetMouseButton(GLFW_MOUSE_BUTTON_2);
	bool res = button>0;

	if (res)
	{
		glfwGetMousePos(x,y);
	}
	return res;
#elif defined (__ANDROID__)
	if (s_lastMouseState ==0)
	{
		*x = s_x;
		*y = s_y;
	}
	return s_lastRMouseState ==0;
#endif
	return 0;
}

bool speed::collision::IsPointInsideBox(float x,float y, float minX, float minY, float maxX, float maxY )
{
	return (x < maxX && x > minX
		&&  y < maxY && y > minY );
}

b2Vec2 speed::collision::GetScreenPos(const b2Vec2& box2dPos)
{
	b2Vec2 ret;
	ret.x = (box2dPos.x + HalfScreenWMeter) * meterToPixel;
	ret.y = (-box2dPos.y + HalfScreenHMeter) * meterToPixel;
	return ret;
}

b2Vec2 speed::collision::GetBox2dPos(const b2Vec2& screenPos)
{
	b2Vec2 ret;
	ret.x = (screenPos.x - HalfScreenWPixel) / meterToPixel;
	ret.y = (-screenPos.y + HalfScreenHPixel) / meterToPixel;
	return ret;
}
b2Vec2 speed::collision::GetBox2dWH(const b2Vec2& screenWH)
{
    b2Vec2 ret;
    ret.x = screenWH.x/ meterToPixel;
    ret.y = screenWH.y/ meterToPixel;
    return ret;
}
bool speed::audio::init()
{
#ifndef EMSCRIPTEN
	audioDevice = OpenDevice();
	KASSERT(audioDevice, "unable to init audio!\n");
	if (!audioDevice) 
	{
		return false;
	}
#endif
	return true;
}

void speed::audio::playMusic(const char *pFile)
{
#ifndef EMSCRIPTEN
	OutputStreamPtr stream(OpenSound(audioDevice, pFile, false));
	if (!stream)
	{
		KLOG("unble to play file %s" , pFile);
	}
	else
	{
		stream->stop();
		stream->setRepeat(true);
		stream->setVolume(1.0f);
		stream->play();
	}
#endif
	
}


void speed::audio::playSFX(const char *pFile)
{
	#ifndef EMSCRIPTEN
    OutputStreamPtr stream(OpenSound(audioDevice, pFile, false));
	if (!stream)
	{
		KLOG("unble to play file %s" , pFile);
	}
	else
	{
		stream->setVolume(1.0f);
		stream->play();
	}
#endif
}

float speed::misc::GetTime()
{
#ifdef __ANDROID__
    clock_gettime(CLOCK_MONOTONIC , &now);
    long long  msec = now.tv_nsec /1000000LL;
   return   double(now.tv_sec + (msec/1000.0));
#elif defined (__APPLE__)
    return 1.0/60.0f;
#else
	return (float) glfwGetTime() ;
#endif
}
