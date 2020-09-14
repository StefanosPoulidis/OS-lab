#include <stdio.h>
#include <stdlib.h>

#include "tree.h"
#include "proc-common.h"

int sleeping=10;
int flag=0;
//pid_t headp=0;

void let_it_fork(struct tree_node * );

int main(int argc, char *argv[])
{	pid_t pid;
	struct tree_node *root;
	int status;
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <input_tree_file>\n\n", argv[0]);
		exit(1);
	}

	root = get_tree_from_file(argv[1]);
	pid = fork();
	if (pid<0){
		perror("main: fork\n");	
		exit(1);
	}
	if (pid==0){
		//call my void for the first child to get its pid for the tree
		let_it_fork(root); 
		exit(1);
	}
	
	//show_pstree(pid);
	//printf("%d\n",headp);
	//sleep(3);
	//if (flag!=0)
	//	show_pstree(headp);
	//else
	//	show_pstree(getpid());
	show_pstree(pid);

	//sleep(3);
	pid = wait(&status);
	//printf("%d\n",root);
	
	explain_wait_status(pid,status);
	//print_tree(root);
	//show_pstree(pid);
	return 0;
}

void let_it_fork(struct tree_node * root)
{
	change_pname(root->name); //gia na allaksoume to id ka8e diadikasias sto epi8ymhto onoma
	int i; 
	int status;	
//	printf("Working with %s node \n" , root->name);
	if (root->nr_children == 0){
		printf("%s: Sleeping\n",root->name);
		sleep(sleeping);//periptwsh pou einai fyllo
		printf("%s: Exiting\n",root->name);
		exit(1);
	}	
	else{
		//periptwsh pou exoume paidia, prepei na ta kanoume fork
		pid_t pid;
		
		for (i=0;i<root->nr_children;i++){
			pid=fork();
			//if (flag==0){
			//	headp=pid;//take the head pid
			//	flag++;
			//}
			if (pid<0){
				perror("problem with fork\n");
				exit(1);
			}
			if (pid == 0){
				let_it_fork(root->children+i);		//child, recursive call gia ka8e pointer se child
				exit(1);
			}	
		}
		
		//edw synexizei mono o goneas (pid>0)
		//kai prepei na perimenei ta paidia tou
		for (i=0;i<root->nr_children;i++){
			printf("%s: Waiting\n",root->name);
			pid=wait(&status);
			explain_wait_status(pid,status); //wait for every child and diplay appropriate message
		}
	}
	printf("%s: Exiting\n",root->name);
}



