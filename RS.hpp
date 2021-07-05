#ifndef RS_HPP
#define RS_HPP
#include"include.hpp"
#include"register.hpp"
#include"ROB.hpp"
extern Sig Wire; 
const int N=20;
struct RS{
    Iter *RSIter;
    struct RSNode{
        bool Busy;
        unsigned int PC;
        int OrderID;
        int Qrs1,Qrs2;
        unsigned int Vrs1,Vrs2;
        unsigned int Offset;
        bool Ready;
        int ResultQ;
    };
    int OldBusynum,NewBusynum;
    RSNode OldARR[N],NewARR[N];


    void Run(){
        if (Wire.ClearFlag)
        {
            for (int i=0;i<N;i++)
                NewARR[i].Busy=0;
            NewBusynum=0;
            return;
        }
        for (int j=0;j<Wire.RSadd.size();j++)
        {
            Instruction tmp=Wire.RSadd[j];
            for (int i=0;i<N;i++)
            if (NewARR[i].Busy==0)
            {
                NewBusynum++;
                NewARR[i].Busy=1;
                NewARR[i].Offset=tmp.Offset;
                NewARR[i].OrderID=tmp.OrderID;
                NewARR[i].PC=tmp.NowPC;
                if (tmp.Rs1==-1) NewARR[i].Qrs1=-1;
                else
                {
                    NewARR[i].Qrs1=RSIter->CPU_REG->OldARRQ[tmp.Rs1];
                    NewARR[i].Vrs1=RSIter->CPU_REG->OldARRV[tmp.Rs1];
                    if (NewARR[i].Qrs1!=-1 && RSIter->CPU_ROB->OldARR.ARR[NewARR[i].Qrs1].Ready)
                    {
                        NewARR[i].Vrs1=RSIter->CPU_ROB->OldARR.ARR[NewARR[i].Qrs1].Result;
                        NewARR[i].Qrs1=-1;
                    }
                }
                if (tmp.Rs2==-1) NewARR[i].Qrs2=-1;
                else
                {
                    NewARR[i].Qrs2=RSIter->CPU_REG->OldARRQ[tmp.Rs2];
                    NewARR[i].Vrs2=RSIter->CPU_REG->OldARRV[tmp.Rs2];
                    if (NewARR[i].Qrs2!=-1 && RSIter->CPU_ROB->OldARR.ARR[NewARR[i].Qrs2].Ready)
                    {
                        NewARR[i].Vrs2=RSIter->CPU_ROB->OldARR.ARR[NewARR[i].Qrs2].Result;
                        NewARR[i].Qrs2=-1;
                    }
                }
                NewARR[i].ResultQ=tmp.Qrd;
                NewARR[i].Ready=(NewARR[i].Qrs1==-1 && NewARR[i].Qrs2==-1);
                break;
            }
        }
        for (int i=0;i<Wire.OldQVUP.size();i++)
        {
            // std::cout<<"RS UPdate "<<Wire.OldQVUP[i]<<std::endl;
            for (int j=0;j<N;j++)
            {
                if (NewARR[j].Qrs1!=-1 && NewARR[j].Qrs1==Wire.OldQVUP[i].Q)
                {
                    NewARR[j].Qrs1=-1;
                    NewARR[j].Vrs1=Wire.OldQVUP[i].V;
                }
                if (NewARR[j].Qrs2!=-1 && NewARR[j].Qrs2==Wire.OldQVUP[i].Q)
                {
                    NewARR[j].Qrs2=-1;
                    NewARR[j].Vrs2=Wire.OldQVUP[i].V;
                }
                if (NewARR[j].Qrs1==-1 && NewARR[j].Qrs2==-1)
                    NewARR[j].Ready=1;
            }
        }
        for (int i=0;i<N;i++)
        if (NewARR[i].Ready && NewARR[i].Busy)
        {
            NewARR[i].Busy=0;
            NewBusynum--;
            EXinput tmp; 
            tmp.OrderID=NewARR[i].OrderID;
            tmp.NowPC=NewARR[i].PC;tmp.Offset=NewARR[i].Offset;tmp.ResultQ=NewARR[i].ResultQ;
            tmp.Vrs1=NewARR[i].Vrs1;tmp.Vrs2=NewARR[i].Vrs2;
            Wire.EXtodo.push_back(tmp);
        }
        /*printf("RSnum%d\n",NewBusynum);
        for (int i=0;i<N;i++)
        if (NewARR[i].Busy)
            printf("RS  PC:%d OrderID:%d Qrs1:%d Qrs2:%d ResultQ:%d\n",NewARR[i].PC,NewARR[i].OrderID,NewARR[i].Qrs1,NewARR[i].Qrs2,NewARR[i].ResultQ);
        */
    }


};

#endif
