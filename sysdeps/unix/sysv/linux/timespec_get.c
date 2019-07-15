/* Copyright (C) 2011-2019 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

#include <time.h>
#include <sysdep.h>
#include <errno.h>

#ifdef HAVE_CLOCK_GETTIME_VSYSCALL
# define HAVE_VSYSCALL
#endif
#include <sysdep-vdso.h>

/* Set TS to calendar time based in time base BASE.  */
int
__timespec_get64 (struct timespec *ts, int base)
{
  switch (base)
    {
      int res;
      INTERNAL_SYSCALL_DECL (err);
    case TIME_UTC:
#ifdef __ASSUME_TIME64_SYSCALLS
# ifndef __NR_clock_gettime64
#  define __NR_clock_gettime64 __NR_clock_gettime
# endif
      res = INTERNAL_VSYSCALL (clock_gettime64, err, 2, CLOCK_REALTIME, ts);
#else
# ifdef __NR_clock_gettime64
      res = INTERNAL_VSYSCALL (clock_gettime64, err, 2, CLOCK_REALTIME, ts);
# endif /* __NR_clock_gettime64 */
      struct timespec ts32;

      res = INTERNAL_VSYSCALL (clock_gettime, err, 2, CLOCK_REALTIME, &ts32);

      if (res == 0 || !INTERNAL_SYSCALL_ERROR_P (res, err))
        {
          ts->tv_sec = ts32.tv_sec;
          ts->tv_nsec = ts32.tv_nsec;
        }
#endif
      if (INTERNAL_SYSCALL_ERROR_P (res, err))
        return 0;
      break;

    default:
      return 0;
    }

  return base;
}

#if __TIMESIZE != 64
int
timespec_get (struct timespec *ts, int base)
{
  int ret;
  struct __timespec64 ts64;

  ret = __timespec_get64 (&ts64, base);

  if (ret == 0 || !INTERNAL_SYSCALL_ERROR_P (ret, err))
    {
      ts->tv_sec = ts64.tv_sec;
      ts->tv_nsec = ts64.tv_nsec;
      if (! in_time_t_range (ts64.tv_sec))
        {
          __set_errno (EOVERFLOW);
          return -1;
        }
    }

  return ret;
}
#endif
