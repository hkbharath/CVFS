/*
 * FileOperations.cpp
 *
 *  Created on: 12-Aug-2014
 *      Author: bharathkumar
 */
#include "FileOperations.h"
#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <stdlib.h>

//init functions

CVFS* getVFS(){
	static CVFS *vfs = 0;

	if(vfs==NULL){
		vfs = new CVFS();
	}
	return vfs;
}

void initVFS(int vfs_size){
	CVFS* fs = getVFS();
	fs->disk = malloc(vfs_size);
	fs->disk_size = vfs_size;
	fs->myft = NULL;
	fs->free = new BLOCK();
	fs->free->size = vfs_size;
	fs->free->offset = 0;
	fs->free->next = fs->free->prev = NULL;
}

int createFile(const char* fileName){

	CVFS *vfs = getVFS();
	FT *trav=vfs->myft,*prev = NULL;
	while(trav!=NULL){
		if(strcmp(trav->name,fileName)==0)
			return ALREADY_EXISTS;
		prev = trav;
		trav=trav->next;
	}
	trav = new FT();
	trav->name = (char*)fileName;
	trav->data = NULL;
	trav->next = NULL;
	trav->size = 0;

	if(prev==NULL)
		vfs->myft = trav;
	else
		prev->next = trav;
	return SUCCESS;
}

int writeFile(const char* fileName,const void* buffer,int buffersize,int offset){
	CVFS *vfs = getVFS();
	FT *trav = vfs->myft;

	if(buffersize > vfs->disk_size)
		return NOMEM;

	while(trav != NULL){
		if(strcmp(trav->name,fileName)==0){
			BLOCK *prev=NULL,*move=trav->data;

			if((offset && trav->size<=offset) || offset<-1)
				return NOTDEF;

			if(offset==-1){
				if(move==NULL){
					move = get_block(buffersize);
					trav->data = move;
				}
				else {
					while(move->next != NULL){
						move = move->next;
					}
					move->next = get_block(buffersize);
					move->next->prev = move;

					prev = move;
					move = move->next;
				}

//				buffersize -= move->size;
				trav->size += move->size;
				offset = 0;
			}

			if(offset>0){
				while(offset>=move->size){
					offset -= move->size;
					prev = move;
					move = move->next;
				}

				memcpy((void*)((char*)vfs->disk + move->offset + offset),buffer,min(buffersize,move->size-offset));
				//printf("written data:%s - to offset %d till %d characters\n",(char*)buffer,move->offset+offset,min(buffersize,move->size-offset));

				buffer = (char*)buffer + min(buffersize,move->size-offset);
				buffersize -= move->size-offset;
				offset = 0;
				prev = move;
				move = move->next;
			}

			while(buffersize>0){
				if(move==NULL){
					if(prev==NULL)
						move = trav->data = get_block(buffersize);
					else{
						prev->next = get_block(buffersize);
						prev->next->prev = prev;
						move = prev->next;
					}
					trav->size += move->size;
				}

				memcpy((void*)((char*)vfs->disk + move->offset),buffer,min(buffersize,move->size));

				//printf("written data:%s - to offset %d till %d characters\n",(char*)buffer,move->offset,min(buffersize,move->size));
				buffersize -= move->size;
				buffer = (char*)buffer + move->size;

				prev = move;
				move = move->next;
			}
			return SUCCESS;
		}
		trav = trav->next;
	}
	return NOTFOUND;
}

int readFile(const char* fileName,void* buffer,int buffersize,int offset){
	CVFS *vfs = getVFS();
	FT *trav = vfs->myft;

	int bytes_read = 0;

	while(trav!=NULL){
		if(strcmp(fileName,trav->name)==0){
			BLOCK* move = trav->data;
			while(move != NULL && buffersize>0){

				while(offset>=move->size){
					offset -= move->size;
					move = move->next;
				}

				memcpy(buffer,(void*)((char*)vfs->disk + move->offset + offset),min(buffersize,move->size-offset));

				buffer = (char*)buffer + min(move->size-offset,buffersize);
				bytes_read += min(move->size-offset,buffersize);
				buffersize -= min(move->size-offset,buffersize);

				offset = 0;
				move = move->next;
			}

			return bytes_read;
		}
		trav = trav->next;
	}
	return -1;
}

