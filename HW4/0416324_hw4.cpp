#include <bits/stdc++.h>
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
int tcase,tlb_miss,page_fault;
struct table_content
{
    int last_access_time,page_number,frame_number;
};

int main(int argc, char const *argv[])
{
    //backing storage
    FILE* BACK_fptr;
    BACK_fptr = fopen(argv[1],"rb"); //read the binary
    //address
    fstream fptr;
    fptr.open(argv[2]);
    //output result
    ofstream ofptr;
    ofptr.open("result.txt",std::ofstream::out);

    bool first=0;
    tlb_miss=0;
    page_fault=0;
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
