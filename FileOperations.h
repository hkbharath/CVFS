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

//Virtual File Init Functions
CVFS* getVFS();
void initVFS(int vfs_size);

//Operartion API's
//Creates a new file in VFS
int createFile(const char* fileName);

// offset 0-start -1=> end else specified offset returns 0 on success or error number.
int writeFile(const char* fileName,const void* buffer,int bufferSize,int offset);

//reads charecters to buffer form the offset of the file
//returns number of bytes read or -1 incase of error
int readFile(const char* filename,void* buffer,int bufferSize,int offset);

//Renames already existing file in VFS
int renameFile(const char *ondName,const char *newName);

//Removes already existing file in VFS
int deleteFile(const char *fileName);

//Utility functions for programmer
BLOCK *get_block(int block_size);
int min(int a,int b);
void add_to_free(BLOCK* add);

//Utility functions for users
void listFiles();
void list_free_blocks();




#endif /* FILEOPERATIONS_H_ */
