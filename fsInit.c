/**************************************************************
* Class::  CSC-415-03 Spring 2024
* Name:: Issac Moreno, Malieka Sutaria
* Student IDs:: 921984788, 922888691
* GitHub-Name:: crvelworld
* Group-Name:: Caffeinated Crew
* Project:: Basic File System
*
* File:: fsInit.c
*
* Description:: Main driver for file system assignment.
*
* This file is where you will start and initialize your system
*
**************************************************************/


#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include "fsLow.h"
#include "mfs.h"
#include "fsVCB.h"
#include "directory.h"
#include "freeSpace.h"
VCB* vcb;

int initFileSystem (uint64_t numberOfBlocks, uint64_t blockSize)
	{
	printf ("Initializing File System with %ld blocks with a block size of %ld\n", numberOfBlocks, blockSize);
	//Allocate space for VCB
	vcb = (VCB*) malloc(BLOCK_SIZE);
	LBAread(vcb, 1, 0);

	//Check if VCB is initialized
	if(vcb->signature == SIGNATURE){
		return 0;
	}

	//Initialize VCB
	initVCB(vcb, numberOfBlocks, blockSize);
	if(vcb == NULL){
		printf("Error: VCB initialization failed.");
		return -1;
	}

	//Initialize free space
	initFreeSpace(vcb, numberOfBlocks);
	if(vcb->freeSpaceManager.bitmap == NULL){
		printf("Error: Free space initialization failed.");
		return -1;
	}

	//Initialize root directory
	initRootDirectory(vcb);

	//Write VCB to disk
	LBAwrite(vcb, 1, 0);
	
	return 0;
	}
	
	
void exitFileSystem (){
	printf ("System exiting\n");
	free(vcb);
}

