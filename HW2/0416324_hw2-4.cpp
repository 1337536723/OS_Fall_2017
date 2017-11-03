#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <sstream>
#include <queue>
#define pb push_back
#define MAX_N 100
#define PAUSE {fgetc(stdin);}
using namespace std;
int time_el,time_quantum_1,time_quantum_2,tcase,cur_pid;

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
    int process_id,arrival_time, burst_time, waiting_time, ta_time, layer;
};
vector<one_process>process;
vector<queue<one_process> >ready_queue;
void round_robin(int time_quantum,int cur_layer)
{
    while(1)
    {
        //PAUSE;
        time_el++;//increment the total elapsed time
        if(process[cur_pid].layer==cur_layer)//only execute the jobs in current layer
            process[cur_pid].burst_time--; //current executing process, so burst time--
        //for the rest of the non-executing process, just increment their waiting time
        //printf("-----------------------------------------------------\n");
        printf("\nTime el %d find job %d, with burst %d cur_layer %d tcase %d\n",time_el,cur_pid+1,process[cur_pid].burst_time,cur_layer,tcase);
        for(int i=0;i<process.size();i++)//for the rest of the non-executing process, just imcrement their waiting time
        {
            if(time_el>process[i].arrival_time&&process[i].burst_time>0&&i!=cur_pid)
            {
                process[i].waiting_time++;
                //cout<<"pid "<<i+1<<" timeup by 1";
            }
        }
        //cout<<endl;
        //if a new job(waiting time is zero) comes during the execution in layer 1, switch to layer 0, than execute that job in layer 0
        if(cur_layer==1)
        {
            for(int i=0;i<process.size();i++)
            {
                if(time_el>=process[i].arrival_time&&process[i].waiting_time==0)
                {
                    cur_layer=0;
                }
            }
        }
        else //Searcing for new process which has just arrived in the ready queue if cur_layer is 0
        {
            for(int i=0;i<process.size();i++)
            {
                //push the arrived process first, if happens concurrently
                if(time_el==process[i].arrival_time&&time_el)
                {
                    ready_queue[cur_layer].push(process[i]);
                    //printf("Push %d into layer 0 \n",i+1);
                }
            }
        }
        if(process[cur_pid].burst_time==0)//if certain job has been done, then context switiching to the job on top of queue
        {
            tcase--;
            int old=cur_pid;
            process[cur_pid].ta_time=time_el-process[cur_pid].arrival_time;
            if(ready_queue[cur_layer].size())//in case of further access cause std::bad_alloc, there are still job can be popped to do
            {
                cur_pid=ready_queue[cur_layer].front().process_id;
                ready_queue[cur_layer].pop();
                //printf("Pid %d has done, switch to %d\n",old+1,cur_pid+1);
            }
            else //or the current ready queue has been done
            {
                //time_el=time_el+(time_quantum-process[cur_pid].burst_time);
                cur_pid=ready_queue[cur_layer+1].front().process_id;//switch to the next_layer's first pid;
                ready_queue[cur_layer+1].pop(); //and pop that job to begin next layer
                //printf("Layer %d is done\n",cur_layer);
                //printf("Pid %d has done, switch to %d due to switching layer\n",old+1,cur_pid+1);
                return ;
            }
        }
        else if(time_el&&((time_el)%time_quantum==0))
        {
            int old=cur_pid;
            if(process[cur_pid].burst_time) //if a job has not finished in the given time_quantum, promote its priority, put it into next layer
            {
                process[cur_pid].layer++;
                ready_queue[cur_layer+1].push(process[cur_pid]);
            }
            //printf("Put %d into the layer %d due to not finished \n",process[cur_pid].process_id+1,cur_layer+1);
            if(ready_queue[cur_layer].size())//in case of further access cause std::bad_alloc, there are still job can be popped to do
            {
                cur_pid=ready_queue[cur_layer].front().process_id;
                ready_queue[cur_layer].pop();
            }
            else//or due to all switicing to next layer, the current layer has been done
            {
                cur_pid=ready_queue[cur_layer+1].front().process_id;//switch to the next_layer's first pid;
                ready_queue[cur_layer+1].pop(); //and pop that job to begin next layer
                //printf("Layer %d is done\n",cur_layer);
                //printf("Pid %d has done, switch to %d due to switching layer\n",old+1,cur_pid+1);
                return ;
            }
            //printf("Time qty is up, switch to process: %d \n",cur_pid+1);
        }
        //printf("XXXXXXXX \n");

        //printf("Layer %d ready queue: ",cur_layer);
        /*queue<one_process> tmp(ready_queue[cur_layer]);
        while(tmp.size())
        {
            int jjjj=tmp.front().process_id;
            cout<<jjjj+1<<" ";
            tmp.pop();
        }
        cout<<endl*/;

    }
}
void sjf(int cur_layer)
{
    int min_pid=0;
    while(tcase)
    {
        //PAUSE;
        int min_burst=999;
        bool job_interrupt=0;
        for(int i=process.size()-1;i>=0;i--) //dynamically search the current min burst time pid (has to be executable)
        {
            if(process[i].burst_time<=min_burst&&process[i].burst_time&& process[i].arrival_time<= time_el)
            {
                min_burst=process[i].burst_time;
                min_pid=i;
            }
        }
        printf("Time el %d find job %d, with min burst %d tcase %d\n",time_el,min_pid+1,min_burst,tcase);

        time_el+=process[min_pid].burst_time;
        //process[min_pid].waiting_time=time_el-process[min_pid].arrival_time; CANT USE THIS ONE, SINCE IT WILL MODIFY THE AFOREMENTIONED PREEMPTED VALUE
        //should use like in the  round_robin
        for(int i=0;i<process.size();i++)//for the rest of the non-executing process, just imcrement their waiting time
        {
            if(time_el>process[i].arrival_time&&process[i].burst_time>0&&i!=min_pid)
            {
                process[i].waiting_time+=process[min_pid].burst_time;
                //cout<<" pid "<<i+1<<" timeup by "<<process[min_pid].burst_time;
            }
        }
        process[min_pid].ta_time=time_el-process[min_pid].arrival_time;
        process[min_pid].burst_time=0;
        tcase--;
        //if a new job(waiting time is zero) comes during the execution in layer 1, switch to layer 0, than execute that job in layer 0
        for(int i=0;i<process.size();i++)
        {
            if(time_el>=process[i].arrival_time&&process[i].waiting_time==0)
            {
                cur_layer=0;
                ready_queue[0].push(process[i]);
                job_interrupt=1;
            }
        }
        if(job_interrupt)//switch to layer 0
        {
            round_robin(time_quantum_1,cur_layer);
        }
    }
}

