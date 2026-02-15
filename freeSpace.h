#ifndef FREESPACE_H
#define FREESPACE_H

#include <stdbool.h>
#include <sys/types.h>
#include "fsVCB.h"
#define BLOCK_SIZE 512

void initFreeSpace(VCB* vcb, uint64_t numberOfBlocks);
int allocateSpace(int blockCount, int blockIndex, uint64_t *bitmap);
void bitUsed(int blockIndex, uint64_t *bitmap);
void bitFree(int blockIndex, uint64_t *bitmap);

#endif

