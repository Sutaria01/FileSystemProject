#include "directory.h"
#include <stdio.h>
void initRootDirectory(VCB* vcb){
    int totalSize = (sizeof(dirEnt)* DIR_ENT_COUNT);
    //Allocate space for root directory
    int blockCnt = (totalSize + BLOCK_SIZE - 1) / BLOCK_SIZE;

    //Create an array of directory entries
    dirEnt dirEntries[DIR_ENT_COUNT];

    //Reserve the first two entries for root directory
    for(int i = 2; i < DIR_ENT_COUNT; i++){
        //Initialize each directory entry to a known free state
        dirEntries[i].name[0] = '\0'; // Empty string
        dirEntries[i].size = 0; // Empty size
        dirEntries[i].acessedTime = 0; // Empty date
        dirEntries[i].modifiedTime = 0; // Empty date
        dirEntries[i].createdTime = 0; // Empty date
        dirEntries[i].location = 6; // Empty location after the first 3 entries
        dirEntries[i].isDir = 0; // Empty isDir
    }

    //Request blocks from the free space system
    int startBlock = allocateSpace(blockCnt, vcb->FSMstart, vcb->freeSpaceManager.bitmap);
    //Set the first directory to "."
    strcpy(dirEntries[0].name, ".");
    dirEntries[0].size = totalSize;
    dirEntries[0].acessedTime = time(NULL);
    dirEntries[0].modifiedTime = time(NULL);
    dirEntries[0].createdTime = time(NULL);
    dirEntries[0].location = 6; // = result;
    dirEntries[0].isDir = 1;

    //Set the second directory entry to ".."
    strcpy(dirEntries[1].name, "..");
    dirEntries[1].size = totalSize;
    dirEntries[1].acessedTime = time(NULL);
    dirEntries[1].modifiedTime = time(NULL);
    dirEntries[1].createdTime = time(NULL);
    dirEntries[1].location = 6; // = result;
    dirEntries[1].isDir = 1;

    //CHANGE TO: result (when not -1)
    LBAwrite(dirEntries, blockCnt, vcb->FSMstart);
    //CHANGE TO: result (when not -1)
    vcb->rootDirectoryLocation = vcb->FSMstart;
    vcb->freeBlocks = vcb->freeBlocks - vcb->FSMstart;
    vcb->FSMstart += blockCnt;
}