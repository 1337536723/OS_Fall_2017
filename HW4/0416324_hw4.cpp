#include <bits/stdc++.h>
#define TLB_SIZE 16
#define PAGE_TABLE_SIZE 256
#define PHYSICAL_MEMORY_SIZE 65536
#define FRAME_SIZE 256
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
    int last_access_time, page_number, frame_number;
};
//required coumter and data structure
vector<table_content> TLB;
vector<int> page_table;
vector<int> phy_memory;

int main(int argc, char const *argv[])
{

    //init
    for(int i=0;i<TLB_SIZE;i++)
    {
        table_content one_tbc;
        one_tbc.last_access_time=-1; //use -1 for null
        one_tbc.page_number=0;
        one_tbc.frame_number=0;
        TLB.pb(one_tbc);
    }
    for(int i=0;i<PAGE_TABLE_SIZE;i++)
    {
        page_table.pb(-1); //use -1 for null
    }
    for(int i=0;i<PHYSICAL_MEMORY_SIZE;i++)
    {

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
    bool tlb_miss_flg=1, page_fault_flg=1;
    int tcase=0, tlb_miss=0, page_fault=0, timestamp=0;
    //address data with bitwise operation
    int page_num_addr, offset_addr, frame_num_addr, phy_addr;
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
        page_num_addr=(tmp&0xFF00)>>8;
        offset_addr=(tmp&0xFF);
        for(int i=0;i<TLB_SIZE;i++)
        {
            if(TLB[i].last_access_time!=-1
            &&TLB[i]==page_num_addr)
            {
                tlb_miss=0;
                frame_num_addr=TLB[i].frame_number;
                TLB[i].last_access_time=timestamp;
                break;
            }
        }
        if(tlb_miss==0) //TLB hit, directly fetch and access the physical memory
        {
            phy_addr=frame_num_addr*FRAME_SIZE+offset_addr;
        }
        else //TLB miss find the page table and load into TLB by using LRU
        {
            for(int i=0;i<PAGE_TABLE_SIZE;i++)
            {
                if(page_table[i]!=-1) //An page table hit
                {
                    frame_num_addr=page_table[i];
                }
                else
                {

                }
            }
        }
        //page fault, load from BACKING STORE.
        tlb_miss=1;
        timestamp++;
        //reload by LRU
    }

    fclose(BACK_fptr);
    fptr.close();
    ofptr.close();
    return 0;
}
