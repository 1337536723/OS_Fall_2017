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

unsigned long long int one_process(int**,int**,int);
unsigned long long int four_process(int**,int**,int);
unsigned long long int* assign_shared_mem(int);
void detach_shm(unsigned long long int*);
void release_all_shm(unsigned long long int*);
int main()
{
    unsigned int dim,tmp;
    unsigned long long int sum=0;
    printf("Please enter the dimension of 2 square matrices \n");
    scanf("%d",&dim);

    //printf("Enter matrix A: ");
    //pointer to pointer allocation
    unsigned int** A = (int **)malloc(dim * sizeof(unsigned int *)); //malloc row
    for(int row=0;row<dim;row++) //malloc each row's col
        A[row] = (int *)malloc(dim * sizeof(unsigned int *));

    for(int row=0;row<dim;row++)
    {
        for(int col=0;col<dim;col++)
        {
            /*printf("Enter element of r=%d,c=%d ",row,col);
            scanf("%d",&tmp);*/
            A[row][col]=row*dim+col;//tmp;
        }
    }
    /*----------------------------------------------------------------------------------*/
    //printf("Enter matrix B: ");
    unsigned int** B = (int **)malloc(dim * sizeof(unsigned int *));//malloc row
    for(int row=0;row<dim;row++) //malloc each row's col
        B[row] = (int *)malloc(dim * sizeof(unsigned int *));

    for(int row=0;row<dim;row++)
    {
        for(int col=0;col<dim;col++)
        {
            /*printf("Enter element of r=%d,c=%d ",row,col);
            scanf("%d",&tmp);*/
            B[row][col]=row*dim+col;//tmp;
        }
    }
    /*Time estimation*/
    struct timeval start, end;

    gettimeofday(&start, 0);
    sum=one_process(A,B,dim);
    gettimeofday(&end, 0);
    int sec = end.tv_sec - start.tv_sec;
    int usec = end.tv_usec - start.tv_usec;
    printf("1-process, checksum = %llu \n",sum);
    printf("elapsed %f ms \n",sec*1000+(usec/1000.0));

    gettimeofday(&start, 0);
    sum=four_process(A,B,dim);
    gettimeofday(&end, 0);
    int sec2 = end.tv_sec - start.tv_sec;
    int usec2 = end.tv_usec - start.tv_usec;
    printf("4-process, checksum = %llu \n",sum);
    printf("elapsed %f ms \n",sec2*1000+(usec2/1000.0));

    printf("Acceleration rate = %f\n",(sec*1000+(usec/1000.0))/(sec2*1000+(usec2/1000.0)));
    for (int i=0;i<dim;i++)
    {
        int* current_ptr_A=A[i];
        free(current_ptr_A);
        int* current_ptr_B=B[i];
        free(current_ptr_B);
    }

    return 0;
}
unsigned long long int one_process(int** A,int** B,int dim)
{
    unsigned long long int sum = 0;
    for(int row=0;row<dim;row++ )
    {
        for(int col=0;col<dim;col++ )
        {
            unsigned long long  int tc = 0;
            for(int k=0;k<dim;k++ )
            {
                tc += (A[row][k] * B[k][col]);
            }
            sum += tc;
        }
    }
    return sum;
}
unsigned long long int four_process(int** A,int** B,int dim)
{
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

    unsigned long long int sum = 0;
    unsigned long long int* shm_addr = assign_shared_mem(dim);

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
            for(int row=0;row<dim/4;row++)
            {
                for(int col=0;col<dim;col++)
                {
                    shm_addr[row*dim+col]=0; //initailize cell as 0
                    for(int k=0;k<dim;k++)
                    {
                        shm_addr[row*dim+col]+=A[row][k]*B[k][col];

                    }
                    //printf("Get value %d\n",shm_addr[row*dim+col]);
                }
            }
            //printf("Matrix ok 4\n");
            detach_shm(shm_addr);
            exit(EXIT_SUCCESS);
        }
        else //child1 (now become parent) for second 1/4 row of matrix multiplication
        {
            for(int row=dim/4;row<dim/2;row++)
            {
                for(int col=0;col<dim;col++)
                {
                    shm_addr[row*dim+col]=0; //initailize cell as 0
                    for(int k=0;k<dim;k++)
                    {
                        shm_addr[row*dim+col]+=A[row][k]*B[k][col];
                    }
                    //printf("Get value %d\n",shm_addr[row*dim+col]);
                }
            }
            //printf("Matrix ok 3\n");
            wait(NULL);
            detach_shm(shm_addr);
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
            for(int row=dim/2;row<(dim*3)/4;row++)
            {
                for(int col=0;col<dim;col++)
                {
                    shm_addr[row*dim+col]=0; //initailize cell as 0
                    for(int k=0;k<dim;k++)
                    {
                        shm_addr[row*dim+col]+=A[row][k]*B[k][col];
                    }
                    //printf("Get value %d\n",shm_addr[row*dim+col]);
                }
            }
            //printf("Matrix ok 2\n");
            detach_shm(shm_addr);
            exit(EXIT_SUCCESS);
        }
        else //main parent (now become parent of TWO CHILD / WAIT 2 TIMES!!) for last 1/4 row of matrix multiplication
        {
            for(int row=(dim*3)/4;row<dim;row++)
            {
                for(int col=0;col<dim;col++)
                {
                    shm_addr[row*dim+col]=0;
                    for(int k=0;k<dim;k++)
                    {
                        shm_addr[row*dim+col]+=A[row][k]*B[k][col];
                    }
                    //printf("Get value %d\n",shm_addr[row*dim+col]);
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
                //printf("shm res %llu \n",shm_addr[i]);
                sum+=shm_addr[i];
            }

            //printf("sum end sum is %llu\n",sum);
            //release_all_shm(shm_addr);
        }

    }

    return sum;
}
unsigned long long int* assign_shared_mem(int dim) //return the address of the shared memory
{
    int shmflg = 0666|IPC_CREAT;// 110110110 permission rw-rw-rw in linux
    //owner rw  group  rw  other   rw
    int shmid=shmget(IPC_PRIVATE,sizeof(unsigned long long int)*dim*dim,shmflg);
    if(shmid==-1)
    {
        fprintf(stderr, "Shared memory get error\n");
        exit(EXIT_FAILURE); //exit with error
    }
    unsigned long long int* shm_addr = shmat(shmid,NULL,shmflg);
    if(shm_addr==-1)
    {
        fprintf(stderr, "Shared memory attach error\n");
        exit(EXIT_FAILURE); //exit with error
    }
    return shm_addr;
}
void detach_shm(unsigned long long int* shm_addr)
{
    int detach=shmdt(shm_addr);
    if(detach==-1)
    {
        fprintf(stderr, "Shared memory detach error\n");
        exit(EXIT_FAILURE);
    }
}
void release_all_shm(unsigned long long int* shm_addr)
{
    //detach_shm(shm_addr);
    int release=shmctl(shm_addr,IPC_RMID /*DELETE SHARED MEMORY*/,0);
    if(release==-1)
    {
        fprintf(stderr, "Release whole shared memory detach error\n");
        exit(EXIT_FAILURE);
    }
}
