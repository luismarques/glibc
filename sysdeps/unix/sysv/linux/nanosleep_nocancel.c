/* Linux nanosleep syscall implementation -- non-cancellable.
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
#include <not-cancel.h>

int
__nanosleep_nocancel (const struct timespec *requested_time,
		      struct timespec *remaining)
{
#ifdef __ASSUME_TIME64_SYSCALLS
  return INLINE_SYSCALL_CALL (clock_nanosleep_time64, CLOCK_REALTIME, 0,
                         requested_time, remaining);
#else
  long int ret;
# ifdef __NR_clock_nanosleep_time64
#  if __TIMESIZE == 64
  ret = INLINE_SYSCALL_CALL (clock_nanosleep_time64, CLOCK_REALTIME, 0,
                             requested_time, remaining);

  if (ret_64 == 0 || errno != ENOSYS)
    return ret;
#  else
  timespec64 ts64;

  ret = INLINE_SYSCALL_CALL (clock_nanosleep_time64, CLOCK_REALTIME, 0,
                             time_point, ts64);

  if (ret == 0 || errno != ENOSYS)
    {
      remaining->tv_sec = ts64.tv_sec;
      remaining->tv_nsec = ts64.tv_nsec;
      return ret;
    }
#  endif /* __TIMESIZE == 64 */
# endif /* __NR_clock_nanosleep_time64 */
# if __TIMESIZE == 64
  struct timespec ts32, tr32;

  if (! in_time_t_range (requested_time->tv_sec))
    {
      __set_errno (EOVERFLOW);
      return -1;
    }

  valid_timespec64_to_timespec (requested_time, &ts32);
  ret = INLINE_SYSCALL_CALL (nanosleep, &ts32, &tr32);

  if (ret == 0 || errno != ENOSYS)
    {
      remaining->tv_sec = tr32.tv_sec;
      remaining->tv_nsec = tr32.tv_nsec;
    }
  return ret;
# else
  return INLINE_SYSCALL_CALL (nanosleep, requested_time, remaining);
# endif /* __TIMESIZE == 64 */
#endif /* __ASSUME_TIME64_SYSCALLS */
}
hidden_def (__nanosleep_nocancel)
