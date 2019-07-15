/* clock_gettime -- Get current time from a POSIX clockid_t.  Linux version.
   Copyright (C) 2003-2019 Free Software Foundation, Inc.
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

#include <sysdep.h>
#include <errno.h>
#include <time.h>
#include "kernel-posix-cpu-timers.h"

#ifdef HAVE_CLOCK_GETTIME_VSYSCALL
# define HAVE_VSYSCALL
#endif
#include <sysdep-vdso.h>

/* Get current value of CLOCK and store it in TP.  */
int
__clock_gettime64 (clockid_t clock_id, struct timespec *tp)
{
#ifdef __ASSUME_TIME64_SYSCALLS
# ifndef __NR_clock_gettime64
#  define __NR_clock_gettime64 __NR_clock_gettime
# endif
   return INLINE_VSYSCALL (clock_gettime64, 2, clock_id, tp);
#else
   int ret;
# ifdef __NR_clock_gettime64
  ret = INLINE_VSYSCALL (clock_gettime64, 2, clock_id, tp);

  if (ret == 0 || errno != ENOSYS)
    return ret;
# endif /* __NR_clock_gettime64 */
  struct timespec tp32;

  ret = INLINE_VSYSCALL (clock_gettime, 2, clock_id, &tp32);

  if (ret == 0 || errno != ENOSYS)
    valid_timespec_to_timespec64(tp32, tp);

  return ret;
#endif /* __ASSUME_TIME64_SYSCALLS */
}

#if __TIMESIZE != 64
int
__clock_gettime (clockid_t clock_id, struct timespec *tp)
{
  int ret;
  struct __timespec64 tp64;

  ret = __clock_gettime64 (clock_id, &tp64);

  if (ret == 0 || errno != ENOSYS)
    {
      if (! in_time_t_range (tp64.tv_sec))
        {
          __set_errno (EOVERFLOW);
          return -1;
        }

      valid_timespec64_to_timespec(&tp64, tp);

      return ret;
    }
}
#endif

weak_alias (__clock_gettime, clock_gettime)
libc_hidden_def (__clock_gettime)
