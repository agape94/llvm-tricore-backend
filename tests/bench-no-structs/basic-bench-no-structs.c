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


#include "benchs-no-structs.h"

/* Forward declarations.  */
// void bench_bits (unsigned short *b_start, unsigned short *b_stop, unsigned short *b_correction, unsigned short *b_delta);
// void bench_shift_right_1 (unsigned short *b_start, unsigned short *b_stop, unsigned short *b_correction, unsigned short *b_delta);
// void bench_shift_left_1 (unsigned short *b_start, unsigned short *b_stop, unsigned short *b_correction, unsigned short *b_delta);
// void bench_shift_left_n (unsigned short *b_start, unsigned short *b_stop, unsigned short *b_correction, unsigned short *b_delta);
void bench_char (unsigned short *b_start, unsigned short *b_stop, unsigned short *b_correction, unsigned short *b_delta);
// void bench_short (unsigned short *b_start, unsigned short *b_stop, unsigned short *b_correction, unsigned short *b_delta);
// void bench_long (unsigned short *b_start, unsigned short *b_stop, unsigned short *b_correction, unsigned short *b_delta);
// void bench_longlong (unsigned short *b_start, unsigned short *b_stop, unsigned short *b_correction, unsigned short *b_delta);


/* Use global variables to prevent Gcc from computing values
   produced by benchmark operations.  */
unsigned long long global_value = -1;
unsigned long long scratch;
unsigned long global_shift;
long l = 0x1234567;
long long ll = 0x12345670000LL;
short s = 0x1234;
char c = 0x12;


// void
// bench_bits (unsigned short *b_start, unsigned short *b_stop, unsigned short *b_correction, unsigned short *b_delta)
// {
//   unsigned short cnt;
//   unsigned char v_char, i_char;
//   unsigned short v_short, i_short;
//   unsigned long v_long, i_long;
//   unsigned long long v_longlong, i_longlong;
  
//   cnt = 0;
//   v_char = global_value;
//   bench_start (b_start);
//   for (i_char = 1 << 7; i_char; i_char >>= 1)
//     if (v_char & i_char)
//       cnt++;
//   bench_stop (b_stop);
//   // bench_report (b, "Bit count (char, %d bits set)", (long) cnt);

//   cnt = 0;
//   v_short = global_value;
//   bench_start (b_start);
//   for (i_short = 1 << 15; i_short; i_short >>= 1)
//     if (v_short & i_short)
//       cnt++;
//   bench_stop (b_stop);
//   // // bench_report (b, "Bit count (short, %d bits set)", (long) cnt);

//   cnt = 0;
//   v_long = global_value;
//   bench_start (b_start);
//   for (i_long = 1L << 31; i_long; i_long >>= 1)
//     if (v_long & i_long)
//       cnt++;
//   bench_stop (b_stop);
//   // bench_report (b, "Bit count (long, %d bits set)", (long) cnt);

//   cnt = 0;
//   v_longlong = global_value;
//   bench_start (b_start);
//   for (i_longlong = 1LL << 63; i_longlong; i_longlong >>= 1)
//     if (v_longlong & i_longlong)
//       cnt++;
//   bench_stop (b_stop);
//   // bench_report (b, "Bit count (long long, %d bits set)", (long) cnt);
// }

// void
// bench_shift_right_1 (unsigned short *b_start, unsigned short *b_stop, unsigned short *b_correction, unsigned short *b_delta)
// {
//   unsigned char v_char;
//   unsigned short v_short;
//   unsigned long v_long;
//   unsigned long long v_longlong;
  
//   v_char = global_value;
//   bench_start (b_start);
//   v_char >>= 1;
//   bench_stop (b_stop);
//   // bench_report (b, "Shift right 1 (char)");

//   scratch += v_char;

//   v_short = global_value;
//   bench_start (b_start);
//   v_short >>= 1;
//   bench_stop (b_stop);
//   // bench_report (b, "Shift right 1 (short)");

//   scratch += v_short;
  
//   v_long = global_value;
//   bench_start (b_start);
//   v_long >>= 1;
//   bench_stop (b_stop);
//   // bench_report (b, "Shift right 1 (long)");

//   scratch += v_long;
  
//   v_longlong = global_value;
//   bench_start (b_start);
//   v_longlong >>= 1;
//   bench_stop (b_stop);
//   // bench_report (b, "Shift right 1 (long long)");

//   scratch += v_longlong;
// }

// void
// bench_shift_left_1 (unsigned short *b_start, unsigned short *b_stop, unsigned short *b_correction, unsigned short *b_delta)
// {
//   unsigned char v_char;
//   unsigned short v_short;
//   unsigned long v_long;
//   unsigned long long v_longlong;
  
//   v_char = global_value;
//   bench_start (b_start);
//   v_char <<= 1;
//   bench_stop (b_stop);
//   // bench_report (b, "Shift left 1 (char)");

//   scratch += v_char;

//   v_short = global_value;
//   bench_start (b_start);
//   v_short <<= 1;
//   bench_stop (b_stop);
//   // bench_report (b, "Shift left 1 (short)");

//   scratch += v_short;
  
//   v_long = global_value;
//   bench_start (b_start);
//   v_long <<= 1;
//   bench_stop (b_stop);
//   // bench_report (b, "Shift left 1 (long)");

//   scratch += v_long;
  
//   v_longlong = global_value;
//   bench_start (b_start);
//   v_longlong <<= 1;
//   bench_stop (b_stop);
//   // bench_report (b, "Shift left 1 (long long)");

//   scratch += v_longlong;
// }

