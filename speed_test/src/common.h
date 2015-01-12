#ifndef common_h__
#define common_h__
#include <stdio.h>
#include <math.h>
/********************************************************************
	created:	2013/09/03
	created:	3:9:2013   22:39
	filename: 	D:\maurizio\Documents\Visual Studio 2010\Projects\king_test\king_test\src\common.h
	file path:	D:\maurizio\Documents\Visual Studio 2010\Projects\king_test\king_test\src
	file base:	common
	file ext:	h
	author:		Maurizio Cerrato
	
	purpose:	Common macros and fuction
*********************************************************************/
#ifdef __ANDROID__
#define _DEBUG
#include <android/log.h>
#endif
#ifdef _DEBUG
#include <assert.h>
#include <crtdbg.h>
#define KASSERT(test, msg ,...)  _ASSERT(test);

#ifndef __ANDROID__
#define KLOG(msg, ...) printf( msg, ##__VA_ARGS__)
#else
#define KLOG(msg, ...) __android_log_print(ANDROID_LOG_VERBOSE, "test", msg, ##__VA_ARGS__)
#endif
#else
#define KASSERT(test, msg, ...) (void) 0
#define KLOG(msg, ...) (void)0
#endif

// check if a number is power of two
inline int isPowerOfTwo (unsigned int x)
{
	return ((x != 0) && !(x & (x - 1)));
}


#define M_PI  3.141593

#define M_PI_2  6.283186

template <class T> void swap ( T& a, T& b )
{
	T c(a); a=b; b=c;
}

template <typename T> int sgn(T val) {
	return (T(0) < val) - (val < T(0));
}

#endif // common_h__
