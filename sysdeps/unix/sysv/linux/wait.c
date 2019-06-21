/* Copyright (C) 1991-2019 Free Software Foundation, Inc.
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

#include <sys/wait.h>
#include <errno.h>
#include <sys/resource.h>
#include <stddef.h>
#include <sysdep-cancel.h>

/* Wait for a child to die.  When one does, put its status in *STAT_LOC
   and return its process ID.  For errors, return (pid_t) -1.  */
pid_t
__libc_wait (int *stat_loc)
{
#ifdef __NR_wait4
  return SYSCALL_CANCEL (wait4, WAIT_ANY, stat_loc, 0,
                         (struct rusage *) NULL);
#else
  siginfo_t infop;
  __pid_t ret;

  ret = SYSCALL_CANCEL (waitid, P_ALL, 0, &infop, WEXITED, NULL);

  if (ret < 0)
      return ret;

  if (stat_loc)
    {
      *stat_loc = 0;
      switch (infop.si_code)
      {
        case CLD_EXITED:
            *stat_loc = infop.si_status << 8;
            break;
        case CLD_DUMPED:
            *stat_loc = 0x80;
            /* Fallthrough */
        case CLD_KILLED:
            *stat_loc |= infop.si_status;
            break;
        case CLD_TRAPPED:
        case CLD_STOPPED:
            *stat_loc = infop.si_status << 8 | 0x7f;
            break;
        case CLD_CONTINUED:
            *stat_loc = 0xffff;
            break;
      }
    }

  return infop.si_pid;
#endif
}

weak_alias (__libc_wait, __wait)
weak_alias (__libc_wait, wait)
