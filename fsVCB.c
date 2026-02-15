#include "fsVCB.h"
#include "freeSpace.h"
#include <stdio.h>
#include <time.h>

void initVCB(VCB* vcb, uint64_t numberOfBlocks, uint64_t blockSize){
    //Clean data for VCB
    memset(vcb, 0, BLOCK_SIZE);

    //Set VCB fields
    strcpy(vcb->name, "Caffeinated Crew FS");
    vcb->signature = SIGNATURE;
    vcb->blockSize = blockSize;
    vcb->totalBlocks = numberOfBlocks;
    vcb->freeBlocks = numberOfBlocks - 1;

    //Write VCB to disk
    LBAwrite(vcb, 1, 0);
}
/*
// Function to create a new directory
int createDirectory(char* name, VCB* vcb, uint64_t parentDirLocation) {
    // Check if there is enough space in the file system
    if (vcb->freeBlocks < 2) {
        printf("Error: Not enough free space to create directory.\n");
        return -1;
    }

    // Create a new directory entry
    dirEnt newDirEntry;
    strcpy(newDirEntry.name, name);
    newDirEntry.size = 0;
    newDirEntry.date = time(NULL);
    newDirEntry.location = vcb->freeSpaceManager.size + 1; // Assuming the first block is for VCB
    newDirEntry.isDir = 1;
    strcpy(newDirEntry.dates, "01/01/00 00:00:00");

    // Write the new directory entry to disk
    LBAwrite(&newDirEntry, 1, newDirEntry.location);

    // Update the parent directory entry
    dirEnt parentDirEntry;
    LBAread(&parentDirEntry, 1, parentDirLocation);
    parentDirEntry.size += 1; // Increment the size of the parent directory
    LBAwrite(&parentDirEntry, 1, parentDirLocation);

    // Update the free blocks count in VCB
    vcb->freeBlocks -= 1;
    LBAwrite(vcb, 1, 0);

    return 0;
}
*/