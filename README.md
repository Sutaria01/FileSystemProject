CSC 415 – Group Term Project
Basic File System Implementation (C | Ubuntu Linux)

This project was completed as part of CSC 415 – Operating Systems. It is a multi-phase group assignment focused on designing and implementing a fully functional file system in C within a Linux environment.

The objective was to simulate core file system behavior, including storage management, directory handling, and file operations, while working at a low level using logical block addressing (LBA).

Project Overview

We designed and implemented a custom file system with:

Volume formatting

Free space allocation and management

Persistent storage

Root directory initialization

Directory structure management

File creation, reading, writing, deletion

File metadata tracking

Buffered I/O operations

Interactive shell interface

The file system operates on a virtual volume (up to 10MB) with a sector size of 512 bytes.

Key Learning Outcomes

Large-scale system design and planning

Collaborative software development

Low-level file system architecture

Logical Block Addressing (LBA) read/write operations

Buffer management

Free space tracking strategies

Persistent data structures

Directory traversal and management

File metadata handling

Technical Implementation
Environment

Language: C

Platform: Ubuntu Linux Virtual Machine

Build System: Makefile

Low-Level Disk Interface

Provided LBA functions:

uint64_t LBAwrite (void * buffer, uint64_t lbaCount, uint64_t lbaPosition);
uint64_t LBAread  (void * buffer, uint64_t lbaCount, uint64_t lbaPosition);


These functions perform block-level reads and writes to the virtual volume.

File Operations Implemented
b_io_fd b_open(char * filename, int flags);
int b_read(b_io_fd fd, char * buffer, int count);
int b_write(b_io_fd fd, char * buffer, int count);
int b_seek(b_io_fd fd, off_t offset, int whence);
int b_close(b_io_fd fd);

Directory Operations Implemented
int fs_mkdir(const char *pathname, mode_t mode);
int fs_rmdir(const char *pathname);
fdDir * fs_opendir(const char *pathname);
struct fs_diriteminfo *fs_readdir(fdDir *dirp);
int fs_closedir(fdDir *dirp);

char * fs_getcwd(char *pathbuffer, size_t size);
int fs_setcwd(char * pathname);
int fs_isFile(char * filename);
int fs_isDir(char * pathname);
int fs_delete(char* filename);

File Metadata (fs_stat)
int fs_stat(const char *filename, struct fs_stat *buf);


Tracks:

File size

Allocated blocks

Block size

Access time

Modification time

Creation time

Additional custom metadata

Interactive Shell (fsshell)

A custom driver program (fsshell.c) was implemented to interact with the file system.

Supported commands:

ls – List directory contents

cp – Copy file

mv – Move file

md – Create directory

rm – Remove file/directory

touch – Create file

cat – Display file contents

cp2l – Copy from test FS to Linux FS

cp2fs – Copy from Linux FS to test FS

cd – Change directory

pwd – Print working directory

history – Command history

help – Help menu

Project Phases
Phase 1 – Volume Formatting

Initialize volume

Create root directory

Establish free space management

Phase 2 – Directory Management

Implement directory creation, deletion, traversal

Maintain directory entries and metadata

Phase 3 – File Operations

Implement open, read, write, seek, close

Manage block allocation and logical mapping

Implement persistence across sessions

Development Standards

Clean, well-documented C code

Consistent naming conventions

Proper indentation

Line length maintained (~80–100 characters)

Standardized file headers

Makefile required for build and execution

Deliverables

All .c and .h source files

fsshell.c driver program

Makefile

Volume file (≤ 10MB)

Detailed PDF write-up including:

Design decisions

Phase planning

Implementation details

Issues encountered

Screenshots of functionality

GitHub repository link

Summary

This project demonstrates a deep understanding of file system internals, low-level disk operations, memory management, and structured software design. It required careful planning, debugging, and collaboration to successfully implement a persistent, functional file system from scratch in C.
