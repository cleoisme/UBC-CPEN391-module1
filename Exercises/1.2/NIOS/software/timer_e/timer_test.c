


/*
 * Create on Jan 17 by Yueyue Zhang
 *
 * Copy-paste from the instruction to test timer
 *
 */


#include <stdio.h>
#include "sys/alt_timestamp.h"
#include "sys/alt_alarm.h"

void timer_test(void) {
	int freq;
	int cycles;
	float duration;
	int ticks_start;
	int ticks_end;
	int ticks_per_s;
	int ticks_duration;
	int timer_period;
	int status;
	int done;

	printf("Timers\n");

	//test on sys_clock_timer
	printf(" Sys Clock Timer\n");

	ticks_per_s = alt_ticks_per_second();
	printf(" Tick Freq: %d\n", ticks_per_s);
	printf(" Recording starting ticks\n");

	ticks_start = alt_nticks();
	printf(" Sleeping for 5 seconds\n");
	usleep(5000000);
	printf(" Recording ending ticks\n");

	ticks_end = alt_nticks();
	ticks_duration = ticks_end - ticks_start;
	duration = (float) ticks_duration / (float) ticks_per_s;
	printf(" The program slept for %d ticks (%f seconds)\n\n", ticks_duration, duration);

	//test on timestamp_timer
	printf(" Timestamp Timer\n");
	freq = alt_timestamp_freq();
	printf(" CPU Freq: %d\n", freq);
	printf(" Resetting Timestamp timer\n");

	alt_timestamp_start();
	printf(" ...Timing the print of this statement...\n");

	cycles = alt_timestamp();
	duration = (float) cycles / (float) freq;
	printf(" It took %d cycles (%f seconds) to print the statement\n\n", cycles, duration);

}
