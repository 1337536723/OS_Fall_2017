/*Put all the a b c in shared memory*/
#include <unistd.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#define ULL unsigned long long int
ULL one_process(ULL*,ULL*,int);
ULL four_process(ULL*,ULL*,ULL*,int);
ULL* assign_shared_mem(int);
void detach_shm(ULL*);
void release_all_shm(ULL*);
int main()
{
    unsigned int dim,tmp;
    ULL sum=0;
    printf("Please enter the dimension of 2 square matrices:  ");
    scanf("%d",&dim);

    //shared memory allocation
    ULL* shm_addr_a = assign_shared_mem(dim);
    for(int row=0;row<dim;row++)
        for(int col=0;col<dim;col++)
            shm_addr_a[row*dim+col]=row*dim+col;
    /*----------------------------------------------------------------------------------*/
    //printf("Enter matrix B: ");
    ULL* shm_addr_b = assign_shared_mem(dim);
    for(int row=0;row<dim;row++)
        for(int col=0;col<dim;col++)
            shm_addr_b[row*dim+col]=row*dim+col;

    ULL* shm_addr_c = assign_shared_mem(dim);

    struct timeval start, end;

    gettimeofday(&start, 0);
    sum=one_process(shm_addr_a,shm_addr_b,dim);
    gettimeofday(&end, 0);
    int sec =abs(end.tv_sec - start.tv_sec);
    int usec = abs(end.tv_usec - start.tv_usec);
    printf("1-process, checksum = %llu \n",sum);
    printf("elapsed %f ms\n", sec*1000+(usec/1000.0));

    gettimeofday(&start, 0);
    sum=four_process(shm_addr_a,shm_addr_b,shm_addr_c,dim);
    gettimeofday(&end, 0);
    int sec2 = abs(end.tv_sec - start.tv_sec);
    int usec2 = abs(end.tv_usec - start.tv_usec);
    printf("4-process, checksum = %llu \n",sum);
    printf("elapsed %f ms\n", sec2*1000+(usec2/1000.0));

    printf("Acceleration rate = %f\n",(sec*1000+(usec/1000.0))/(sec2*1000+(usec2/1000.0)));
    return 0;
}
ULL one_process(ULL* shm_addr_a,ULL* shm_addr_b,int dim)
{
    ULL sum=0;
    for(int row=0;row<dim*dim;row+=dim)
    {
        for(int col=0;col<dim;col++)
        {
            ULL cell=0; //a cell
            for(int k=0;k<dim;k++)
            {
                cell+=shm_addr_a[row+k]*shm_addr_b[k*dim+col];
            }
            //printf("Getvalue %llu \n",cell);
            sum+=cell;
        }
    }
    return sum;
}
/*
process tree should be like

            main
            /     \
fork1----> /      \  <---- fork2
          /       \
         child1   child3
            |
            | <---- fork2 from child1
            |
        child2 from child1
*/

