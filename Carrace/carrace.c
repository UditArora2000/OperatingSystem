#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>

int cheat_to_car1[2];
int cheat_to_car2[2];
int car1_and_report[2];
int car2_and_report[2];
int retval;

void cheatmode()
{
	while(1) {
		char reply[100];
		printf("%s\n","Do you want to cheat?" );
		retval=scanf("%s",reply);
		int newloc1=-1,newloc2=-1,carno=0;
		if(reply[0]=='y'&&reply[1]=='e'&&reply[2]=='s') {
			printf("%s\n","Which car do you want to relocate (1 for car1 and 2 for car2)" );
			retval=scanf("%d",&carno);
			printf("%s %d\n","Enter new location of car",carno );
			if(carno==1)
				retval=scanf("%d",&newloc1);
			else
				retval=scanf("%d",&newloc2);
		}
		int pid=fork();
		if(pid==0) {
			retval=write(cheat_to_car1[1],&newloc1,sizeof(newloc1));
			retval=write(cheat_to_car2[1],&newloc2,sizeof(newloc2));
			break;
		}
		else if(pid>0) {
			wait(NULL);
		}
		else {
			printf("%s\n","Couldn't fork" );
		}	
	}
}

void car1()
{
	int position=0;
	int ret;
	while(1) {
		char str[100];
		ret=fcntl(car1_and_report[0],F_SETFL,fcntl(car1_and_report[0],F_GETFL) | O_NONBLOCK);
		retval=read(car1_and_report[0],str,100);
		if(str[0]=='s'&&str[1]=='t'&&str[2]=='o'&&str[3]=='p') {
			printf("%s\n","Exiting car1 method" );
			kill(getpid(),SIGKILL);
		}
		sleep(1);
		int randomnumber;
		srand(getpid() + (time(0) << (2 * sizeof(pid_t))));
		randomnumber=rand()%10+1;
		position+=randomnumber;
		int reloc=-1;
		ret=fcntl(cheat_to_car1[0],F_SETFL,fcntl(cheat_to_car1[0],F_GETFL) | O_NONBLOCK);
		retval=read(cheat_to_car1[0],&reloc,sizeof(reloc));
		if(reloc!=-1) {
			position=reloc;
		}
		int pid=fork();
		if(pid==0) {
			int pos=position;
			retval=write(car1_and_report[1],&position,sizeof(position));
			kill(getpid(),SIGKILL);
		}
		else if(pid>0) {
			wait(NULL);
		}
		else {
			printf("%s\n","Couldn't fork" );
		}
	}
}

void car2()
{
	int position=0;
	int ret;
	while(1) {
		char str[100];
		ret=fcntl(car2_and_report[0],F_SETFL,fcntl(car2_and_report[0],F_GETFL) | O_NONBLOCK);
		retval=read(car2_and_report[0],str,100);
		if(str[0]=='s'&&str[1]=='t'&&str[2]=='o'&&str[3]=='p') {
			printf("%s\n","Exiting car2 method" );
			kill(getpid(),SIGKILL);
		}
		sleep(1);
		int randomnumber;
		srand(getpid() + (time(0) << (2 * sizeof(pid_t))));
		randomnumber=rand()%10+1;
		position+=randomnumber;
		int reloc=-1;
		ret=fcntl(cheat_to_car2[0],F_SETFL,fcntl(cheat_to_car2[0],F_GETFL) | O_NONBLOCK);
		retval=read(cheat_to_car2[0],&reloc,sizeof(reloc));
		if(reloc!=-1) {
			position=reloc;
		}
		int pid=fork();
		if(pid==0) {
			int pos=position;
			retval=write(car2_and_report[1],&position,sizeof(position));
			kill(getpid(),SIGKILL);
		}
		else if(pid>0) {
			wait(NULL);
		}
		else {
			printf("%s\n","Couldn't fork" );
		}
	}
}

void report(int p4)
{
	int flags;
	while(1) {
		sleep(1);
		int position1=0,position2=0;
		
		flags=fcntl(car1_and_report[0],F_GETFL);
		flags &= ~O_NONBLOCK;
		fcntl(car1_and_report[0],F_SETFL,flags);
		retval=read(car1_and_report[0],&position1,sizeof(position1));

		flags=fcntl(car2_and_report[0],F_GETFL);
		flags &= ~O_NONBLOCK;
		fcntl(car2_and_report[0],F_SETFL,flags);
		retval=read(car2_and_report[0],&position2,sizeof(position2));

		if(position1<100 && position2<100) {
			printf("%s %d\n","Position of car1 is",position1 );
			printf("%s %d\n","Position of car2 is",position2 );
			printf("\n");
		}
		else {
			if(position1>=100 && position2<100) {
				printf("%s\n","Car 1 is the winner" );
			}
			else if(position2>=100 && position1<100) {
				printf("%s\n","Car 2 is the winner" );
			}
			else if(position1>=100 && position2>=100) {
				printf("%s\n","The race was tied" );
			}
			int pid=fork();
			if(pid==0) {
				retval=write(car1_and_report[1],"stop\0",5);
				retval=write(car2_and_report[1],"stop\0",5);
				kill(getpid(),SIGKILL);
			}
			else if(pid>0) {
				wait(NULL);
			}
			else {
				printf("%s\n","Couldn't fork" );
			}
			kill(p4,SIGKILL);
			return;
		}
	}
}

int main()
{
	retval=pipe(cheat_to_car1);
	retval=pipe(cheat_to_car2);
	retval=pipe(car1_and_report);
	retval=pipe(car2_and_report);

	int p1=fork();
	if(p1==0) {
		cheatmode();
	}
	else if(p1>0) {
		int p2=fork();
		if(p2==0) {
			car1();
		}
		else if(p2>0) {
			int p3=fork();
			if(p3==0) {
				car2();
			}
			else if(p3>0) {
				report(p1);
				wait(NULL);
			}
			else {
				printf("%s\n","Couldn't fork" );
			}
			wait(NULL);
		}
		else {
			printf("%s\n","Couldn't fork" );
		}
		wait(NULL);
	}
	else {
		printf("%s\n","Couldn't fork" );
	}
	return 0;
}