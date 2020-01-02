/* Define where padding goes in struct semid_ds.  Generic version.
   Copyright (C) 2018-2020 Free Software Foundation, Inc.
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
   <https://www.gnu.org/licenses/>.  */

#ifndef _SYS_SEM_H
# error "Never use <bits/sem-pad.h> directly; include <sys/sem.h> instead."
#endif

#include <bits/timesize.h>

/* On most architectures, padding goes after time fields for 32-bit
   systems and is omitted for 64-bit systems.  Some architectures pad
   before time fields instead, or omit padding despite being 32-bit,
   or include it despite being 64-bit.  This must match the layout
   used for struct semid64_ds in <asm/sembuf.h>, as glibc does not do
   layout conversions for this structure.  */

#define __SEM_PAD_AFTER_TIME (__TIMESIZE == 32)

/* If we are using a 32-bit architecture with a 64-bit time_t let's
   define __SEM_PAD_BEFORE_TIME as 1. This is because the kernel is
   expecting a type __kernel_old_time_t which is 32-bit even with a
   64-bit time_t. Instead of adding another check, let's just set
   __SEM_PAD_BEFORE_TIME as that will use a long type instead of
   long long.  */
#if __WORDSIZE == 32 && __TIMESIZE == 64
# define __SEM_PAD_BEFORE_TIME 1
#else
# define __SEM_PAD_BEFORE_TIME 0
#endif
