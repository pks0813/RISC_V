#ifndef LSBUFFER_HPP
#define LSBUFFER_HPP
#include"include.hpp"
#include"ROB.hpp"
extern Sig Wire;
struct LSBuffer{
    Iter *LSBIter;
    struct LSBNode{
        int OrderID;
        int nexPC;
        int Time=3;
        int Qrs1=-1,Qrs2=-1;
        unsigned int Vrs1,Vrs2;
        bool Ready=0;
        bool Commit=0;
        unsigned int Offset;
        int ResultQ;
    };
    PKSQueue<LSBNode> OldARR,NewARR;
    LSBuffer():OldARR(30),NewARR(30){}
    void Run(){
        if (Wire.ClearFlag)
        {
            while (NewARR.Size() && NewARR.Back().Commit==0)
                NewARR.R=(NewARR.R-1+NewARR.size)%NewARR.size; 
        }
        // for (int i=0;i<Wire.OldQVUP.size();i++)
        // {
        //     for (int j=0;j<NewARR.Size();j++)
        //     {
        //         if (NewARR[j].Qrs1==Wire.OldQVUP[i].Q)
        //         {
        //             NewARR[j].Qrs1=-1;
        //             NewARR[j].Vrs1=Wire.OldQVUP[i].V;
        //         }
        //         if (NewARR[j].Qrs2==Wire.OldQVUP[i].Q)
        //         {
        //             NewARR[j].Qrs2=-1;
        //             NewARR[j].Vrs2=Wire.OldQVUP[i].V;
        //         }
        //         if (NewARR[j].Qrs1==-1 && NewARR[j].Qrs2==-1)
        //             NewARR[j].Ready=1;
        //     }
        // }
        for (int i=0;i<Wire.LSBCOMMIT.size();i++)
        {
            for (int j=0;j<NewARR.Size();j++)
            if (NewARR[j].Commit==0 && NewARR[j].ResultQ==Wire.LSBCOMMIT[i]&& NewARR[j].OrderID<=17 && NewARR[j].OrderID>=15)
                {NewARR[j].Commit=1;break;}
        }
        if (Wire.ClearFlag) return;
        // printf("LSBUFFER %d %d %d %d\n",NewARR.Size(),NewARR.Front().Ready,NewARR.Front().Commit,NewARR.Front().OrderID);
        if (NewARR.Size() && NewARR.Front().Ready && (NewARR.Front().Commit ||(NewARR.Front().OrderID<=14 && NewARR.Front().OrderID>=10)))
        {
            NewARR.Front().Time--;
            if (NewARR.Front().Time==0)
            {
                // printf("WOWWOWOW\n");
                if (NewARR.Front().OrderID<=17 && NewARR.Front().OrderID>=15)
                {
                    Update(NewARR.Front().Offset,11);
                    unsigned int X=NewARR.Front().Vrs1+NewARR.Front().Offset;
                    unsigned int tmp=NewARR.Front().Vrs2;
                    for (int i=0;i<(1<<(NewARR.Front().OrderID-15));i++)
                    {
                        LSBIter->CPU_MEM[X+i]=tmp&255u;
                        tmp>>=8;
                    }
                    NewARR.Delete();
                }
                else
                {
                    Update(NewARR.Front().Offset,11);
                    unsigned int X=NewARR.Front().Vrs1+NewARR.Front().Offset;
                    int BIT=NewARR.Front().OrderID-10;
                    if (BIT>=3) BIT-=3;
                    unsigned int tmp=0;
                    for (int i=(1<<BIT)-1;i>=0;i--)
                        tmp=(tmp<<8)|(unsigned int)(LSBIter->CPU_MEM[X+i]);
                    if (NewARR.Front().OrderID<=11 && NewARR.Front().OrderID>=10)
                        Update(tmp,(1<<BIT)*8-1);
                    QVP INS(NewARR.Front().ResultQ,tmp,NewARR.Front().nexPC);
                    Wire.NewQVUP.push_back(INS);
                    NewARR.Delete();
                }
            }
        }
        for (int i=0;i<Wire.LSBadd.size();i++)
        {
            LSBNode tmp;
            tmp.Time=3;tmp.Ready=0;tmp.Commit=0;tmp.Qrs1=-1;tmp.Qrs2=-1;
            tmp.OrderID=Wire.LSBadd[i].OrderID;
            tmp.nexPC=Wire.LSBadd[i].NowPC+4;
            if (Wire.LSBadd[i].Rs1!=-1)
            {
                std::pair<int,unsigned int> Q=LSBIter->CPU_REG->Read(Wire.LSBadd[i].Rs1);
                tmp.Qrs1=Q.first;
                tmp.Vrs1=Q.second;
                if (tmp.Qrs1!=-1 && LSBIter->CPU_ROB->OldARR.ARR[tmp.Qrs1].Ready)
                {    
                    tmp.Vrs1=LSBIter->CPU_ROB->OldARR.ARR[tmp.Qrs1].Result;
                    tmp.Qrs1=-1;
                }
            }else tmp.Qrs1=-1;
            if (Wire.LSBadd[i].Rs2!=-1)
            {
                std::pair<int,unsigned int> Q=LSBIter->CPU_REG->Read(Wire.LSBadd[i].Rs2);
                tmp.Qrs2=Q.first;
                tmp.Vrs2=Q.second;
                if (tmp.Qrs2!=-1 && LSBIter->CPU_ROB->OldARR.ARR[tmp.Qrs2].Ready)
                {    
                    tmp.Vrs2=LSBIter->CPU_ROB->OldARR.ARR[tmp.Qrs2].Result;
                    tmp.Qrs2=-1;
                }
            }else tmp.Qrs2=-1;
            if (tmp.Qrs1==-1 && tmp.Qrs2==-1) tmp.Ready=1;
            tmp.Offset=Wire.LSBadd[i].Offset;
            tmp.ResultQ=Wire.LSBadd[i].Qrd;
            NewARR.Insert(tmp);
        }       
        for (int i=0;i<Wire.OldQVUP.size();i++)
        {
            for (int j=0;j<NewARR.Size();j++)
            {
                if (NewARR[j].Qrs1==Wire.OldQVUP[i].Q)
                {
                    NewARR[j].Qrs1=-1;
                    NewARR[j].Vrs1=Wire.OldQVUP[i].V;
                }
                if (NewARR[j].Qrs2==Wire.OldQVUP[i].Q)
                {
                    NewARR[j].Qrs2=-1;
                    NewARR[j].Vrs2=Wire.OldQVUP[i].V;
                }
                if (NewARR[j].Qrs1==-1 && NewARR[j].Qrs2==-1)
                    NewARR[j].Ready=1;
            }
        }
    }
};
#endif



