/* Linux waitpid syscall implementation.
   Copyright (C) 1991-2019 Free Software Foundation, Inc.
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
#include <sysdep-cancel.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

__pid_t
__waitpid (__pid_t pid, int *stat_loc, int options)
{
#ifdef __NR_waitpid
  return SYSCALL_CANCEL (waitpid, pid, stat_loc, options);
#elif defined(__NR_wait4)
  return SYSCALL_CANCEL (wait4, pid, stat_loc, options, NULL);
#else
  __pid_t ret;
  idtype_t idtype = P_PID;
  siginfo_t infop;

  if (pid < -1)
    {
      idtype = P_PGID;
      pid *= -1;
    }
  else if (pid == -1)
    {
      idtype = P_ALL;
    }
  else if (pid == 0)
    {
      /* FIXME: This can lead to a race condition */
      idtype = P_PGID;
      pid = getpgrp();
    }

  options |= WEXITED;

  ret = SYSCALL_CANCEL (waitid, idtype, pid, &infop, options, NULL);

  if (ret < 0)
    {
      return ret;
    }

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
libc_hidden_def (__waitpid)
weak_alias (__waitpid, waitpid)
