#ifndef INSTRUCTION_HPP
#define INSTRUCTION_HPP
#include"include.hpp"
#include"ROB.hpp"
#include"RS.hpp"
extern Sig Wire;
const unsigned int KEYVEC[50][3]=
{{Fail,Fail,55},//LUI
{Fail,Fail,23},//AUIPC
{Fail,Fail,111},//JAL
{Fail,0,103},//JALR
{Fail,0,99},//BEQ
{Fail,1,99},//BNE
{Fail,4,99},//BLT
{Fail,5,99},//BGE
{Fail,6,99},//BLTU
{Fail,7,99},//BGEU
{Fail,0,3},//LB
{Fail,1,3},//LH
{Fail,2,3},//LW
{Fail,4,3},//LBU
{Fail,5,3},//LHU
{Fail,0,35},//SB
{Fail,1,35},//SH
{Fail,2,35},//SW
{Fail,0,19},//ADDI
{Fail,2,19},//SLTI
{Fail,3,19},//SLTIU
{Fail,4,19},//XORI
{Fail,6,19},//ORI
{Fail,7,19},//ANDI
{0,1,19},//SLLI
{0,5,19},//SRLI
{32,5,19},//SRAI
{0,0,51},//ADD
{32,0,51},//SUB
{0,1,51},//SLL
{0,2,51},//SLT
{0,3,51},//SLTU
{0,4,51},//XOR
{0,5,51},//SRL
{32,5,51},//SRA
{0,6,51},//OR
{0,7,51}//AND
};
struct InstructionUnit{
    Iter *IUIter;
    unsigned int OldPC,NewPC;
    unsigned int Oldread=0,Newread=0;
    PKSQueue<std::pair<unsigned int,unsigned int> > OldARR,NewARR;
    InstructionUnit():OldARR(30),NewARR(30){}
    void GetOrder(unsigned int &tmp,const unsigned int &nowPC){
        tmp=0;
        for (int i=3;i>=0;i--) 
            tmp=tmp<<8|(unsigned int)(IUIter->CPU_MEM[i+Newread]);
    }
    void Analyse(const unsigned int &Order,Instruction &Tmp)
    {
        if (Order==267388179u)
        {
            Tmp.OrderID=37;
            return;
        }
        Tmp.OrderID=37;
        unsigned int Key[3];
        Key[0]=0;Key[1]=0;Key[2]=0;
        Copy(Order,Key[0],25,31,0);
        Copy(Order,Key[1],12,14,0);
        Copy(Order,Key[2],0,6,0);
        // printf("%d %d %d\n",Key[0],Key[1],Key[2]);
        for (int i=0;i<37;i++)
        {
            bool flat=1;
            for (int j=0;j<3;j++)
            if (KEYVEC[i][j]!=Fail && KEYVEC[i][j]!=Key[j])
                flat=0;
            if (flat) Tmp.OrderID=i;
        }
        // printf("%d\n",OrderID);
        if (Tmp.OrderID<=1)
        {
            Tmp.Offset=0;
            Tmp.Rs1=-1;
            Tmp.Rs2=-1;
            Tmp.Rd=0;
            Copy(Order,Tmp.Offset,12,31,12);
            Copy(Order,Tmp.Rd,7,11,0);
            return;            
        }
        if (Tmp.OrderID==2)
        {
            Tmp.Offset=0;
            Tmp.Rs1=-1;
            Tmp.Rs2=-1;
            Tmp.Rd=0;
            Copy(Order,Tmp.Offset,31,31,20);
            Copy(Order,Tmp.Offset,21,30,1);
            Copy(Order,Tmp.Offset,20,20,11);
            Copy(Order,Tmp.Offset,12,19,12);
            Copy(Order,Tmp.Rd,7,11,0);
            return;
        }
        if (Tmp.OrderID>=15 && Tmp.OrderID<=17)//SH SW SB
        {
            Tmp.Offset=0;
            Tmp.Rs1=0;
            Tmp.Rs2=0;
            Tmp.Rd=-1;
            Copy(Order,Tmp.Offset,25,31,5);
            Copy(Order,Tmp.Rs2,20,24,0);
            Copy(Order,Tmp.Rs1,15,19,0);            
            Copy(Order,Tmp.Offset,7,11,0);
            return;
        }
        if (Tmp.OrderID>=4 && Tmp.OrderID<=9)
        {
            Tmp.Offset=0;
            Tmp.Rs1=0;
            Tmp.Rs2=0;
            Tmp.Rd=-1;
            Copy(Order,Tmp.Offset,31,31,12);
            Copy(Order,Tmp.Offset,25,30,5);
            Copy(Order,Tmp.Rs2,20,24,0);
            Copy(Order,Tmp.Rs1,15,19,0);
            Copy(Order,Tmp.Offset,8,11,1);
            Copy(Order,Tmp.Offset,7,7,11);
            return;
        }
        if (Tmp.OrderID==3 || Tmp.OrderID>=10 && Tmp.OrderID<=23)
        {
            Tmp.Offset=0;
            Tmp.Rs1=0;
            Tmp.Rs2=-1;
            Tmp.Rd=0;
            Copy(Order,Tmp.Offset,20,31,0);
            Copy(Order,Tmp.Rs1,15,19,0);
            Copy(Order,Tmp.Rd,7,11,0);
            return;
        }
        if (Tmp.OrderID>=24 && Tmp.OrderID<=26)
        {
            Tmp.Offset=0;
            Tmp.Rs1=0;
            Tmp.Rs2=-1;
            Tmp.Rd=0;
            Copy(Order,Tmp.Offset,20,24,0);
            Copy(Order,Tmp.Rs1,15,19,0);
            Copy(Order,Tmp.Rd,7,11,0);
            return;
        }        
        if (Tmp.OrderID>=27 && Tmp.OrderID<=36)
        {
            Tmp.Offset=0;
            Tmp.Rs1=0;
            Tmp.Rs2=0;
            Tmp.Rd=0;
            Copy(Order,Tmp.Rs2,20,24,0);
            Copy(Order,Tmp.Rs1,15,19,0);
            Copy(Order,Tmp.Rd,7,11,0);
            return;
        }
        Tmp.OrderID=37;
        return;
    }
    void Run(){
        if (Wire.ROBCOMMIT)
        {
            if (Wire.NexPC!=NewPC+4)
            {
                NewARR.clear();
                NewPC=Wire.NexPC;
                Newread=Wire.NexPC;
            }else NewPC=Wire.NexPC;
        }
        // printf("%d %d %d %d\n",NewARR.Size(),IUIter->CPU_ROB->OldARR.Size(),IUIter->CPU_RS->OldBusynum,IUIter->CPU_LSB->OldARR.Size());
        if (NewARR.Size()>=1 && IUIter->CPU_ROB->OldARR.Size()<30 && IUIter->CPU_RS->OldBusynum<16 && IUIter->CPU_LSB->OldARR.Size()<16)
        {
            Wire.IssueFlag=1;
            Wire.IssuePair=NewARR.Front();
            NewARR.Delete();
        }
        if (NewARR.Size()<10)
        {
            unsigned int tmp;   
            GetOrder(tmp,Newread);
            NewARR.Insert(std::make_pair(Newread,tmp));
            Newread+=4;
        }
    }
    void Issue(){
        if (Wire.IssueFlag)
        {
            Wire.IssueFlag=0;
            // printf("Issue %u %u\n",Wire.IssuePair.first,Wire.IssuePair.second);
            Instruction tmp;
            Analyse(Wire.IssuePair.second,tmp);
            tmp.NowPC=Wire.IssuePair.first;
            tmp.Qrd=IUIter->CPU_ROB->OldARR.R;
            Wire.ROBIns.push_back(tmp);
            if (tmp.OrderID<=17 && tmp.OrderID>=10)
                Wire.LSBadd.push_back(tmp);
            else Wire.RSadd.push_back(tmp);
            if (tmp.Rd!=-1)
                Wire.REGforceChan.push_back(std::make_pair(tmp.Rd,tmp.Qrd));
            // std::cout<<tmp<<std::endl;
        }
    }
};



#endif