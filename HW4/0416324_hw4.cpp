#include <bits/stdc++.h>
#define TLB_SIZE 16
#define PAGE_TABLE_SIZE 256
#define PHYSICAL_MEMORY_SIZE 256
#define pb push_back
using namespace std;
/**************************************************************************************
Specifics
Assume the TLB, page table, and physical memory is empty at the beginning.
• 2^4 entries in the TLB (Use LRU replacement Algorithm)
• 2^8 entries in the page table
• Page size of 2^8 bytes
• Frame size of 2^8 bytes
• 2^8 frames
• Physical memory of 65536 bytes (256 frames * 256-byte frame size)

Struecture: 3vector

**************************************************************************************/

struct table_content
{
    int last_access_time,page_number,frame_number;
};

int main(int argc, char const *argv[])
{
    //required coumter and data structure
    int tcase=0,tlb_miss=0,page_fault=0;
    vector<table_content> TLB;
    vector<int> page_table;
    vector<int> phy_memory;
    //init
    for(int i=0;i<TLB_SIZE;i++)
    {
        table_content one_tbc;
        one_tbc.last_access_time=0;
        one_tbc.page_number=0;
        one_tbc.frame_number=0;
        TLB.pb(one_tbc);
    }
    for(int i=0;i<PAGE_TABLE_SIZE;i++)
    {
        page_table.pb(-1); //use -1 for null
        phy_memory.pb(-1); //use -1 for null
    }
    //backing storage
    FILE* BACK_fptr;
    BACK_fptr = fopen(argv[1],"rb"); //read the binary
    //address
    fstream fptr;
    fptr.open(argv[2]);
    //output result
    ofstream ofptr;
    ofptr.open("result.txt",std::ofstream::out);
    //statistical data
    bool first=0;
    tlb_miss=0;
    page_fault=0;
    //operation and algorithm implementation
    while(fptr)
    {
        int tmp;
        fptr>>tmp;
        if(!first)
        {
            first=1;
            tcase=tmp;
        }
        cout<<"Fptr reads "<<tmp<<endl;
        //search the TLB first

        //TLB miss find the page table and load in

        //page fault, load from BACKING STORE.
    }

    fclose(BACK_fptr);
    fptr.close();
    ofptr.close();
    return 0;
}
