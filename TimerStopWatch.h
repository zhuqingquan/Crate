/**
 *	@date		2015:6:26   18:05
 *	@name	 	TimerStopWatch.h
 *	@author		zhuqingquan	
 *	@brief		����ʱ��������Ϊ���롣������ȡ��ʼʱ�������ʱ�䣬�Լ������ȡʱ�䣬����������ϴ�ʱ�����¼����
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
		 *	@brief			��ʼʱ�䡣�ýӿ���stop�ɶԵ��û�ȡ����start��stop֮���ʱ������
		 *	@return			LONGLONG ���ø÷�����ʱ�䣬����
		 **/
		LONGLONG start();
		/**
		 *	@name			stop
		 *	@brief			����ʱ�䡣�ýӿ���start�ɶԵ��û�ȡ����start��stop֮���ʱ������
		 *	@return			LONGLONG ���ø÷�����ʱ�䣬����
		 **/
		LONGLONG stop();
		/**
		 *	@name			interval
		 *	@brief			��ȡ����start��stop֮���ʱ������
		 *					���start��stopû���ϸ�ɶԵ��ã������ȵ���stop�ٵ���start����÷���ֵ����Ϊ����
		 *	@return			LONGLONG ʱ����������
		 **/
		LONGLONG interval();

		/**
		 *	@name			hit
		 *	@brief			L��ȡ��ǰ��ʱʱ��������һʱ����ʱ����
		 *	@return			
		LONGLONG hit();ONGLONG ���ε���hit֮���ʱ������ ��һ�ε���hit����-1
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
