#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <sstream>
#define pb push_back
#define MAX_N 100
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
    bool job_done;
};
bool mycompare(one_process op1,one_process op2)
{
    return op1.burst_time<op2.burst_time;
}
int main()
{
    fstream fptr;
    fptr.open("Q2.txt");

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
            process[cnt].job_done=0;
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

    int time_el=0,min_pid=0;
    //find the first process to execute
    for(int i=0;i<process.size();i++)
    {
        if(process[i].arrival_time==0)
        {
            process[i].waiting_time=0;
            min_pid=i;
            break;
        }
    }
    process[min_pid].job_done=1;
    process[min_pid].waiting_time=0;
    process[min_pid].ta_time=process[min_pid].burst_time;
    time_el+=process[min_pid].burst_time;
    //tcase--;
    for(;;)
    {
        int min_burst=999;
        bool all_end=1;
        for(int i=0;i<process.size();i++)
        {
            if(!process[i].job_done)
                all_end=0;
        }
        if(all_end)
            break;

        for(int i=process.size()-1;i>=0;i--) //dynamically search the current min burst time pid (has to be executable)
        {
            if(process[i].burst_time<=min_burst&& !process[i].job_done&& process[i].arrival_time<= time_el)
            {
                min_burst=process[i].burst_time;
                min_pid=i;
            }
        }
        //printf("minpid now %d\n",min_pid+1);
        if(!process[min_pid].job_done)
        {
            //printf("Time el %d find job %d, with min burst %d \n",time_el,min_pid+1,min_burst);
            process[min_pid].waiting_time=time_el-process[min_pid].arrival_time;
            time_el+=process[min_pid].burst_time;
            process[min_pid].ta_time=time_el-process[min_pid].arrival_time;
            process[min_pid].job_done=1;
        }
        else
        {
            time_el++;
        }
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
//The following code will cause the following error p2: malloc.c:2394: sysmalloc: Assertion `(old_top == initial_top (av) && old_size == 0) || ((unsigned long) (old_size) >= MINSIZE && prev_inuse (old_top) && ((unsigned long) old_end & (pagesize - 1)) == 0)' failed.
//Aborted (core dumped), quite strange
/*while(fptr)
{
    fptr>>some;

    cout<<"";
    if(!get_tcase)
    {
        tcase=some;
        process.resize(tcase);
        get_tcase=1;
    }
    else
    {
        if(!get_burst)
        {
            process[cnt].arrival_time=some;
            process[cnt].process_id=cnt+1;
            process[cnt].waiting_time=0;
            process[cnt].ta_time=0;
            process[cnt].job_done=0;
            cnt++;//count up for one data
            if(cnt==tcase)
            {
                get_burst=1;
                cnt=0;
            }
        }
        else
        {
            process[cnt].burst_time=some;
            cnt++;
        }
    }
}*/
//process.pop_back();
//initilaize calculation
