#ifndef EX_HPP
#define EX_HPP
#include"include.hpp"

extern Sig Wire;
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





#endif