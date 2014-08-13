/*
 * VFS.cpp
 *
 *  Created on: 12-Aug-2014
 *      Author: bharathkumar
 */
#include "FileOperations.h"
#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <stdlib.h>

int main(){
	CVFS *vfs = getVFS();
	int diskSize = 100;
	initVFS(diskSize);
	printf("%d\n",vfs->disk_size);
	createFile("hello");
	createFile("world");
	createFile("good");
	createFile("workingFine");

	writeFile("hello","looks good",10,0);
	writeFile("world","nice good work",14,0);
	writeFile("hello"," nice",5,-1);

	void *str = malloc(100);
	printf("Reading file hello\n");
	printf("%d\n",readFile("hello",str,100,0));
	printf("%s\n",(char*)str);
	memset(str,0,100);

	writeFile("hello","insert in middle",16,7);
	printf("\n\nReading file hello after write\n");
	printf("%d\n",readFile("hello",str,100,0));
	printf("%s\n",(char*)str);
	memset(str,0,100);

	printf("\n\nReading file world\n");
	printf("%d\n",readFile("world",str,100,0));
	printf("%s\n",(char*)str);
	printf("\n\n");
	listFiles();
	printf("\n\nFreeblocks before delete\n\n");
	list_free_blocks();
	deleteFile("hello");
	printf("\nFreeblocks after delete :\n\n");
	list_free_blocks();
	renameFile("world","work");
	printf("\n\n");
	listFiles();
}