ULL four_process(ULL* shm_addr_a,ULL* shm_addr_b,ULL* shm_addr_c,int dim)
{

    ULL sum = 0;


    //do fork
    pid_t pid;
    pid=fork();
    //printf("Fork 1 for child1\n");
    if(pid <0)
    {
        // if error, fork failed
        fprintf(stderr, "Fork Failed");
        exit(EXIT_FAILURE);
    }
    else if(pid==0) //child1
    {
        pid_t pid2;
        pid2=fork();
        //printf("Fork 2 for child2\n");
        if(pid2<0)
        {
            fprintf(stderr, "Fork Failed");
            exit(EXIT_FAILURE);
        }
        else if(pid2==0) //child2 for first 1/4 row of matrix multiplication
        {
            for(int row=0,cnt=0;row<(dim*dim)/4;row+=dim,cnt++)
            {
                for(int col=0;col<dim;col++)
                {
                    shm_addr_c[cnt*dim+col]=0; //initailize cell as 0
                    for(int k=0;k<dim;k++)
                    {
                        shm_addr_c[cnt*dim+col]+=shm_addr_a[row+k]*shm_addr_b[k*dim+col];
                    }
                    //printf("Get value %llu\n",shm_addr_c[cnt*dim+col]);
                }
            }
            //printf("Matrix ok 4\n");
            //dont forget to detach shm otherwise SIGSEGV
            detach_shm(shm_addr_a);
            detach_shm(shm_addr_b);
            detach_shm(shm_addr_c);
            exit(EXIT_SUCCESS);
        }
        else //child1 (now become parent) for second 1/4 row of matrix multiplication
        {
            for(int row=(dim*dim)/4,cnt=dim/4;row<(dim*dim)/2;row+=dim,cnt++)
            {
                for(int col=0;col<dim;col++)
                {
                    shm_addr_c[cnt*dim+col]=0; //initailize cell as 0
                    for(int k=0;k<dim;k++)
                    {
                        shm_addr_c[cnt*dim+col]+=shm_addr_a[row+k]*shm_addr_b[k*dim+col];
                    }
                    //printf("Get value %llu\n",shm_addr_c[cnt*dim+col]);
                }
            }
            //printf("Matrix ok 3\n");
            wait(NULL);
            //dont forget to detach shm otherwise SIGSEGV
            detach_shm(shm_addr_a);
            detach_shm(shm_addr_b);
            detach_shm(shm_addr_c);
            exit(EXIT_SUCCESS);
        }

    }
    else //main parent
    {
        pid_t pid3;
        pid3=fork();
        //printf("Fork 3 for child3\n");
        if(pid3<0)
        {
            fprintf(stderr, "Fork Failed");
            exit(EXIT_FAILURE);
        }
        else if(pid3==0) //child3 for third 1/4 row of matrix multiplication
        {
            for(int row=(dim*dim)/2,cnt=dim/2;row<(dim*dim)*3/4;row+=dim,cnt++)
            {
                for(int col=0;col<dim;col++)
                {
                    shm_addr_c[cnt*dim+col]=0; //initailize cell as 0
                    for(int k=0;k<dim;k++)
                    {
                        shm_addr_c[cnt*dim+col]+=shm_addr_a[row+k]*shm_addr_b[k*dim+col];
                    }
                    //printf("Get value %llu\n",shm_addr_c[cnt*dim+col]);
                }
            }
            //printf("Matrix ok 2\n");
            //dont forget to detach shm otherwise SIGSEGV
            detach_shm(shm_addr_a);
            detach_shm(shm_addr_b);
            detach_shm(shm_addr_c);
            exit(EXIT_SUCCESS);
        }
        else //main parent (now become parent of TWO CHILD / WAIT 2 TIMES!!) for last 1/4 row of matrix multiplication
        {
            for(int row=(dim*dim)*3/4,cnt=dim*3/4;row<(dim*dim);row+=dim,cnt++)
            {
                for(int col=0;col<dim;col++)
                {
                    shm_addr_c[cnt*dim+col]=0; //initailize cell as 0
                    for(int k=0;k<dim;k++)
                    {
                        shm_addr_c[cnt*dim+col]+=shm_addr_a[row+k]*shm_addr_b[k*dim+col];
                    }
                    //printf("Get value %llu\n",shm_addr_c[cnt*dim+col]);
                }
            }
            //printf("Matrix ok \n");
            wait(NULL); //wait for child1 and child3 to finish!!!! DUAL WAIT!!!
            wait(NULL);
            /*exit(EXIT_SUCCESS); DONT DO THIS, OR WILL EXIT AND NO RETURN
            AND NEGLECT THE FUTURE PROCESS SINCE IT IS TREATED AS TERMINATED NORMALLY
            */
            for(unsigned int i=0;i<dim*dim;i++) //only the main parent to do this, do not write before return sum, which will cause over calculation
            {
                //printf("shm res %llu \n",shm_addr_c[i]);
                sum+=shm_addr_c[i];
            }

            //
            //release_all_shm(shm_addr_c);
        }

    }

    return sum;
}
ULL* assign_shared_mem(int dim) //return the address of the shared memory
{
    int shmflg = 0666|IPC_CREAT;// 110110110 permission rw-rw-rw in linux
    //owner rw  group  rw  other   rw
    int shmid=shmget(IPC_PRIVATE,sizeof(ULL)*dim*dim,shmflg);
    if(shmid==-1)
    {
        fprintf(stderr, "Shared memory get error\n");
        exit(EXIT_FAILURE); //exit with error
    }
    ULL* shm_addr = shmat(shmid,NULL,shmflg);
    if(shm_addr==-1)
    {
        fprintf(stderr, "Shared memory attach error\n");
        exit(EXIT_FAILURE); //exit with error
    }
    return shm_addr;
}
void detach_shm(ULL* shm_addr)
{
    int detach=shmdt(shm_addr);
    if(detach==-1)
    {
        fprintf(stderr, "Shared memory detach error\n");
        exit(EXIT_FAILURE);
    }
}
void release_all_shm(ULL* shm_addr)
{
    //detach_shm(shm_addr);
    int release=shmctl(shm_addr,IPC_RMID /*DELETE SHARED MEMORY*/,0);
    if(release==-1)
    {
        fprintf(stderr, "Release whole shared memory detach error\n");
        exit(EXIT_FAILURE);
    }
}
