#include "mfs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>   // Needed for chdir function
#include <sys/stat.h> // Needed for mkdir function
#include <sys/types.h>
#include "helpers.h"

int fs_setcwd(const char *pathname)
{
    returnInfo *retInfo = (returnInfo *)malloc(sizeof(returnInfo));
    if (retInfo == NULL)
    {
        perror("Memory allocation failed");
        return -1;
    }
    // Parse the path
    int parse = parsePath(pathname, retInfo);
    // Check if the directory entry exists
    if (parse == 0)
    {
        if (retInfo->index == -1)
        {
            printf("Invalid Path: File does not exist");
            free(retInfo);
            retInfo = NULL;
            return -1;
        }
        // Check if the entry is a directory
        if (retInfo->parent[retInfo->index].isDir == 0)
        {
            printf("Invalid Path: Not a directory");
            free(retInfo);
            retInfo = NULL;
            return -1;
        }
        // Change the current working directory
        printf("retInfo->index: %d\n", retInfo->index);
        printf("retInfo->parent[retInfo->index].name: %s\n", retInfo->parent[retInfo->index].name);
        printf("retInfo->parent[retInfo->index].location: %ld\n", retInfo->parent[retInfo->index].location);
        cwd = readDirectory(&retInfo->parent[retInfo->index]);
        printf("cwd[0].name: %s\n", cwd[0].name);
        printf("cwd[1].name: %s\n", cwd[1].name);
        free(retInfo);
        retInfo = NULL;
        return 0;
    }
    else
    {
        printf("Error: fs_setcwd() internal error");
        free(retInfo);
        retInfo = NULL;
        return -1;
    }
}

// Function to get the current working directory
char *fs_getcwd(char *pathname, size_t size)
{
    if (cwd == NULL)
    {
        cwd = getRootDirectory();
        return cwd;
    }
    dirEnt *temp = cwd;
    dirEnt *temp2;
    char *pathBuffer = (char *)malloc(size);
    if (pathBuffer == NULL)
    {
        perror("Memory allocation failed");
        return NULL;
    }
    char *tempPath = malloc(size);
    if (tempPath == NULL)
    {
        perror("Memory allocation failed");
        return NULL;
    }

    // Check if the current directory is the root directory
    if (temp->location == 6)
    { // Root directory location
        // Append root indicator
        strcpy(pathBuffer, "/");
    }
    else
    {
        // Iterate through the directory paths and append the name
        while (temp->location != 6)
        { // Root directory location
            // Get parent
            // printf("temp[1]->name: %d\n", temp[1].name);
            // printf("temp[1]->location: %d\n", temp[1].location);
            temp2 = readDirectory(&temp[1]);
            strcpy(pathBuffer, "/");
            // Append name of directory
            strcat(pathBuffer, temp2[getIndexOfEntry(temp2, temp)].name);
            strcat(pathBuffer, tempPath);
            strcpy(tempPath, pathBuffer);
            temp = temp2;
        }
    }

    strncpy(pathname, pathBuffer, size);
    return pathname;
}

// Function to create a new directory
int fs_mkdir(const char *pathname, mode_t mode)
{
    returnInfo *retInfo = (returnInfo *)malloc(sizeof(returnInfo));
    if (retInfo == NULL)
    {
        perror("Memory allocation failed");
        return -1;
    }
    // Parse the path
    int parse = parsePath(pathname, retInfo); // Make directory
    if (retInfo->index == -1)
    {
        // Create a new directory
        int index = getFreeIndex(retInfo->parent);
        initDirectory(retInfo->parent);
        if (index == -1)
        {
            printf("Error: Directory is full");
            free(retInfo);
            retInfo = NULL;
            return -1;
        }
        // Initialize the new directory
        strcpy(retInfo->parent[index].name, retInfo->name);
        retInfo->parent[index].size = 0;
        retInfo->parent[index].acessedTime = time(NULL);
        retInfo->parent[index].modifiedTime = time(NULL);
        retInfo->parent[index].createdTime = time(NULL);
        retInfo->parent[index].location = 32;
        retInfo->parent[index].isDir = 1;
        // Write the new directory to disk
        int blocks = retInfo->parent->size * BLOCK_SIZE;
        LBAwrite(retInfo->parent, blocks, 6);
    }
    else
    {
        printf("Error: Directory already exists\n");
        free(retInfo);
        retInfo = NULL;
        return -1;
    }
}

// Open directory
fdDir *fs_opendir(const char *pathname)
{
    returnInfo *retInfo = malloc(sizeof(returnInfo));
    if (retInfo == NULL)
    {
        printf("Memory allocation failed");
        return NULL;
    }

    fdDir *dir = malloc(sizeof(fdDir));
    if (dir == NULL)
    {
        printf("Memory allocation failed");
        return NULL;
    }
    int parse = parsePath(pathname, retInfo);
    if (parse == 0)
    {
        dir->d_reclen = sizeof(struct fs_diriteminfo);
        dir->dirEntryPosition = 0;
        dir->directory = readDirectory(&retInfo->parent[retInfo->index]);
        dir->di = malloc(sizeof(struct fs_diriteminfo));
    }
    else
    {
        printf("Error: fs_opendir() internal error");
        free(retInfo);
        free(dir);
        retInfo = NULL;
        dir = NULL;
        return NULL;
    }
}

// Read directory
struct fs_diriteminfo *fs_readdir(fdDir *dirp)
{

    int entries = dirp->directory->size / sizeof(dirEnt);
    for (int i = dirp->dirEntryPosition; i < entries; i++)
    {
        // Only return a used entry
        if (isEntryUsed(&(dirp->directory[i])) == 1)
        {
            strcpy(dirp->di->d_name, dirp->directory[i].name);
            dirp->di->fileType = dirp->directory[i].isDir;
            dirp->dirEntryPosition = i + 1;
            return dirp->di;
        }
    }
    return NULL;
}

int fs_closedir(fdDir *dirp)
{
    dirp->directory->acessedTime = time(NULL);
    free(dirp->di);
    free(dirp);
    return 0;
}

int fs_isDir(char *pathname)
{
    int isFile = fs_isFile(pathname);
    if (isFile == -1)
    {
        return -1;
    }
    return !isFile;
}

// Chek if an entry is a file
// Returns 1 if it is a file, 0 otherwise
int fs_isFile(char *filename)
{
    returnInfo *retInfo = (returnInfo *)malloc(sizeof(returnInfo));
    if (retInfo == NULL)
    {
        perror("Memory allocation failed");
        return -1;
    }
    // Parse the path
    int parse = parsePath(filename, retInfo);
    // File does not exist
    if (retInfo->index == -1)
    {
        printf("Error: File does not exist");
        return -1;
    }

    // Check if the entry is a file
    int result = !retInfo->parent[retInfo->index].isDir;
    free(retInfo);
    retInfo = NULL;
    return result;
}

// Get the stats of a file
// Returns 0 on success and -1 on failure
int fs_stat(const char *path, struct fs_stat *buf)
{
    // Make a container to store the return information
    returnInfo *retInfo = (returnInfo *)malloc(sizeof(returnInfo));
    // Error Check
    if (retInfo == NULL)
    {
        printf("Error: Memory allocation failed");
        return -1;
    }

    // Parse path and get file
    int result = parsePath(path, retInfo);

    // Check if the directory entry exists
    if (retInfo->index == -1)
    {
        printf("Error: Entry does not exist");
        return -1;
    }
    // Success->Fill in the buffer
    if (result == 0)
    {
        int indx = retInfo->index;
        strcpy(buf->name, retInfo->name);
        buf->st_size = retInfo->parent[indx].size;
        buf->st_blksize = BLOCK_SIZE;
        buf->st_blocks = (retInfo->parent[indx].size + 511) / 512;
        buf->st_accesstime = retInfo->parent[indx].acessedTime;
        buf->st_modtime = retInfo->parent[indx].modifiedTime;
        buf->st_createtime = retInfo->parent[indx].createdTime;
    }
    // Other error
    else
    {
        printf("Error: fs_stat() internal error");
    }
    free(retInfo);
    retInfo = NULL;
    return 0;
}

int fs_delete(char *filename)
{
    // Make a container to store the return information
    returnInfo *retInfo = (returnInfo *)malloc(sizeof(returnInfo));
    if (retInfo == NULL)
    {
        printf("Error: Memory allocation failed");
        return -1;
    }

    // Parse path and get file
    int result = parsePath(filename, retInfo);

    if (retInfo->index == -1)
    {
        printf("Error: File does not exist");
        return -1;
    }
    if(!isFile(retInfo->parent, retInfo->index)) {
        printf("Error: Not a file");
        return -1;
    }
    
    // Success->Delete the file
    // Initialize directory entry to a known free state
    retInfo->parent[retInfo->index].name[0] = '\0';                       // Empty string
    retInfo->parent[retInfo->index].size = 0;                              // Empty size
    retInfo->parent[retInfo->index].acessedTime = 0;                        // Empty date
    retInfo->parent[retInfo->index].modifiedTime = 0;                       // Empty date
    retInfo->parent[retInfo->index].createdTime = 0;                        // Empty date
    retInfo->parent[retInfo->index].location = retInfo->parent->location; // Empty location
    retInfo->parent[retInfo->index].isDir = 0;                            // Empty isDir

    // Update parent directory
    LBAwrite(retInfo->parent, 1, retInfo->parent->location);

    free(retInfo);
    retInfo = NULL;
    return 0;
}

int fs_rmdir(const char *pathname)
{
    //Make a container to store the return information
    returnInfo *retInfo = (returnInfo *)malloc(sizeof(returnInfo));
    if(retInfo == NULL) {
        printf("Error: Memory allocation failed\n");
        return -1;
    }

    //Parse path and get file
    int result = parsePath(pathname, retInfo);

    if(retInfo->index == -1) {
        printf("Error: Directory does not exist\n");
        return -1;
    }
    if(!isDirectory(&retInfo->parent[retInfo->index])){
        printf("Error: Not a directory\n");
        return -1;
    }

    //Check if directory is empty
    if (!isDirEmpty(readDirectory(&retInfo->parent[retInfo->index]))){
        printf("Error: Directory is not empty\n");
        return -1;
    }

    // Success->Delete the directory
    // Initialize directory entry to a known free state
    retInfo->parent[retInfo->index].name[0] = '\0';                       // Empty string
    retInfo->parent[retInfo->index].size = 0;                              // Empty size
    retInfo->parent[retInfo->index].acessedTime = 0;                        // Empty date
    retInfo->parent[retInfo->index].modifiedTime = 0;                       // Empty date
    retInfo->parent[retInfo->index].createdTime = 0;                        // Empty date
    retInfo->parent[retInfo->index].location = retInfo->parent->location; // Empty location
    retInfo->parent[retInfo->index].isDir = 0;                            // Empty isDir

    // Update parent directory
    LBAwrite(retInfo->parent, retInfo->parent->size * BLOCK_SIZE, retInfo->parent->location);
    free(retInfo);
    retInfo = NULL;
    return 0;
}
