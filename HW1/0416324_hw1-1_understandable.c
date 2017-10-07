#include <unistd.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <stdlib.h>
/*
    fork diagram should be

        main
          |  fork1
        child1
fork2    /   \  fork 3
    child2  child3
fork4 |        | fork4
    child4  child5
*/

int main()
{
    unsigned int fork_cnt=0;
    int parent_pid,child_pid;
    printf("Main function start : pid= %d \n",getpid());

    pid_t pid=0,pid2=0,pid3=0,pid4=0,pid5=0;
    //printf("------------------#%d Fork executes! \n",fork_cnt+1);
    parent_pid=getpid();
    pid=fork(); //fork1 to get child1 return child ID and itself
    fork_cnt++;
    child_pid=getpid();
    if(pid<0)
    {
        fprintf(stderr, "Fork failed\n");
        return 1;
    }
    else if(pid==0) //for child1 to execute since it is the child process
    {
        printf("Forka %d. I'm the child %d, my parent is %d.\n",fork_cnt,child_pid,parent_pid);

        //printf("------------------#%d Fork executes! \n",fork_cnt+1);
        parent_pid=getpid();
        pid2=fork(); //fork2 for child2
        fork_cnt++;
        child_pid=getpid();

        if(pid2<0)
        {
            fprintf(stderr, "Fork failed\n");
            return 1;
        }
        else if(pid2==0) //after fork, print child only
        {
                printf("Forkb %d. I'm the child %d, my parent is %d.\n",fork_cnt,child_pid,parent_pid);
                if(pid2==0) //fork4 for child4
                {
                    parent_pid=getpid();
                    pid4=fork(); //fork3 for child3
                    fork_cnt++;
                    child_pid=getpid();

                    if(pid4<0)
                    {
                        fprintf(stderr, "Fork failed\n");
                        return 1;
                    }
                    else if(pid4==0) //after fork, print child only
                    {
                        printf("Forkd %d. I'm the child %d, my parent is %d.\n",fork_cnt,child_pid,parent_pid);
                        exit(EXIT_SUCCESS);
                    }
                    else //child2 wait for child4
                    {
                        //printf("Process %d wait for its child \n",getpid());
                        wait(NULL);
                        //printf("Process %d 's child terminated \n",getpid());
                    }
                    exit(EXIT_SUCCESS);

                }
        }
        else
        {
            //printf("------------------#%d Fork executes! \n",fork_cnt);
            parent_pid=getpid();
            pid3=fork(); //fork3 for child3
            child_pid=getpid();

            if(pid3<0)
            {
                fprintf(stderr, "Fork failed\n");
                return 1;
            }
            else if(pid3==0) //after fork, print child only
            {
                    printf("Forkc %d. I'm the child %d, my parent is %d.\n",fork_cnt,child_pid,parent_pid);
                    if(pid3==0) //fork4 for child5
                    {
                        parent_pid=getpid();
                        pid5=fork(); //fork3 for child3
                        fork_cnt++;
                        child_pid=getpid();

                        if(pid5<0)
                        {
                            fprintf(stderr, "Fork failed\n");
                            return 1;
                        }
                        else if(pid5==0) //after fork, print child only
                        {
                            printf("Forke %d. I'm the child %d, my parent is %d.\n",fork_cnt,child_pid,parent_pid);
                            exit(EXIT_SUCCESS);
                        }
                        else //child2 wait for child4
                        {
                            ////printf("Process %d wait for its child \n",getpid());
                            wait(NULL);
                            ////printf("Process %d 's child terminated \n",getpid());
                        }
                        exit(EXIT_SUCCESS);
                    }
            }
            else
            {

                //child1 wait for both child2 and child3
                //printf("Process %d wait for its child \n",getpid());
                wait(NULL);
                //printf("Process %d 's child terminated \n",getpid());

                //printf("Process %d wait for its child \n",getpid());
                wait(NULL);
                //printf("Process %d 's child terminated \n",getpid());
                exit(EXIT_SUCCESS);
            }

        }
    }
    else //main wait for child1
    {
        //printf("Process %d wait for its child \n",getpid());
        wait(NULL);
        //printf("Process %d 's child terminated \n",getpid());
    }

    return 0;
}
