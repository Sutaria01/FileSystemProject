#ifndef FSVCB_H
#define FSVCB_H

#include <stdlib.h>
#include <string.h> //For strcpy
#include <sys/types.h>
#include "fsLow.h"
#define BLOCK_SIZE 512
#define MAX_NAME_LENGTH 201 //193 + sizeof(dirEnt) = 256 aka BSIZE/2
#define DIR_ENT_COUNT 50
#define SIGNATURE 0x324F83F332953018 //Random signature

typedef struct {
    uint64_t* bitmap; // Pointer to the free space bitmap
    uint64_t size; // Size of the bitmap (used to bring into memory)
} FSM;

typedef struct VCB {
    char name[MAX_NAME_LENGTH + 1]; // Name of volume
    uint64_t blockSize;             // Max block size (standard)
    uint64_t rootDirectoryLocation; // Location of the root directory
    uint64_t totalBlocks;           // Total blocks used by the program
    uint64_t freeBlocks;            // Total number of free blocks
    FSM freeSpaceManager;           // Pointer to the free space bitmap
                                    // & also contains start and end blocks
    uint64_t FSMstart;              // First free block
    uint64_t FSMend;                // End block
    uint64_t signature;             // Initialized flag
} VCB;

//VCB initialization
void initVCB(VCB* vcb, uint64_t numberOfBlocks, uint64_t blockSize);
#endif