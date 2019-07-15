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

int
__timespec_get (struct timespec *ts, int base)
{
#ifdef __ASSUME_TIME64_SYSCALLS
  return INTERNAL_VSYSCALL (clock_gettime64, err, 2, CLOCK_REALTIME, ts);
#else
  long int ret;
# ifdef __NR_clock_gettime64
#  if __TIMESIZE == 64
  ret = INTERNAL_VSYSCALL (clock_gettime64, err, 2, CLOCK_REALTIME, ts);

  if (ret == 0 || errno != ENOSYS)
    {
      return ret;
    }
#  else
  struct __timespec64 ts64;

  ret = INTERNAL_VSYSCALL (clock_gettime64, err, 2, CLOCK_REALTIME, &ts64);

  if (ret == 0 || errno != ENOSYS)
    {
      ts->tv_sec = ts64.tv_sec;
      ts->tv_nsec = ts64.tv_nsec;
      return ret;
    }
#  endif /* __TIMESIZE == 64 */
# endif /* __NR_clock_gettime64 */
# if __TIMESIZE == 64
  struct timespec ts32;

  if (! in_time_t_range (ts->tv_sec))
    {
      __set_errno (EOVERFLOW);
      return -1;
    }

  valid_timespec64_to_timespec (ts, &ts32);
  ret = INTERNAL_VSYSCALL (clock_gettime, err, 2, CLOCK_REALTIME, &ts32);

  if (ret == 0 || errno != ENOSYS)
    {
      ts->tv_sec = ts32.tv_sec;
      ts->tv_nsec = ts32.tv_nsec;
    }
  return ret;
# else
  return INTERNAL_VSYSCALL (clock_gettime, err, 2, CLOCK_REALTIME, ts);
# endif /* __TIMESIZE == 64 */
#endif
}

/* Set TS to calendar time based in time base BASE.  */
int
timespec_get (struct timespec *ts, int base)
{
  switch (base)
    {
      int res;
      INTERNAL_SYSCALL_DECL (err);
    case TIME_UTC:
      res = __timespec_get (ts, base);
      if (INTERNAL_SYSCALL_ERROR_P (res, err))
        return 0;
      break;

    default:
      return 0;
    }

  return base;
}