int main()
{
    fstream fptr;
    fptr.open("Q5.txt");
    int some,wt=0,tt=0,cnt=0;//,total_tt=0,cnt=0;
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
    time_quantum_1=data_in[tcase*2+1];
    time_quantum_2=data_in[tcase*2+2];
    //printf("Time qty 1 %d qty2 %d INITIL T CASE IS %d\n",time_quantum_1,time_quantum_2,tcase);
    cnt=0;
    //fetching data
    for(int i=1;i<=tcase*2;i++)
    {
        process[cnt];
        if(i<=tcase)
        {
            process[cnt].arrival_time=data_in[i];
            process[cnt].process_id=cnt; //no+1
            process[cnt].waiting_time=0;
            process[cnt].ta_time=0;
            process[cnt].layer=0; //all of them at first layer
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
    //find the first process to execute
    for(int i=0;i<process.size();i++)
    {
        if(process[i].arrival_time==0)
        {
            process[i].waiting_time=0;
            cur_pid=i;
            break;
        }
    }
    //initilaize the ready_queue in layer 0 and layer 1
    ready_queue.resize(3);
    //start to do MLFQ
    while(tcase)
    {
        round_robin(time_quantum_1,0);
        if(ready_queue[0].size()==0)
            round_robin(time_quantum_2,1);
        if(ready_queue[1].size()==0)
            sjf(2);
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
	return 0;
}
