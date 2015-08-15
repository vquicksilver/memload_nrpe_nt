/******************************************************************************
 *
 * Program: Memory Load plugin for Nagios (nrpe_nt).
 * License: GPL
 *
 * Designed as a Nagios Plugin (based on plugins by Ethan Galstad).
 * Copyright (C) 2004 Stephen Strudwick (Pipex Communications, sas@pipex.net)
 *
 * License Information:
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *****************************************************************************/

/*
 * Some lines of code are based or taken from the NetSaintService for NT
 * which is Copyright (c) 2000 Jan Christian Kaldestad which as far as I can tell
 * is freeware.
 */

/*
 * Original version by GX Networks, Stephen Strudwick.
 * Modified by Víctor Enríquez Miguel, victor.enriquez@hispaweb.com for working on 64 bits.
 */

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <tchar.h>

#define OK 0
#define Warning 1
#define Critical 2
#define Unknown 3

int _tmain(int argc, char* argv[]){

	unsigned int warning_threshold;
	unsigned int critical_threshold;	
	MEMORYSTATUSEX statex;

	/* Get memory values from the OS */
	statex.dwLength = sizeof(statex);
	GlobalMemoryStatusEx(&statex);

	/* Get Values from command line */
	if (argc != 3) {
		printf ("NRPE_NT Plugin, MEMORY Load, Hispaweb, Victor Enriquez\n");
      		printf ("Based on NRPE_NT Plugin by GX Networks, Stephen Strudwick\n");
      		printf ("usage: memload warning threshold(%%) critical threshold(%%)\n");
      		printf ("example: 70 90\n");
      		return Unknown;
	}

    	warning_threshold = atoi(argv[1]);
	critical_threshold = atoi(argv[2]);

	statex.ullTotalPageFile = statex.ullTotalPageFile/1048576;
	statex.ullAvailPhys = statex.ullAvailPhys/1048576;
	statex.ullTotalPhys = statex.ullTotalPhys/1048576;
	statex.ullAvailPageFile = statex.ullAvailPageFile/1048576;

	if (statex.ullTotalPageFile <= 0) {
		printf("Unknown totalswap");
		return Unknown;
	}
  
	if (statex.ullAvailPhys <= 0) {
		printf("Unknown freephys");
    		return Unknown;
	}
 
	if (statex.ullTotalPhys <= 0) {
		printf("Unknown totalphys");
      		return Unknown;
	}
  
	if (statex.ullAvailPageFile <= 0) {
		printf("Unknown freeswap");
      		return Unknown;
	}

	double mean_used = 
		((double)((statex.ullTotalPhys)-(statex.ullAvailPhys)) / 
			(double)statex.ullTotalPhys) * 100;

	double mean_free = 
		((double)statex.ullAvailPhys / 
			(double)statex.ullTotalPhys) * 100;

	double mean_pused = 
		((double)((statex.ullTotalPageFile)-(statex.ullAvailPageFile)) / 
			(double)statex.ullTotalPageFile) * 100;

	double mean_pfree = 
		((double)statex.ullAvailPageFile / 
			(double)statex.ullTotalPageFile) * 100;

	_tprintf("Mem: %I64d MB (%d%%) / %I64d MB (%d%%) ", 
		((statex.ullTotalPhys)-(statex.ullAvailPhys)), 
		(int)floor(mean_used), 
		(statex.ullTotalPhys), 
		(int)floor(mean_free));

	/* hack, on some systems the paged memory values are reversed !?? 
 	 * So dont print out anything if its fscked */
	if (mean_pused >= 0 && mean_pused <= 100) {
		_tprintf("Paged Mem: %I64d MB (%d%%) / %I64d MB (%d%%)\n", 
		((statex.ullTotalPageFile)-(statex.ullAvailPageFile)), 
		(int)floor(mean_pused), 
		(statex.ullTotalPageFile), 
		(int)floor(mean_pfree));
    	}

	if (mean_used >= critical_threshold)
		return Critical;

	if (mean_used >= warning_threshold)
		return Warning;

	printf("Plugin status OK\n");

	return OK;
}
