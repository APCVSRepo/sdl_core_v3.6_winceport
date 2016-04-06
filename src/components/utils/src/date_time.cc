/*
* Copyright (c) 2014, Ford Motor Company
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* Redistributions of source code must retain the above copyright notice, this
* list of conditions and the following disclaimer.
*
* Redistributions in binary form must reproduce the above copyright notice,
* this list of conditions and the following
* disclaimer in the documentation and/or other materials provided with the
* distribution.
*
* Neither the name of the Ford Motor Company nor the names of its contributors
* may be used to endorse or promote products derived from this software
* without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*/

#ifdef MODIFY_FUNCTION_SIGN
#include <global_first.h>
#endif
#ifdef OS_WIN32
#include <time.h>
#include <assert.h>
#elif defined(OS_MAC)
#else
#include <sys/time.h>
#endif
#include <stdint.h>
#include "utils/date_time.h"
#ifdef OS_WINCE
#include "utils/global.h"
#endif
#ifdef MODIFY_FUNCTION_SIGN
#include "utils/macro.h"
#endif
#ifdef OS_WIN32
void clock_gettime(int i, timespec * tm)
{
	if (i == CLOCK_MONOTONIC)
	{
		unsigned _int64 cur = GetTickCount();
		tm->tv_sec = cur / 1000;
		tm->tv_nsec = (cur % 1000) * 1000;
	}
	else if (i == CLOCK_REALTIME)
	{
		time_t t;
		::time(&t);
		tm->tv_sec = t;
		tm->tv_nsec = 0;
	}
	else
		assert(false);
}
#elif defined(OS_MAC)
void clock_gettime(int i, timespec * tm)
{
	clock_serv_t cclock;
    mach_timespec_t mts;
    host_get_clock_service(mach_host_self(), i, &cclock);
    clock_get_time(cclock, &mts);
    mach_port_deallocate(mach_task_self(), cclock);
    tm->tv_sec = mts.tv_sec;
    tm->tv_nsec = mts.tv_nsec;
}
#endif

namespace date_time {

#ifdef OS_WIN32
#else
int32_t const DateTime::MILLISECONDS_IN_SECOND;
int32_t const DateTime::MICROSECONDS_IN_MILLISECONDS;
#endif

TimevalStruct DateTime::getCurrentTime() {
  TimevalStruct currentTime;
#ifdef OS_WIN32
  timespec tm;
  clock_gettime(CLOCK_REALTIME, &tm);
  currentTime.tv_sec = (long)tm.tv_sec;
  currentTime.tv_usec = tm.tv_nsec;
#else
#ifdef OS_MAC
    struct timezone timeZone;
#else
  struct timezone timeZone;
#endif
  gettimeofday(&currentTime, &timeZone);
#endif

  return currentTime;
}


#ifdef MODIFY_FUNCTION_SIGN
void DateTime::getCurrentTimeString(std::string &strTime){
	char timestr[40];
	memset(timestr, 0, 40);
	TimevalStruct ts = getCurrentTime();
	snprintf(timestr, 40, "%d.%d", ts.tv_sec, ts.tv_usec);
	strTime = timestr;
}
#endif

int64_t DateTime::getmSecs(const TimevalStruct &time) {
  return static_cast<int64_t>(time.tv_sec) * MILLISECONDS_IN_SECOND
      + time.tv_usec / MICROSECONDS_IN_MILLISECONDS;
}

int64_t DateTime::getuSecs(const TimevalStruct &time) {
  return static_cast<int64_t>(time.tv_sec) * MILLISECONDS_IN_SECOND
      * MICROSECONDS_IN_MILLISECONDS + time.tv_usec;
}

int64_t DateTime::calculateTimeSpan(const TimevalStruct& sinceTime) {
  return calculateTimeDiff(getCurrentTime(), sinceTime);
}

int64_t DateTime::calculateTimeDiff(const TimevalStruct &time1,
                                    const TimevalStruct &time2){
  TimevalStruct timeDifference;
  timeDifference.tv_sec = time1.tv_sec - time2.tv_sec;
  timeDifference.tv_usec = time1.tv_usec - time2.tv_usec;

  if ( timeDifference.tv_usec < 0 ) {
    timeDifference.tv_sec--;
    timeDifference.tv_usec += MILLISECONDS_IN_SECOND
                            * MICROSECONDS_IN_MILLISECONDS;
  }
  return getmSecs(timeDifference);
}

}  // namespace date_time
