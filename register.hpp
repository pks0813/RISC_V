#ifndef REGISTER_HPP
#define REGISTER_HPP
#include"include.hpp"
extern Sig Wire;
struct Regis{
    Iter *REGIter;
    int OldARRQ[32],NewARRQ[32];
    unsigned int OldARRV[32],NewARRV[32];
    Regis(){
        for (int i=0;i<32;i++)
            OldARRQ[i]=-1,NewARRQ[i]=-1;
    }
    // void Write(int ID,const std::pair<int,unsigned int> &tmp)
    // {
    //     ARRQ[ID]=tmp.first;
    //     ARRV[ID]=tmp.second;
    // }
    std::pair<int,unsigned int> Read(const int &ID){
        return std::make_pair(OldARRQ[ID],OldARRV[ID]);
    }
    void Run(){
        for (int i=0;i<Wire.REGforceChan.size();i++)
            NewARRQ[Wire.REGforceChan[i].first]=Wire.REGforceChan[i].second;
        for (int i=0;i<Wire.RegChan.size();i++)
        if (Wire.RegChan[i].rd!=-1)
        {
            NewARRV[Wire.RegChan[i].rd]=Wire.RegChan[i].V;
            if (NewARRQ[Wire.RegChan[i].rd]==Wire.RegChan[i].Q)
                NewARRQ[Wire.RegChan[i].rd]=-1;
        }
        if (Wire.ClearFlag)
            for (int i=0;i<32;i++) 
                NewARRQ[i]=-1;
        // if (Wire.ROBCOMMIT)
        // {
        //     for (int i=0;i<32;i++) printf("%u ",NewARRV[i]);
        //     printf("\n");
        // }
        NewARRQ[0]=-1;
        NewARRV[0]=0;
    }
    
};

#endif