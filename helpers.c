#include "helpers.h"

dirEnt* cwd = NULL;

//Parse a path and store the components in the buffer
//Returns 0 if successful, -1 if not (invalid path/entry does not exist)
int parsePath(const char *path, returnInfo *buffer){
    // Tokenize the path using '/' and ',' as delimiters
    char *token;
    char *tempPath = strdup(path);  // Duplicate the path string
    char *saveptr;
    int counter = 0;
    //Directory pointers for current, parent, and last directory
    dirEnt *tempDir;
    dirEnt *lastDir = NULL;
    dirEnt *tempEntry = NULL;

    // Check if the first argument is "/"
    if (path[0] == '/'){
        // If it is, tempDir points to the Root directory
        tempDir = getRootDirectory();
        counter++; // Skip the root directory when tokenizing
    }
    else{
        //Otherwise, tempDir points to the current working directory (cwd)
        tempDir = getCWD();
    }

    // Tokenize the path and store each component in arguments array
    for (token = strtok_r(tempPath, "/ ,", &saveptr); token; token = strtok_r(NULL, "/ ,", &saveptr)){
        if (counter++ == MAX_PATH_COMPONENTS){
            // Reached the maximum number of components, break the loop
            break;
        }
        // Check if the current component is a directory
        tempEntry = seekDirectory(tempDir, token);
        if (tempEntry != NULL && tempEntry->isDir == 1){
            //Store the last accessed directory (parent)
            lastDir = tempDir;
            // Update tempDir to the found subdirectory
            tempDir = readDirectory(tempEntry);
        }
        else{
            lastDir = tempDir;
            break;
        }
    }
    if(tempEntry == NULL){
        // Clean up and return NULL if a component is not found or is not a directory
        strcpy(buffer->name, token ? token : "");
        buffer->parent = lastDir;
        buffer->index = -1;
        free(tempPath);
        return -1;
    }
    // Store the final component in the buffer
    strcpy(buffer->name, token ? token : "");
    buffer->parent = lastDir;
    buffer->index = getIndexOfEntry(lastDir, tempEntry);

    // Clean up
    free(tempPath);
    return 0;
}

//Get the VCB of our file system
//Caller shall free the memory
VCB* getVCB(){
    VCB *vcb = (VCB *)malloc(BLOCK_SIZE);
    LBAread(vcb, 1, 0);
    return vcb;
}

//Get the root directory of our file system
//Caller shall free the memory
dirEnt* getRootDirectory(){
    VCB *vcb = getVCB();
    int totalSize = (sizeof(dirEnt)* DIR_ENT_COUNT);
    //Allocate space for root directory
    int blockCnt = (totalSize + BLOCK_SIZE - 1) / BLOCK_SIZE;
    dirEnt *returnDir = (dirEnt*) malloc(blockCnt * BLOCK_SIZE);
    if(returnDir == NULL){
        perror("Memory allocation failed");
        return NULL;
    }
    LBAread(returnDir, blockCnt, 6);
    //free(vcb);
    return returnDir;
}
//Search a directory for an entry
dirEnt* seekDirectory(dirEnt* dir, char* token){
    int i;
    for(i = 0; i < DIR_ENT_COUNT; i++){
        if(strcmp(dir[i].name, token) == 0){
            return &dir[i];
        }
    }
    return NULL;
}
//Get an index of an entry in a directory
int getIndexOfEntry(dirEnt* directory, dirEnt* entry){
    int i;
    for(i = 0; i < DIR_ENT_COUNT; i++){
        if(&directory[i] == entry){
            return i;
        }
    }
    return -1;
}
int getFreeIndex(dirEnt* directory){
    int i;
    for(i = 0; i < DIR_ENT_COUNT; i++){
        if(directory[i].name[0] == '\0'){
            return i;
        }
    }
    return -1;
}
//Read an entry from disk into memory
dirEnt* readDirectory(dirEnt* tempEntry){
    int size = sizeof(dirEnt) * DIR_ENT_COUNT;
    int blocks = (size + BLOCK_SIZE - 1) / BLOCK_SIZE;
    dirEnt *returnDir = (dirEnt*) malloc(blocks * BLOCK_SIZE);
    if(returnDir == NULL){
        perror("Memory allocation failed");
        return NULL;
    }
    int readResult = LBAread(returnDir, blocks, tempEntry->location);
    return returnDir;
}
//Initialize a directory
void initDirectory(dirEnt* parent){
    int i;
    dirEnt directory[DIR_ENT_COUNT];

    for(i = 2; i < DIR_ENT_COUNT; i++){
        directory[i].name[0] = '\0'; // Empty string
        directory[i].size = 0; // Empty size
        directory[i].acessedTime = 0; // Empty date
        directory[i].modifiedTime = 0; // Empty date
        directory[i].createdTime = 0; // Empty date
        directory[i].location = 32; // Empty location
        directory[i].isDir = 0; // Empty isDir
    }
    strcpy(directory[0].name, ".");
    directory[0].isDir = 1;
    directory[0].location = 32;

    strcpy(directory[1].name, "..");
    directory[1].isDir = 1;
    directory[1].location = parent->location;

    int size = sizeof(dirEnt) * DIR_ENT_COUNT;
    int blocks = (size + BLOCK_SIZE - 1) / BLOCK_SIZE;
    int result = LBAwrite(directory, blocks, 32);
}

int isEntryUsed(dirEnt* entry){
    if(entry->name[0] != '\0'){
        return 1;
    }
    return 0;
}

//Get the current working directory
dirEnt* getCWD(){
    if(cwd == NULL){
        cwd = getRootDirectory();
        return cwd;
    }
    return cwd;
}
//Check if a directory entry is a file
//Returns 1 if it is a file, 0 if it is a directory
int isFile(dirEnt* parent, int index){
    if(parent[index].isDir == 0){
        return 1;
    }
    return 0;
}
//Check if a directory entry is a directory
//Returns 1 if it is a directory, 0 if it is a file
int isDirectory(dirEnt* parent){
    if(parent->isDir == 1){
        return 1;
    }
    return 0;
}

int isDirEmpty(dirEnt* dir){
    int i;
    for(i = 2; i < DIR_ENT_COUNT; i++){
        if(dir[i].name[0] != '\0'){
            return 0;
        }
    }
    return 1;
}