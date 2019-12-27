#ifndef __timeval_defined
#define __timeval_defined 1

#include <bits/types.h>

/* A time value that is accurate to the nearest
   microsecond but also has a range of years.  */
struct timeval
{
  __time_t tv_sec;		/* Seconds.  */
  __suseconds_t tv_usec;	/* Microseconds.  */
};

/* A version of 'struct timeval' with `long` time_t
   and suseconds_t.  */
struct __timeval_long
{
  long tv_sec;		/* Seconds.  */
  long tv_usec;		/* Microseconds.  */
};
#endif
