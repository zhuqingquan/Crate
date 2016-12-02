#include "TimerStopWatch.h"

using namespace Util;

TimerStopWatch::TimerStopWatch()
: m_llStart(0), m_llStop(0)
, m_llLastHit(0)
{
	QueryPerformanceFrequency(&m_freq);
}

TimerStopWatch::~TimerStopWatch()
{
	m_llLastHit = m_llStart = m_llStop = 0;
}

LONGLONG TimerStopWatch::start()
{
	QueryPerformanceCounter(&m_now);
	m_llStart = m_now.QuadPart * 1000;
	return m_llStart;
}

LONGLONG TimerStopWatch::stop()
{
	QueryPerformanceCounter(&m_now);
	m_llStop = m_now.QuadPart * 1000;
	return m_llStop;
}

LONGLONG TimerStopWatch::interval()
{
	return m_llStop==0 || m_llStart==0 ? 0 : (m_llStop-m_llStart) / (m_freq.QuadPart);
}

//LONGLONG TimerStopWatch::hit()
//{
//	QueryPerformanceCounter(&m_now);
//	LONGLONG llnow = m_now.QuadPart / 1000;
//	if(m_llLastHit==0)
//	{
//		m_llLastHit = llnow;
//		return -1;
//	}
//	else
//	{
//		LONGLONG result = llnow - m_llLastHit;
//		m_llLastHit = llnow;
//		return result;
//	}
//}