// void
// bench_shift_left_n (unsigned short *b_start, unsigned short *b_stop, unsigned short *b_correction, unsigned short *b_delta)
// {
//   unsigned char v_char;
//   unsigned short v_short;
//   unsigned long v_long;
//   unsigned long long v_longlong;
  
//   v_char = global_value;
//   bench_start (b_start);
//   v_char <<= global_shift;
//   bench_stop (b_stop);
//   // bench_report (b, "Shift left non-const (N=%d, char)", global_shift);

//   scratch += v_char;

//   v_short = global_value;
//   bench_start (b_start);
//   v_short <<= global_shift;
//   bench_stop (b_stop);
//   // bench_report (b, "Shift left non-const (N=%d, short)", global_shift);

//   scratch += v_short;
  
//   v_long = global_value;
//   bench_start (b_start);
//   v_long <<= global_shift;
//   bench_stop (b_stop);
//   // bench_report (b, "Shift left non-const (N=%d, long)", global_shift);

//   scratch += v_long;
  
//   v_longlong = global_value;
//   bench_start (b_start);
//   v_longlong <<= global_shift;
//   bench_stop (b_stop);
//   // bench_report (b, "Shift left non-const (N=%d, long long)", global_shift);

//   scratch += v_longlong;
// }

void
bench_char (unsigned short *b_start, unsigned short *b_stop, unsigned short *b_correction, unsigned short *b_delta)
{
  bench_start (b_start);
  c = c * c;
  bench_stop (b_stop);
  // bench_report (b, "Char mul (%d)", (long) c);

  bench_start (b_start);
  c = c + c;
  bench_stop (b_stop);
  // bench_report (b, "Char add (%d)", (long) c);

  bench_start (b_start);
  c = -c;
  bench_stop (b_stop);
  // bench_report (b, "Char neg (%d)", (long) c);

  bench_start (b_start);
  c = c / 3;
  bench_stop (b_stop);
  // bench_report (b, "Char div (%d)", (long) c);
}

// void
// bench_short (unsigned short *b_start, unsigned short *b_stop, unsigned short *b_correction, unsigned short *b_delta)
// {
//   bench_start (b_start);
//   s = s * s;
//   bench_stop (b_stop);
//   // bench_report (b, "Short mul (%d)", (long) s);

//   bench_start (b_start);
//   s = s + s;
//   bench_stop (b_stop);
//   // bench_report (b, "Short add (%d)", (long) s);

//   bench_start (b_start);
//   s = -s;
//   bench_stop (b_stop);
//   // bench_report (b, "Short neg (%d)", (long) s);

//   bench_start (b_start);
//   s = s / 3;
//   bench_stop (b_stop);
//   // bench_report (b, "Short div (%d)", (long) s);
// }

// void
// bench_long (unsigned short *b_start, unsigned short *b_stop, unsigned short *b_correction, unsigned short *b_delta)
// {
//   bench_start (b_start);
//   l = l * l;
//   bench_stop (b_stop);
//   // bench_report (b, "Long mul (%d)", (long) l);

//   bench_start (b_start);
//   l = l + l;
//   bench_stop (b_stop);
//   // bench_report (b, "Long add (%d)", (long) l);

//   bench_start (b_start);
//   l = -l;
//   bench_stop (b_stop);
//   // bench_report (b, "Long neg (%d)", (long) l);

//   bench_start (b_start);
//   l = l / 3;
//   bench_stop (b_stop);
//   // bench_report (b, "Long div (%d)", (long) l);
// }

// void
// bench_longlong (unsigned short *b_start, unsigned short *b_stop, unsigned short *b_correction, unsigned short *b_delta)
// {
//   bench_start (b_start);
//   ll = ll * ll;
//   bench_stop (b_stop);
//   // bench_report (b, "Long long mul (%d)", (long) l);

//   bench_start (b_start);
//   ll = ll + ll;
//   bench_stop (b_stop);
//   // bench_report (b, "Long long add (%d)", (long) l);

//   bench_start (b_start);
//   ll = -ll;
//   bench_stop (b_stop);
//   // bench_report (b, "Long long neg (%d)", (long) l);

//   bench_start (b_start);
//   ll = ll / 3;
//   bench_stop (b_stop);
//   // bench_report (b, "Long long div (%d)", (long) l);
// }

/* Main, run the benchmarks.  */
int
main ()
{
  unsigned short b_start = 0;
  unsigned short b_stop = 0;
  unsigned short b_correction = 0;
  unsigned short b_delta = 0;

  // unsigned short *b_start_ptr = &b_start;
  // unsigned short *b_stop_ptr = &b_stop;
  // unsigned short *b_correction_ptr = &b_correction;
  // unsigned short *b_delta_ptr = &b_delta;
  // b_start_ptr++;
  // b_stop_ptr--;
  // b_correction_ptr+=2;
  // b_delta_ptr+=100;

  
  bench_init (&b_start, &b_stop, &b_correction, &b_delta);
  bench_char (&b_start, &b_stop, &b_correction, &b_delta);
  // bench_short (&b_start, &b_stop, &b_correction, &b_delta);
  // bench_long (&b_start, &b_stop, &b_correction, &b_delta);
  // bench_longlong (&b_start, &b_stop, &b_correction, &b_delta);
  // bench_shift_right_1 (&b_start, &b_stop, &b_correction, &b_delta);
  // bench_shift_left_1 (&b_start, &b_stop, &b_correction, &b_delta);
  // bench_bits (&b_start, &b_stop, &b_correction, &b_delta);
  return 0;
}