int renameFile(const char* oldName,const char* newName){
	CVFS *vfs=getVFS();
	FT *trav = vfs->myft;
	while(trav!=NULL){
		if(strcmp(trav->name,oldName)==0){
			trav->name = (char*)newName;
			return SUCCESS;
		}
	}
	return NOTFOUND;
}

int deleteFile(const char* fileName){
	CVFS *vfs=getVFS();
	FT *trav = vfs->myft;
	FT *prev = NULL;
	while(trav!=NULL){

		if(strcmp(trav->name,fileName)==0){
			BLOCK *del = trav->data,*next;
			while(del!=NULL){
				next = del->next;
				add_to_free(del);
				del = next;
			}
			if(prev == NULL)
				vfs->myft = trav->next;
			else
				prev->next = trav->next;
			//printf("root %d %d\n",vfs->free->offset,vfs->free->size);

			del = vfs->free;

			while(del->next != NULL){
				if(del->offset+del->size == del->next->offset){
					del->size += del->next->size;
					next = del->next;
					del->next = del->next->next;
					free(next);
				}
				del = del->next;
				if(del==NULL)
					break;
			}

			free(trav);
			return SUCCESS;
		}

		prev = trav;
		trav = trav->next;
	}

	return NOTFOUND;
}

BLOCK* get_block(int block_size){
	CVFS* vfs = getVFS();
	BLOCK *ret = vfs->free,*suit;
	int max = 0;
	while(ret!=NULL){
		if(ret->size>block_size){
			BLOCK* found = new BLOCK();
			found->offset =ret->offset;
			found->size = block_size;
			found->next = NULL;
			found->prev = NULL;
			ret->offset = ret->offset+block_size;
			ret->size-=block_size;
			return found;
		}
		if(ret->size==block_size){
			ret->prev->next = ret->next;
			ret->next->prev = ret->prev;
			return ret;
		}
		if(ret->size>max){
			max = ret->size;
			suit = ret;
		}
	}
	return suit;
}

void listFiles(){
	CVFS *vfs = getVFS();
	FT* trav = vfs->myft;

	while(trav!=NULL){
		printf("%s\n",trav->name);
		trav = trav->next;
	}
}

int min(int a,int b){
	return a<=b?a:b;
}

void add_to_free(BLOCK* add){
	CVFS *vfs = getVFS();
	BLOCK *move = vfs->free,*prev=NULL;
	while(move != NULL){
		if(move->offset == add->offset+add->size){
			move->offset=add->offset;
			move->size += add->size;
			//printf("adding block offset %d:size %d to start of move\n",add->offset,add->size);
			return;
		}
		if(move->offset+move->size == add->offset){
			move->size += add->size;
			//printf("adding block offset %d:size %d to end of move\n",add->offset,add->size);
			return;
		}
		if(move->offset>add->offset+add->size){
			if(prev==NULL){
				add->next = vfs->free;
				add->prev = NULL;
				vfs->free->prev = add;
				vfs->free = add;
				return;
			}
			add->next = move;
			add->prev = prev;
			move->prev = add;
			prev->next = add;
			return;
		}
		prev = move;
		move = move->next;
	}

	if(prev==NULL){
		vfs->free = add;
	}

	prev->next = add;
	//printf("adding block offset %d:size %d at end\n",add->offset,add->size);
}

void list_free_blocks(){
	CVFS* vfs = getVFS();
	BLOCK *move = vfs->free;

	while(move != NULL){
		printf("offset %d : size %d\n",move->offset,move->size);
		move = move->next;
	}
}
