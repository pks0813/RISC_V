


#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include <fstream>
#include<vector>

#define Fail 16777215
using namespace std;
struct RS;
struct ROB;
struct EX;
struct InstructionUnit;
struct LSBuffer;
struct Regis;   
struct Memory;
struct Iter{
    RS *CPU_RS;
    ROB *CPU_ROB;
    EX *CPU_EX;
    InstructionUnit *CPU_IU;
    LSBuffer *CPU_LSB;
    Regis *CPU_REG;
    unsigned char *CPU_MEM;
};
#include<vector>
template<class T>
class PKSQueue{
    public:
    T* ARR;
    int size;
    int L,R;
    PKSQueue(int X):size(X){
        ARR=new T[size];
        L=0;R=0;
    }
    int Size(){
        if (R>=L) return R-L;
        return R-L+size;
    }
    int Insert(const T &X){
        int tmp=R;
        ARR[R]=X;
        (++R)%=size;
        return tmp;
    }
    void Delete(){
        if (L==R) {printf("Queue delete Error\n");exit(0);}
        (++L)%=size;
    }
    T& operator[] (const int &X){
        return ARR[(X+L)%size];
    }    
    T& Front(){
        return ARR[L];
    }    
    T& Back(){
        return ARR[((R-1+size)%size)];
    }
    void clear(){
        L=0;
        R=0;
    }
    PKSQueue<T>& operator= (const PKSQueue<T> &X){
        size=X.size;
        L=X.L;R=X.R;
        for (int i=0;i<size;i++)
            ARR[i]=X.ARR[i];
        return *this;
    }
    ~PKSQueue(){    delete []ARR;  }
}; 
struct QVP{
    int Q;
    unsigned int V,PC; 
    QVP(const int &X1,const unsigned int &X2,const unsigned int &X3):Q(X1),V(X2),PC(X3){}
    friend ostream& operator<<(ostream &output,const QVP &c){
        output<<"QVP Q:"<<c.Q<<"   V:"<<c.V<<"  PC:"<<c.PC;
        return output;
    }
};
struct REGinput{
    int rd;
    int Q;
    unsigned int V;
    REGinput(const int &X1,const int &X2,const int &X3):rd(X1),Q(X2),V(X3){}
};
struct EXinput{
    int OrderID;
    unsigned int Offset,Vrs1,Vrs2,NowPC;
    int ResultQ;
    friend ostream& operator<<(ostream &output,const EXinput &c){
        output<<"EXinput PC:"<<c.NowPC<<"   OrderID:"<<c.OrderID<<"  Offset:"<<c.Offset<<"   Rs1:"<<c.Vrs1<<"   Rs2:"<<c.Vrs2<<"   Q:"<<c.ResultQ;
        return output;
    }
};
class Instruction
{
    public:
    unsigned int NowPC;
    unsigned int Order;
    int OrderID=Fail;
    unsigned int Offset; 
    int Rs1=-1;
    // int QRs1,VRs1;
    int Rs2=-1;
    // int QR2,VRs2;
    int Rd=-1;    
    int Qrd;
    Instruction(const int &PC=0):NowPC(PC){ }
    
    friend ostream& operator<<(ostream &output,const Instruction &c){
        output<<"PC:"<<c.NowPC<<"   OrderID:"<<c.OrderID<<"  Offset:"<<c.Offset<<"   Rs1:"<<c.Rs1<<"   Rs2:"<<c.Rs2<<"   Rd:"<<c.Rd;
        return output;
    }
};
struct Sig{
    std::vector<QVP> OldQVUP,NewQVUP    ;//用来更新QVPC（LSB EX 写入）（LSB ROB RS 读取）
    std::vector<REGinput> RegChan;
    std::vector<std::pair<int,int>> REGforceChan; 
    std::vector<unsigned int> LSBCOMMIT;
    std::vector<EXinput> EXtodo;   
    std::vector<Instruction>ROBIns,RSadd,LSBadd;
    bool ROBCOMMIT;
    bool IssueFlag;
    std::pair<unsigned int,unsigned int> IssuePair;
    unsigned int NexPC;
    bool ClearFlag;
}Wire;
void Copy(unsigned int A,unsigned int &B,int lA,int rA,int lB){
        if (rA!=31) A&=(1u<<(rA+1))-1;
        A>>=lA;
        B|=(A<<lB);
}
void Copy(unsigned int A,int &B,int lA,int rA,int lB){
        if (rA!=31) A&=(1u<<(rA+1))-1;
        A>>=lA;
        B|=(A<<lB);
}
int CharToInt(const char &X){
    if (X>='0' && X<='9')  return X-'0';
    return X-'A'+10;}   
void Update(unsigned int &X,const int &BIT){
    if (BIT==31) return;
    if ((X>>BIT)&1)
        X|=((1<<(31-BIT))-1)<<(BIT+1);
}

