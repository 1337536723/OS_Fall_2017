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

Struecture: 3vectors

**************************************************************************************/
struct table_content
{
    int last_access_time, page_number, frame_number;
};
//required coumter and data structure
vector<table_content> TLB;
vector<int> page_table;
vector<vector<int> > phy_memory;
char tmp_loaded_data[FRAME_SIZE];
//LRU sorting TLB[0] has the earliest access time
bool LRU_compare(table_content table_a, table_content table_b)
{
    return table_a.last_access_time < table_b.last_access_time;
}
//
int main(int argc, char const *argv[])
{
    /*************************************initl**************************************/
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
        page_table.pb(-1); //use -1 for null(result in page fault) and 1 for exist
    }
    phy_memory.resize(256);
    /*for(int i=0;i<PHYSICAL_MEMORY_SIZE;i++) //use -1 for null
    {
        phy_memory[i].resize(0); //the real memory 1d
    }*/
    /**********************************backing storage**************************************/
    FILE* BACK_fptr;
    BACK_fptr = fopen(argv[1],"rb"); //read the binary
    //address
    fstream fptr;
    fptr.open(argv[2]);
    //output result
    ofstream ofptr;
    ofptr.open("result.txt",std::ofstream::out);
    /********************************statistical data**********************************/
    bool first=0;
    bool tlb_miss_flg=1, page_fault_flg=1;
    int tcase=0, tlb_hits=0, page_fault=0, timestamp=0, accessed_value=0;
    /*********************address data with bitwise operation*************************/
    int tmp, page_num_addr, offset_addr, frame_num_addr, phy_addr;
    /****************operation and algorithm implementation***************************/
    while(fptr>>tmp)
    {
        cout<<"Fptr reads "<<tmp<<endl;
        if(!first)
        {
            first=1;
            tcase=tmp;
        }
        else
        {
            page_num_addr=(tmp&0xFF00)>>8;
            offset_addr=(tmp&0xFF);
            for(int i=0;i<TLB_SIZE;i++)
            {
                if(TLB[i].last_access_time!=-1
                &&TLB[i].page_number==page_num_addr)
                {
                    tlb_miss_flg=0;
                    frame_num_addr=TLB[i].frame_number;
                    TLB[i].last_access_time=timestamp;
                    break;
                }
            }
            if(tlb_miss_flg==0) //TLB hit, directly fetch and access the physical memory
            {
                phy_addr=frame_num_addr*FRAME_SIZE+offset_addr;
                tlb_hits++;
            }
            else //TLB miss find the page table and load into TLB by using LRU
            {
                /*A page table miss, fetch from BACKING and reload to physical memory, update such frame in phy mem
                and update the page table to indicate the page can be found as well*/
                if(page_table[page_num_addr]==-1)
                {
                    //the memory is byte-addressable, the char fits such requirements
                    fseek(BACK_fptr, page_num_addr*256, SEEK_SET);
                    fread(tmp_loaded_data, sizeof(char), 256, BACK_fptr); //bin file read byte by byte
                    page_table[page_num_addr]=page_fault; //update the existance of the corresponding memory frame to say it exisis in phy mem
                    for(int i=0;i<FRAME_SIZE;i++)
                    {
                        phy_memory[page_fault].pb(tmp_loaded_data[i]);
                    }
                    phy_addr=page_table[page_num_addr]*FRAME_SIZE+offset_addr;
                    accessed_value=phy_memory[page_table[page_num_addr]][offset_addr];
                    page_fault++;
                }
                else //A page table hit
                {
                    phy_addr=page_table[page_num_addr]*FRAME_SIZE+offset_addr;
                    accessed_value=phy_memory[page_table[page_num_addr]][offset_addr];
                }
                //Update the TLB by using LRU
                sort(TLB.begin(), TLB.end(), LRU_compare);
                TLB[0].page_number=page_num_addr;
                TLB[0].frame_number=page_table[page_num_addr];
            }
            int tmp_2=(int) accessed_value&0xFF;
            ofptr<<phy_addr<<" "<<accessed_value<<endl;
            tlb_miss_flg=1;
            timestamp++;
        }
    }
    ofptr<<"TLB hits: "<<tlb_hits<<endl<<"Page Faults: "<<page_fault;
    fclose(BACK_fptr);
    fptr.close();
    ofptr.close();
    return 0;
}
