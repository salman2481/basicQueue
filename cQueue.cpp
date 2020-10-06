/*
FIRST IN FIRST OUT
The topRef is always pointing to the HEAD element of the Queue
The tailRef will be pointing to the last node element of the Queue
*/
#include<iostream>
#include"nodeClass.cpp"
#include"cStack.cpp"
using namespace std;

class cQueue : protected cStack {
	// The trailing tail pointer to keep look at the last node of the Queue
	cNode *tailRef;
public:
	//Default and Parameterized constructor
	cQueue() : tailRef(NULL) {}
	cQueue(cStack *&stackRef) : cStack(*stackRef), tailRef(topRef) {}

	//cQueue Class copy constructor
	cQueue(const cQueue &src) {
		this->topRef = src.topRef;
		this->tailRef = src.tailRef;

		if (src.topRef) {
			cNode *sptr, *dptr;
			dptr = this->topRef = new cNode(*src.topRef);
			sptr = topRef->nextNode;

			while (sptr) {
				dptr = new cNode(*sptr);
				dptr = dptr->nextNode;
				sptr = sptr->nextNode;
			}
			tailRef = dptr;
		}
	}

	/*
	Retrives node element at the top of the Queue
	As only the first element is to be retrieved from the Queue
	so the pop function of stack can can work
	*/
	cNode* remove() { return cStack::pop(); }

	/*
	Add node element at last of the Queue
	To accomplish First In First Out
	*/
	cQueue& add(cNode *&nodeRef) {
		if (tailRef)
		{
			tailRef->nextNode = nodeRef;
			tailRef = tailRef->nextNode;
		}
		else
		{
			tailRef = topRef = nodeRef;
		}
		tailRef->nextNode = NULL;
		nodeRef = NULL;
		
		return *this;
	}

	/*
	Ptinting the Queue from Top to Bottom
	Just elements form Queue are printed in an arranged manner
	printStack function of cStack class can work well
	*/
	void printQueue() { cStack::printStack(); }

	/*
	THe overloaded Assignment operator
	Cascaded assignments also enabled by returning reference
	*/
	cQueue& operator = (const cQueue &src) {
		if (this == &src) { return *this; }
		if (true) { cQueue temp; temp.topRef = topRef; temp.tailRef = tailRef; }
		if (true) {
			cQueue temp = src; topRef = temp.topRef; tailRef = temp.tailRef; temp.topRef = temp.topRef = NULL;
		}
		return *this;
	}

	/*
	Destructor of stack class is called implicitely and work fine
	No need to redefne the destructor
	*/

};
#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <limits.h>
#include <errno.h>

#define CMD_HISTORY_SIZE 10 

static char * cmd_history[CMD_HISTORY_SIZE];
static int cmd_history_count = 0;


static void exec_cmd(const char * line)
{
	char * CMD = strdup(line);
	char *params[10];
	int argc = 0;

	params[argc++] = strtok(CMD, " ");
	while(params[argc-1] != NULL){	
		params[argc++] = strtok(NULL, " ");
	}

	argc--; 


	int background = 0;
	if(strcmp(params[argc-1], "&") == 0){
		background = 1; 
		params[--argc] = NULL;	
	}

	int fd[2] = {-1, -1};	

	while(argc >= 3){


		if(strcmp(params[argc-2], ">") == 0){	// output


			fd[1] = open(params[argc-1], O_CREAT|O_WRONLY|O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP|S_IWGRP);
			if(fd[1] == -1){
				perror("open");
				free(CMD);
				return;
			}

			params[argc-2] = NULL;
			argc -= 2;
		}else if(strcmp(params[argc-2], "<") == 0){ // input
			fd[0] = open(params[argc-1], O_RDONLY);
			if(fd[0] == -1){
				perror("open");
				free(CMD);
				return;
			}
			params[argc-2] = NULL;
			argc -= 2;
		}else{
			break;
		}
	}

	int status;
	pid_t pid = fork();	
	switch(pid){
		case -1:	
			perror("fork");
			break;
		case 0:	// child
			if(fd[0] != -1){	
				if(dup2(fd[0], STDIN_FILENO) != STDIN_FILENO){
					perror("dup2");
					exit(1);
				}
			}
			if(fd[1] != -1){	
				if(dup2(fd[1], STDOUT_FILENO) != STDOUT_FILENO){
					perror("dup2");
					exit(1);
				}
			}
			execvp(params[0], params);
			perror("execvp");
			exit(0);
		default: // parent
			close(fd[0]);close(fd[1]);
			if(!background)
				waitpid(pid, &status, 0);
			break;
	}
	free(CMD);
}


static void add_to_history(const char * cmd){
	if(cmd_history_count == (CMD_HISTORY_SIZE-1)){	
		int i;
		free(cmd_history[0]);	

		for(i=1; i < cmd_history_count; i++)
			cmd_history[i-1] = cmd_history[i];
		cmd_history_count--;
	}
	cmd_history[cmd_history_count++] = strdup(cmd);	
}



static void run_from_history(const char * cmd){
	int index = 0;
	if(cmd_history_count == 0){
		printf("No commands in history\n");
		return ;
	}

	if(cmd[1] == '!') 
		index = cmd_history_count-1;
	else{
		index = atoi(&cmd[1]) - 1;	
		if((index < 0) || (index > cmd_history_count)){ 
			fprintf(stderr, "No such command in history.\n");
			return;
		}
	}
	printf("%s\n", cmd);	
	exec_cmd(cmd_history[index]);	
}



static void list_history(){
	int i;
	for(i=cmd_history_count-1; i >=0 ; i--){
		printf("%i %s\n", i+1, cmd_history[i]);
	}
}



static void signal_handler(const int rc){
	switch(rc){
		case SIGTERM:
		case SIGINT:
			break;
		
		case SIGCHLD:
			
			while (waitpid(-1, NULL, WNOHANG) > 0);
			break;
	}
}

// main

int main(int argc, char *argv[]){

	

	struct sigaction act, act_old;
	act.sa_handler = signal_handler;
	act.sa_flags = 0;
	sigemptyset(&act.sa_mask);
	if(	(sigaction(SIGINT,  &act, &act_old) == -1) ||
		(sigaction(SIGCHLD,  &act, &act_old) == -1)){ 
		perror("signal");
		return 1;
	}

	

	size_t line_size = 100;
	char * line = (char*) malloc(sizeof(char)*line_size);
	if(line == NULL){	
		perror("malloc");
		return 1;	
	}

	int inter = 0; 
	while(1){
		if(!inter)
			printf("mysh > ");
		if(getline(&line, &line_size, stdin) == -1){	
			if(errno == EINTR){	  
				clearerr(stdin); 
				inter = 1;	 
				continue; 
			}
			perror("getline");
			break;
		}
		inter = 0; 

		int line_len = strlen(line);
		if(line_len == 1){	
			continue;
		}
		line[line_len-1] = '\0'; 

		
		if(strcmp(line, "exit") == 0){ 
			break;
		}else if(strcmp(line, "history") == 0){ 
			list_history();
		}else if(line[0] == '!'){ 
			run_from_history(line);
		}else{ 
			add_to_history(line); 
			exec_cmd(line); 
		}
	}

	free(line);
	return 0;
}
