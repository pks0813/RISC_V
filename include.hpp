#ifndef INCLUDE_HPP
#define INCLUDE_HPP


#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include <fstream>
#include<vector>
#include"tools.hpp"
#include"Queue.hpp"
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
#endif