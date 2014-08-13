/*
 * FileOperations.h
 *
 *  Created on: 12-Aug-2014
 *      Author: bharathkumar
 */

#ifndef FILEOPERATIONS_H_
#define FILEOPERATIONS_H_

enum {SUCCESS,NOMEM,ALREADY_EXISTS,NOTDEF,NOTFOUND};

//Used to Keeps tracks of data blocks allocated to a file and free blocks available
struct BLOCK{
	int offset,size;
	BLOCK *next;
	BLOCK *prev;
};

//file table stores list of files and list of data blocks related to it
struct FT{
	char* name;
	BLOCK *data;
	int size;
	FT *next;
};

//Virtual file syatem
struct CVFS{
	void *disk;
	int disk_size;
	FT *myft;
	BLOCK *free;
};
///////////////////////////////////////////////
//Virtual File Initialization Functions
//////////////////////////////////////////////

//can be used to get pointer to Virtual file system
CVFS* getVFS();

//intialize the VFS to certain size;
void initVFS(int vfs_size);




//////////////////////////////////////////////
//Operation API's
/////////////////////////////////////////////

//Creates a new file in VFS
int createFile(const char* fileName);

// offset 0-start -1=> end else specified offset
// returns 0 on success or error number.
int writeFile(const char* fileName,const void* buffer,int bufferSize,int offset);

//reads characters to buffer form the offset of the file
//returns number of bytes read or -1 in case of error
int readFile(const char* filename,void* buffer,int bufferSize,int offset);

//Renames already existing file in VFS
int renameFile(const char *ondName,const char *newName);

//Removes already existing file in VFS
int deleteFile(const char *fileName);

//Utility functions for programmer

//get a free block of specified size
BLOCK *get_block(int block_size);
//find min of two numbers
int min(int a,int b);
//make data block a fee block
void add_to_free(BLOCK* add);

//Utility functions for users
void listFiles();
void list_free_blocks();

#endif
