// ======================================================================
//	MyUnixUtils.hpp
//
//	Revision History:
//	Rev.01 2008/08/05	The First edition
//	Rev.02 2010/05/07	Change license to the modified BSD license
//
//	This file is licensed under the modified BSD license.
//
//	Copyright (C) 2008-2010 Dairoku Sekiguchi
//	All Rights Reserved.
//
//	Redistribution and use in source and binary forms, with or without
//	modification, are permitted provided that the following conditions
//	are met:
//
//	*	Redistributions of source code must retain the above copyright
//		notice, this list of conditions and the following disclaimer.
//
//	*	Redistributions in binary form must reproduce the above
//		copyright notice, this list of conditions and the following
//		disclaimer in the documentation and/or other materials provided
//		with the distribution.
//
//	*	Neither the name of ViewPLUS Inc. nor the names of
//		its contributors may be used to endorse or promote products
//		derived from this software without specific prior written
//		permission.
//
//	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//	"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//	LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//	A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
//	OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//	SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//	LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//	THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//	OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// ======================================================================
/*!
	\file		MyUnixUtils.hpp
	\author		Dairoku Sekiguchi
	\version	1.0
	\date		2008
	\license	modified BSD license
	\brief		Unix Utilites

	Collection of useful unix utilites
*/


#ifndef __MY_UNIX_UTILS_H
#define	__MY_UNIX_UTILS_H

//	Includes ------------------------------------------------------------
#include <limits.h>
#include <time.h>
#include <sys/time.h>


// ----------------------------------------------------------------------
//	typedefs
// ----------------------------------------------------------------------
typedef	unsigned int	timeout_t;


// ----------------------------------------------------------------------
//	macros
// ----------------------------------------------------------------------
#define TIMEOUT_T_MAX				UINT_MAX
#define MY_MILLI_SECOND_UNIT		1000
#define MY_MICRO_SECOND_UNIT		1000000
#define MY_NANO_SECOND_UNIT			1000000000


// ----------------------------------------------------------------------
//	MyUnixUtils classe
// ----------------------------------------------------------------------
class MyUnixUtils
{
public:
	static void	Sleep(timeout_t inMilliseconds)
	{
		struct timespec	reqTime;

		reqTime.tv_sec = inMilliseconds / 1000;
		inMilliseconds -= (reqTime.tv_sec * 1000);
		reqTime.tv_nsec = inMilliseconds * 1000000;

		nanosleep(&reqTime, NULL);

		//usleep(inMilliseconds * 1000);  // microseconds to miliseconds
	}

	static int GetUnixTimeout(struct timespec *outTime, timeout_t inMilliseconds)
	{
		int		error;

		error = GetUnixTime(outTime);
		if (error != 0)
			return error;

		CalcUnixTimeout(outTime, inMilliseconds);

		return 0;
	}

	static int GetUnixTime(struct timespec *outTime)
	{
	#ifdef _USE_CLOCK_GETTIME_

		return clock_gettime(CLOCK_REALTIME, outTime);
	#else
		int		error;
		struct timeval  timeVal;
		struct timezone timeZone;

		error = gettimeofday(&timeVal, &timeZone);
		if (error != 0)
			return error;

		outTime->tv_sec = timeVal.tv_sec;
		outTime->tv_nsec = timeVal.tv_usec * MY_MILLI_SECOND_UNIT;

		return error;
	#endif
	}

	static unsigned int GetTickCount()
	{
		unsigned int    tick;

	#ifdef _USE_CLOCK_GETTIME_
		struct timespec t;

		clock_gettime(CLOCK_REALTIME, &t);
		tick = t.tv_sec * MY_MILLI_SECOND_UNIT;
		tick += (t.tv_nsec / MY_MICRO_SECOND_UNIT);

		return tick;
	#else
		int		error;
		struct timeval  timeVal;
		struct timezone timeZone;

		error = gettimeofday(&timeVal, &timeZone);
		if (error != 0)
			return 0;

		tick = timeVal.tv_sec * MY_MILLI_SECOND_UNIT;
		tick += (timeVal.tv_usec / MY_MILLI_SECOND_UNIT);

		return tick;
	#endif
	}

	static void	CalcUnixTimeout(struct timespec *ioTime, timeout_t inMilliseconds)
	{
		timeout_t		t;

		t = inMilliseconds / MY_MILLI_SECOND_UNIT;
		if (TIMEOUT_T_MAX - ioTime->tv_sec < t)
		{
			ioTime->tv_sec = TIMEOUT_T_MAX;
			ioTime->tv_nsec = TIMEOUT_T_MAX;
			return;
		}

		ioTime->tv_sec += t;
		inMilliseconds = inMilliseconds - t * MY_MILLI_SECOND_UNIT;
		ioTime->tv_nsec += inMilliseconds * MY_MICRO_SECOND_UNIT;
		if (ioTime->tv_nsec >= MY_NANO_SECOND_UNIT)
		{
			(ioTime->tv_nsec) -= MY_NANO_SECOND_UNIT;

			if (TIMEOUT_T_MAX - ioTime->tv_sec == 0)
			{
				ioTime->tv_nsec = MY_NANO_SECOND_UNIT;
				return;
			}
			(ioTime->tv_sec)++;
		}
	}
};

#endif	// #ifdef __MY_UNIX_UTILS_H
