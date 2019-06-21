/* Linux nanosleep syscall implementation.
   Copyright (C) 2017-2019 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

#include <time.h>
#include <sysdep-cancel.h>
#include <not-cancel.h>

/* Pause execution for a number of nanoseconds.  */
int
__nanosleep_time64 (const struct timespec *requested_time,
                    struct timespec *remaining)
{
#if defined(__ASSUME_TIME64_SYSCALLS)
# ifndef __NR_clock_nanosleep_time64
#  define __NR_clock_nanosleep_time64 __NR_clock_nanosleep
# endif
  return SYSCALL_CANCEL (clock_nanosleep_time64, CLOCK_REALTIME, 0,
                         requested_time, remaining);
#else
# ifdef __NR_clock_nanosleep_time64
  long int ret_64;

  ret_64 = SYSCALL_CANCEL (clock_nanosleep_time64, CLOCK_REALTIME, 0,
                           requested_time, remaining);

  if (ret_64 == 0 || errno != ENOSYS)
    return ret_64;
# endif /* __NR_clock_nanosleep_time64 */
  int ret;
  struct timespec ts32, tr32;

  if (! in_time_t_range (requested_time->tv_sec))
    {
      __set_errno (EOVERFLOW);
      return -1;
    }

  valid_timespec64_to_timespec (requested_time, &ts32);
  ret = SYSCALL_CANCEL (nanosleep, &ts32, &tr32);

  if ((ret == 0 || errno != ENOSYS) && remaining)
    valid_timespec_to_timespec64(&tr32, remaining);

  return ret;
#endif /* __ASSUME_TIME64_SYSCALLS */
}

#if __TIMESIZE != 64
int
__nanosleep (const struct timespec *requested_time,
             struct timespec *remaining)
{
  int r;
  timespec64 treq64, trem64;


  valid_timespec_to_timespec64(req, &treq64);
  r = __nanosleep_time64 (&treq64, &trem64);

  if (r == 0 || errno != ENOSYS)
    {
      if (! in_time_t_range (trem64->tv_sec))
        {
          __set_errno (EOVERFLOW);
          return -1;
        }

      if (remaining)
        valid_timespec64_to_timespec(&tr32, remaining);
    }

  return r;
}
#endif

hidden_def (__nanosleep)
weak_alias (__nanosleep, nanosleep)
