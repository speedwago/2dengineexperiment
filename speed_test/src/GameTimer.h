#ifndef GameTimer_h__
#define GameTimer_h__
/********************************************************************
	created:	2013/09/09
	created:	9:9:2013   23:20
	filename: 	D:\maurizio\Documents\Visual Studio 2010\Projects\\\src\GameTimer.h
	file path:	D:\maurizio\Documents\Visual Studio 2010\Projects\\\src
	file base:	GameTimer
	file ext:	h
	author:		Maurizio Cerrato
	
	purpose:	handles game timed event 
*********************************************************************/

#include <stdio.h>
//#include "List.h"
/// \brief callback to the timer function.
typedef void(*TimerCallback)( void* pData);

/// \brief define the kind of timer.
enum TimerType
{
	NORMAL,
	LOOP,
	STOP,
};
/// \brief this class handles timed event in game trough callbacks 
class GameTimer
{
public:

	/// \brief default constructo
	GameTimer();

	/// \brief default constructor
	GameTimer(float fTarget, TimerType type = NORMAL , TimerCallback pCall = NULL, void* pData = NULL);

    /// \brief init the timer
	void Init(float fTarget, TimerType type = NORMAL , TimerCallback pCall = NULL, void* pData = NULL);

	/// \brief destructor
	~GameTimer();

	/// \brief update the timer , if  the tartget is reached then fire the callback
	void Update(float Delta);

	/// \brief restart the timer
	void Restart(TimerType type);

	///\ brief check if is stopped or not.
	bool IsStopped();

	/// \brief set the callback 
	void SetCallback( TimerCallback pCall  ,void *data);

	/// zbrief return the remaning time
	float GetRemainingTime() const;

private:

	/// \brief non copyable
	GameTimer(const GameTimer&){};

	/// \brief non copyable
	GameTimer& operator=(const GameTimer&){return *this;};

	/// \brief the callback to call when the timer reac the destination.
	TimerCallback m_pCallBack;

	/// \brief data for the callback
	void* m_pCallBackData;

	/// \brief elapsed time
	float m_fElapsed;

	/// \brief tartget time;
	float m_fTarget;

	/// \brief time type.
	TimerType m_type;
};
#endif // GameTimer_h__
