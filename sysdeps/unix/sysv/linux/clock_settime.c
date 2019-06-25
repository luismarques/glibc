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

#include <errno.h>
#include <sysdep.h>
#include <time.h>

/* Set CLOCK to value TP.  */
int
__clock_settime64 (clockid_t clock_id, const struct __timespec64 *tp)
{
  /* Make sure the time cvalue is OK.  */
  if (tp->tv_nsec < 0 || tp->tv_nsec >= 1000000000)
    {
      __set_errno (EINVAL);
      return -1;
    }

#ifdef __ASSUME_TIME64_SYSCALLS
# ifndef __NR_clock_settime64
#  define __NR_clock_settime64 __NR_clock_settime
# endif
  return INLINE_SYSCALL_CALL (clock_settime64, clock_id, tp);
#else
# ifdef __NR_clock_settime64
  int ret = INLINE_SYSCALL_CALL (clock_settime64, clock_id, tp);
  if (ret == 0 || errno != ENOSYS)
    return ret;
# endif
  if (! in_time_t_range (tp->tv_sec))
    {
      __set_errno (EOVERFLOW);
      return -1;
    }

  struct timespec ts32;
  valid_timespec64_to_timespec (tp, &ts32);
  return INLINE_SYSCALL_CALL (clock_settime, clock_id, &ts32);
#endif
}
weak_alias (__clock_settime, clock_settime)

int
__clock_settime (clockid_t clock_id, const struct timespec *tp)
{
  struct __timespec64 ts64;

  valid_timespec_to_timespec64 (tp, &ts64);
  return __clock_settime64 (clock_id, &ts64);
}
