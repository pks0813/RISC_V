#ifndef CPU_HPP
#define CPU_HPP
#include<bits/stdc++.h>
#include"include.hpp"
#include"RS.hpp"
#include"EX.hpp"
#include"LSBuffer.hpp"
#include"ROB.hpp"
#include"Instruction.hpp"
#include"register.hpp"
// #include<windows.h>
extern Sig Wire;  
struct CPU{
    Iter CDB;
    unsigned char Mem[500000];
    void duru(){
        int nowPC=0;
        char S[50];
        cin>>S;
        while (!cin.eof())
        {
            // printf("%d ",nowPC);
            // cout<<S<<endl;
            if (S[0]=='@') 
            {
                nowPC=0;
                int len=strlen(S+1);
                for (int i=1;i<=len;i++)
                    nowPC=nowPC*16+CharToInt(S[i]);
                cin>>S;
                continue;
            }
            CDB.CPU_MEM[nowPC]=CharToInt(S[0])*16+CharToInt(S[1]);
            nowPC++;
            cin>>S;
        }
    }
    CPU(){ 
        CDB.CPU_RS=new RS;
        CDB.CPU_ROB=new ROB;
        CDB.CPU_EX=new EX;
        CDB.CPU_IU=new InstructionUnit;
        CDB.CPU_LSB=new LSBuffer;
        CDB.CPU_REG=new Regis;   
        CDB.CPU_MEM=this->Mem;
        CDB.CPU_RS->RSIter=&CDB;
        CDB.CPU_ROB->ROBIter=&CDB;
        CDB.CPU_EX->EXIter=&CDB;
        CDB.CPU_IU->IUIter=&CDB;
        CDB.CPU_LSB->LSBIter=&CDB;
        CDB.CPU_REG->REGIter=&CDB;
    }
    void Update(){
        CDB.CPU_ROB->OldARR=CDB.CPU_ROB->NewARR;
        CDB.CPU_LSB->OldARR=CDB.CPU_LSB->NewARR;
        for (int i=0;i<7;i++)
            CDB.CPU_RS->OldARR[i]=CDB.CPU_RS->NewARR[i];
        CDB.CPU_RS->OldBusynum=CDB.CPU_RS->NewBusynum;
        CDB.CPU_IU->OldPC=CDB.CPU_IU->NewPC;
        CDB.CPU_IU->Oldread=CDB.CPU_IU->Newread;
        CDB.CPU_IU->OldARR=CDB.CPU_IU->NewARR;
        for (int i=0;i<32;i++)
            {
                CDB.CPU_REG->OldARRQ[i]=CDB.CPU_REG->NewARRQ[i];
                CDB.CPU_REG->OldARRV[i]=CDB.CPU_REG->NewARRV[i];
            }
        Wire.OldQVUP=Wire.NewQVUP;Wire.NewQVUP.clear();
        Wire.RegChan.clear();Wire.REGforceChan.clear();
        Wire.LSBCOMMIT.clear();
        Wire.ROBIns.clear();
        Wire.RSadd.clear();Wire.LSBadd.clear();
        Wire.ROBCOMMIT=0;
        // Wire.IssueFlag=0;
        Wire.ClearFlag=0;
    }
    void Update2(){
        Wire.EXtodo.clear();
    }
    void run(){
        duru();

        srand(time(0));
        int p[6];
        for (int i=0;i<5;i++) p[i]=i;
        while (1)
        {
            random_shuffle(p,p+5);
            for (int i=0;i<5;i++)
            switch (p[i])
            {
            case 0:
                CDB.CPU_ROB->Run();
                break;
            case 1:
                CDB.CPU_LSB->Run();
                break;
            case 2:
                CDB.CPU_RS->Run();
                break;
            case 3:
                CDB.CPU_REG->Run();
                break;
            
            default:
                CDB.CPU_IU->Run();
                break;
            }
            Update();
            // for (int i=0;i<CDB.CPU_IU->OldARR.Size();i++)
            //     printf("%u %u\n",CDB.CPU_IU->OldARR[i].first,CDB.CPU_IU->OldARR[i].second);
            // printf("_______\n");

            CDB.CPU_EX->run();
            CDB.CPU_IU->Issue();
            CDB.CPU_ROB->Check();
            Update2();    
            // if (cnt>100000) exit(0);

        }



    }
};



#endif