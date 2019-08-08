/* Copyright (C) 2003-2019 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Ulrich Drepper <drepper@redhat.com>, 2003.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of the
   License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; see the file COPYING.LIB.  If
   not, see <http://www.gnu.org/licenses/>.  */

#include <errno.h>
#include <stdlib.h>
#include <time.h>
#include <sysdep.h>

int
timerfd_settime (int __ufd, int __flags, const struct itimerspec *__utmr,
                 struct itimerspec *__otmr)
{
#ifdef __ASSUME_TIME64_SYSCALLS
  /* Delete the kernel timer object.  */
   return INLINE_SYSCALL (timerfd_settime64, 4, __ufd, __flags, __utmr, __otmr);
#else
   int ret;
# ifdef __NR_timerfd_settime64
#  if __TIMESIZE == 64
  ret = INLINE_SYSCALL (timerfd_settime64, 4, __ufd, __flags, __utmr, __otmr);

  if (ret == 0 || errno != ENOSYS)
    {
      return ret;
    }
#  else
  struct __itimerspec64 ts64;
  ret = INLINE_SYSCALL (timerfd_settime64, 4, __ufd, __flags, __utmr, &ts64);

  if (ret == 0 || errno != ENOSYS)
    {
      __utmr->it_interval.tv_sec = ts64.it_interval.tv_sec
      __utmr->it_interval.tv_nsec = ts64.it_interval.tv_nsec
      if (! in_time_t_range (__utmr->it_interval.tv_sec))
        {
          __set_errno (EOVERFLOW);
          return -1;
        }

      __utmr->it_value.tv_sec = ts64.it_value.tv_sec
      __utmr->it_value.tv_nsec = ts64.it_value.tv_nsec
      if (! in_time_t_range (__utmr->it_value.tv_sec))
        {
          __set_errno (EOVERFLOW);
          return -1;
        }

      return 0;
    }
#  endif /* __TIMESIZE == 64 */
# endif /* __NR_timerfd_settime */
# if __TIMESIZE == 64
  struct itimerspec ts32;

  if (! in_time_t_range (__utmr->it_interval.tv_sec) ||
      ! in_time_t_range (__utmr->it_value.tv_sec))
    {
      __set_errno (EOVERFLOW);
      return -1;
    }

  ret = INLINE_SYSCALL (timerfd_settime, 4, __ufd, __flags, __utmr, &ts32);

  if (ret == 0 || errno != ENOSYS)
    {
      __utmr->it_interval.tv_sec = ts32.it_interval.tv_sec
      __utmr->it_interval.tv_nsec = ts32.it_interval.tv_nsec
      if (! in_time_t_range (__utmr->it_interval.tv_sec))
        {
          __set_errno (EOVERFLOW);
          return -1;
        }

      __utmr->it_value.tv_sec = ts32.it_value.tv_sec
      __utmr->it_value.tv_nsec = ts32.it_value.tv_nsec
      if (! in_time_t_range (__utmr->it_value.tv_sec))
        {
          __set_errno (EOVERFLOW);
          return -1;
        }

      return 0;
    }
  return ret;
# else
    return INLINE_SYSCALL (timerfd_settime, 4, __ufd, __flags, __utmr, __otmr);
# endif /* __TIMESIZE == 64 */
#endif /* __ASSUME_TIME64_SYSCALLS */
}
