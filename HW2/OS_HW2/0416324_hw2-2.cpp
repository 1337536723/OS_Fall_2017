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
    int process_id,arrival_time,burst_time;
};
bool mycompare(const one_process &process_a,const one_process &process_b)
{
    return process_a.burst_time < process_b.burst_time;
}
int main()
{
    fstream fptr;
    fptr.open("Q2.txt");
    int some,tcase,wt=0,tt=0,total_wt=0,total_tt=0,cnt=0;
    vector<one_process> process;
    bool get_tcase=0,get_burst=0;

    while(fptr)
    {
        fptr>>some;
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

    for(int i=0;i<process.size();i++)
    {
        cout<<"Process "<<process[i].process_id<<" wait "<<process[i].arrival_time<<" burst "<<process[i].burst_time<<endl;
    }
    cout<<"good"<<endl;
    fptr.close();
    return 0;
}
