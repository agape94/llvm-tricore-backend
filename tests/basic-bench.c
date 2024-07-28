/* Benchmark for some basic type operations
   Copyright (C) 2001, 2002 Free Software Foundation, Inc.
   Written by Stephane Carrez (stcarrez@nerim.fr)       

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


#include "benchs.h"

/* Forward declarations.  */
void bench_bits (bench_t *b);
void bench_shift_right_1 (bench_t *b);
void bench_shift_left_1 (bench_t *b);
void bench_shift_left_n (bench_t *b);
void bench_char (bench_t *b);
void bench_short (bench_t *b);
void bench_long (bench_t *b);
void bench_longlong (bench_t *b);


/* Use global variables to prevent Gcc from computing values
   produced by benchmark operations.  */
unsigned long long global_value = -1;
unsigned long long scratch;
unsigned long global_shift;
long l = 0x1234567;
long long ll = 0x12345670000LL;
short s = 0x1234;
char c = 0x12;


void
bench_bits (bench_t *b)
{
  unsigned short cnt;
  unsigned char v_char, i_char;
  unsigned short v_short, i_short;
  unsigned long v_long, i_long;
  unsigned long long v_longlong, i_longlong;
  
  cnt = 0;
  v_char = global_value;
  bench_start (b);
  for (i_char = 1 << 7; i_char; i_char >>= 1)
    if (v_char & i_char)
      cnt++;
  bench_stop (b);
  bench_report (b, "Bit count (char, %d bits set)", (long) cnt);

  cnt = 0;
  v_short = global_value;
  bench_start (b);
  for (i_short = 1 << 15; i_short; i_short >>= 1)
    if (v_short & i_short)
      cnt++;
  bench_stop (b);
  bench_report (b, "Bit count (short, %d bits set)", (long) cnt);

  cnt = 0;
  v_long = global_value;
  bench_start (b);
  for (i_long = 1L << 31; i_long; i_long >>= 1)
    if (v_long & i_long)
      cnt++;
  bench_stop (b);
  bench_report (b, "Bit count (long, %d bits set)", (long) cnt);

  cnt = 0;
  v_longlong = global_value;
  bench_start (b);
  for (i_longlong = 1LL << 63; i_longlong; i_longlong >>= 1)
    if (v_longlong & i_longlong)
      cnt++;
  bench_stop (b);
  bench_report (b, "Bit count (long long, %d bits set)", (long) cnt);
}

void
bench_shift_right_1 (bench_t *b)
{
  unsigned char v_char;
  unsigned short v_short;
  unsigned long v_long;
  unsigned long long v_longlong;
  
  v_char = global_value;
  bench_start (b);
  v_char >>= 1;
  bench_stop (b);
  bench_report (b, "Shift right 1 (char)");

  scratch += v_char;

  v_short = global_value;
  bench_start (b);
  v_short >>= 1;
  bench_stop (b);
  bench_report (b, "Shift right 1 (short)");

  scratch += v_short;
  
  v_long = global_value;
  bench_start (b);
  v_long >>= 1;
  bench_stop (b);
  bench_report (b, "Shift right 1 (long)");

  scratch += v_long;
  
  v_longlong = global_value;
  bench_start (b);
  v_longlong >>= 1;
  bench_stop (b);
  bench_report (b, "Shift right 1 (long long)");

  scratch += v_longlong;
}

void
bench_shift_left_1 (bench_t *b)
{
  unsigned char v_char;
  unsigned short v_short;
  unsigned long v_long;
  unsigned long long v_longlong;
  
  v_char = global_value;
  bench_start (b);
  v_char <<= 1;
  bench_stop (b);
  bench_report (b, "Shift left 1 (char)");

  scratch += v_char;

  v_short = global_value;
  bench_start (b);
  v_short <<= 1;
  bench_stop (b);
  bench_report (b, "Shift left 1 (short)");

  scratch += v_short;
  
  v_long = global_value;
  bench_start (b);
  v_long <<= 1;
  bench_stop (b);
  bench_report (b, "Shift left 1 (long)");

  scratch += v_long;
  
  v_longlong = global_value;
  bench_start (b);
  v_longlong <<= 1;
  bench_stop (b);
  bench_report (b, "Shift left 1 (long long)");

  scratch += v_longlong;
}

