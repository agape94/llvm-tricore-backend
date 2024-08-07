/* Benchmarks
   Copyright (C) 2001 Free Software Foundation, Inc.
   Written by Stephane Carrez (stcarrez@worldnet.fr)    

This file is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2, or (at your option) any
later version.

In addition to the permissions in the GNU General Public License, the
Free Software Foundation gives you unlimited permission to link the
compiled version of this file with other programs, and to distribute
those programs without any restriction coming from the use of this
file.  (The General Public License restrictions do apply in other
respects; for example, they cover modification of the file, and
distribution when not linked into another program.)

This file is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; see the file COPYING.  If not, write to
the Free Software Foundation, 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  */

#ifndef _BENCHS_H
#define _BENCHS_H


#include "param.h"
#include "ports.h"

struct bench
{
  unsigned short b_start;

  unsigned short b_stop;

  unsigned short b_correction;

  unsigned short b_delta;
};
typedef struct bench bench_t;

extern void
bench_init (bench_t *b);

extern void bench_start (bench_t *b);

extern __inline__ void
bench_start (bench_t *b)
{
  b->b_start = 0;
  // b->b_start = get_timer_counter ();
}

extern void bench_stop (bench_t *b);

extern __inline__ void
bench_stop (bench_t *b)
{
  b->b_stop = 100;
  // b->b_stop = get_timer_counter ();
}

extern void
bench_report (bench_t *b, const char *msg, ...);


extern void
bench_empty (bench_t *b);

#endif