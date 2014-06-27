/*
 * monitoring-new.c
 *
 *  Created on: 11.06.2014
 *      Author: hpcneich
 */

#include "http-post.h"
#include "monitoring-new.h"

struct timespec tim, tim2;

time_t sec_to_sleep = 0;
long nanos_to_sleep = 999999999; //must be less than 10e8 !!!

double get_cpu_usage(void) {

	tim.tv_sec = sec_to_sleep;
	tim.tv_nsec = nanos_to_sleep;
	unsigned int tog = 0;
	jiff cpu_use[2], cpu_nic[2], cpu_sys[2], cpu_idl[2], cpu_iow[2], cpu_xxx[2],
			cpu_yyy[2], cpu_zzz[2];
	jiff duse, dsys, didl, diow, dstl, Div, divo2;
	int debt = 0;

	getprocstat(cpu_use, cpu_nic, cpu_sys, cpu_idl, cpu_iow, cpu_xxx, cpu_yyy,
			cpu_zzz);

	tog = !tog;

//	usleep(sleep_time);
//	if (nanosleep(&tim, &tim2))
//		printf("error in nanosleep!");
	sleep(1);

	getprocstat(cpu_use + tog, cpu_nic + tog, cpu_sys + tog, cpu_idl + tog,
			cpu_iow + tog, cpu_xxx + tog, cpu_yyy + tog, cpu_zzz + tog);

	duse = cpu_use[tog] - cpu_use[!tog] + cpu_nic[tog] - cpu_nic[!tog];
	dsys = cpu_sys[tog] - cpu_sys[!tog] + cpu_xxx[tog] - cpu_xxx[!tog]
			+ cpu_yyy[tog] - cpu_yyy[!tog];
	didl = cpu_idl[tog] - cpu_idl[!tog];
	diow = cpu_iow[tog] - cpu_iow[!tog];
	dstl = cpu_zzz[tog] - cpu_zzz[!tog];

	Div = duse + dsys + didl + diow + dstl;
	divo2 = Div / 2UL;
//	printf("duse: %lld dsys: %lld didl: %lld diow: %lld dstl: %lld \n", duse,
//			dsys, didl, diow, dstl);

	if (debt) {
		didl = (int) didl + debt;
		debt = 0;
	}
	if ((int) didl < 0) {
		debt = (int) didl;
		didl = 0;
	}

	// usage for userspace applications

	// usage for systemspace applications
//
#if 1
//	printf("(100 * %llu + %llu) / %llu \n", duse, divo2, Div);
	jidd userusage = ((100 * (jidd) duse + (jidd) divo2) / (jidd) Div);
	return (double) userusage;

#else
	jidd systemusage = ((100 * (jidd) dsys + (jidd) divo2) / (jidd) Div);
	return (double) systemusage;
#endif
}

void getprocstat(jiff *restrict cuse, jiff *restrict cice, jiff *restrict csys,
		jiff *restrict cide, jiff *restrict ciow, jiff *restrict cxxx,
		jiff *restrict cyyy, jiff *restrict czzz) {
	static char buff[READBUFFER_SIZE + 1];

	static int fd;
	const char* b;

	if (fd) {
		lseek(fd, 0L, SEEK_SET);
	} else {
		fd = open("/proc/stat", O_RDONLY, 0);
		if (fd == -1)
			printf("Error when opening /proc/stat");
	}

	read(fd, buff, READBUFFER_SIZE - 1);

	b = strstr(buff, "cpu ");
	if (b)
		sscanf(b, "cpu %llu %llu %llu %llu %llu %llu %llu %llu", cuse, cice,
				csys, cide, ciow, cxxx, cyyy, czzz);
}

int get_mem_usage(void) {

	unsigned long mfre, mtot;

	getprocmeminfo(&mfre, &mtot);
	double frac = (double) mfre / (double) mtot;

	return (int) (frac * 100.0);
}

void getprocmeminfo(unsigned long *restrict mfre, unsigned long *restrict mtot) {

	static char buff[READBUFFER_SIZE + 1];
	tim.tv_sec = sec_to_sleep;
	tim.tv_nsec = nanos_to_sleep;

	static int fd;
	const char* b;
	if (fd) {
		lseek(fd, 0L, SEEK_SET);
	} else {
		fd = open("/proc/meminfo", O_RDONLY, 0);
		if (fd == -1)
			printf("Error when opening /proc/meminfo");
	}
	read(fd, buff, READBUFFER_SIZE - 1);

	b = strstr(buff, "MemFree: ");
	if (b)
		sscanf(b, "MemFree: %lu kB", mfre);

	b = strstr(buff, "MemTotal: ");
	if (b)
		sscanf(b, "MemTotal: %lu kB", mtot);
//	usleep(sleep_time);
//	nanosleep(&tim, &tim2);
	sleep(1);
}
