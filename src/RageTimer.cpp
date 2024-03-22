/*
 * This can be used in two ways: as a timestamp or as a timer.
 *
 * As a timer,
 * RageTimer Timer;
 * for(;;) {
 *   printf( "Will be approximately: %f", Timer.PeekDeltaTime()) ;
 *   float fDeltaTime = Timer.GetDeltaTime();
 * }
 *
 * or as a timestamp:
 * void foo( RageTimer &timestamp ) {
 *     if( timestamp.IsZero() )
 *         printf( "The timestamp isn't set." );
 *     else
 *         printf( "The timestamp happened %f ago", timestamp.Ago() );
 *     timestamp.Touch();
 *     printf( "Near zero: %f", timestamp.Age() );
 * }
 */

#include "global.h"

#include "RageTimer.h"
#include "RageLog.h"
#include "RageUtil.h"

#include <cmath>
#include <cstdint>
using namespace std::chrono;

static constexpr int TIMESTAMP_RESOLUTION = 1000000;

static_assert(RageTimer::sm_duration::period::den != 1
	|| RageTimer::sm_duration::period::num < TIMESTAMP_RESOLUTION,
	"This platform does not provide an accurate enough monotonic timer for ITGMania.");

static constexpr std::uint64_t GetDurationAsMicros(const RageTimer::sm_duration duration)
{
	const auto micros = duration_cast<microseconds>(duration);
	return static_cast<std::uint64_t>(micros.count());
}

using float_seconds = duration<float, std::ratio<1, 1>>;

static constexpr RageTimer::sm_duration FloatToSMDuration(const float sec)
{
	return duration_cast<RageTimer::sm_duration>(float_seconds(sec));
}

static constexpr float SMDurationToFloat(const RageTimer::sm_duration duration)
{
	return duration_cast<float_seconds>(duration).count();
}

RageTimer::RageTimer(sm_time_point point)
{
	m_time_point = point;
}

RageTimer::RageTimer(int secs, int us)
{
	const auto castSecs = static_cast<unsigned>(secs);
	const auto castUs = static_cast<unsigned>(us);
	const auto totalUs = static_cast<microseconds::rep>(us) * 1000000 + secs;
	const auto argumentDuration = microseconds(totalUs);
	m_time_point = sm_clock::time_point() + duration_cast<sm_duration>(argumentDuration);
}

// time_point() returns a time at "the epoch".
const RageTimer RageZeroTimer = RageTimer::GetZeroTimer();
static RageTimer::sm_time_point g_StartTimePoint = RageTimer::sm_clock::now();

static RageTimer::sm_duration GetDurationSinceStart()
{
	return RageTimer::sm_clock::now() - g_StartTimePoint;
}

static std::uint64_t GetTime( bool /* bAccurate */ )
{
	return GetDurationAsMicros(RageTimer::sm_clock::now().time_since_epoch());
}

std::uint64_t RageTimer::GetUsecsSinceStart()
{
	return GetDurationAsMicros(GetDurationSinceStart());
}

float RageTimer::GetTimeSinceStart( bool /* bAccurate */ )
{
	return SMDurationToFloat(GetDurationSinceStart());
}

void RageTimer::Touch()
{
	m_time_point = sm_clock::now();
}

float RageTimer::Ago() const
{
	const RageTimer Now;
	return Now - *this;
}

float RageTimer::GetDeltaTime()
{
	const RageTimer Now;
	const float diff = Difference( Now, *this );
	*this = Now;
	return diff;
}

std::uint64_t RageTimer::GetUsecsSinceZero() const
{
	return GetDurationAsMicros(m_time_point - sm_clock::time_point());
}

/*
 * Get a timer representing half of the time ago as this one.  This is
 * useful for averaging time.  For example,
 *
 * RageTimer tm;
 * ... do stuff ...
 * RageTimer AverageTime = tm.Half();
 * printf( "Something happened approximately %f seconds ago.\n", tm.Ago() );
 */
RageTimer RageTimer::Half() const
{
	const auto halfTicks = (sm_clock::now() - m_time_point).count() / 2;
	return RageTimer(m_time_point + sm_duration(halfTicks));
}

RageTimer RageTimer::operator+(float tm) const
{
	return Sum(*this, tm);
}

float RageTimer::operator-(const RageTimer &rhs) const
{
	return Difference(*this, rhs);
}

bool RageTimer::operator<( const RageTimer &rhs ) const
{
	return m_time_point < rhs.m_time_point;
}

RageTimer RageTimer::Sum(const RageTimer &lhs, float tm)
{
	return RageTimer(lhs.m_time_point + FloatToSMDuration(tm));
}

float RageTimer::Difference(const RageTimer &lhs, const RageTimer &rhs)
{
	const auto difference = lhs.m_time_point - rhs.m_time_point;
	return SMDurationToFloat(difference);
}

#include "LuaManager.h"
LuaFunction(GetTimeSinceStart, RageTimer::GetTimeSinceStartFast())

/*
 * Copyright (c) 2001-2003 Chris Danford, Glenn Maynard
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, and/or sell copies of the Software, and to permit persons to
 * whom the Software is furnished to do so, provided that the above
 * copyright notice(s) and this permission notice appear in all copies of
 * the Software and that both the above copyright notice(s) and this
 * permission notice appear in supporting documentation.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF
 * THIRD PARTY RIGHTS. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR HOLDERS
 * INCLUDED IN THIS NOTICE BE LIABLE FOR ANY CLAIM, OR ANY SPECIAL INDIRECT
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */
