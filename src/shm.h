/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2020.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>

/* Hard-coded keys for IPC objects */

#define SHM_RESULTS "shm_results"
#define SHM_SEMAPHORE "shm_sempahore"
#define SHM_SIZE 4096
#ifndef BUF_SIZE	  /* Allow "cc -D" to override definition */
#define BUF_SIZE 1024 /* Size of transfer buffer */
#endif

struct shmseg {			/* Defines structure of shared memory segment */
	int cnt;			/* Number of bytes used in 'buf' */
	char buf[BUF_SIZE]; /* Data being transferred */
};