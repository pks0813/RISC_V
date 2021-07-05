#ifndef ROB_HPP
#define ROB_HPP
#include"include.hpp"
extern Sig Wire;

std::string Name[50]={"&RISC_V::LUI","&RISC_V::AUIPC","&RISC_V::JAL","&RISC_V::JALR","&RISC_V::BEQ","&RISC_V::BNE","&RISC_V::BLT","&RISC_V::BGE",
                      "&RISC_V::BLTU","&RISC_V::BGEU","&RISC_V::LB","&RISC_V::LH","&RISC_V::LW","&RISC_V::LBU","&RISC_V::LHU","&RISC_V::SB","&RISC_V::SH",
                      "&RISC_V::SW","&RISC_V::ADDI","&RISC_V::SLTI","&RISC_V::SLTIU","&RISC_V::XORI","&RISC_V::ORI","&RISC_V::ANDI","&RISC_V::SLLI","&RISC_V::SRLI",
                      "&RISC_V::SRAI","&RISC_V::ADD","&RISC_V::SUB","&RISC_V::SLL","&RISC_V::SLT","&RISC_V::SLTU","&RISC_V::XOR","&RISC_V::SRL",
                      "&RISC_V::SRA","&RISC_V::OR","&RISC_V::AND"};
// extern int num;
const int Size=40;
struct ROB{
    Iter *ROBIter;
    struct ROBNode{
        int OrderID;
        int Rs;
        unsigned int Result;
        unsigned int NexPC;
        bool Ready=0; 
        ROBNode(const int &X=0,const int &Y=0):OrderID(X),Rs(Y){ }
    };
    PKSQueue<ROBNode> OldARR,NewARR;
    ROB():OldARR(40),NewARR(40){}
    void Run(){
        if (Wire.ROBCOMMIT)
        {
            if (NewARR.Size()==0) {printf("ERROR\n");}
            // ++num;
            // std::cout<<Name[NewARR.Front().OrderID]<<"  "<<NewARR.Front().Result<<"  "<<NewARR.Front().NexPC<<std::endl;
            // std::cout<<Name[NewARR.Front().OrderID]<<std::endl;
            if (NewARR.Front().OrderID==37)
            {
                // printf("PRINT()");
                printf("%d\n",ROBIter->CPU_REG->OldARRV[10]&255u);
                exit(0);
            }
            // NewARR.Front().Ready=0; ???????????
            NewARR.Delete();
        }
        if (Wire.ClearFlag) 
        {
            NewARR.clear();
            return;
        }
        for (int i=0;i<Wire.ROBIns.size();i++)
        {
            ROBNode tmp;
            tmp.OrderID=Wire.ROBIns[i].OrderID;
            tmp.Rs=Wire.ROBIns[i].Rd;
            tmp.NexPC=Wire.ROBIns[i].NowPC+4;
            NewARR.Insert(tmp);
        }
        for (int i=0;i<Wire.OldQVUP.size();i++)
        {
            NewARR.ARR[Wire.OldQVUP[i].Q].Ready=1;
            NewARR.ARR[Wire.OldQVUP[i].Q].Result=Wire.OldQVUP[i].V;
            NewARR.ARR[Wire.OldQVUP[i].Q].NexPC=Wire.OldQVUP[i].PC;
        }
    }
    void Check(){
        if (OldARR.Size() && (OldARR.Front().Ready || (OldARR.Front().OrderID<=17 && OldARR.Front().OrderID>=15)))
        {
            Wire.ROBCOMMIT=1;
            if (OldARR.Front().NexPC!=Wire.NexPC+4) Wire.ClearFlag=1;
            REGinput tmp(OldARR.Front().Rs,OldARR.L,OldARR.Front().Result); 
            Wire.RegChan.push_back(tmp);
            if (OldARR.Front().OrderID<=17 && OldARR.Front().OrderID>=15)
                Wire.LSBCOMMIT.push_back(OldARR.L);
            Wire.NexPC=OldARR.Front().NexPC;
        }
    }

};





#endif