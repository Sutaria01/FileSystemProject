/**************************************************************
* Class::  CSC-415-03 Spring 2024
* Name:: Malieka Sutaria
* Student IDs:: 922888691
* Project:: Basic File System
*
* File:: b_io.c
*
* Description:: Basic File System - Key File I/O Operations
*
**************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>			// for malloc
#include <string.h>			// for memcpy
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "b_io.h"

#define MAXFCBS 20
#define B_CHUNK_SIZE 512

typedef struct b_fcb
	{
	/** TODO add al the information you need in the file control block **/
	char * buf;		//holds the open file buffer
	int index;		//holds the current position in the buffer
	int buflen;		//holds how many valid bytes are in the buffer
	} b_fcb;
	
b_fcb fcbArray[MAXFCBS];

int startup = 0;	//Indicates that this has not been initialized

//Method to initialize our file system
void b_init ()
	{
	//init fcbArray to all free
	for (int i = 0; i < MAXFCBS; i++)
		{
		fcbArray[i].buf = NULL; //indicates a free fcbArray
		}
		
	startup = 1;
	}

//Method to get a free FCB element
b_io_fd b_getFCB ()
	{
	for (int i = 0; i < MAXFCBS; i++)
		{
		if (fcbArray[i].buf == NULL)
			{
			return i;		//Not thread safe (But do not worry about it for this assignment)
			}
		}
	return (-1);  //all in use
	}
	
// Interface to open a buffered file
// Modification of interface for this assignment, flags match the Linux flags for open
// O_RDONLY, O_WRONLY, or O_RDWR
b_io_fd b_open(char* filename, int flags)
{
    b_io_fd returnFd;
    if (startup == 0)
        b_init(); //Initialize our system

    returnFd = b_getFCB(); // get our own file descriptor
    // check for error - all used FCB's
    int fd;

    if (flags & O_CREAT)
    {
        fd = open(filename, flags, 0644);
    }
    else
    {
        fd = open(filename, flags);
    }
    if (fd == -1)
    {
        perror("Error opening file.");
        return -1;
    }
    fcbArray[returnFd].buf = malloc(B_CHUNK_SIZE);
    if (fcbArray[returnFd].buf == NULL)
    {
        perror("Error allocating memory for buffer");
        close(fd);
        return -1;
    }
    fcbArray[returnFd].index = 0;
    fcbArray[returnFd].buflen = 0;
    return (returnFd); // all set
}


// Interface to seek function	
int b_seek (b_io_fd fd, off_t offset, int whence)
	{
	if (startup == 0) b_init();  //Initialize our system

	// check that fd is between 0 and (MAXFCBS-1)
	if ((fd < 0) || (fd >= MAXFCBS))
		{
		return (-1); 					//invalid file descriptor
		}
		
		
	return lseek(fd, offset, whence); //Change this
	}



// Interface to write function	
int b_write (b_io_fd fd, char * buffer, int count)
	{
	if (startup == 0) 
	b_init();  //Initialize our system

	// check that fd is between 0 and (MAXFCBS-1)
	if ((fd < 0) || (fd >= MAXFCBS))
		{
		return (-1); 					//invalid file descriptor
		}

	int bytes_written = write(fd, buffer, count);
	if (bytes_written == -1)
	{
		perror("Error writing to file");
	}	
		
	return bytes_written; //Change this
	}



// Interface to read a buffer

// Filling the callers request is broken into three parts
// Part 1 is what can be filled from the current buffer, which may or may not be enough
// Part 2 is after using what was left in our buffer there is still 1 or more block
//        size chunks needed to fill the callers request.  This represents the number of
//        bytes in multiples of the blocksize.
// Part 3 is a value less than blocksize which is what remains to copy to the callers buffer
//        after fulfilling part 1 and part 2.  This would always be filled from a refill 
//        of our buffer.
//  +-------------+------------------------------------------------+--------+
//  |             |                                                |        |
//  | filled from |  filled direct in multiples of the block size  | filled |
//  | existing    |                                                | from   |
//  | buffer      |                                                |refilled|
//  |             |                                                | buffer |
//  |             |                                                |        |
//  | Part1       |  Part 2                                        | Part3  |
//  +-------------+------------------------------------------------+--------+
int b_read (b_io_fd fd, char * buffer, int count)
	{

	if (startup == 0) 
	b_init();  //Initialize our system

	// check that fd is between 0 and (MAXFCBS-1)
	if ((fd < 0) || (fd >= MAXFCBS))
		{
		return (-1); 					//invalid file descriptor
		}

	int bytes_read = read(fd, buffer, count);
	if (bytes_read == -1)
	{
		perror("Error reading from file");
	}	
	return bytes_read;	//Change this
	}
	
// Interface to Close the file	
int b_close (b_io_fd fd)
{
	if (startup == 0)
	b_init(); // initialize system

	if ((fd < 0) || (fd >= MAXFCBS))
	{
		return -1;
	}
	if (close(fd) == -1)
	{
		perror("Error closing file");
		return -1;
	}
	free(fcbArray[fd].buf);
	fcbArray[fd].buf = NULL;
	
	return 0;
}