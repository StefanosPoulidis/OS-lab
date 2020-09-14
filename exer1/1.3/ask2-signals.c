#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "tree.h"
#include "proc-common.h"

void let_it_fork(struct tree_node * root);

int main(int argc, char *argv[])
{
	pid_t pid;
	int status;
	struct tree_node *root;

	if (argc < 2){
		fprintf(stderr, "Usage: %s <tree_file>\n", argv[0]);
		exit(1);
	}

	/* Read tree into memory */
	root = get_tree_from_file(argv[1]);

	/* Fork root of process tree */
	pid = fork();
	if (pid < 0) {
		perror("main: fork");
		exit(1);
	}
	if (pid == 0) {
		/* Child */
		/* root of my tree */
		/*for every child I enter this if and 
		decide in let_it_fork if it is a father of another child 
		to fork again until I reach leaves*/
		let_it_fork(root);
		exit(0);
	}
	
	/*
	 * Father
	 */
	/* for ask2-signals */
	wait_for_ready_children(1);
	/* for ask2-{fork, tree} */
	/* sleep(SLEEP_TREE_SEC); */
	/* Print the process tree root at pid */
	show_pstree(pid);
	kill(pid, SIGCONT);
	/* Wait for the root of the process tree to terminate */
	wait(&status);
	explain_wait_status(pid, status);
	return 0;
}



void let_it_fork(struct tree_node * root)
{	
	//definition of the necessary variables
	change_pname(root->name); //gia na allaksoume to id ka8e diadikasias sto epi8ymhto onoma
        int i;
        int status;
	pid_t p[root->nr_children];
//create an array to hold all my children's PIDs so as to send SIGCONT 
        if (root->nr_children == 0){
                printf("Process %s started. It's a leaf\n",root->name);
        	printf("Process %s: Waiting for SIGCONT\n",root->name);
	        raise(SIGSTOP);
		printf("PID = %ld, name = %s is awake\n",
                (long)getpid(), root->name);
		printf("Process %s: Exiting\n",root->name);
                exit(0);
        }
        else{
                //periptwsh pou exoume paidia, prepei na ta kanoume fork
                pid_t pid;
		printf("Process %s started creating children\n",root->name);
                for (i=0;i<root->nr_children;i++){ 
                       pid=fork();  
	              	if (pid<0){
                                perror("problem with fork\n");
                                exit(1);
                        }
                        if (pid == 0){
                                let_it_fork(root->children+i);          //child, recursive call gia ka8e pointer se child
				printf("Process %s : Exiting\n",root->name); 
                               exit(0);
                        }
			else{ 	//so as to enter only for the parent
				p[i]=pid;
			}
			wait_for_ready_children(1); //waits for every child
		}		
		//wait_for_ready_children(root->nr_children);
		printf("PID= %ld, name = %s is waiting for SIGCONT\n",(long)getpid(),root->name);
		raise(SIGSTOP); //waiting for SIGCONT
		printf("PID = %ld, name = %s is awake\n",
                (long)getpid(), root->name);
		//edw prepei na steilei SIGCONT diadoxika sta paidia ths
		for(i=0;i<root->nr_children;i++){
			kill(p[i],SIGCONT);
			wait(&status);
			explain_wait_status(p[i],status);
		} 
//		pid=wait(&status);
//		explain_wait_status(pid,status);
        	exit(0);
	}	
}
