#include <unistd.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/time.h>


int main()
{
    unsigned int fork_cnt=0;
    int parent_pid,child_pid;

    pid_t pid;
    parent_pid=getpid();
    pid=fork(); //fork1 to get child return child ID and itself
    fork_cnt++;
    child_pid=getpid();
    if(pid<0)
    {
        printf("Fork failed, FUCK YOU");
        return 1;
    }
    else if(pid==0)
        printf("Fork %d. I'm the child %d, my parent is %d.\n",fork_cnt,child_pid,parent_pid); //use getpid to get MY OWN PROCESS ID (ex: 666 in child self, however, return 0 to parent)
    /*----------fork1 ends fork 2 starts-----------*/
    if(pid<0)
    {
        printf("Fork failed, FUCK YOU");
        return 1;
    }
    else if(pid==0)
    {
        parent_pid=getpid();
        pid=fork(); //fork2 update pid
        fork_cnt++;
        child_pid=getpid();

        if(pid==0) //after fork, print child only
            printf("Fork %d. I'm the child %d, my parent is %d.\n",fork_cnt,child_pid,parent_pid);

        if(pid>0) //since former main function does not enter this loop, second loop will still not come in
        {
            parent_pid=getpid();
            pid=fork(); //fork2 update pid
            child_pid=getpid();
            if(pid==0) //after fork, print child only
                printf("Fork %d. I'm the child %d, my parent is %d.\n",fork_cnt,child_pid,parent_pid);
        }
    }

    /*----------fork2 ends fork 3 starts-----------*/
    if(pid<0)
    {
        printf("Fork failed, FUCK YOU");
        return 1;
    }
    else if(pid==0)
    {
        parent_pid=getpid();
        pid=fork(); //fork2 update pid
        fork_cnt++;
        child_pid=getpid();
        if(pid==0) //after fork, print child only
            printf("Fork %d. I'm the child %d, my parent is %d.\n",fork_cnt,child_pid,parent_pid);
    }

    return 0;
}
