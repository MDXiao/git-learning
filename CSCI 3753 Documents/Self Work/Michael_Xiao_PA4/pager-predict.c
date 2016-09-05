/*
 * File: pager-predict.c
 * Author:       Andy Sayler
 *               http://www.andysayler.com
 * Adopted From: Dr. Alva Couch
 *               http://www.cs.tufts.edu/~couch/
 *
 * Project: CSCI 3753 Programming Assignment 4
 * Create Date: Unknown
 * Modify Date: 2012/04/03
 * Description:
 * 	This file contains an predictive pageit
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
				pc = q[proc].pc;
				page = pc/PAGESIZE; //Keeps track of the page number (divided by the page size)
				//Which is 128
				if(!q[proc].pages[page]) //Chekc if page has been swapped
				{

					if(!pagein(proc,page)) //If we are unalbe to page in
					{

						//tickTemp is placed to keep track of the tick at the
            //exact moment of the page swap.
						tickTemp = tick;
						for(index=0; index < MAXPROCPAGES; index++)
						{
							if(timestamps[proc][index]>0 && timestamps[proc][index] < tickTemp)
								{
									tickTemp = timestamps[proc][index];
									pagetmp = index;
									//We find the oldest page in the timestamps, and we page out
                  //The oldest based on the pagetmp in timestamps
								}
						}


						if(pageout(proc,pagetmp))
						{ //Once paged out, we set the timestamp back to zero
								timestamps[proc][pagetmp]=0;
						}
					}
				}
						//Otherwise set the timestamp back to the current tick
				else
				{
					timestamps[proc][page]=tick;
				}
			}
		}

    /* advance time for next pageit iteration */
    tick++;
}

