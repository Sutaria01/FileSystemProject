#include <stdio.h>
#include "fsVCB.h"
#include "directory.h"
#include "fsLow.h"
//#include "mfs.h"
//#include "b_io.h" //Unneeded as of now


#ifndef HELPERS_H
#define HELPERS_H

#define NAMESIZE 256
#define MAX_PATH_COMPONENTS 128

extern dirEnt* cwd;

//Structure to store the return information from parsePath
typedef struct returnInfo{
    dirEnt *parent;
    int index;
    char name[NAMESIZE];
} returnInfo;

int parsePath(const char *path, returnInfo *buffer);
VCB* getVCB();
dirEnt* getCWD();
dirEnt* getRootDirectory();
dirEnt* seekDirectory(dirEnt* tempDir, char* token);
dirEnt* readDirectory(dirEnt* tempEntry);
int getIndexOfEntry(dirEnt* directory, dirEnt* entry);
void initDirectory(dirEnt* parent);
int getFreeIndex(dirEnt* directory);
int isEntryUsed(dirEnt* entry);
int isFile(dirEnt* parent, int index);
int isDirectory(dirEnt* parent);
int isDirEmpty(dirEnt* dir);

#endif /* HELPERS_H */