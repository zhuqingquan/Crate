/**
 *	@date		2015:6:26   18:05
 *	@name	 	TimerStopWatch.h
 *	@author		zhuqingquan	
 *	@brief		秒表计时器，精度为毫秒。包括获取起始时间与结束时间，以及按点获取时间，并计算距离上次时间点的事件间隔
 **/

#pragma once
#ifndef _YY_UTILS_TIMER_STOP_WATCH_H_
#define _YY_UTILS_TIMER_STOP_WATCH_H_

#include <Windows.h>

namespace Util
{
	class TimerStopWatch
	{
	public:
		TimerStopWatch();
		~TimerStopWatch();

		/**
		 *	@name			start
		 *	@brief			起始时间。该接口与stop成对调用获取调用start与stop之间的时间间隔。
		 *	@return			LONGLONG 调用该方法这时间，毫秒
		 **/
		LONGLONG start();
		/**
		 *	@name			stop
		 *	@brief			结束时间。该接口与start成对调用获取调用start与stop之间的时间间隔。
		 *	@return			LONGLONG 调用该方法这时间，毫秒
		 **/
		LONGLONG stop();
		/**
		 *	@name			interval
		 *	@brief			获取调用start与stop之间的时间间隔。
		 *					如果start与stop没有严格成对调用，或者先调用stop再调用start，则该返回值可能为负数
		 *	@return			LONGLONG 时间间隔，毫秒
		 **/
		LONGLONG interval();

		/**
		 *	@name			hit
		 *	@brief			L获取当前计时时间点距离上一时间点的时间间隔
		 *	@return			
		LONGLONG hit();ONGLONG 两次调用hit之间的时间间隔， 第一次调用hit返回-1
		 **/
	private:
		LARGE_INTEGER m_freq;
		LARGE_INTEGER m_now;

		LONGLONG m_llStart;
		LONGLONG m_llStop;

		LONGLONG m_llLastHit;
	};//class TimerStopWatch
}//namespace Util

#endif //_YY_UTILS_TIMER_STOP_WATCH_H_
