#ifndef function_hpp
#define function_hpp

#include <iostream>
//combine_signed與combine_unsigned都是將字元合成4 byte，差異在於合成什麼樣的形式。而這樣的形式將決定前面的extension bits。
unsigned int combine_4_unsigned(unsigned char a,unsigned char b,unsigned char c,unsigned char d);
int combine_4_signed(unsigned char a,unsigned char b,unsigned char c,unsigned char d);
unsigned int combine_2_unsigned(unsigned char a,unsigned char b);
int combine_2_signed(unsigned char a,unsigned char b);
unsigned int combine_1_unsigned(unsigned char a);
int combine_1_signed(unsigned char a);

unsigned char cut_op(unsigned int a);//op = 6 bits < 1 byte
unsigned char cut_rs(unsigned int a);//rs = 5 bits < 1 byte
unsigned char cut_rt(unsigned int a);//rt = 5 bits < 1 byte
unsigned char cut_rd(unsigned int a);//rd = 5 bits < 1 byte
unsigned int cut_shamt(unsigned int a);//shamt = 5 bits < 1 byte
unsigned char cut_funct(unsigned int a);//func = 6 bits < 1 byte
unsigned int cut_immediate_unsigned(unsigned int a);//immediate = 16 bits = 2 bytes
int cut_immediate_signed(unsigned int a);//immediate = 16 bits = 2 bytes
unsigned int cut_address(unsigned int a);//address = 26 bits < 4 byte

int cut_HI(long a);
int cut_LO(long a);

unsigned char* seperate_4(int in);
unsigned char* seperate_2(int in);

int writeToRegZero(int reg);
int numOverflow(int a, int b);
int overwriteHiLoReg(int HI, int LO);
int memoAdOverflow(int ad, int num);
int dataMisaligned(int ad, int num);
#endif /* function_hpp */
