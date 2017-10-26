#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <sstream>
#define pb push_back
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
int main()
{
    fstream fptr;
    fptr.open("Q1.txt");
    int some,tcase,wt=0,tt=0,total_wt=0,total_tt=0;
    vi process;
    bool get_tcase=0;
    while(fptr)
    {
        fptr>>some;
        if(!get_tcase)
        {
            tcase=some;
            get_tcase=1;
        }
        else
        {
            process.pb(some);
        }
    }
    process.pop_back();
    cout<<"Process     Waiting Time     Turnaround Time"<<endl;
    for(int i=0;i<process.size();i++)
    {
        wt=(i==0)?0:(tt);
        tt+=process[i];
        total_tt+=tt;
        total_wt+=wt;
        cout<<setw(12)<<left<<trick(i+1)<<setw(17)<<left<<wt<<setw(14)<<left<<tt<<endl;
    }
    printf("Average waiting time: %lf\n",total_wt/(double)process.size());
    printf("Average turnaround time: %lf\n",total_tt/(double)process.size());
    fptr.close();
    return 0;
}
