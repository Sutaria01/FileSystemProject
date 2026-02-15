/**************************************************************
* Class::  CSC-415-03 Spring 2024
* Name:: Issac Moreno, Malieka Sutaria
* Student IDs:: 921984788, 922888691
* GitHub-Name:: crvelworld
* Group-Name:: Caffeinated Crew
* Project:: Basic File System
*
* File:: directory.h
*
* Description:: 
*
**************************************************************/

#ifndef DIRECTORY_H
#define DIRECTORY_H

#include <time.h>
#include "freeSpace.h"

typedef struct {
    char name[MAX_NAME_LENGTH + 1];  // Name of the file or directory
    uint64_t size;                   // Size of the file or directory
    time_t acessedTime;               // Last modification date
    time_t modifiedTime;              // Last access date
    time_t createdTime;               // Creation date
    uint64_t location;               // Location of the file or directory
    uint64_t isDir;                  // Flag indicating if it's a directory
} dirEnt;

void initRootDirectory(VCB* vcb);

#endif