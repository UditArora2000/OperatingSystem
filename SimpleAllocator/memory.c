#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <assert.h>
#include <math.h>
#include "memory.h"
#include <stdbool.h>

#define PAGE_SIZE 4096

static void *alloc_from_ram(size_t size)
{
	assert((size % PAGE_SIZE) == 0 && "size must be multiples of 4096");
	void* base = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_ANON|MAP_PRIVATE, -1, 0);
	if (base == MAP_FAILED)
	{
		printf("Unable to allocate RAM space\n");
		exit(0);
	}
	return base;
}

static void free_ram(void *addr, size_t size)
{
	munmap(addr, size);
}

void *bucket[9];
int no_of_bp=0;

struct metadata {
	struct metadata *next;
	int chunksize;
	int bytes_available;
};
typedef struct metadata metadata;


void myfree(void *ptr) {
	long int x=(long int)(ptr)/4096;
	x*=4096;
	metadata *mt=(metadata *)x;
	if((long int)ptr%4096==0) {
		x-=4096;
		metadata *fin=(metadata *)x;
		free_ram((void *)x,(*fin).chunksize);
		return ;
	}
	(*mt).bytes_available+=(*mt).chunksize;

	int mtap=0;
	int ran=1;
	while(ran<(*mt).chunksize) {
		mtap+=1;
		ran*=2;
	}
	metadata *freed=(metadata *)ptr;
	(*freed).next=(metadata *)bucket[mtap-4];
	(*freed).bytes_available=0;
	(*freed).chunksize=(*mt).chunksize;
	bucket[mtap-4]=(void *)freed;

	long int q;

	if((*mt).bytes_available==4080) {
		metadata *temp=(metadata *)bucket[mtap-4];
		metadata *prev=NULL;
		while(temp!=NULL && temp!=(*mt).next) {
			q=(long int)(temp)/4096;
			q*=4096;
			metadata *mtemp=(metadata *)q;
			int f=0;
			if((*mtemp).bytes_available==4080) {
				if(bucket[mtap-4]==(void *)temp) {
					f=2;
				}
				else {
					(*prev).next=(*temp).next;
					f=1;	
				}
			}
			if(f==0) {
				prev=temp;
				temp=(*temp).next;
			}
			else if(f==1) {
				temp=(*prev).next;
			}
			else {
				bucket[mtap-4]=(*temp).next;
				temp=(*temp).next;
			}
		}
		free_ram((void *)mt,4096);
	}
}


void *mymalloc(size_t size) {	
	int mtap=0;
	int ran=1;
	while(ran<size) {
		mtap+=1;
		ran*=2;
	}
	if(mtap==0) {
		mtap+=1;
	}
	int chunksize=1;
	for(int aa=0;aa<mtap;aa++) {
		chunksize*=2;
	}
	if(chunksize<16) {
		chunksize=16;
		mtap=4;
	}

	if(chunksize>4096) {
		int sop=0;
		while(sop<size) {
			sop+=4096;
		}
		sop+=4096;
		void *ptr=alloc_from_ram(sop);
		metadata *mt=(metadata *)ptr;
		(*mt).chunksize=sop;
		(*mt).bytes_available=0;
		(*mt).next=NULL;
		return ptr+4096;
	}

	if(bucket[mtap-4]!=NULL) {
		metadata *mt=(metadata *)bucket[mtap-4];
		if((*mt).bytes_available<chunksize && (*mt).bytes_available>0) {
			void *newpage=alloc_from_ram(PAGE_SIZE);
			for(int j=(int)4096/chunksize-1;j>=0;j--) {
				metadata *mtemp=(metadata *)(newpage+j*chunksize);
				if(j==0) {
					(*mtemp).bytes_available=4080;
				}
				else {
					(*mtemp).bytes_available=0;
				}
				(*mtemp).chunksize=chunksize;
				if(j==0) {
					(*mtemp).next=(metadata *)bucket[mtap-4];
				}
				else {
					(*mtemp).next=(metadata *)(newpage+(j-1)*chunksize);
				}
			}
			bucket[mtap-4]=newpage+4096-chunksize;
			metadata *mt=(metadata *)newpage;
			(*mt).bytes_available-=chunksize;
			metadata *head=(metadata *)bucket[mtap-4];
			bucket[mtap-4]=(void *)((*head).next);
			(*head).next=NULL;
			return (void *)head;
		}

		else {
			long int x=(long int)(bucket[mtap-4])/4096;
			x*=4096;
			metadata *mt=(metadata *)x;
			(*mt).bytes_available-=chunksize;
			metadata *head=(metadata *)bucket[mtap-4];
			bucket[mtap-4]=(void *)((*head).next);
			(*head).next=NULL;
			return (void *)head;
		}
	}

	else {
		if(chunksize==4096) {
			void *newpage=alloc_from_ram(4096*2);
			metadata *mt=(metadata *)newpage;
			(*mt).chunksize=4096*2;
			(*mt).bytes_available=0;
			(*mt).next=NULL;
			return newpage+4096;
		}

		void *newpage=alloc_from_ram(PAGE_SIZE);
		for(int j=(int)4096/chunksize-1;j>=0;j--) {
			metadata *mt=(metadata *)(newpage+j*chunksize);
			if(j==0) {
				(*mt).bytes_available=4080;
			}
			else {
				(*mt).bytes_available=0;
			}
			(*mt).chunksize=chunksize;
			if(j==0) {
				(*mt).next=NULL;
			}
			else {
				(*mt).next=(metadata *)(newpage+(j-1)*chunksize);
			}
		}
		bucket[mtap-4]=newpage+4096-chunksize;
		metadata *mt=(metadata *)newpage;
		(*mt).bytes_available-=chunksize;
		metadata *head=(metadata *)bucket[mtap-4];
		bucket[mtap-4]=(void *)((*head).next);
		(*head).next=NULL;
		return (void *)head;
	}
}
