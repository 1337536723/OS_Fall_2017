#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
int sum=5;
int main()
{
    pid_t pid=fork();
    if(!pid)
    {

        sum--;
        printf("sum in child %d \n",sum);
        exit(0);
    }
    else
    {
        sum++;    
       // wait(NULL);
        printf("sum = %d \n",sum);
       // exit(0);
    }
    
    return 0;
}
