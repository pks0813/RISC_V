#ifndef RISC_V_HPP
#define RISC_V_HPP
#include"tools.hpp"
#include<bits/stdc++.h>
using namespace std;
unsigned int FullChar=255;
void Update(unsigned int &X,const int &BIT){
    if (BIT==31) return;
    if ((X>>BIT)&1)
        X|=((1<<(31-BIT))-1)<<(BIT+1);
}

const unsigned int Fail=16777215;
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
string Name[50];
class RISC_V{
    public:
    void (RISC_V::*Func[50])();
    unsigned char RISC_Memory[500000];
    unsigned int RISC_Reg[32];
    unsigned int nowPC;
    unsigned int tmp[6];
    void YCL(){
        Name[0]="&RISC_V::LUI";Name[1]="&RISC_V::AUIPC"; Name[2]="&RISC_V::JAL"; Name[3]="&RISC_V::JALR";
        Name[4]="&RISC_V::BEQ";Name[5]="&RISC_V::BNE"; Name[6]="&RISC_V::BLT"; Name[7]="&RISC_V::BGE";
        Name[8]="&RISC_V::BLTU";Name[9]="&RISC_V::BGEU"; Name[10]="&RISC_V::LB"; Name[11]="&RISC_V::LH";
        Name[12]="&RISC_V::LW";Name[13]="&RISC_V::LBU"; Name[14]="&RISC_V::LHU"; Name[15]="&RISC_V::SB";
        Name[16]="&RISC_V::SH";Name[17]="&RISC_V::SW"; Name[18]="&RISC_V::ADDI"; Name[19]="&RISC_V::SLTI";
        Name[20]="&RISC_V::SLTIU";Name[21]="&RISC_V::XORI"; Name[22]="&RISC_V::ORI"; Name[23]="&RISC_V::ANDI";
        Name[24]="&RISC_V::SLLI";Name[25]="&RISC_V::SRLI"; Name[26]="&RISC_V::SRAI"; Name[27]="&RISC_V::ADD";
        Name[28]="&RISC_V::SUB";Name[29]="&RISC_V::SLL"; Name[30]="&RISC_V::SLT"; Name[31]="&RISC_V::SLTU";
        Name[32]="&RISC_V::XOR";Name[33]="&RISC_V::SRL"; Name[34]="&RISC_V::SRA"; Name[35]="&RISC_V::OR";
        Name[36]="&RISC_V::AND";

        
        Func[0]=&RISC_V::LUI;Func[1]=&RISC_V::AUIPC; Func[2]=&RISC_V::JAL; Func[3]=&RISC_V::JALR;
        Func[4]=&RISC_V::BEQ;Func[5]=&RISC_V::BNE; Func[6]=&RISC_V::BLT; Func[7]=&RISC_V::BGE;
        Func[8]=&RISC_V::BLTU;Func[9]=&RISC_V::BGEU; Func[10]=&RISC_V::LB; Func[11]=&RISC_V::LH;
        Func[12]=&RISC_V::LW;Func[13]=&RISC_V::LBU; Func[14]=&RISC_V::LHU; Func[15]=&RISC_V::SB;
        Func[16]=&RISC_V::SH;Func[17]=&RISC_V::SW; Func[18]=&RISC_V::ADDI; Func[19]=&RISC_V::SLTI;
        Func[20]=&RISC_V::SLTIU;Func[21]=&RISC_V::XORI; Func[22]=&RISC_V::ORI; Func[23]=&RISC_V::ANDI;
        Func[24]=&RISC_V::SLLI;Func[25]=&RISC_V::SRLI; Func[26]=&RISC_V::SRAI; Func[27]=&RISC_V::ADD;
        Func[28]=&RISC_V::SUB;Func[29]=&RISC_V::SLL; Func[30]=&RISC_V::SLT; Func[31]=&RISC_V::SLTU;
        Func[32]=&RISC_V::XOR;Func[33]=&RISC_V::SRL; Func[34]=&RISC_V::SRA; Func[35]=&RISC_V::OR;
        Func[36]=&RISC_V::AND;
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
            RISC_Memory[nowPC]=CharToInt(S[0])*16+CharToInt(S[1]);
            nowPC++;
            cin>>S;
        }
        // printf("x[0]%d\n",RISC_Memory[0]);
    }    
    
    void Print(){
        printf("%d\n",(unsigned int)(RISC_Reg[10]) & 255u);
        exit(0);
    }

    //第一类
    void LUI(){
        unsigned int now=tmp[0];
        RISC_Reg[tmp[1]]=now;
        nowPC+=4;
    }
    void AUIPC(){
        nowPC+=tmp[0];
        RISC_Reg[tmp[1]]=nowPC;
    }
    
    //第二类
    void JAL(){
        RISC_Reg[tmp[1]]=nowPC+4;
        Update(tmp[0],20);
        nowPC+=tmp[0];
    }
    
    //第三类
    void SB(){
        int bit=1;
        unsigned int nowIndex=RISC_Reg[tmp[1]];
        Update(tmp[0],11);
        for (unsigned int i=0;i<bit;i++)
        {
            RISC_Memory[RISC_Reg[tmp[2]]+tmp[0]+i]=nowIndex&FullChar;
            nowIndex>>=8;
        }        
        nowPC+=4;   
    }
    void SH(){
        int bit=2;
        unsigned int nowIndex=RISC_Reg[tmp[1]];
        Update(tmp[0],11);
        for (unsigned int i=0;i<bit;i++)
        {
            RISC_Memory[RISC_Reg[tmp[2]]+tmp[0]+i]=nowIndex&FullChar;
            nowIndex>>=8;
        }        
        nowPC+=4;   
    }
    void SW(){
        int bit=4;
        unsigned int nowIndex=RISC_Reg[tmp[1]];
        Update(tmp[0],11);
        for (unsigned int i=0;i<bit;i++)
        {
            RISC_Memory[RISC_Reg[tmp[2]]+tmp[0]+i]=(nowIndex&FullChar);
            nowIndex>>=8;
        }        
        nowPC+=4;   
    }
    
    //第四类
    void BEQ(){
        if (RISC_Reg[tmp[1]]==RISC_Reg[tmp[2]])
        {
            Update(tmp[0],12);
            nowPC+=tmp[0];
        }else 
        nowPC+=4;
    }
    void BNE(){
        if (RISC_Reg[tmp[1]]!=RISC_Reg[tmp[2]])
        {
            Update(tmp[0],12);
            nowPC+=tmp[0];
        }else
        nowPC+=4;
    }
    void BLT(){
        if (static_cast<int>(RISC_Reg[tmp[1]])>static_cast<int>(RISC_Reg[tmp[2]]))
        {
            Update(tmp[0],12);
            nowPC+=tmp[0];
        }else
        nowPC+=4;
    }
    void BGE(){
        if (static_cast<int>(RISC_Reg[tmp[1]])<=static_cast<int>(RISC_Reg[tmp[2]]))
        {
            Update(tmp[0],12);
            nowPC+=tmp[0];
        }else
        nowPC+=4;
    }
    void BLTU(){
        if (RISC_Reg[tmp[1]]>RISC_Reg[tmp[2]])
        {
            Update(tmp[0],12);
            nowPC+=tmp[0];
        }else
        nowPC+=4;
    }
    void BGEU(){
        if (RISC_Reg[tmp[1]]<=RISC_Reg[tmp[2]])
        {
            Update(tmp[0],12);
            nowPC+=tmp[0];
        }else
        nowPC+=4;
    }
    
    //第五类
    void JALR(){
        Update(tmp[0],11);
        unsigned int t=nowPC+4u;
        nowPC=(RISC_Reg[tmp[1]]+tmp[0])&(~1u);
        RISC_Reg[tmp[2]]=t;
    }

    void LB(){
        int bit=1;
        Update(tmp[0],11);
        unsigned int now=0;
        for (int i=bit-1;i>=0;i--)
            now=(now<<8)|(unsigned int)RISC_Memory[RISC_Reg[tmp[1]]+tmp[0]+(unsigned int)i];
        Update(now,bit*8-1);
        RISC_Reg[tmp[2]]=now;
        nowPC+=4;
    }

    void LH(){
        int bit=2;
        Update(tmp[0],11);
        unsigned int now=0;
        for (int i=bit-1;i>=0;i--)
            now=(now<<8)|(unsigned int)RISC_Memory[RISC_Reg[tmp[1]]+tmp[0]+(unsigned int)i];
        Update(now,bit*8-1);
        RISC_Reg[tmp[2]]=now;
        nowPC+=4;
    }

    void LW(){
        int bit=4;
        Update(tmp[0],11);
        unsigned int now=0;
        for (int i=bit-1;i>=0;i--)
            now=(now<<8)|(unsigned int)RISC_Memory[RISC_Reg[tmp[1]]+tmp[0]+(unsigned int)i];
        RISC_Reg[tmp[2]]=now;
        nowPC+=4;
    }

    void LBU(){
        int bit=1;
        Update(tmp[0],11);
        unsigned int now=0;
        for (int i=bit-1;i>=0;i--)
            now=(now<<8)|(unsigned int)RISC_Memory[RISC_Reg[tmp[1]]+tmp[0]+(unsigned int)i];
        RISC_Reg[tmp[2]]=now;
        nowPC+=4;
    }

    void LHU(){
        int bit=2;
        Update(tmp[0],11);
        unsigned int now=0;
        for (int i=bit-1;i>=0;i--)
            now=(now<<8)|(unsigned int)RISC_Memory[RISC_Reg[tmp[1]]+tmp[0]+(unsigned int)i];
        RISC_Reg[tmp[2]]=now;
        nowPC+=4;
    }

    void ADDI(){
        Update(tmp[0],11);
        unsigned int now=RISC_Reg[tmp[1]]+tmp[0];
        RISC_Reg[tmp[2]]=now;
        nowPC+=4;
    }
    void SLTI(){
        Update(tmp[0],11);
        unsigned int now=(static_cast<int>(RISC_Reg[tmp[1]])<static_cast<int>(tmp[0]));
        RISC_Reg[tmp[2]]=now;        
        nowPC+=4;
    }
          
    void SLTIU(){
        Update(tmp[0],11);
        unsigned int now=(RISC_Reg[tmp[1]]<tmp[0]);
        RISC_Reg[tmp[2]]=now;        
        nowPC+=4;
    }

    void XORI(){
        Update(tmp[0],11);
        unsigned int now=RISC_Reg[tmp[1]]^tmp[0];
        RISC_Reg[tmp[2]]=now;
        nowPC+=4;
    }

    void ORI(){
        Update(tmp[0],11);
        unsigned int now=RISC_Reg[tmp[1]]|tmp[0];
        RISC_Reg[tmp[2]]=now;
        nowPC+=4;
    }
    void ANDI(){
        Update(tmp[0],11);
        unsigned int now=RISC_Reg[tmp[1]]&tmp[0];
        RISC_Reg[tmp[2]]=now;
        nowPC+=4;
    }
    
    //第六类
    void SLLI(){
        unsigned int now=RISC_Reg[tmp[1]];
        now=(now<<tmp[0]);
        RISC_Reg[tmp[2]]=now;
        nowPC+=4;
    }

    void SRLI(){
        unsigned int now=RISC_Reg[tmp[1]];
        now=(now>>tmp[0]);
        RISC_Reg[tmp[2]]=now;
        nowPC+=4;
    }

    void SRAI(){
        unsigned int now=RISC_Reg[tmp[1]];
        now=(now>>tmp[0]);
        Update(now,31-tmp[0]);
        RISC_Reg[tmp[2]]=now;
        nowPC+=4;
    }
        
    void ADD(){
        unsigned int now1=RISC_Reg[tmp[1]];
        unsigned int now2=RISC_Reg[tmp[0]];
        unsigned int now=now1+now2;
        RISC_Reg[tmp[2]]=now;
        nowPC+=4;
    }
    
    void SUB(){
        unsigned int now1=RISC_Reg[tmp[1]];
        unsigned int now2=RISC_Reg[tmp[0]];
        now2=(~now2)+1u;
        unsigned int now=now1+now2;
        RISC_Reg[tmp[2]]=now;
        nowPC+=4;
    }
    
    void SLL(){
        unsigned int now1=RISC_Reg[tmp[1]];
        unsigned int now2=RISC_Reg[tmp[0]];
        unsigned int now=now1;
        now=(now1<<(now2&31u));
        RISC_Reg[tmp[2]]=now;
        nowPC+=4;
    }
    
    void SLT(){
        unsigned int now1=RISC_Reg[tmp[1]];
        unsigned int now2=RISC_Reg[tmp[0]];
        unsigned int now=(static_cast<int>(now1)<static_cast<int>(now2));
        RISC_Reg[tmp[2]]=now;
        nowPC+=4;
    }
    
    void SLTU(){
        unsigned int now1=RISC_Reg[tmp[1]];
        unsigned int now2=RISC_Reg[tmp[0]];
        unsigned int now=(now1<now2);
        RISC_Reg[tmp[2]]=now;
        nowPC+=4;
    }
    
    void XOR(){
        unsigned int now1=RISC_Reg[tmp[1]];
        unsigned int now2=RISC_Reg[tmp[0]];
        unsigned int now=now1^now2;
        RISC_Reg[tmp[2]]=now;    
        nowPC+=4;
    }
    
    void SRL(){
        unsigned int now1=RISC_Reg[tmp[1]];
        unsigned int now2=RISC_Reg[tmp[0]];
        unsigned int now=(now1>>(now2&31u));
        RISC_Reg[tmp[2]]=now;    
        nowPC+=4;
    }
    
    void SRA(){
        unsigned int now1=RISC_Reg[tmp[1]];
        unsigned int now2=RISC_Reg[tmp[0]];
        unsigned int now=(now1>>(now2&31u));
        Update(now,31u-(now2&31u));
        RISC_Reg[tmp[2]]=now;    
        nowPC+=4;
    }
    
    void OR(){
        unsigned int now1=RISC_Reg[tmp[1]];
        unsigned int now2=RISC_Reg[tmp[0]];
        unsigned int now=now1|now2;
        RISC_Reg[tmp[2]]=now;    
        nowPC+=4;
    }
    
    void AND(){
        unsigned int now1=RISC_Reg[tmp[1]];
        unsigned int now2=RISC_Reg[tmp[0]];
        unsigned int now=now1&now2;
        RISC_Reg[tmp[2]]=now;    
        nowPC+=4;
    }
    
    void GetOrder(unsigned int &tmp){
        tmp=0;
        for (int i=3;i>=0;i--) 
            tmp=tmp<<8|(unsigned int)(RISC_Memory[i+nowPC]);
    }
    void Copy(unsigned int A,unsigned int &B,int lA,int rA,int lB){
        if (rA!=31) A%=(1u<<(rA+1));
        A>>=lA;
        B|=(A<<lB);
    }
    void Analyse(const unsigned int &Order,unsigned int &OrderID,unsigned int ARR[])
    {
        if (Order==267388179u)
            Print();
        OrderID=Fail;
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
            if (flat) OrderID=i;
        }
        // printf("%d\n",OrderID);
        
        ARR[0]=0;ARR[1]=0;ARR[2]=0;
        if (OrderID<=1)
        {
            Copy(Order,ARR[0],12,31,12);
            Copy(Order,ARR[1],7,11,0);
            return;            
        }
        if (OrderID==2)
        {
            Copy(Order,ARR[0],31,31,20);
            Copy(Order,ARR[0],21,30,1);
            Copy(Order,ARR[0],20,20,11);
            Copy(Order,ARR[0],12,19,12);
            Copy(Order,ARR[1],7,11,0);
            return;
        }
        if (OrderID>=15 && OrderID<=17)//SH SW SB
        {
            Copy(Order,ARR[0],25,31,5);
            Copy(Order,ARR[1],20,24,0);
            Copy(Order,ARR[2],15,19,0);            
            Copy(Order,ARR[0],7,11,0);
            return;
        }
        if (OrderID>=4 && OrderID<=9)
        {
            Copy(Order,ARR[0],31,31,12);
            Copy(Order,ARR[0],25,30,5);
            Copy(Order,ARR[1],20,24,0);
            Copy(Order,ARR[2],15,19,0);
            Copy(Order,ARR[0],8,11,1);
            Copy(Order,ARR[0],7,7,11);
            return;
        }
        if (OrderID==3 || OrderID>=10 && OrderID<=23)
        {
            Copy(Order,ARR[0],20,31,0);
            Copy(Order,ARR[1],15,19,0);
            Copy(Order,ARR[2],7,11,0);
            return;
        }
        if (OrderID>=24)
        {
            Copy(Order,ARR[0],20,24,0);
            Copy(Order,ARR[1],15,19,0);
            Copy(Order,ARR[2],7,11,0);
            return;
        }

        printf("wow\n");
        exit(0);
    }

    void Game(){
        RISC_Reg[0]=0;
        YCL();
        nowPC=0;
        // int cnt=0;
        while (1)
        {
            // printf("cnt%d\n",++cnt);
            unsigned int Order,OrderID;
            GetOrder(Order);
            Analyse(Order,OrderID,tmp);
            JIN16(nowPC);
            cout<<"     "<<Order<<"   "<<OrderID<<"   "<<Name[OrderID]<<"    "<<((unsigned int)(RISC_Reg[10]) & 255u)<<endl; 
            // printf("%u %u %u\n",tmp[0],tmp[1],tmp[2]);
            // this->*Func[OrderID]();
            (this->*((RISC_V*)this)->RISC_V::Func[OrderID])();
            RISC_Reg[0]=0;
        }
    }
    
};
#endif