void
bench_shift_left_n (bench_t *b)
{
  unsigned char v_char;
  unsigned short v_short;
  unsigned long v_long;
  unsigned long long v_longlong;
  
  v_char = global_value;
  bench_start (b);
  v_char <<= global_shift;
  bench_stop (b);
  bench_report (b, "Shift left non-const (N=%d, char)", global_shift);

  scratch += v_char;

  v_short = global_value;
  bench_start (b);
  v_short <<= global_shift;
  bench_stop (b);
  bench_report (b, "Shift left non-const (N=%d, short)", global_shift);

  scratch += v_short;
  
  v_long = global_value;
  bench_start (b);
  v_long <<= global_shift;
  bench_stop (b);
  bench_report (b, "Shift left non-const (N=%d, long)", global_shift);

  scratch += v_long;
  
  v_longlong = global_value;
  bench_start (b);
  v_longlong <<= global_shift;
  bench_stop (b);
  bench_report (b, "Shift left non-const (N=%d, long long)", global_shift);

  scratch += v_longlong;
}

void
bench_char (bench_t *b)
{
  bench_start (b);
  c = c * c;
  bench_stop (b);
  bench_report (b, "Char mul (%d)", (long) c);

  bench_start (b);
  c = c + c;
  bench_stop (b);
  bench_report (b, "Char add (%d)", (long) c);

  bench_start (b);
  c = -c;
  bench_stop (b);
  bench_report (b, "Char neg (%d)", (long) c);

  bench_start (b);
  c = c / 3;
  bench_stop (b);
  bench_report (b, "Char div (%d)", (long) c);
}

void
bench_short (bench_t *b)
{
  bench_start (b);
  s = s * s;
  bench_stop (b);
  bench_report (b, "Short mul (%d)", (long) s);

  bench_start (b);
  s = s + s;
  bench_stop (b);
  bench_report (b, "Short add (%d)", (long) s);

  bench_start (b);
  s = -s;
  bench_stop (b);
  bench_report (b, "Short neg (%d)", (long) s);

  bench_start (b);
  s = s / 3;
  bench_stop (b);
  bench_report (b, "Short div (%d)", (long) s);
}

void
bench_long (bench_t *b)
{
  bench_start (b);
  l = l * l;
  bench_stop (b);
  bench_report (b, "Long mul (%d)", (long) l);

  bench_start (b);
  l = l + l;
  bench_stop (b);
  bench_report (b, "Long add (%d)", (long) l);

  bench_start (b);
  l = -l;
  bench_stop (b);
  bench_report (b, "Long neg (%d)", (long) l);

  bench_start (b);
  l = l / 3;
  bench_stop (b);
  bench_report (b, "Long div (%d)", (long) l);
}

void
bench_longlong (bench_t *b)
{
  bench_start (b);
  ll = ll * ll;
  bench_stop (b);
  bench_report (b, "Long long mul (%d)", (long) l);

  bench_start (b);
  ll = ll + ll;
  bench_stop (b);
  bench_report (b, "Long long add (%d)", (long) l);

  bench_start (b);
  ll = -ll;
  bench_stop (b);
  bench_report (b, "Long long neg (%d)", (long) l);

  bench_start (b);
  ll = ll / 3;
  bench_stop (b);
  bench_report (b, "Long long div (%d)", (long) l);
}

/* Main, run the benchmarks.  */
int
main ()
{
  struct bench b;
  
  bench_init (&b);
  bench_char (&b);
  bench_short (&b);
  bench_long (&b);
  bench_longlong (&b);
  bench_shift_right_1 (&b);
  bench_shift_left_1 (&b);
  bench_bits (&b);
  return 0;
}
