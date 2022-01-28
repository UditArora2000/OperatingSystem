#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

char * get_filename(char *str,int start,int *end) {
	char arr[100];
	char *sliced;
	for(int i=start;i<strlen(str);i++) {
		if(str[i]!='>' && str[i]!=' ' && str[i]!='<' && str[i]!='|') {
			arr[i-start]=str[i];
			*end=i;
		}
		else {
			*end=i-1;
			break;
		}
	}
	sliced=arr;
	return sliced;
}

int main() {
	
	char cmd[1000];
	while(1) {
		int saved_stdin=dup(0);
		int saved_stdout=dup(1);
		int saved_stderr=dup(2);
		printf("\033[1;34m");
		printf("%s ","$" );
		printf("\033[0m" );
		// write(1,"$",2);
		for(int p=0;p<1000;p++)
			cmd[p]=NULL;
		if(scanf("%[^\n]%*c",cmd)!=1) {
			break;
		}
		int blanks=0;
		for(int i=0;i<strlen(cmd);i++) {
			if(cmd[i]==' ')
				blanks+=1;
			else
				break;
		}	
		int h=blanks;
		int stderr_to_out=0;
		int n=strlen(cmd);
		while(h<n && h!=0) {
			cmd[h-blanks]=cmd[h];
			cmd[h]=NULL;
			h+=1;
		}
		n=strlen(cmd);
		h=n-1;
		while(h>=0) {
			if(cmd[h]==' ')
				cmd[h]=NULL;
			else
				break;
			h-=1;
		}
		char final[1000]="";
		char *args[1000];
		for(int i=0;i<1000;i++) {
			args[i]=NULL;
		}
		int c=0;
		int st=0,end=0;
		char arr[100][50];
		for(int p=0;p<100;p++)
			for(int q=0;q<50;q++) 
				arr[p][q]=NULL;		
		int pipe_present=0;
		int file_desc;
		char *args_for_pipes[100][1000];
		char final_for_pipes[100][1000];
		for(int i=0;i<100;i++) {
			for(int j=0;j<1000;j++) {
				final_for_pipes[i][j]=NULL;
				args_for_pipes[i][j]=NULL;
			}
		}
		int c_for_pipes=0;
		cmd[strlen(cmd)]=' ';
		for(int k=0;k<strlen(cmd);k++) {
			if(cmd[k]=='|') {
				pipe_present=1;
				break;
			}
		}
		int output_from_pipes[100];
		char output_file_pipe[100][100];
		int input_from_file[100];
		char input_file_pipe[100][100];
		for(int p=0;p<100;p++) {
			output_from_pipes[p]=0;
			input_from_file[p]=0;
			for(int q=0;q<100;q++) {
				output_file_pipe[p][q]=NULL;
				input_file_pipe[p][q]=NULL;
			}
		}	

		int i=0;
		if(!pipe_present) {
			while(i<strlen(cmd)) {
				if(cmd[i]=='e' && cmd[i+1]=='x' && cmd[i+2]=='i' && cmd[i+3]=='t') {
					exit(0);
				}
				else if(cmd[i]=='1') {
					while(cmd[i+1]==' ')
						i+=1;
					if(cmd[i+1]=='>') {
						int val;
						while(cmd[i+2]==' ') i+=1;
						char *filename=get_filename(cmd,i+2,&val); 
						char fn[100]="";
						for(int j=0;j<strlen(filename);j++) {
							fn[j]=filename[j];
						}
						close(1);
						file_desc=open(fn,O_WRONLY | O_CREAT, 0777);	
					}
					else {
						perror("Error received");
					}
				}
				else if(cmd[i]=='2') {
					while(cmd[i+1]==' ')
						i+=1;
					if(cmd[i+1]=='>') {
						int val;
						i+=1;
						while(cmd[i+1]==' ') i+=1;
						if(cmd[i+1]!='&') {
							char *filename=get_filename(cmd,i+2,&val); 
							i=val;
							char fn[100]="";
							for(int j=0;j<strlen(filename);j++) {
								fn[j]=filename[j];
							}
							close(2);
							file_desc=open(fn,O_WRONLY | O_CREAT);	
						}
						else {
							i+=2;
							close(2);
							dup(1);
						}
					}
					else {
						perror("Error received");
					}
				}
			
				else if(cmd[i]=='>' && cmd[i+1]!='>') {
					
					if(cmd[i-1]!=' ') {
						for(int j=st;j<end;j++) {
							char w=cmd[j];
							arr[c][strlen(arr[c])]=w;
						}
						if(c==0) {
							strcat(final,arr[c]);
							args[c]=final;
						}
						else {
							args[c]=arr[c];
						}
						c+=1;
						st=i+1;
						end=st;
					}

					while(cmd[i+1]==' ') {
						i+=1;
					}
					int val;
					char *filename=get_filename(cmd,i+1,&val); 
					i=val;
					st=i;
					end=st;
					char fn[100]="";
					for(int j=0;j<strlen(filename);j++) {
						fn[j]=filename[j];
					}
					close(1);
					file_desc=open(fn,O_WRONLY | O_CREAT,0777);
				}

				else if(cmd[i]=='>' && cmd[i+1]=='>') {

					if(cmd[i-1]!=' ') {
						for(int j=st;j<end;j++) {
							char w=cmd[j];
							arr[c][strlen(arr[c])]=w;
						}
						if(c==0) {
							strcat(final,arr[c]);
							args[c]=final;
						}
						else {
							args[c]=arr[c];
						}
						c+=1;
						st=i+1;
						end=st;
					}

					while(cmd[i+2]==' ') {
						i+=1;
					}
					int val;
					char *filename=get_filename(cmd,i+2,&val);
					i=val;
					st=i;
					end=st;
					char fn[100]="";
					for(int j=0;j<strlen(filename);j++) {
						fn[j]=filename[j];
					}
					close(1);
					file_desc=open(fn,O_WRONLY | O_APPEND | O_CREAT,0777);
				}

				else if(cmd[i]=='<') {

					if(cmd[i-1]!=' ') {
						for(int j=st;j<end;j++) {
							char w=cmd[j];
							arr[c][strlen(arr[c])]=w;
						}
						if(c==0) {
							strcat(final,arr[c]);
							args[c]=final;
						}
						else {
							args[c]=arr[c];
						}
						c+=1;
						st=i+1;
						end=st;
					}
					while(cmd[i+1]==' ') {
						i+=1;
					}
					int val;
					while(cmd[i+1]==' ') i+=1;
					char *filename=get_filename(cmd,i+1,&val);
					i=val;
					st=i;
					end=st;
					char fn[100]="";
					for(int j=0;j<strlen(filename);j++) {
						fn[j]=filename[j];
					}
					close(0);
					file_desc=open(fn,O_RDONLY);
				}

				else if(cmd[i]==' ') {
					while(cmd[i+1]==' ') {
						i+=1;
					}
					for(int j=st;j<end;j++) {
						char w=cmd[j];
						arr[c][strlen(arr[c])]=w;
					}
					if(c==0) {
						strcat(final,arr[c]);
						args[c]=final;
					}
					else {
						args[c]=arr[c];
					}
					c+=1;
					st=i+1;
					end=st;
				}

				else {
					end+=1;
				}
				i+=1;
			}
		}

		else {
			while(i<strlen(cmd)) {
				if(cmd[i]=='e' && cmd[i+1]=='x' && cmd[i+2]=='i' && cmd[i+3]=='t') {
					exit(0);
				}
				else if(cmd[i]=='1') {
					while(cmd[i+1]==' ')
						i+=1;
					if(cmd[i+1]=='>') {
						i+=1;
						int val;
						while(cmd[i+1]==' ') i+=1;
						char *filename=get_filename(cmd,i+1,&val); 
						i=val;
						for(int j=0;j<strlen(filename);j++) {
							output_file_pipe[c_for_pipes][j]=filename[j];
						}
						output_from_pipes[c_for_pipes]=1;
					}
					else {
						perror("Invalid syntax");
					}
				}
				else if(cmd[i]=='2') {
					while(cmd[i+1]==' ')
						i+=1;
					if(cmd[i+1]=='>') {
						i+=1;
						int val;
						while(cmd[i+1]==' ') i+=1;
						if(cmd[i+1]!='&') {
							char *filename=get_filename(cmd,i+1,&val); 
							i=val;
							for(int j=0;j<strlen(filename);j++) {
								output_file_pipe[c_for_pipes][j]=filename[j];
							}
						}
						else {
							stderr_to_out=1;
							i+=2;
						}
						output_from_pipes[c_for_pipes]=3;	
					}
					else {
						perror("Invalid syntax");
					}
				}
				else if(cmd[i]=='|') {

					if(cmd[i-1]!=' ' && input_from_file[c_for_pipes]==0 && output_from_pipes[c_for_pipes]==0) {
						for(int j=st;j<end;j++) {
							char w=cmd[j];
							arr[c][strlen(arr[c])]=w;
						}
						if(c==0) {
							strcat(final_for_pipes[c_for_pipes],arr[c]);
							args_for_pipes[c_for_pipes][c]=final_for_pipes[c_for_pipes];
						}
						else {
							args_for_pipes[c_for_pipes][c]=malloc(sizeof(arr[c]));
							memcpy(args_for_pipes[c_for_pipes][c],arr[c],sizeof(arr[c]));
						}
					}
					st=i+1;
					end=st;
					for(int p=0;p<100;p++) 
						for(int q=0;q<50;q++)
							arr[p][q]=NULL;
					c=0;
					c_for_pipes+=1;
				}

			
				else if(cmd[i]=='>' && cmd[i+1]!='>') {
					
					if(cmd[i-1]!=' ') {
						for(int j=st;j<end;j++) {
							char w=cmd[j];
							arr[c][strlen(arr[c])]=w;
						}
						if(c==0) {
							strcat(final_for_pipes[c_for_pipes],arr[c]);
							args_for_pipes[c_for_pipes][c]=final_for_pipes[c_for_pipes];
						}
						else {
							args_for_pipes[c_for_pipes][c]=malloc(sizeof(arr[c]));
							memcpy(args_for_pipes[c_for_pipes][c],arr[c],sizeof(arr[c]));
						}
						c+=1;
						st=i+1;
						end=st;
					}

					while(cmd[i+1]==' ') {
						i+=1;
					}
					int val;
					char *filename=get_filename(cmd,i+1,&val); 
					i=val;
					st=i;
					end=st;
					output_from_pipes[c_for_pipes]=1;
					// char fn[100]="";
					for(int j=0;j<strlen(filename);j++) {
						output_file_pipe[c_for_pipes][j]=filename[j];
					}
				}

				else if(cmd[i]=='>' && cmd[i+1]=='>') {

					if(cmd[i-1]!=' ') {
						for(int j=st;j<end;j++) {
							char w=cmd[j];
							arr[c][strlen(arr[c])]=w;
						}
						if(c==0) {
							strcat(final_for_pipes[c_for_pipes],arr[c]);
							args_for_pipes[c_for_pipes][c]=final_for_pipes[c_for_pipes];
						}
						else {
							args_for_pipes[c_for_pipes][c]=malloc(sizeof(arr[c]));
							memcpy(args_for_pipes[c_for_pipes][c],arr[c],sizeof(arr[c]));
						}
						c+=1;
						st=i+1;
						end=st;
					}
					while(cmd[i+2]==' ') {
						i+=1;
					}
					int val;
					char *filename=get_filename(cmd,i+2,&val);
					i=val;
					st=i;
					end=st;
					for(int j=0;j<strlen(filename);j++) {
						output_file_pipe[c_for_pipes][j]=filename[j];
					}
					output_from_pipes[c_for_pipes]=2;
				}

				else if(cmd[i]=='<') {
					if(cmd[i-1]!=' ') {
						for(int j=st;j<end;j++) {
							char w=cmd[j];
							arr[c][strlen(arr[c])]=w;
						}
						if(c==0) {
							strcat(final_for_pipes[c_for_pipes],arr[c]);
							args_for_pipes[c_for_pipes][c]=final_for_pipes[c_for_pipes];
						}
						else {
							args_for_pipes[c_for_pipes][c]=malloc(sizeof(arr[c]));
							memcpy(args_for_pipes[c_for_pipes][c],arr[c],sizeof(arr[c]));
						}
						c+=1;
						st=i+1;
						end=st;
					}
					while(cmd[i+1]==' ') {
						i+=1;
					}
					int val;
					char *filename=get_filename(cmd,i+1,&val);
					i=val;
					st=i;
					end=st;
					input_from_file[c_for_pipes]=1;
					for(int p=0;p<strlen(filename);p++) {
						input_file_pipe[c_for_pipes][p]=filename[p];
					}
				}

				else if(cmd[i]==' ') {
					if(cmd[i-1]!='|' && input_from_file[c_for_pipes]==0 && output_from_pipes[c_for_pipes]==0) {
						while(cmd[i+1]==' ') 
							i+=1;
						for(int j=st;j<end;j++) {
							char w=cmd[j];
							arr[c][strlen(arr[c])]=w;
						}
						if(c==0) {
							strcat(final_for_pipes[c_for_pipes],arr[c]);
							args_for_pipes[c_for_pipes][c]=final_for_pipes[c_for_pipes];
						}
						else {
							args_for_pipes[c_for_pipes][c]=malloc(sizeof(arr[c]));
							memcpy(args_for_pipes[c_for_pipes][c],arr[c],sizeof(arr[c]));
						}
						c+=1;
					}
					else {
						while(cmd[i+1]==' ') 
							i+=1;
					}
					st=i+1;	end=st;
				}
				else {
					end+=1; 
				}
				i+=1;
			}
		}	

		if(pipe_present) {
			int fd[c_for_pipes][2];
			i=0;
			while(i<c_for_pipes+1) {
				pipe(fd[i]);
				int pid=fork();
				if(pid==0) {
					close(fd[i][0]);
					if(i!=c_for_pipes) {
						close(1);
						dup(fd[i][1]);
					}
					if(input_from_file[i]==1) {
						close(0);
						file_desc=open(input_file_pipe[i],O_RDONLY);
					}
					if(output_from_pipes[i]==1) {
						close(1);
						file_desc=open(output_file_pipe[i], O_WRONLY | O_CREAT, 0777);
					}
					else if(output_from_pipes[i]==2) {
						close(1);
						file_desc=open(output_file_pipe[i], O_WRONLY | O_APPEND | O_CREAT, 0777);
					}
					else if(output_from_pipes[i]==3) {
						if(stderr_to_out==0) {
							close(2);
							file_desc=open(output_file_pipe[i], O_WRONLY | O_CREAT, 0777);
						}
						else {
							close(2);
							dup(1);
						}
					}
					close(fd[i][1]);
					execvp(args_for_pipes[i][0],args_for_pipes[i]);
					perror("Error received");	
				}
				else if(pid>0) {
					wait(NULL);
					close(fd[i][1]);
					close(0);
					dup(fd[i][0]);
					if(i==c_for_pipes) {
						dup2(saved_stdin,0);
						close(saved_stdin);
					}
					close(fd[i][0]);
				}
				else 
					printf("%s\n","Failed to fork" );
				i+=1;
			}
		}
		else {
			for(int ab=0;ab<1000;ab++) {
				if(args[ab]==NULL)
					break;
				if(	strlen(args[ab])==0) 
					args[ab]=NULL;
			}
			int pid=fork();
			if(pid==0) {
				execvp(args[0],args);
				perror("Error received");
			}
			else if(pid>0) 
				wait(NULL);
			else 
				printf("%s\n","Failed to fork");
		}
		dup2(saved_stderr,2);
		close(saved_stderr);
		dup2(saved_stdout,1);
		close(saved_stdout);
		dup2(saved_stdin,0);
		close(saved_stdin);
	}
}