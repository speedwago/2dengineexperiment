#ifndef _Interpolator_h_
#define _Interpolator_h_
/********************************************************************
	created:	2013/09/10
	created:	10:9:2013   22:12
	filename: 	D:\maurizio\Documents\Visual Studio 2010\Projects\king_test\king_test\src\Interpolator.h
	file path:	D:\maurizio\Documents\Visual Studio 2010\Projects\king_test\king_test\src
	file base:	Interpolator
	file ext:	h
	author:		Maurizio Cerrato
	
	purpose:	Interpolator Class
*********************************************************************/
#include "common.h"
//#include "List.h"

// \brief defines the kind of interpolation looping we need
enum LoopDirections
{
	LOOP_FORWARD_DIRECTION = 0,
	LOOP_BOUNCE_DIRECTION
};

/// \brief function that defines the interpolation
typedef float(*InterpolatorFunction)(float t, float t1, float v1, float dv);

/// \brief callback to call when the interpolation target is reached
typedef void(*InterpolatorCallback)(void* pData);


/// \brief this class handles interpolation of a given float reference
class Interpolator
{
public:

	 /// \brief structor
	Interpolator();
	
	/// \brief destructor
	~Interpolator();

	/// \brief init the interpolator
	bool Init(float startTime,
		float duration,
		InterpolatorFunction pFunction,
		float target, 
		float *pValue, 
		InterpolatorCallback pOnFinished = NULL,
		void* pData = NULL,
		int loopCount = 1, 
		LoopDirections loopDirection = LOOP_FORWARD_DIRECTION, 
		bool animationMode = false	);

	/// \shutdown interpolator
	void	 Term();

	/// \brief starts the interpolation
	bool	 Launch();

	/// \brief update the interpolation
	void	 Update(float delta);

	/// \brief check initialization
	bool	 IsInitialized();

	/// \brief check lauched
	bool	 IsLaunched();

	/// \brief check termination
	bool	 IsTerminated();

private:

	
	/// \name status flags
	/// \{

	bool	 m_initialized;
	
	bool	 m_launched;
	
	bool	 m_terminated;
	
	bool	 m_startTimePassed;

	// \}

	int	 m_loopCount;

	LoopDirections	m_loopDirection;

	float	 m_life;
	float	 m_Startlife;

	float	 m_duration;

	InterpolatorFunction	m_pFunction;

	float	 m_target;

	float	 m_delta;

	float	 *m_pValue;

	InterpolatorCallback	m_pOnFinished;
	void* m_pData;
	
	float  m_startTime;

	bool	 m_animationMode;	
	
	bool	 m_goBack;	 
	
	float	 m_oldDuration;
	
	float	 m_oldValue;
	
	float	 m_oldTarget;
};

// interpolator functions
#define ELASTIC_AMPLITUDE	1.0625f
#define ELASTIC_PERIOD	 0.3f
/// \brief linear interpolation
inline float  Linear(float t, float t1, float v1, float dv)
{
	return v1 + (dv * t * t1);
}

/// \brief  quat ease in interpolation
inline float  QuadIn(float t, float t1, float v1, float dv)
{
	t = 1.0f - (t * t1);
	return v1 + dv - (dv * t * t);
}

/// \brief  quat ease out
inline float  QuadOut(float t, float t1, float v1, float dv)
{
	t = t * t1;
	return v1 + (dv * t * t);
}

/// \brief  quat ease in out interpolation
inline float  QuadInOut(float t, float t1, float v1, float dv)
{
	t = t * t1 * 2.0f;

	if (t < 1.0f)
	{
		return v1 + (dv * t * t * 0.5f);
	}
	else
	{
		t = 2.0f - t;

		return v1 + dv - (dv * t * t * 0.5f);
	}
}

/// \brief  cubic ease in interpolation
inline float  CubeIn(float t, float t1, float v1, float dv)
{
	t = 1.0f - (t * t1);
	return v1 + dv - (dv * t * t * t);
}

/// \brief  cubic ease out  interpolation
inline float  CubeOut(float t, float t1, float v1, float dv)
{
	t = t * t1;
	return v1 + (dv * t * t * t);
}

/// \brief  cubic ease in out interpolation
inline float  CubeInOut(float t, float t1, float v1, float dv)
{
	t = t * t1 * 2.0f;
	if (t < 1.0f)
	{
		return v1 + (dv * t * t * t * 0.5f);
	}
	else
	{
		t = 2.0f - t;

		return v1 + dv - (dv * t * t * t * 0.5f);
	}
}

/// \brief sin ease in
inline float  SinIn(float t, float t1, float v1, float dv)
{
	return v1 + (dv * sinf((float)(t * t1 * M_PI_2)));
}

/// \brief sin ease out
inline float  SinOut(float t, float t1, float v1, float dv)
{
	return v1 + dv - (dv * sinf((float)((1.0f - (t * t1)) * M_PI_2)));
}

/// \brief sin ease in out
inline float  SinInOut(float t, float t1, float v1, float dv)
{
	t = t * t1;
	return v1 + (dv * (sinf((float)(t * M_PI - M_PI_2)) * 0.5f + 0.5f));
}

/// \brief sin ease out in
inline float  SinOutIn(float t, float t1, float v1, float dv)
{
	t = t * t1 * 2.0f;
	if (t < 1.0f)
	{
		return v1 + (dv * sinf((float)(t * M_PI_2)) * 0.5f);
	}
	else
	{
		return v1 + dv - (dv * sinf((float)(t * M_PI_2)) * 0.5f);
	}

}

/// \brief elastic out
inline float  ElasticOut(float t, float t1, float v1, float dv)
{
	float s;
	float amplitude = ELASTIC_AMPLITUDE;

	t *= t1;

	if (amplitude < fabs(dv))
	{
		amplitude = dv;
		s = ELASTIC_PERIOD / 4.0f;
	}
	else
	{
		s = ELASTIC_PERIOD / (2.0f * (float)M_PI) * (float)asinf((float)(dv / amplitude));
	}

	return -amplitude * powf(2.0f, -10.0f * t) * sinf((float)((t - s) * -(2.0f * M_PI) / ELASTIC_PERIOD)) + v1 + dv;

}

/// \brief elastic  in
inline float  ElasticIn(float t, float t1, float v1, float dv)
{
	float s;
	float amplitude = ELASTIC_AMPLITUDE;

	t *= t1;

	if (amplitude < fabs(dv))
	{
		amplitude = dv;
		s = ELASTIC_PERIOD / 4;
	}
	else
	{
		s = ELASTIC_PERIOD / (2.0f * (float) M_PI) * (float)asinf((float)(dv / amplitude));
	}

	return -(amplitude * powf(2.0f, 10.0f * (t - 1.0f)) * sinf((float)(((1.0f - t) - s) * (2.0f * M_PI) / ELASTIC_PERIOD))) + v1;

}


#endif
