/**
    Copyright (C) powturbo 2015-2023
    SPDX-License-Identifier: GPL v2 License

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

    - email    : powturbo [AT] gmail.com
    - github   : https://github.com/powturbo
    - homepage : https://sites.google.com/site/powturbo/
    - twitter  : https://twitter.com/powturbo

    TurboRLE - "Most efficient and fastest Run Length Encoding"
**/      
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/device.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#ifdef __APPLE__
#include <sys/malloc.h>
#else
#include <malloc.h>
#endif
  #ifdef _MSC_VER
#include "vs/getopt.h"
  #else
#include <getopt.h> 
#endif
#include "trle.h"
#include "time_.h"

#include "mrle.c"

#ifdef __clang__
#pragma clang diagnostic ignored "-Wdeclaration-after-statement"
#endif

#define ID_MEMCPY 8

const float data[] = {
    9.739696383476257324e-01,
    9.132320880889892578e-01,
    8.655097484588623047e-01,
    8.232104182243347168e-01,
    7.462038993835449219e-01,
    6.420824527740478516e-01
};

void libmemcpy(unsigned char *dst, unsigned char *src, int len) {
  memcpy(dst, src, len);
}

unsigned bench(unsigned char *in, unsigned n, unsigned char *out, unsigned char *cpy, int id) { 
  unsigned l = 0;
  memrcpy(cpy,in,n); 
  switch(id) {
    case 1: TMBENCH("\ntrle   ", l=trlec(  in, n, out) ,n); 						 printf("%10u %5.1f%%", l, (double)l*100.0/n); TMBENCH("",trled(  out, l, cpy, n) ,n); break;
    case 2: TMBENCH("\nsrle 0 ", l=srlec(  in, n, out) ,n); 						 printf("%10u %5.1f%%", l, (double)l*100.0/n); TMBENCH("",srled(  out, l, cpy, n) ,n); break;      
    case 3: TMBENCH("\nsrle 8 ", l=srlec8( in, n, out, 0xda) ,n); 				     printf("%10u %5.1f%%", l, (double)l*100.0/n); TMBENCH("",srled8( out, l, cpy, n, 0xda) ,n); break;      
    case 4: TMBENCH("\nsrle 16", l=srlec16(in, n, out, 0xdada) ,n); 				 printf("%10u %5.1f%%", l, (double)l*100.0/n); TMBENCH("",srled16(out, l, cpy, n, 0xdada) ,n); break;      
    case 5: TMBENCH("\nsrle 32", l=srlec32(in, n, out, 0xdadadadau) ,n); 			 printf("%10u %5.1f%%", l, (double)l*100.0/n); TMBENCH("",srled32(out, l, cpy, n, 0xdadadadau) ,n); break;      
    case 6: TMBENCH("\nsrle 64", l=srlec64(in, n, out, 0xdadadadadadadadaull) ,n); 	 printf("%10u %5.1f%%", l, (double)l*100.0/n); TMBENCH("",srled64(out, l, cpy, n, 0xdadadadadadadadaull) ,n); break;      
    case 7: TMBENCH("\nmrle   ", l=mrlec(  in, n, out), n);						 	 printf("%10u %5.1f%%", l, (double)l*100.0/n); TMBENCH("",mrled(  out, cpy, n), n); break;
    case 8: TMBENCH("\nmemcpy ", libmemcpy(in,out,n) ,n);  							 printf("%10u %5.1f%%", n, (double)100.0); return n;
	default: return l;
  }
  memcheck(in,n,cpy);
  return l;
}

int main(int argc, char* argv[]) {
  unsigned trips = 4, cmp = 1, b = 1 << 30, esize = 4, lz = 0, fno, id = 0;

  // Allocate memory for input and output
  unsigned n = sizeof(data) / sizeof(data[0]);
  float *in = (float*)malloc(n * sizeof(float));
  unsigned char *out = (unsigned char*)malloc(n * 4 / 3 + 1024);
  unsigned char *cpy = NULL;

  if (!in || !out) {
    fprintf(stderr, "Memory allocation error\n");
    exit(-1);
  }

  for (int i = 0; i < n; i++) {
    in[i] = data[i];
  }

  tm_init(trips, 1);

  if (!id) {
    printf("function  E MB/s   size      ratio   D MB/s");      
    for(int i = 1; i <= ID_MEMCPY; i++) {
      bench(in, n, out, cpy, i);
    }
  } else {
    bench(in, n, out, cpy, id);    
  }

  // Free allocated memory
  free(in);
  free(out);
  if (cpy) {
    free(cpy);
  }

  return 0;
}
