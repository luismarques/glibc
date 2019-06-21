/* Copyright (C) 2003-2019 Free Software Foundation, Inc.
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
#include <errno.h>

#include <sysdep-cancel.h>
#include "kernel-posix-cpu-timers.h"

/* We can simply use the syscall.  The CPU clocks are not supported
   with this function.  */
int
__clock_nanosleep (clockid_t clock_id, int flags, const struct timespec *req,
		   struct timespec *rem)
{
  int r = -1;

  if (clock_id == CLOCK_THREAD_CPUTIME_ID)
    return EINVAL;
  if (clock_id == CLOCK_PROCESS_CPUTIME_ID)
    clock_id = MAKE_PROCESS_CPUCLOCK (0, CPUCLOCK_SCHED);

  /* If the call is interrupted by a signal handler or encounters an error,
     it returns a positive value similar to errno.  */
  INTERNAL_SYSCALL_DECL (err);


#ifdef __ASSUME_TIME64_SYSCALLS
  r = INTERNAL_SYSCALL_CANCEL (clock_nanosleep_time64, err, clock_id,
                               flags, req, rem);
#else
# ifdef __NR_clock_nanosleep_time64
#  if __TIMESIZE == 64
  long int ret_64;

  ret_64 = INTERNAL_SYSCALL_CANCEL (clock_nanosleep_time64, err, clock_id,
                                    flags, req, rem);

  if (ret_64 == 0 || errno != ENOSYS)
    r = ret_64;
#  else
  timespec64 ts64;

  r = INTERNAL_SYSCALL_CANCEL (clock_nanosleep_time64, err,
                               clock_id, flags, req,
                               ts64);

  if (r == 0 || errno != ENOSYS)
    {
      rem->tv_sec = ts64.tv_sec;
      rem->tv_nsec = ts64.tv_nsec;
      return r;
    }
#  endif /* __TIMESIZE == 64 */
# endif /* __NR_clock_nanosleep_time64 */
# if __TIMESIZE == 64
  struct timespec ts32, tr32;

  if (r < 0)
    {
      if (! in_time_t_range (req->tv_sec))
        {
          __set_errno (EOVERFLOW);
          return -1;
        }

      valid_timespec64_to_timespec (req, &ts32);
      r =  INTERNAL_SYSCALL_CANCEL (clock_nanosleep, err, &ts32, &tr32);

      if (r == 0 || errno != ENOSYS)
        {
          rem->tv_sec = tr32.tv_sec;
          rem->tv_nsec = tr32.tv_nsec;
        }
    }
# else
  if (r < 0)
      r =  INTERNAL_SYSCALL_CANCEL (clock_nanosleep, err, req, rem);
# endif /* __TIMESIZE == 64 */
#endif /* __ASSUME_TIME64_SYSCALLS */

  return (INTERNAL_SYSCALL_ERROR_P (r, err)
	  ? INTERNAL_SYSCALL_ERRNO (r, err) : 0);
}
weak_alias (__clock_nanosleep, clock_nanosleep)
