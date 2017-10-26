#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <sstream>
#define pb push_back
#define MAX_N 1000000
#define PAUSE {fgetc(stdin);}
using namespace std;
typedef vector<int> vi;
template <typename T>
string num_to_str(T num)
{
     ostringstream ss;
     ss << num;
     return ss.str();
}
string trick(int process_num)
{
    return "P["+num_to_str(process_num)+"]";
}
struct one_process
{
    int arrival_time, process_id, burst_time, waiting_time, ta_time;
};
int main()
{
    fstream fptr;
    fptr.open("Q3.txt");

    vector<one_process> process;
    int some,tcase,wt=0,tt=0,cnt=0;//,total_tt=0,cnt=0;
    int data_in [MAX_N] ;
    bool get_tcase=0,get_burst=0;

    while(fptr)
    {
        fptr>>data_in[cnt];
        cnt++;
    }
    //initilaize data storage
    process.resize(data_in[0]);
    tcase=data_in[0];
    cnt=0;
    //fetch the data from buffer data_in since the latter cause undefined behavior
    for(int i=1;i<=tcase*2;i++)
    {
        process[cnt];
        if(i<=tcase)
        {
            process[cnt].arrival_time=data_in[i];
            process[cnt].process_id=cnt+1;
            process[cnt].waiting_time=0;
            process[cnt].ta_time=0;
            cnt++;//count up for one data
            if(cnt==tcase)
            {
                get_burst=1;
                cnt=0;
            }
        }
        else
        {
            process[cnt].burst_time=data_in[i];
            cnt++;
        }
    }
    //process.pop_back();
    //initilaize calculation
    int time_el=0,task_el=0,cur_pid=0; //time elapsed, time elapsed for a certain task
    process[0].waiting_time=0;
    /*for(int i=0;i<process.size();i++)
        cout<<"ar "<<process[i].arrival_time<<" bu "<<process[i].burst_time<<endl;*/
    int min_burst=9999,min_pid=0;
	for(;;time_el++)
    {
        //printf("\nTime el %d find job %d, with min burst %d \n",time_el,min_pid+1,min_burst);
        for(int i=0;i<process.size();i++) //accumulate the waiting_time of the job which is not runing currently
        //this should be done before job switching, or at the time such as t=4 to 5 , p2 accumulation will be cut off
        {
            if(time_el>process[i].arrival_time&&process[i].burst_time>0&&i!=min_pid)
            {
                process[i].waiting_time++;
                //cout<<"pid "<<i+1<<" timeup by 1";
            }
        }
        min_burst=9999,min_pid=0;
        //printf("Remaining burst time :");
        for(int i=0;i<process.size();i++) //dynamically search the current min burst time pid (has to be executable)
        {
            if(process[i].burst_time<min_burst&& process[i].burst_time>0 && process[i].arrival_time<= time_el)
            {
                min_burst=process[i].burst_time;
                min_pid=i;
            }
            //cout<<"pid "<<i+1<<" rbt "<<process[i].burst_time<<" ";
        }
        //cout<<endl;
        //running the SRJF

        process[min_pid].burst_time--;
        if(process[min_pid].burst_time==0) //certain job has been done
        {
            tcase--;
            process[min_pid].ta_time=time_el-process[min_pid].arrival_time+1; //dont forget to +1 since time_el++ is at the loopend (total time elapsed)
        }

        //PAUSE;
        if(tcase==0)
            break;
    }
    cout<<"Process     Waiting Time     Turnaround Time"<<endl;
    for(int i=0;i<process.size();i++)
        cout<<setw(12)<<left<<trick(i+1)<<setw(17)<<left<<process[i].waiting_time<<setw(14)<<left<<process[i].ta_time<<endl;

    int total_wt=0,total_tt=0;
    for(int i=0;i<process.size();i++)
    {
        total_wt+=process[i].waiting_time;
        total_tt+=process[i].ta_time;
    }

    printf("Average waiting time: %lf\n",total_wt/(double)process.size());
    printf("Average turnaround time: %lf\n",total_tt/(double)process.size());
    fptr.close();
	return 0;
}
