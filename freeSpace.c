#include <stdio.h>
#include "freeSpace.h"

void initFreeSpace(VCB* vcb, uint64_t numberOfBlocks){
    // Number of bytes needed for bitmap
    int numBytes = (numberOfBlocks + 7) / 8;
    // Number of blocks needed for bitmap
    int numBlocks = (numBytes + BLOCK_SIZE - 1) / BLOCK_SIZE;
    //Allocate space for bytes needed
    vcb->freeSpaceManager.bitmap = malloc(numBlocks * BLOCK_SIZE);

    //Error check
    if (vcb->freeSpaceManager.bitmap == NULL) {
        printf("Error: Memory allocation failed!");
        free(vcb->freeSpaceManager.bitmap);
    }
    //Initialize bitmap to all 0s
    int result = allocateSpace(numBlocks, 1, vcb->freeSpaceManager.bitmap); //1
    //Error check
    if(result == -1){
        printf("Error: Not enough free space to allocate bitmap.");
        free(vcb->freeSpaceManager.bitmap);
    }
    //Set used blocks to 1
    for(int i = 0; i < result + numBlocks; i++) {
        bitUsed(i, vcb->freeSpaceManager.bitmap);
    }
    //Set starting block to after bitmap
    vcb->FSMstart = numBlocks + 1;
    //Set ending block to number of blocks from user
    vcb->FSMend = numberOfBlocks;
    //Size of bitmap in bits (for reading later)
    vcb->freeSpaceManager.size = numBlocks * BLOCK_SIZE;
    LBAwrite(vcb->freeSpaceManager.bitmap, numBlocks, 1);
}

//function allocates space needed and checks if it has enough
int allocateSpace(int blockCount, int blockIndex, uint64_t *bitmap){
    //beginning of the freespace, holds the location of the freespace
    //checked from the VCB
    int beg = 0;
    int count = 0;

    int j = 0;
    int k = 0;

    bool flag = true;

    //find the index of the byte that has the bit
    int byteIndex = blockIndex/8;
    //find bit position in the byte
    int bitPosition = blockIndex % 8;
    //bit mask to set bit pos to 0
    int bitmask = ~(1 << bitPosition);
    while(flag){
        //change block size depending on what we have
        for(int i=beg; i<blockCount; ++i) {
            if(!(bitmap[byteIndex] & bitmask)) {
                count++;
                if(count == blockCount) {
                    int allocSpace = beg;
                    //increment beg to point to next free block
                    beg++;
                    return allocSpace;
                }
            } else {
                //reset count and update beg to the next block
                count = 0;
                beg++;
            }
        }

       //return -1 if not enough free space
        return -1;
    }
}

// function find bits to be stored into bitmap
void bitUsed(int blockIndex, uint64_t *bitmap){
    //find index of int in the bitmap
    int byteIndex = blockIndex/8;
    //find bit position in the int
    int bitPosition = blockIndex %8;
    //find bitmask with the bit
    int bitmask = 1 << bitPosition;

    //set bit from the int into the bitmap
    bitmap[byteIndex] |= bitmask;

}


//function used to free any bit not in use
void bitFree(int blockIndex, uint64_t *bitmap){
    //find the index of the byte that has the bit
    int byteIndex = blockIndex/8;
    //find bit position in the byte
    int bitPosition = blockIndex % 8;
    //bit mask to set bit pos to 0
    int bitmask = ~(1 << bitPosition);
    
    //possibly need to add free to free from space allocation

    //clear bit in the byte of the bitmap
    bitmap[byteIndex] &= bitmask;
}
