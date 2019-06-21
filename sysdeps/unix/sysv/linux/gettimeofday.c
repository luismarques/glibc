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
#ifdef __ASSUME_TIME64_SYSCALLS
  long int ret;
  struct timespec now;

  ret = INLINE_VSYSCALL (clock_gettime64, 2, CLOCK_REALTIME,
                         &now);

  if (ret == 0 || errno != ENOSYS)
    {
      /* Convert from timespec to timeval */
      tv->tv_sec = now.tv_sec;
      tv->tv_usec = now.tv_nsec / 1000;
    }
  return ret;
#else
# ifdef __NR_clock_nanosleep_time64
  long int ret;
#  if __TIMESIZE == 64
  ret = INLINE_VSYSCALL (clock_gettime64, 2, CLOCK_REALTIME,
                         &now);

  if (ret == 0 || errno != ENOSYS)
    {
      /* Convert from timespec to timeval */
      tv->tv_sec = now.tv_sec;
      tv->tv_usec = now.tv_nsec / 1000;
      return ret;
    }
#  else
  struct __timespec64 now;

  ret = INLINE_VSYSCALL (clock_gettime64, 2, CLOCK_REALTIME,
                         &now);

  if (ret == 0 || errno != ENOSYS)
    {
      /* Convert from timespec to timeval */
      tv->tv_sec = now.tv_sec;
      tv->tv_usec = now.tv_nsec / 1000;
      return ret;
    }
#  endif /* __TIMESIZE == 64 */
# endif /* __NR_clock_nanosleep_time64 */
# if __TIMESIZE == 64
  if (! in_time_t_range (tv->tv_sec))
    {
      __set_errno (EOVERFLOW);
      return -1;
    }
# endif /* __TIMESIZE == 64 */
  return INLINE_VSYSCALL (gettimeofday, 2, tv, tz);
#endif
}
libc_hidden_def (__gettimeofday)
weak_alias (__gettimeofday, gettimeofday)
libc_hidden_weak (gettimeofday)
