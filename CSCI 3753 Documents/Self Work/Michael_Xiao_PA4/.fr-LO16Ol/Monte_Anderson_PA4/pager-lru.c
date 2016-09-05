/*
 * File: pager-lru.c
 * Author:       Andy Sayler
 *               http://www.andysayler.com
 * Adopted From: Dr. Alva Couch
 *               http://www.cs.tufts.edu/~couch/
 *
 * Project: CSCI 3753 Programming Assignment 4
 * Create Date: Unknown
 * Modify Date: 2012/04/03
 * Description:
 * 	This file contains an lru pageit
 *      implmentation.
 */

#include <stdio.h> 
#include <stdlib.h>
#include "simulator.h"

void pageit(Pentry q[MAXPROCESSES]) { 
    
    /* This file contains the stub for an LRU pager */
    /* You may need to add/remove/modify any part of this file */

    /* Static vars */
    static int initialized = 0;
    static int tick = 1; // artificial time
    static int timestamps[MAXPROCESSES][MAXPROCPAGES];

    /* Local vars */
    
    //Here, we added ticktemp to keep track of the ticks for our temporary pages.
    int proctmp;
    int pagetmp;
    int proc;
    int pc;
    int page;
    int index;
    int tickTemp;   
    
    /* initialize static vars on first run */
    if(!initialized){
		
		for(proctmp=0; proctmp < MAXPROCESSES; proctmp++)
		{
			for(pagetmp=0; pagetmp < MAXPROCPAGES; pagetmp++)
			{
				timestamps[proctmp][pagetmp] = 0; 
			}
		}
		initialized = 1;
	}
			
		for(proc=0; proc<MAXPROCESSES; proc++) 
		{ 
		/* Is process active? */
			if(q[proc].active) 
			{
				pc = q[proc].pc; 		        // Program counter for process
				page = pc/PAGESIZE; 		// Page the program counter needs
				
				if(!q[proc].pages[page]) 
				{
											//This is where we actually page in the new page, by calling (proc,page).
					if(!pagein(proc,page)) 
					{

											//Ticktemp keeps track of the tempory tick of entire system time.
						tickTemp = tick;
						for(index=0; index < MAXPROCPAGES; index++) 
						{
											
							if(timestamps[proc][index]>0 && timestamps[proc][index] < tickTemp)
								{
									tickTemp = timestamps[proc][index];
									pagetmp = index;
									//printf("| %i \n ",page); //Used for testing output to visualize what is being output.
								} 
						}
						
											//We pageout the old (lease recently used) pages by setting their pagetmp to 0 and taking out the temp page we currently had in place.
											
						if(pageout(proc,pagetmp))
							{
								timestamps[proc][pagetmp]=0;
							}
					}
				}
											//Else, we set the timestamp for the proc,page at the current tick.
				else
				{
					timestamps[proc][page]=tick;
				}	    
			}
		} 
    /* TODO: Implement LRU Paging */
    //fprintf(stderr, "pager-lru not yet implemented. Exiting...\n");
    //exit(EXIT_FAILURE);

    /* advance time for next pageit iteration */
    tick++;
} 
