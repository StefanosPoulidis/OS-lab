#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "proc-common.h"

#define SLEEP_PROC_SEC  10
#define SLEEP_TREE_SEC  3

/*
 * Create this process tree:
 * A-+-B---D
 *   `-C
 */
/*
 * The initial process forks the root of the process tree,
 * waits for the process tree to be completely created,
 * then takes a photo of it using show_pstree().
 *
 * How to wait for the process tree to be ready?
 * In ask2-{fork, tree}:
 *      wait for a few seconds, hope for the best.
 * In ask2-signals:
 *      use wait_for_ready_children() to wait until
 *      the first process raises SIGSTOP.
 */
int main(void)
{
	pid_t pid;
	int status;

	/* Fork root of process tree */
	pid = fork();
	if (pid < 0) {
		perror("main: fork");
		exit(1);
	}
	if (pid == 0) {
		/* A */
		//fork_procs();
		pid=fork(); //fork for B
		if (pid<0){
			perror("main: fork");
		}
		if (pid == 0){
			/* B */
			//fork_procs(); 
			pid=fork();//fork for D
			if (pid<0){
				perror("main: fork");
			}
			if (pid == 0){
				/* D */
				change_pname("D");
				printf("D: Sleeping...\n");
				sleep(SLEEP_PROC_SEC);
				//pid = wait(&status);
				//explain_wait_status(pid,status);
				printf("D: Exiting...\n");
			        exit(13);

			}
			change_pname("B");
			printf("B: Waiting\n");
			pid = wait(&status); //B waits for D to end
			explain_wait_status(pid, status);
			printf("B: Exiting...\n");
			exit(19);	

		}
		/* A continues here */
		pid=fork();//fork for C
		if (pid<0){
			perror("main: fork");
		}
		if (pid == 0){
			/* C */
			change_pname("C");
		        printf("C: Sleeping...\n");
		        sleep(SLEEP_PROC_SEC);
		       // pid = wait(&status);
			//explain_wait_status(pid,status);
			printf("C: Exiting...\n");
			exit(17);
																			
		}



		change_pname("A");
		printf("A: Waiting\n");
		pid = wait(&status);
		explain_wait_status(pid,status);
		pid = wait(&status);
		explain_wait_status(pid,status);//A waits for its children B C to end
		printf("A: Exiting...\n");
		exit(16);
	}

	/*
	 * Father
	 */
	/* for ask2-signals */
	/* wait_for_ready_children(1); */

	/* for ask2-{fork, tree} */
	sleep(SLEEP_TREE_SEC);

	/* Print the process tree root at pid */
	show_pstree(pid);
	printf("\n");
	printf("\n");
	printf("\n");
	show_pstree(getpid());
	/* for ask2-signals */
	/* kill(pid, SIGCONT); */

	/* Wait for the root of the process tree to terminate */
	pid = wait(&status);
	explain_wait_status(pid, status);
	
	return 0;
}
