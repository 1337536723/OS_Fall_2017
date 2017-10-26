#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <iomanip>
#include <algorithm>
#define pb push_back
using namespace std;
typedef vector<int> vi;
struct one_process
{
    int arrival_time, process_id, burst_time, waiting_time, ta_time;
    bool job_done;
};
int main()
{
    fstream fptr;
    fptr.open("Q2.txt");
    int some,tcase,wt=0,tt=0,total_wt=0,cnt=0;//,total_tt=0,cnt=0;
    vector<one_process> process;
    bool get_tcase=0,get_burst=0;

    while(fptr)
    {
        fptr>>some;

		cout<<"cnt "<<cnt<<"some "<<some<<endl;
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
                process[cnt].waiting_time=0
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
    }
    //process.pop_back();
    int time_el=process[0].burst_time;
	for(;;)
    {
        int min_burst=process[0].burst_time,min_pid=0;
        for(int i=1;i<process.size();i++) //dynamically search the current min burst time pid (has to be executable)
        {
            if(process[i].burst_time<min_burst&& !process[i].job_done&& process[i].arrival_time< time_el)
            {
                min_burst=process[i].burst_time;
                min_pid=i;
            }
        }
        process[min_pid].waiting_time=time_el-process[min_pid].arrival_time;
        time_el+=process[min_pid].burst_time;
        pcocess[min_pid].ta_time=time_el;
        printf("Time el is now %d now execute job \n",time_el,min_pid);
    }
    cout<<"Process     Waiting Time     Turnaround Time"<<endl;
    for(int i=0;i<process.size();i++)
    {

        cout<<setw(12)<<left<<trick(i+1)<<setw(17)<<left<<process[i].waiting_time<<setw(14)<<left<<process[i].ta_time<<endl;
    }
    cout<<"good"<<endl;
    fptr.close();
	exit(0);
}
