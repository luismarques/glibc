/* C11 threads thread sleep implementation.
   Copyright (C) 2018-2019 Free Software Foundation, Inc.
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

#include "thrd_priv.h"

int
thrd_sleep (const struct timespec* time_point, struct timespec* remaining)
{
  INTERNAL_SYSCALL_DECL (err);
  int ret = -1;

#ifdef __ASSUME_TIME64_SYSCALLS
  ret = INTERNAL_SYSCALL_CANCEL (clock_nanosleep_time64, err, CLOCK_REALTIME,
                                 0, time_point, remaining);
#else
# ifdef __NR_clock_nanosleep_time64
#  if __TIMESIZE == 64
  long int ret_64;

  ret_64 = INTERNAL_SYSCALL_CANCEL (clock_nanosleep_time64, err, CLOCK_REALTIME,
                                    0, time_point, remaining);

  if (ret_64 == 0 || errno != ENOSYS)
    ret = ret_64;
#  else
  timespec64 ts64;

  ret = INTERNAL_SYSCALL_CANCEL (clock_nanosleep_time64, err,
                                 CLOCK_REALTIME, 0, time_point,
                                 ts64);

  if (ret == 0 || errno != ENOSYS)
    {
      remaining->tv_sec = ts64.tv_sec;
      remaining->tv_nsec = ts64.tv_nsec;
    }
#  endif /* __TIMESIZE == 64 */
# endif /* __NR_clock_nanosleep_time64 */
# if __TIMESIZE == 64
  if (ret < 0)
    {
      struct timespec ts32, tr32;

      if (! in_time_t_range (time_point->tv_sec))
        {
          __set_errno (EOVERFLOW);
          return -1;
        }

      valid_timespec64_to_timespec (time_point, &ts32);
      ret =  INTERNAL_SYSCALL_CANCEL (nanosleep, err, &ts32, &tr32);

      if (ret == 0 || errno != ENOSYS)
        {
          remaining->tv_sec = tr32.tv_sec;
          remaining->tv_nsec = tr32.tv_nsec;
        }
    }
# else
  if (ret < 0)
      ret =  INTERNAL_SYSCALL_CANCEL (nanosleep, err, time_point, remaining);
# endif /* __TIMESIZE == 64 */
#endif /* __ASSUME_TIME64_SYSCALLS */

  if (INTERNAL_SYSCALL_ERROR_P (ret, err))
    {
      /* C11 states thrd_sleep function returns -1 if it has been interrupted
         by a signal, or a negative value if it fails.  */
      ret = INTERNAL_SYSCALL_ERRNO (ret, err);
      if (ret == EINTR)
        return -1;
      return -2;
    }
  return 0;
}