const unsigned int KEYVEC[50][3]={{Fail,Fail,55},//LUI
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
struct EX{
    Iter *EXIter;
    EXinput NowEX;
    QVP (EX::*Func[50])();
    EX(){
        Func[0]=&EX::LUI;Func[1]=&EX::AUIPC; Func[2]=&EX::JAL; Func[3]=&EX::JALR;
        Func[4]=&EX::BEQ;Func[5]=&EX::BNE; Func[6]=&EX::BLT; Func[7]=&EX::BGE;
        Func[8]=&EX::BLTU;Func[9]=&EX::BGEU; 
        // Func[10]=&EX::LB; Func[11]=&EX::LH;
        // Func[12]=&EX::LW;Func[13]=&EX::LBU; Func[14]=&EX::LHU; 
        // Func[15]=&EX::SB; Func[16]=&EX::SH;Func[17]=&EX::SW; 
        Func[18]=&EX::ADDI; Func[19]=&EX::SLTI;
        Func[20]=&EX::SLTIU;Func[21]=&EX::XORI; Func[22]=&EX::ORI; Func[23]=&EX::ANDI;
        Func[24]=&EX::SLLI;Func[25]=&EX::SRLI; Func[26]=&EX::SRAI; Func[27]=&EX::ADD;
        Func[28]=&EX::SUB;Func[29]=&EX::SLL; Func[30]=&EX::SLT; Func[31]=&EX::SLTU;
        Func[32]=&EX::XOR;Func[33]=&EX::SRL; Func[34]=&EX::SRA; Func[35]=&EX::OR;
        Func[36]=&EX::AND;Func[37]=&EX::PRINT;
    }
    QVP PRINT(){
        return QVP(0u,0u,0u);
    }
    //第一类
    QVP LUI(){
        return QVP(NowEX.ResultQ,NowEX.Offset,NowEX.NowPC+4);
        // unsigned int now=tmp[0];
        // RISC_Reg[tmp[1]]=now;
        // nowPC+=4;
    }
    QVP AUIPC(){//???
        return QVP(NowEX.ResultQ,NowEX.Offset+NowEX.NowPC,NowEX.NowPC+4);
        // RISC_Reg[tmp[1]]=nowPC+tmp[0];
    }
    
    //第二类
    QVP JAL(){
        Update(NowEX.Offset,20);    
        return QVP(NowEX.ResultQ,NowEX.NowPC+4u,NowEX.NowPC+NowEX.Offset);
        // Update(tmp[0],20);
        // nowPC+=tmp[0];
    }

    //第四类
    QVP BEQ(){
        if (NowEX.Vrs1==NowEX.Vrs2)
        {
            Update(NowEX.Offset,12);
            return QVP(NowEX.ResultQ,0u,NowEX.NowPC+NowEX.Offset);
        }
        return QVP(NowEX.ResultQ,0u,NowEX.NowPC+4u);
    }
    QVP BNE(){
        if (NowEX.Vrs1!=NowEX.Vrs2)
        {
            Update(NowEX.Offset,12);
            return QVP(NowEX.ResultQ,0u,NowEX.NowPC+NowEX.Offset);
        }
        return QVP(NowEX.ResultQ,0u,NowEX.NowPC+4u);
    }
    QVP BLT(){
        if (static_cast<int>(NowEX.Vrs2)>static_cast<int>(NowEX.Vrs1))
        {
            Update(NowEX.Offset,12);
            return QVP(NowEX.ResultQ,0u,NowEX.NowPC+NowEX.Offset);
        }
        return QVP(NowEX.ResultQ,0u,NowEX.NowPC+4u);
    }
    QVP BGE(){
        if (static_cast<int>(NowEX.Vrs2)<=static_cast<int>(NowEX.Vrs1))
        {
            Update(NowEX.Offset,12);
            return QVP(NowEX.ResultQ,0u,NowEX.NowPC+NowEX.Offset);
        }
        return QVP(NowEX.ResultQ,0u,NowEX.NowPC+4u);
    }
    QVP BLTU(){
        if (NowEX.Vrs2>NowEX.Vrs1)
        {
            Update(NowEX.Offset,12);
            return QVP(NowEX.ResultQ,0u,NowEX.NowPC+NowEX.Offset);
        }
        return QVP(NowEX.ResultQ,0u,NowEX.NowPC+4u);
    }
    QVP BGEU(){
        if (NowEX.Vrs2<=NowEX.Vrs1)
        {
            Update(NowEX.Offset,12);
            return QVP(NowEX.ResultQ,0u,NowEX.NowPC+NowEX.Offset);
        }
        return QVP(NowEX.ResultQ,0u,NowEX.NowPC+4u);
    }
    
    //第五类
    QVP JALR(){
        Update(NowEX.Offset,11);
        return QVP(NowEX.ResultQ,NowEX.NowPC+4u,(NowEX.Vrs1+NowEX.Offset)&(~1u));

        // Update(tmp[0],11);
        // unsigned int t=nowPC+4u;
        // nowPC=(RISC_Reg[tmp[1]]+tmp[0])&(~1u);
        // RISC_Reg[tmp[2]]=t;
    }

    QVP ADDI(){  
        Update(NowEX.Offset,11);
        return QVP(NowEX.ResultQ,NowEX.Vrs1+NowEX.Offset,NowEX.NowPC+4);

        // Update(tmp[0],11);
        // unsigned int now=RISC_Reg[tmp[1]]+tmp[0];
        // RISC_Reg[tmp[2]]=now;
        // nowPC+=4;
    }
    QVP SLTI(){
        Update(NowEX.Offset,11);
        return QVP(NowEX.ResultQ,(static_cast<int>(NowEX.Vrs1)<static_cast<int>(NowEX.Offset)),NowEX.NowPC+4);

        // Update(tmp[0],11);
        // unsigned int now=(static_cast<int>(RISC_Reg[tmp[1]])<static_cast<int>(tmp[0]));
        // RISC_Reg[tmp[2]]=now;        
        // nowPC+=4;
    }
          
    QVP SLTIU(){
        Update(NowEX.Offset,11);
        return QVP(NowEX.ResultQ,(NowEX.Vrs1<NowEX.Offset),NowEX.NowPC+4);
        // Update(tmp[0],11);
        // unsigned int now=(RISC_Reg[tmp[1]]<tmp[0]);
        // RISC_Reg[tmp[2]]=now;        
        // nowPC+=4;
    }

    QVP XORI(){
        
        Update(NowEX.Offset,11);
        return QVP(NowEX.ResultQ,(NowEX.Vrs1^NowEX.Offset),NowEX.NowPC+4);
        // Update(tmp[0],11);
        // unsigned int now=RISC_Reg[tmp[1]]^tmp[0];
        // RISC_Reg[tmp[2]]=now;
        // nowPC+=4;
    }

    QVP ORI(){
        Update(NowEX.Offset,11);
        return QVP(NowEX.ResultQ,(NowEX.Vrs1|NowEX.Offset),NowEX.NowPC+4);
        // Update(tmp[0],11);
        // unsigned int now=RISC_Reg[tmp[1]]|tmp[0];
        // RISC_Reg[tmp[2]]=now;
        // nowPC+=4;
    }
    QVP ANDI(){
        Update(NowEX.Offset,11);
        return QVP(NowEX.ResultQ,(NowEX.Vrs1&NowEX.Offset),NowEX.NowPC+4);
        // Update(tmp[0],11);
        // unsigned int now=RISC_Reg[tmp[1]]&tmp[0];
        // RISC_Reg[tmp[2]]=now;
        // nowPC+=4;
    }
    
    //第六类
    QVP SLLI(){
        return QVP(NowEX.ResultQ,(NowEX.Vrs1<<NowEX.Offset),NowEX.NowPC+4);
        // unsigned int now=RISC_Reg[tmp[1]];
        // now=(now<<tmp[0]);
        // RISC_Reg[tmp[2]]=now;
        // nowPC+=4;
    }

    QVP SRLI(){
        return QVP(NowEX.ResultQ,(NowEX.Vrs1>>NowEX.Offset),NowEX.NowPC+4);
        // unsigned int now=RISC_Reg[tmp[1]];
        // now=(now>>tmp[0]);
        // RISC_Reg[tmp[2]]=now;
        // nowPC+=4;
    }

    QVP SRAI(){
        unsigned int tmp=NowEX.Vrs1>>NowEX.Offset;
        Update(tmp,31-NowEX.Offset);
        return QVP(NowEX.ResultQ,tmp,NowEX.NowPC+4);
        // unsigned int now=RISC_Reg[tmp[1]];
        // now=(now>>tmp[0]);
        // Update(now,31-tmp[0]);
        // RISC_Reg[tmp[2]]=now;
        // nowPC+=4;
    }
        
    QVP ADD(){
        return QVP(NowEX.ResultQ,(NowEX.Vrs1+NowEX.Vrs2),NowEX.NowPC+4);
        // unsigned int now1=RISC_Reg[tmp[1]];
        // unsigned int now2=RISC_Reg[tmp[0]];
        // unsigned int now=now1+now2;
        // RISC_Reg[tmp[2]]=now;
        // nowPC+=4;
    }
    
    QVP SUB(){
        return QVP(NowEX.ResultQ,(NowEX.Vrs1-NowEX.Vrs2),NowEX.NowPC+4);
        // unsigned int now1=RISC_Reg[tmp[1]];
        // unsigned int now2=RISC_Reg[tmp[0]];
        // now2=(~now2)+1u;
        // unsigned int now=now1+now2;
        // RISC_Reg[tmp[2]]=now;
        // nowPC+=4;
    }
    
    QVP SLL(){
        return QVP(NowEX.ResultQ,(NowEX.Vrs1<<(31u&NowEX.Vrs2)),NowEX.NowPC+4);
        // unsigned int now1=RISC_Reg[tmp[1]];
        // unsigned int now2=RISC_Reg[tmp[0]];
        // unsigned int now=now1;
        // now=(now1<<(now2&31u));
        // RISC_Reg[tmp[2]]=now;
        // nowPC+=4;
    }
    
    QVP SLT(){
        return QVP(NowEX.ResultQ,(static_cast<int>(NowEX.Vrs1)<static_cast<int>(NowEX.Vrs2)),NowEX.NowPC+4);
        // unsigned int now1=RISC_Reg[tmp[1]];
        // unsigned int now2=RISC_Reg[tmp[0]];
        // unsigned int now=(static_cast<int>(now1)<static_cast<int>(now2));
        // RISC_Reg[tmp[2]]=now;
        // nowPC+=4;
    }
    
    QVP SLTU(){
        return QVP(NowEX.ResultQ,(NowEX.Vrs1<NowEX.Vrs2),NowEX.NowPC+4);
        
        // unsigned int now1=RISC_Reg[tmp[1]];
        // unsigned int now2=RISC_Reg[tmp[0]];
        // unsigned int now=(now1<now2);
        // RISC_Reg[tmp[2]]=now;
        // nowPC+=4;
    }
    
    QVP XOR(){
        return QVP(NowEX.ResultQ,(NowEX.Vrs1^NowEX.Vrs2),NowEX.NowPC+4);
        // unsigned int now1=RISC_Reg[tmp[1]];
        // unsigned int now2=RISC_Reg[tmp[0]];
        // unsigned int now=now1^now2;
        // RISC_Reg[tmp[2]]=now;    
        // nowPC+=4;
    }
    
    QVP SRL(){
        return QVP(NowEX.ResultQ,(NowEX.Vrs1>>(31u&NowEX.Vrs2)),NowEX.NowPC+4);
        // unsigned int now1=RISC_Reg[tmp[1]];
        // unsigned int now2=RISC_Reg[tmp[0]];
        // unsigned int now=(now1>>(now2&31u));
        // RISC_Reg[tmp[2]]=now;    
        // nowPC+=4;
    }
    
    QVP SRA(){
        unsigned int tmp=NowEX.Vrs1>>(31u&NowEX.Vrs2);
        Update(tmp,31-(31u&NowEX.Vrs2));
        return QVP(NowEX.ResultQ,tmp,NowEX.NowPC+4);
        // unsigned int now1=RISC_Reg[tmp[1]];
        // unsigned int now2=RISC_Reg[tmp[0]];
        // unsigned int now=(now1>>(now2&31u));
        // Update(now,31u-(now2&31u));
        // RISC_Reg[tmp[2]]=now;    
        // nowPC+=4;
    }
    
    QVP OR(){
        return QVP(NowEX.ResultQ,(NowEX.Vrs1|NowEX.Vrs2),NowEX.NowPC+4);
        // unsigned int now1=RISC_Reg[tmp[1]];
        // unsigned int now2=RISC_Reg[tmp[0]];
        // unsigned int now=now1|now2;
        // RISC_Reg[tmp[2]]=now;    
        // nowPC+=4;
    }
    
    QVP AND(){
        return QVP(NowEX.ResultQ,(NowEX.Vrs1&NowEX.Vrs2),NowEX.NowPC+4);
        // unsigned int now1=RISC_Reg[tmp[1]];
        // unsigned int now2=RISC_Reg[tmp[0]];
        // unsigned int now=now1&now2;
        // RISC_Reg[tmp[2]]=now;    
        // nowPC+=4;
    }
    
    void run(){
        for (int i=0;i<Wire.EXtodo.size();i++)
        {
            NowEX=Wire.EXtodo[i];
            // std::cout<<NowEX<<std::endl;
            Wire.OldQVUP.push_back((this->*EX::Func[NowEX.OrderID])());                      
            // std::cout<<Wire.OldQVUP.back()<<std::endl;
        }
    }
};
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
        for (int i=0;i<10;i++)
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
        while (1)
        {
            CDB.CPU_ROB->Run();
            CDB.CPU_LSB->Run();
            CDB.CPU_RS->Run();
            CDB.CPU_REG->Run();
            CDB.CPU_IU->Run();
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
CPU CPU;
int main(){
    CPU.run();
}