#include "function.hpp"
#include <iostream>
unsigned int combine_4_unsigned(unsigned char a,unsigned char b,unsigned char c,unsigned char d){
    unsigned int toWord = 0;
    toWord = toWord | a;
    toWord = toWord << 8;
    toWord = toWord | b;
    toWord = toWord << 8;
    toWord = toWord | c;
    toWord = toWord << 8;
    toWord = toWord | d;
    return toWord;
}

int combine_4_signed(unsigned char a,unsigned char b,unsigned char c,unsigned char d){
    int toWord = 0;
    toWord = toWord | a;
    toWord = toWord << 8;
    toWord = toWord | b;
    toWord = toWord << 8;
    toWord = toWord | c;
    toWord = toWord << 8;
    toWord = toWord | d;
    return toWord;
}


unsigned int combine_2_unsigned(unsigned char a,unsigned char b){
    unsigned int toWord = 0;
    toWord = toWord | a;
    toWord = toWord << 8;
    toWord = toWord | b;
    toWord = toWord << 16;
    toWord = toWord >> 16;
    return toWord;
}

int combine_2_signed(unsigned char a,unsigned char b){
    int toWord = 0;
    toWord = toWord | a;
    toWord = toWord << 8;
    toWord = toWord | b;
    toWord = toWord << 16;
    toWord = toWord >> 16;
    return toWord;
}

unsigned int combine_1_unsigned(unsigned char a){
    unsigned int toWord = 0;
    toWord = toWord | a;
    toWord = toWord << 24;
    toWord = toWord >> 24;
    return toWord;
}

int combine_1_signed(unsigned char a){
    int toWord = 0;
    toWord = toWord | a;
    toWord = toWord << 24;
    toWord = toWord >> 24;
    return toWord;
}

unsigned char cut_op(unsigned int a){
    unsigned char op = 0;
    op = a >> 26;
    return op;
}

unsigned char cut_rs(unsigned int a){
    unsigned char rs;
    a = a << 6;
    rs = a >> 27;
    return rs;
}

unsigned char cut_rt(unsigned int a){
    unsigned char rt = 0;
    a = a << 11;
    rt = a >> 27;
    return rt;
}

unsigned char cut_rd(unsigned int a){
    unsigned char rd = 0;
    a = a << 16;
    rd = a >> 27;
    return rd;
}

unsigned int cut_shamt(unsigned int a){
    unsigned int shamt = 0;
    a = a << 21;
    shamt = a >> 27;
    return shamt;
}

unsigned char cut_funct(unsigned int a){
    unsigned char funct = 0;
    a = a << 26;
    funct = a >> 26;
    return funct;
}

unsigned int cut_immediate_unsigned(unsigned int a){
    unsigned int immediate = 0;
    a = a << 16;
    immediate = a >> 16;
    return immediate;
}

int cut_immediate_signed(unsigned int a){
    int immediate = a;
    immediate = immediate << 16;
    immediate = immediate >> 16;
    return immediate;
}

unsigned int cut_address(unsigned int a){
    unsigned int address = 0;
    a = a << 6;
    address = a >> 6;
    return address;
}

int cut_HI(long a){
    int HI = 0;
    HI = a >> 32;
    return HI;
}

int cut_LO(long a){
    int LO = 0;
    a = a << 32;
    LO = a >> 32;
    return LO;
}

unsigned char* seperate_4(int in){
    unsigned char* toChar = new unsigned char[4];//動態取的記憶體，如果使用toChar[4]的話，這個function call用完就沒了
    int a = in, b = in, c = in, d = in;
    
    a = a >> 24;
    
    b = b << 8;
    b = b >> 24;
    
    c = c << 16;
    c = c >> 24;
    
    d = d << 24;
    d = d >> 24;
    
    toChar[0] = (char)a;//最左
    toChar[1] = (char)b;
    toChar[2] = (char)c;
    toChar[3] = (char)d;//最右
    
    return toChar;
}

unsigned char* seperate_2(int in){
    unsigned char* toChar = new unsigned char[2];//動態取的記憶體，如果使用toChar[4]的話，這個function call用完就沒了
    short a = (short)in, b = (short)in;//自動cut掉前面
    
    a = a >> 8;
    
    b = b << 8;
    b = b >> 8;
    
    toChar[0] = (char)a;
    toChar[1] = (char)b;
    
    return toChar;
}

int writeToRegZero(int reg){
    return (reg == 0);
}

int numOverflow(int a, int b){//++ -> - // -- -> +
    return (((a > 0) && (b > 0) && (a + b <= 0)) || ((a < 0) && (b < 0) && (a + b >= 0)));
}

int overwriteHiLoReg(int HI, int LO){
    return ((HI == 0) || (LO == 0));
}

int memoAdOverflow(int ad, int num){
    return (numOverflow(ad, num)) ? 1 : ((ad + num > 1024 ) || (ad < 0));
}

int dataMisaligned(int ad, int num){
    return (ad % num != 0);
}
