#include "thread.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

struct thread {
	void *esp;
	struct thread *next;
	struct thread *prev;
};

struct thread *ready_list = NULL;
struct thread *cur_thread = NULL;

void context_switch(struct thread *prev, struct thread *next);

static void push_back(struct thread *t)
{
	struct thread *temp=ready_list;
	if(temp==NULL) {
		ready_list=t;
		ready_list->next=ready_list;
		ready_list->prev=ready_list;
	}
	else {
		t->prev=ready_list->prev;
		t->next=ready_list;
		if(ready_list->prev==NULL) {
			printf("%s\n","why" );
		}
		ready_list->prev->next=t;
		ready_list->prev=t;
	}
}

static struct thread *pop_front()
{
	if(ready_list->next==ready_list) {
		struct thread *t=ready_list;
		ready_list=NULL;
		return t;
	}
	else if(ready_list!=NULL) {
		struct thread *temp=ready_list;
		ready_list->prev->next=ready_list->next;
		ready_list->next->prev=ready_list->prev;
		ready_list=ready_list->next;
		temp->next=NULL;
		temp->prev=NULL;
		return temp;
	}
}

static void schedule()
{
	struct thread *prev = cur_thread;
	struct thread *next = pop_front(ready_list);
	cur_thread = next;
	context_switch(prev,next);
}

static void schedule1()
{
	if(cur_thread!=NULL) {
		push_back(cur_thread);
		schedule();
	}
	else {
		cur_thread=malloc(sizeof(struct thread));
		push_back(cur_thread);
		schedule();
	}
}

void create_thread(func_t func, void *param)
{
	unsigned *stack=malloc(4096);
	stack+=1024;
	struct thread *new_thread=malloc(sizeof(struct thread));
	*(stack)=(unsigned)param;
	*(stack-1)=0;
	*(stack-2)=(unsigned)func;
	*(stack-3)=0;
	*(stack-4)=0;
	*(stack-5)=0;
	*(stack-6)=0;
	new_thread->esp=(void *)(stack-6);
	push_back(new_thread);
}

void thread_yield()
{
	schedule1();
}

void thread_exit()
{
	schedule();
}

void wait_for_all()
{
	while(ready_list!=NULL) {
		schedule1();
	}
}