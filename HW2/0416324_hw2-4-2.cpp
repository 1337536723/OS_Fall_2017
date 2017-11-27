#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <sstream>
#include <queue>
#define INT_MAX 0x7FFFFFFF
#define pb push_back
#define MAX_N 100
#define PAUSE {fgetc(stdin);}
using namespace std;
int time_el,time_quantum_1,time_quantum_2,tcase,cur_pid;
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
    int process_id, arrival_time, burst_time, burst_time_run, waiting_time, ta_time, final_time;
    bool in_ready;
};
bool mycompare(one_process p1,one_process p2)
{
    return p1.process_id<p2.process_id;
}
vector<one_process>process;
vector<one_process> mul_level_queue()
{
    int time_el=0,cur_layer=0,cur_time_quantum=0,min_pid=0;
    bool switch_layer=0,switch_job=0; //need to swithc the layer
    vector<queue<one_process>> ready_queue;
    ready_queue.resize(3); //does not use 0 only 1 2 for 1 based calculation
    vector<one_process> sjf_queue,result;
    //result.resize(tcase);
    while(tcase)
    {
        for(int i=0;i<process.size();i++)
        {
            //cout<<"Process "<<i+1<<"  arrival time is "<<process[i].arrival_time<<endl;
            if(process[i].arrival_time<=time_el && process[i].in_ready==0)
            {
                ready_queue[1].push(process[i]);
                process[i].in_ready=1;
                //printf("Time %d push job %d \n",time_el,i+1);
            }
        }

        if(cur_time_quantum) //a task is able to run in its time quantum
        {
            if(cur_time_quantum==1) //it will be drained out in this time
                switch_job=1;

            if(cur_layer==1)
            {
                cur_time_quantum--; //decrease the time quantum
                ready_queue[cur_layer].front().burst_time_run--; //do such job, decrease the time
                //printf("Current time %d job %d layer1\n",time_el,ready_queue[cur_layer].front().process_id+1);
                if(ready_queue[cur_layer].front().burst_time_run==0) //if it is done
                {
                    cur_time_quantum=time_quantum_1; //restore
                    switch_job=0; //does not need to switch job, since it is naturally terminated ,not preempted

                    ready_queue[cur_layer].front().waiting_time=time_el-ready_queue[cur_layer].front().arrival_time-ready_queue[cur_layer].front().burst_time+1;
                    ready_queue[cur_layer].front().ta_time=time_el-ready_queue[cur_layer].front().arrival_time+1;
                    result.push_back(ready_queue[cur_layer].front());
                    //printf("Job has done !Current time %d job %d layer1\n",time_el,ready_queue[cur_layer].front().process_id+1);
                    ready_queue[cur_layer].pop();

                    tcase--;
                }
            }
            else if(cur_layer==2)
            {
                cur_time_quantum--; //decrease the time quantum
                ready_queue[cur_layer].front().burst_time_run--; //do such job, decrease the time
                //printf("Current time %d job %d layer2\n",time_el,ready_queue[cur_layer].front().process_id+1);
                if(ready_queue[cur_layer].front().burst_time_run==0) //if it is done
                {
                    cur_time_quantum=time_quantum_2; //restore
                    switch_job=0; //does not need to switch job, since it is naturally terminated ,not preempted

                    ready_queue[cur_layer].front().waiting_time=time_el-ready_queue[cur_layer].front().arrival_time-ready_queue[cur_layer].front().burst_time+1;
                    ready_queue[cur_layer].front().ta_time=time_el-ready_queue[cur_layer].front().arrival_time+1;
                    result.push_back(ready_queue[cur_layer].front());
                    //printf("Job has done !Current time %d job %d layer2\n",time_el,ready_queue[cur_layer].front().process_id+1);
                    ready_queue[cur_layer].pop();

                    tcase--;
                }
            }
            else if(cur_layer==3)
            {
                cur_time_quantum--;
                sjf_queue[min_pid].burst_time_run--;
                switch_job=0; //not move job =0, make sure that switch_job is needless in layer 3
                //printf("Current time %d job %d layer3 \n",time_el,sjf_queue[min_pid].process_id+1);
                if(sjf_queue[min_pid].burst_time_run==0)
                {
                    sjf_queue[min_pid].waiting_time=time_el-sjf_queue[min_pid].arrival_time-sjf_queue[min_pid].burst_time+1;
                    sjf_queue[min_pid].ta_time=time_el-sjf_queue[min_pid].arrival_time+1;

                    result.push_back(sjf_queue[min_pid]);
                    //printf("Job has done!Current time %d job %d layer3 \n",time_el,sjf_queue[min_pid].process_id+1);
                    sjf_queue.erase(sjf_queue.begin()+min_pid);

                    tcase--;
                }
            }

        }
        else // a task was swiped out due to reach its time_quantum
        {
            if(switch_job) //only layer1,2 can be able to use round robin
            {
                if(cur_layer==1) // swipe to layer 2 due to time out
                {
                    switch_job=0; //reset the swithc_job
                    ready_queue[cur_layer+1].push(ready_queue[cur_layer].front());
                    //printf("Current time %d job %d was swiped to layer2\n",time_el,ready_queue[cur_layer].front().process_id+1);
                    ready_queue[cur_layer].pop();
                }
                else if(cur_layer==2) // swipe to layer 3 due to time out
                {
                    switch_job=0; //reset the switch job
                    sjf_queue.push_back(ready_queue[cur_layer].front()); //swipe to layer 3. the last layer, due to time outr
                    //printf("Current time %d job %d was swiped to layer3\n",time_el,ready_queue[cur_layer].front().process_id+1);
                    ready_queue[cur_layer].pop();
                }

            }

            //a job is interrupted due to something has to run in the top layer
            if(ready_queue[1].size())
            {
                cur_time_quantum=time_quantum_1-1; //do it right now
                cur_layer=1;
                ready_queue[1].front().burst_time_run--;

                if(ready_queue[1].front().burst_time_run==0) //the burst time has done
                {
                    cur_time_quantum=time_quantum_1; //regain
                    ready_queue[1].front().waiting_time=time_el-ready_queue[1].front().arrival_time-ready_queue[1].front().burst_time+1;
                    ready_queue[1].front().ta_time=time_el-ready_queue[1].front().arrival_time+1;
                    result.push_back(ready_queue[1].front());

                    ready_queue[1].pop();

                    tcase--;
                }
            }
            else
            {
                if(ready_queue[2].size()) //then check layer 2
                {
                    cur_time_quantum=time_quantum_2-1; //do it right now
                    cur_layer=2;
                    ready_queue[2].front().burst_time_run--;

                    if(ready_queue[2].front().burst_time_run==0) //the burst time has done
                    {
                        cur_time_quantum=time_quantum_2; //regain
                        ready_queue[2].front().waiting_time=time_el-ready_queue[2].front().arrival_time-ready_queue[2].front().burst_time+1;
                        ready_queue[2].front().ta_time=time_el-ready_queue[2].front().arrival_time+1;
                        result.push_back(ready_queue[2].front());

                        ready_queue[2].pop();

                        tcase--;
                    }
                }
                else //or do layer 3
                {
                    if(sjf_queue.size())
                    {
                        cur_layer=3;
                        min_pid=0;
                        int min_burst=INT_MAX;
                        //serach from end for min pid
                        for(int i=sjf_queue.size()-1;i>=0;i--)
                        {
                            if(sjf_queue[i].burst_time_run<min_burst)
                            {
                                min_pid=i;
                                min_burst=sjf_queue[i].burst_time_run;
                            }
                        }
                        //finished 3 before going to upper layer so current_time_quantum is still positive to run
                        cur_time_quantum=sjf_queue[min_pid].burst_time_run-1;
                        sjf_queue[min_pid].burst_time_run--;

                        if(sjf_queue[0].burst_time_run==0)
                        {
                            sjf_queue[min_pid].waiting_time=time_el-sjf_queue[min_pid].arrival_time-sjf_queue[min_pid].burst_time+1;
                            sjf_queue[min_pid].ta_time=time_el-sjf_queue[min_pid].arrival_time+1;

                            result.push_back(sjf_queue[min_pid]);

                            sjf_queue.erase(sjf_queue.begin()+min_pid);

                            tcase--;
                        }
                    }
                }
            }

        }

        if(cur_time_quantum>0&&cur_layer==1&&ready_queue[1].size()==0) //a layer has been done, reset, no switch_job is needed.
        {
            switch_job==0;
            cur_time_quantum=0;
        }
        if(cur_time_quantum>0&&cur_layer==2&&ready_queue[2].size()==0)
        {
            switch_job==0;
            cur_time_quantum=0;
        }

        time_el++;
    }


    return result;
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
            process[cnt].in_ready=0; //all of them at first layer
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
            process[cnt].burst_time_run=data_in[i];
            cnt++;
        }
    }
    //find the first process to execute
    /*for(int i=0;i<process.size();i++)
    {
        if(process[i].arrival_time==0)
        {
            process[i].waiting_time=0;
            cur_pid=i;
            break;
        }
    }*/
    //cout<<"qty1 qty2 "<<time_quantum_1<<" , "<<time_quantum_2<<endl;
    vector<one_process> result=mul_level_queue();
    sort(result.begin(),result.end(),mycompare);
    cout<<"Process     Waiting Time     Turnaround Time "<<endl;
    for(int i=0;i<process.size();i++)
        cout<<setw(12)<<left<<trick(result[i].process_id+1)<<setw(17)<<left<<result[i].waiting_time<<setw(14)<<left<<result[i].ta_time<<endl;

    int total_wt=0,total_tt=0;
    for(int i=0;i<process.size();i++)
    {
        total_wt+=result[i].waiting_time;
        total_tt+=result[i].ta_time;
    }

    printf("Average waiting time: %lf\n",total_wt/(double)process.size());
    printf("Average turnaround time: %lf\n",total_tt/(double)process.size());
    fptr.close();
	return 0;
	return 0;
}
