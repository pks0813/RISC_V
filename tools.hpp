#ifndef TOOLS_HPP
#define TOOLS_HPP
using namespace std;   

#define Fail 16777215




#include<vector>
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
};
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
char IntToChar(const char &X){
    if (X>=0 && X<=9)  return X+'0';
    return X+'A'-10;}
void JIN16(unsigned int &Q){
    printf("%c",IntToChar(Q>>12));
    printf("%c",IntToChar((Q>>8)&15u));
    printf("%c",IntToChar((Q>>4)&15u));
    printf("%c",IntToChar(Q&15u));
    
}
#endif