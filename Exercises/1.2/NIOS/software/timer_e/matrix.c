

/*
 * Create on Jan 17 by Yueyue Zhang
 *
 * Copy-paste from the instruction to do the matrix multiply
 *
 */

#include <stdio.h>
#include "sys/alt_timestamp.h"
#define SIZE 100

void matrix(void) {
   int a[SIZE][SIZE];
   int b[SIZE][SIZE];
   int c[SIZE][SIZE];

   int start_time;
   int end_time;
   int sum;

   int i, j, k;

   start_time = (int)alt_timestamp();

   for(i=0; i<SIZE; i++) {
   	  for(j=0; j<SIZE; j++) {
   		  sum = 0;
   		  for(k=0; k<SIZE; k++) {
   			  sum = sum + a[i][k]*b[k][j];
   		  }
   		  c[i][j] = sum;
   	  }
   }

   end_time =(int)alt_timestamp();

   printf("time taken: %d clock ticks \n", end_time - start_time);
   printf("            %f seconds \n", (float)(end_time-start_time)/(float)alt_timestamp_freq());

}
