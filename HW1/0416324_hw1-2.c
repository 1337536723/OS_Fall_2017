/*Reference data
http://blog.csdn.net/ec06cumt/article/details/51444961
http://note.heron.me/2013/11/using-shared-memory-for-matrix.html
*/

#include <unistd.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <time.h>



unsigned long long int one_process(int**,int**)
unsigned int* assign_shared_mem(int)
int main()
{
    int dim;
    printf("Please enter the dimension of 2 square matrices \n");
    scanf("%d",&dim);

    pid_t pid;
    pid=fork();
    for(int i=0;i<3;i++)
    {
        if(pid==0)
        {
            pid=fork();
        }
    }

    return 0;
}
unsigned long long int one_process(int** A,int** B,int dim)
{
    unsigned long long sum = 0;
    for(int row=0 ; row<dim ; row++ )
    {
        for(int col=0 ; col<dim ; col++ )
        {
            unsigned long long  int tc = 0;
            for(int k=0 ; k<dim ; k++ )
            {
                tc += (A[row][k] * B[k][col]);
            }
            sum += tc;
        }
    }
    return sum;
}
unsigned int* assign_shared_mem(int dim) //return the address of the shared memory
{
    int shmflg = 0666|IPC_CREAT;// 110110110 permission rw-rw-rw in linux
    //owner rw  group  rw  other   rw
    shmget(,,);

    unsigned int* shm_addr;
    if(shm_addr==-1)
    {
        fprintf(stderr, "Shared memory error\n", );
    }
}
