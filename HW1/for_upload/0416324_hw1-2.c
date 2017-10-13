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
//#include <libexplain/shmctl.h>
#define UNT unsigned int
UNT one_process(UNT*,UNT*,int);
UNT four_process(UNT*,UNT*,UNT*,int);
UNT* assign_shared_mem(int,int,int []);
void detach_shm(UNT*);
void release_all_shm(int);
int main()
{
    unsigned int dim,tmp;
    UNT sum=0;
    printf("Dimension:  ");
    scanf("%d",&dim);
    //shm address collection
    int shmid_arr[3]={0,0,0};

    //shared memory allocation
    UNT* shm_addr_a = assign_shared_mem(dim,0,shmid_arr);
    for(int row=0;row<dim;row++)
        for(int col=0;col<dim;col++)
            shm_addr_a[row*dim+col]=row*dim+col;
    /*----------------------------------------------------------------------------------*/
    //printf("Enter matrix B: ");
    UNT* shm_addr_b = assign_shared_mem(dim,1,shmid_arr);
    for(int row=0;row<dim;row++)
        for(int col=0;col<dim;col++)
            shm_addr_b[row*dim+col]=row*dim+col;

    UNT* shm_addr_c = assign_shared_mem(dim,2,shmid_arr);

    struct timeval start, end;

    gettimeofday(&start, 0);
    sum=one_process(shm_addr_a,shm_addr_b,dim);
    gettimeofday(&end, 0);
    int sec =abs(end.tv_sec - start.tv_sec);
    int usec = abs(end.tv_usec - start.tv_usec);
    printf("1-process, checksum = %u \n",sum);
    printf("elapsed %f ms\n", sec*1000+(usec/1000.0));

    gettimeofday(&start, 0);
    sum=four_process(shm_addr_a,shm_addr_b,shm_addr_c,dim);
    gettimeofday(&end, 0);
    int sec2 = abs(end.tv_sec - start.tv_sec);
    int usec2 = abs(end.tv_usec - start.tv_usec);

    printf("4-process, checksum = %u \n",sum);
    printf("elapsed %f ms\n", sec2*1000+(usec2/1000.0));

    //printf("Acceleration rate = %f\n",(sec*1000+(usec/1000.0))/(sec2*1000+(usec2/1000.0)));

    release_all_shm(shmid_arr[0]);
    release_all_shm(shmid_arr[1]);
    release_all_shm(shmid_arr[2]);
    //printf("Shared memory release success! \n");
    return 0;
}
UNT one_process(UNT* shm_addr_a,UNT* shm_addr_b,int dim)
{
    UNT sum=0;
    for(int row=0;row<dim*dim;row+=dim)
    {
        for(int col=0;col<dim;col++)
        {
            UNT cell=0; //a cell
            for(int k=0;k<dim;k++)
            {
                cell+=shm_addr_a[row+k]*shm_addr_b[k*dim+col];
            }
            //printf("Getvalue %u \n",cell);
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

UNT four_process(UNT* shm_addr_a,UNT* shm_addr_b,UNT* shm_addr_c,int dim)
{

    UNT sum = 0;
    //do fork
    pid_t pid;
    pid=fork();
    ////printf("Fork 1 for child1\n");
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
        ////printf("Fork 2 for child2\n");
        if(pid2<0)
        {
            fprintf(stderr, "Fork Failed");
            exit(EXIT_FAILURE);
        }
        else if(pid2==0) //child2 for first 1/4 row of matrix multiplication
        {
            ////printf("pid: %d \n",getpid());
            for(int row=0,cnt=0;row<(dim*dim)&&cnt<dim;)
            {

                for(int col=0;col<dim;col++)
                {
                    shm_addr_c[cnt*dim+col]=0; //initailize cell as 0
                    //printf("Cnt is now %d and row is now %d\n",cnt,row);
                    for(int k=0;k<dim;k++)
                    {
                        //printf("get a value %d \n",shm_addr_a[row+k]);
                        shm_addr_c[cnt*dim+col]+=shm_addr_a[row+k]*shm_addr_b[k*dim+col];
                    }
                    //printf("Get value %u\n",shm_addr_c[cnt*dim+col]);
                }
                cnt+=4;
                row=(dim*cnt);

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
            //printf("pid: %d \n",getpid());
            for(int row=dim,cnt=1;row<(dim*dim)&&cnt<dim;)
            {
                //printf("cntB %d \n",cnt);
                for(int col=0;col<dim;col++)
                {
                    //printf("Row in second loop inside %d",row);
                    shm_addr_c[cnt*dim+col]=0; //initailize cell as 0
                    //printf("Cnt is now %d\n",cnt);
                    for(int k=0;k<dim;k++)
                    {
                        shm_addr_c[cnt*dim+col]+=shm_addr_a[row+k]*shm_addr_b[k*dim+col];
                    }
                    //printf("Get value %u\n",shm_addr_c[cnt*dim+col]);
                }
                cnt+=4;
                row=(dim*cnt);
                //printf("Row outside %d \n",row);
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
            //printf("pid: %d \n",getpid());
            for(int row=dim*2,cnt=2;row<dim*dim&&cnt<dim;)
            {
                //printf("cntC %d \n",cnt);
                for(int col=0;col<dim;col++)
                {
                    shm_addr_c[cnt*dim+col]=0; //initailize cell as 0
                    //printf("Cnt is now %d\n",cnt);
                    for(int k=0;k<dim;k++)
                    {
                        shm_addr_c[cnt*dim+col]+=shm_addr_a[row+k]*shm_addr_b[k*dim+col];
                    }
                    //printf("Get value %u\n",shm_addr_c[cnt*dim+col]);
                }
                cnt+=4;
                row=(dim*cnt);
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
            //printf("pid: %d \n",getpid());
            for(int row=dim*3,cnt=3;row<dim*dim&&cnt<dim;)
            {
                //printf("cntD %d \n",cnt);
                for(int col=0;col<dim;col++)
                {
                    shm_addr_c[cnt*dim+col]=0; //initailize cell as 0
                    //printf("Cnt is now %d\n",cnt);
                    for(int k=0;k<dim;k++)
                    {
                        shm_addr_c[cnt*dim+col]+=shm_addr_a[row+k]*shm_addr_b[k*dim+col];
                    }
                    //printf("Get value %u\n",shm_addr_c[cnt*dim+col]);
                }
                cnt+=4;
                row=(dim*cnt);
            }
            //printf("Matrix ok \n");
            wait(NULL); //wait for child1 and child3 to finish!!!! DUAL WAIT!!!
            wait(NULL);
            /*exit(EXIT_SUCCESS); DONT DO THIS, OR WILL EXIT AND NO RETURN
            AND NEGLECT THE FUTURE PROCESS SINCE IT IS TREATED AS TERMINATED NORMALLY
            */
            //release_all_shm(shm_addr_c);
            for(unsigned int i=0;i<dim*dim;i++) //only the main parent to do this, do not write before return sum, which will cause over calculation
            {
                //printf("shm res %u \n",shm_addr_c[i]);
                sum+=shm_addr_c[i];
            }
        }
    }
    //printf("Check for zombie process \n");
    //system("ps aux | grep 'Z'");
    return sum;
}
UNT* assign_shared_mem(int dim,int shmid_arr_index,int shmid_arr[]) //return the address of the shared memory
{
    int shmflg = 0666|IPC_CREAT;// 110110110 permission rw-rw-rw in linux
    //owner rw  group  rw  other   rw
    int shmid=shmget(IPC_PRIVATE,sizeof(UNT)*dim*dim,shmflg);
    shmid_arr[shmid_arr_index]=shmid;
    if(shmid==-1)
    {
        fprintf(stderr, "Shared memory get error\n");
        perror("shmget");
        exit(EXIT_FAILURE); //exit with error
    }
    UNT* shm_addr = shmat(shmid,NULL,shmflg);
    if(shm_addr==-1)
    {
        fprintf(stderr, "Shared memory attach error\n");
        perror("shmat");
        exit(EXIT_FAILURE); //exit with error
    }
    return shm_addr;
}
void detach_shm(UNT* shm_addr)
{
    int detach=shmdt(shm_addr);
    if(detach==-1)
    {
        fprintf(stderr, "Shared memory detach error\n");
        perror("shmdt");
        exit(EXIT_FAILURE);
    }
}
void release_all_shm(int shmid)
{
    //detach_shm(shm_addr);
    int release=shmctl(shmid,IPC_RMID /*DELETE SHARED MEMORY*/,0);
    if(release==-1)
    {
        fprintf(stderr, "Release whole shared memory detach error\n");
        //fprintf(stderr, "%s\n", explain_shmctl(shm_addr, IPC_RMID, 0));
        perror("shmctl");
        exit(EXIT_FAILURE);
    }
}
