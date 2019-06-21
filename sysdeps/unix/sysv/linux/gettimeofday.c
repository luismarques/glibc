/* Copyright (C) 2015-2019 Free Software Foundation, Inc.

   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of the
   License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

#include <errno.h>
#include <sys/time.h>

#undef __gettimeofday

#ifdef HAVE_GETTIMEOFDAY_VSYSCALL
# define HAVE_VSYSCALL
#endif
#include <sysdep-vdso.h>

/* Get the current time of day and timezone information,
   putting it into *tv and *tz.  If tz is null, *tz is not filled.
   Returns 0 on success, -1 on errors.  */
int
__gettimeofday (struct timeval *tv, struct timezone *tz)
{
  long int ret_64;
#ifdef __ASSUME_TIME64_SYSCALLS
# ifndef __NR_clock_gettime64
#  define __NR_clock_gettime64 __NR_clock_gettime
# endif
  struct timespec now;

  ret_64 = INLINE_VSYSCALL (clock_gettime64, 2, CLOCK_REALTIME,
                         &now);

  if (ret_64 == 0 || errno != ENOSYS)
    valid_timespec64_to_timeval((struct __timespec64*) &now, tv);

  return ret_64;
#else
# ifdef __NR_clock_gettime64
  ret_64 = INLINE_VSYSCALL (clock_gettime64, 2, CLOCK_REALTIME,
                         &now);

  if (ret_64 == 0 || errno != ENOSYS)
    {
      valid_timespec64_to_timeval((struct __timespec64*) &now, tv);
      return ret_64;
    }
# endif /* __NR_clock_gettime64 */
  int ret = INLINE_VSYSCALL (gettimeofday, 2, tv, tz);

  if (! in_time_t_range (tv->tv_sec))
    {
      __set_errno (EOVERFLOW);
      return -1;
    }

  return ret;
#endif
}

#if __TIMESIZE != 64
int
__gettimeofday (struct timeval *tv, struct timezone *tz)
{
  int ret;
  timeval64 tv64;
  struct __timespec64 now;

  ret = __gettimeofday (&tv64, tz);

  if (ret == 0 || errno != ENOSYS)
    {
      /* Convert from timespec to timeval */
      tv->tv_sec = tv64.tv_sec;
      tv->tv_usec = tv64.tv_nsec;
      return ret;
    }

  return ret;
}
#endif

libc_hidden_def (__gettimeofday)
weak_alias (__gettimeofday, gettimeofday)
libc_hidden_weak (gettimeofday)
