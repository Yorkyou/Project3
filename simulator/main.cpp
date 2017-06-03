#include <iostream>
#include <fstream>
#include <iomanip>
#include "function.hpp"
#include "CMP.hpp"
using namespace std;
int main(int argc, char* argv[]) {
    int command[10] = {0};
    if(argc == 11){
        command[0]=atoi(argv[1]);
        command[1]=atoi(argv[2]);
        command[2]=atoi(argv[3]);
        command[3]=atoi(argv[4]);
        command[4]=atoi(argv[5]);
        command[5]=atoi(argv[6]);
        command[6]=atoi(argv[7]);
        command[7]=atoi(argv[8]);
        command[8]=atoi(argv[9]);
        command[9]=atoi(argv[10]);
    }
    else{
        command[0]=64;
        command[1]=32;
        command[2]=8;
        command[3]=16;
        command[4]=16;
        command[5]=4;
        command[6]=4;
        command[7]=16;
        command[8]=4;
        command[9]=1;
    }
    CMP I(command[0], command[2], command[4], command[5], command[6]);
    CMP D(command[1], command[3], command[7], command[8], command[9]);
    //===========================================================讀取參數========================
    fstream IIMAGE, DIMAGE, REPORT, SNAPSHOT;
    IIMAGE.open("/Users/huangyenju/Xcode/Archi2017_Project3/iimage.bin", ios::in | ios::binary);
    DIMAGE.open("/Users/huangyenju/Xcode/Archi2017_Project3/dimage.bin", ios::in | ios::binary);
    REPORT.open("/Users/huangyenju/Xcode/Archi2017_Project3/report.rpt", ios::out | ios::trunc);
    SNAPSHOT.open("/Users/huangyenju/Xcode/Archi2017_Project3/snapshot.rpt", ios::out | ios::trunc);
    
    int reg[32] = {0};//signed
    int pre_reg[32] = {0};//pre_reg[]用來記錄先前的版本，最後做偵測，不同則印出
    
    char pc_byte[4] = {0};//program counter的指標
    char iimageNumber_byte[4] = {0};//unsign char的意思等同於"unsigned byte!!!"
    char iimage_byte[4] = {0};
    
    char sp_byte[4] = {0};//$sp的指標
    char dimageNumber_byte[4] = {0};
    char dimage_byte[1];
    
    unsigned int pc;
    unsigned int iimageNumber;//它是一個真正的integer所以理論上有+-值，但其實宣告成unsigned int應該也無所謂 //Todo:看討論區
    unsigned int iimage[256] = {0};
    
    //不用宣告sp因為reg[29]就是sp本身
    unsigned int dimageNumber;
    unsigned char dimage[1024] = {0};
    
    //===================================以下開始讀取檔案===================================
    //fread(pc_byte, sizeof(unsigned char), 4, IIMAGE);//注意little indian and big indian的區別
    IIMAGE.read(pc_byte, 4);
    pc = (unsigned)combine_4_signed(pc_byte[0], pc_byte[1], pc_byte[2], pc_byte[3]);
    
    //fread(iimageNumber_byte, sizeof(unsigned char), 4, IIMAGE);
    IIMAGE.read(iimageNumber_byte, 4);
    iimageNumber = combine_4_signed(iimageNumber_byte[0], iimageNumber_byte[1], iimageNumber_byte[2], iimageNumber_byte[3]);
    
    for(int i = 0; i < iimageNumber; i++){
        IIMAGE.read(iimage_byte, 4);
        iimage[i] = combine_4_signed(iimage_byte[0], iimage_byte[1], iimage_byte[2], iimage_byte[3]);
    }
    
    //fread(sp_byte, sizeof(unsigned char), 4, DIMAGE);
    DIMAGE.read(sp_byte, 4);
    reg[29] = (unsigned)combine_4_signed(sp_byte[0], sp_byte[1], sp_byte[2], sp_byte[3]);
    
    //fread(dimageNumber_byte, sizeof(unsigned char), 4, DIMAGE);
    DIMAGE.read(dimageNumber_byte, 4);
    dimageNumber = (unsigned)combine_4_signed(dimageNumber_byte[0], dimageNumber_byte[1], dimageNumber_byte[2], dimageNumber_byte[3]);
    
    for(int i = 0; i < dimageNumber*4; i++){
        DIMAGE.read(dimage_byte, 1);
        dimage[i] = (unsigned)dimage_byte[0];
    }
    
    //===================================以下開始執行指令===================================
    unsigned char op = 0;
    unsigned char rs = 0;
    unsigned char rt = 0;
    unsigned char rd = 0;
    unsigned char funct = 0;
    int C = 0;
    
    long product = 0;//long = 32 bits(int = 16 bits)，乘法會用到
    
    int HI = 0;
    int pre_HI = 0;
    
    int LO = 0;
    int pre_LO = 0;
    
    int HI_accessible = 1;
    int LO_accessible = 1;
    
    int current_pc = pc;
    int diff = 0;
    int flag[6] = {0};//這個開關打開以後跳脫迴圈
    
    unsigned int unsigned_rs = 0;
    unsigned int unsigned_rt = 0;
    
    int cycle = 0;
    
    for(cycle =0; 1; cycle++){
        if(cycle == 0){
            SNAPSHOT << "cycle " << dec << cycle << endl;
            for(int i = 0;  i < 32;  i++){
                SNAPSHOT << "$" << setfill('0') << setw(2) << dec << i << ": 0x" << setfill('0') << setw(8) << hex << reg[i] << endl;
                pre_reg[i] = reg[i];
            }
            SNAPSHOT << "$HI: 0x" << setfill('0') << setw(8) << hex << HI << endl;
            SNAPSHOT << "$LO: 0x" << setfill('0') << setw(8) << hex << LO << endl;
            SNAPSHOT << "$PC: 0x" << setfill('0') << setw(8) << hex << current_pc << endl;
            SNAPSHOT << "\n\n";
        }
        else{
            SNAPSHOT << "cycle " << dec << cycle << endl;
            for(int i = 0;  i < 32;  i++){
                if(pre_reg[i] != reg[i]){
                    SNAPSHOT << "$" << setfill('0') << setw(2) << dec << i << ": 0x" << setfill('0') << setw(8) << hex << reg[i] << endl;
                    pre_reg[i] = reg[i];
                }
            }
            if(pre_HI != HI){
                SNAPSHOT << "$HI: 0x" << setfill('0') << setw(8) << hex << HI << endl;
                pre_HI = HI;
            }
            if(pre_LO != LO){
                SNAPSHOT << "$LO: 0x" << setfill('0') << setw(8) << hex << LO << endl;
                pre_LO = LO;
            }
            SNAPSHOT << "$PC: 0x" << setfill('0') << setw(8) << hex << current_pc << endl;
            SNAPSHOT << "\n\n";
        }
        
        I.checkCMP(current_pc, cycle);
        
        
        if(current_pc < (signed)pc){
            current_pc = current_pc + 4;
        }
        else{
            op = cut_op(iimage[diff]);
            switch(op){
                    //=============R type=============
                case 0x00:
                    funct = cut_funct(iimage[diff]);
                    rs = cut_rs(iimage[diff]);
                    rt = cut_rt(iimage[diff]);
                    rd = cut_rd(iimage[diff]);
                    C = cut_shamt(iimage[diff]);
                    switch(funct){
                        case 0x20://add(0, 1)
                            flag[0] = writeToRegZero(rd);
                            flag[1] = numOverflow(reg[rs], reg[rt]);
                            
                            reg[rd] = reg[rs] + reg[rt];
                            
                            if(writeToRegZero(rd)){
                                reg[rd] = 0;
                            }
                            
                            current_pc = current_pc + 4;
                            break;
                            
                        case 0x21://addu(ignore overflow)(0)
                            flag[0] = writeToRegZero(rd);
                            
                            reg[rd] = reg[rs] + reg[rt];
                            
                            if(writeToRegZero(rd)){
                                reg[rd] = 0;
                            }
                            
                            current_pc = current_pc + 4;
                            break;
                            
                        case 0x22://sub(0, 1)
                            flag[0] = writeToRegZero(rd);
                            flag[1] = numOverflow(reg[rs], (-1) * reg[rt]);
                            
                            reg[rd] = reg[rs] - reg[rt];
                            
                            if(writeToRegZero(rd)){
                                reg[rd] = 0;
                            }
                            
                            current_pc = current_pc + 4;
                            break;
                            
                        case 0x24://and(0)
                            flag[0] = writeToRegZero(rd);
                            
                            reg[rd] = reg[rs] & reg[rt];
                            
                            if(writeToRegZero(rd)){
                                reg[rd] = 0;
                            }
                            
                            current_pc = current_pc + 4;
                            break;
                            
                        case 0x25://or(0)
                            flag[0] = writeToRegZero(rd);
                            
                            reg[rd] = reg[rs] | reg[rt];
                            
                            if(writeToRegZero(rd)){
                                reg[rd] = 0;
                            }
                            
                            current_pc = current_pc + 4;
                            break;
                            
                        case 0x26://xor(0)
                            flag[0] = writeToRegZero(rd);
                            
                            reg[rd] = reg[rs] ^ reg[rt];
                            
                            if(writeToRegZero(rd)){
                                reg[rd] = 0;
                            }
                            
                            current_pc = current_pc + 4;
                            break;
                            
                        case 0x27://nor(0)
                            flag[0] = writeToRegZero(rd);
                            
                            reg[rd] = ~(reg[rs] | reg[rt]);
                            
                            if(writeToRegZero(rd)){
                                reg[rd] = 0;
                            }
                            
                            current_pc = current_pc + 4;
                            break;
                            
                        case 0x28://nand(0)
                            flag[0] = writeToRegZero(rd);
                            
                            reg[rd] = ~(reg[rs] & reg[rt]);
                            
                            if(writeToRegZero(rd)){
                                reg[rd] = 0;
                            }
                            
                            current_pc = current_pc + 4;
                            break;
                            
                        case 0x2A://slt(0)
                            flag[0] = writeToRegZero(rd);
                            
                            reg[rd] = (reg[rs] < reg[rt]) ? 1 : 0;
                            
                            if(writeToRegZero(rd)){
                                reg[rd] = 0;
                            }
                            
                            current_pc = current_pc + 4;
                            break;
                            
                        case 0x00://sll(0)()
                            flag[0] = writeToRegZero(rd);
                            
                            unsigned_rt = reg[rt];
                            reg[rd] = unsigned_rt << C;
                            
                            if(writeToRegZero(rd)){
                                reg[rd] = 0;
                            }
                            
                            if((rt == 0) && (rd == 0) && (C ==0)){
                                flag[0] = 0;
                            }
                            
                            current_pc = current_pc + 4;
                            break;
                            
                        case 0x02://srl(0)
                            flag[0] = writeToRegZero(rd);
                            
                            unsigned_rt = reg[rt];
                            reg[rd] = unsigned_rt >> C;
                            
                            if(writeToRegZero(rd)){
                                reg[rd] = 0;
                            }
                            
                            current_pc = current_pc + 4;
                            break;
                            
                        case 0x03://sra(0)
                            flag[0] = writeToRegZero(rd);
                            
                            reg[rd] = reg[rt] >> C;
                            
                            if(writeToRegZero(rd)){
                                reg[rd] = 0;
                            }
                            
                            current_pc = current_pc + 4;
                            break;
                            
                        case 0x08://jr
                            current_pc = reg[rs];
                            break;
                            
                        case 0x18://mult(本來就不會overflow)(2)
                            flag[2] = overwriteHiLoReg(HI_accessible, LO_accessible);
                            
                            product = (long)reg[rs] * (long)reg[rt];
                            HI = cut_HI(product);
                            LO = cut_LO(product);
                            
                            HI_accessible = 0;
                            LO_accessible = 0;
                            
                            current_pc = current_pc + 4;
                            break;
                            
                        case 0x19://multu(ignore overflow)(2)
                            flag[2] = overwriteHiLoReg(HI_accessible, LO_accessible);
                            
                            unsigned_rs = reg[rs];
                            unsigned_rt = reg[rt];
                            product = (long)unsigned_rs * (long)unsigned_rt;
                            HI = cut_HI(product);
                            LO = cut_LO(product);
                            
                            HI_accessible = 0;
                            LO_accessible = 0;
                            
                            current_pc = current_pc + 4;
                            break;
                            
                        case 0x10://mfhi(0)
                            flag[0] = writeToRegZero(rd);
                            
                            reg[rd] = HI;
                            
                            if(writeToRegZero(rd)){
                                reg[rd] = 0;
                            }
                            
                            HI_accessible = 1;
                            LO_accessible = 1;
                            
                            current_pc = current_pc + 4;
                            break;
                            
                        case 0x12://mflo(0)
                            flag[0] = writeToRegZero(rd);
                            
                            reg[rd] = LO;
                            
                            if(writeToRegZero(rd)){
                                reg[rd] = 0;
                            }
                            
                            HI_accessible = 1;
                            LO_accessible = 1;
                            
                            current_pc = current_pc + 4;
                            break;
                            
                        default :
                            ;
                    }
                    break;
                    
                    //=============I type=============
                case 0x08://addi(0, 1)
                    rs = cut_rs(iimage[diff]);
                    rt = cut_rt(iimage[diff]);
                    C = cut_immediate_signed(iimage[diff]);
                    flag[1] = numOverflow(reg[rs], C);
                    flag[0] = writeToRegZero(rt);
                    
                    reg[rt] = reg[rs] + C;
                    
                    if(writeToRegZero(rt)){
                        reg[rt] = 0;
                    }
                    
                    current_pc = current_pc + 4;
                    break;
                    
                case 0x09://addiu(0)//還是會overflow，只是不作回報
                    rs = cut_rs(iimage[diff]);
                    rt = cut_rt(iimage[diff]);//
                    C = cut_immediate_signed(iimage[diff]);
                    flag[0] = writeToRegZero(rt);
                    reg[rt] = reg[rs] + C;
                    
                    if(writeToRegZero(rt)){
                        reg[rt] = 0;
                    }
                    
                    current_pc = current_pc + 4;
                    break;
                    
                case 0x23://lw(0, 1, 3, 4)
                    rs = cut_rs(iimage[diff]);
                    rt = cut_rt(iimage[diff]);
                    C = cut_immediate_signed(iimage[diff]);
                    flag[1] = numOverflow(reg[rs], C);
                    flag[0] = writeToRegZero(rt);
                    flag[3] = (flag[1]) ? 1 : memoAdOverflow(reg[rs] + C, 4);
                    flag[4] = dataMisaligned(reg[rs] + C, 4);
                    
                    if((flag[3] == 1) || (flag[4] == 1)){
                        break;
                    }
                    
                    D.checkCMP(reg[rs] + C, cycle);
                    
                    reg[rt] = combine_4_signed(dimage[reg[rs] + C], dimage[reg[rs] + C + 1], dimage[reg[rs] + C + 2], dimage[reg[rs] + C + 3]);
                    
                    if(writeToRegZero(rt)){
                        reg[rt] = 0;
                    }
                    
                    current_pc = current_pc + 4;
                    break;
                    
                case 0x21://lh(0, 1, 3, 4)
                    rs = cut_rs(iimage[diff]);
                    rt = cut_rt(iimage[diff]);
                    C = cut_immediate_signed(iimage[diff]);
                    flag[1] = numOverflow(reg[rs], C);
                    flag[0] = writeToRegZero(rt);
                    flag[3] = (flag[1]) ? 1 : memoAdOverflow(reg[rs] + C, 2);
                    flag[4] = dataMisaligned(reg[rs] + C, 2);
                    
                    if((flag[3] == 1) || (flag[4] == 1)){
                        break;
                    }
                    
                    D.checkCMP(reg[rs] + C, cycle);
                    
                    reg[rt] = combine_2_signed(dimage[reg[rs] + C], dimage[reg[rs] + C + 1]);//int用意在於將2 byte轉乘4 byte
                    
                    if(writeToRegZero(rt)){
                        reg[rt] = 0;
                    }
                    
                    current_pc = current_pc + 4;
                    break;
                    
                case 0x25://lhu(0, 1, 3, 4)
                    rs = cut_rs(iimage[diff]);
                    rt = cut_rt(iimage[diff]);
                    C = cut_immediate_signed(iimage[diff]);
                    flag[1] = numOverflow(reg[rs], C);
                    flag[0] = writeToRegZero(rt);
                    flag[3] = (flag[1]) ? 1 : memoAdOverflow(reg[rs] + C, 2);
                    flag[4] = dataMisaligned(reg[rs] + C, 2);
                    
                    if((flag[3] == 1) || (flag[4] == 1)){
                        break;
                    }
                    
                    D.checkCMP(reg[rs] + C, cycle);
                    
                    reg[rt] = combine_2_unsigned(dimage[reg[rs] + C], dimage[reg[rs] + C + 1]);
                    
                    if(writeToRegZero(rt)){
                        reg[rt] = 0;
                    }
                    
                    current_pc = current_pc + 4;
                    break;
                    
                case 0x20://lb(0, 1, 3)
                    rs = cut_rs(iimage[diff]);
                    rt = cut_rt(iimage[diff]);
                    C = cut_immediate_signed(iimage[diff]);
                    flag[1] = numOverflow(reg[rs], C);
                    flag[0] = writeToRegZero(rt);
                    flag[3] = (flag[1]) ? 1 : memoAdOverflow(reg[rs] + C, 1);
                    
                    if(flag[3] == 1){
                        break;
                    }
                    
                    D.checkCMP(reg[rs] + C, cycle);
                    
                    reg[rt] = combine_1_signed(dimage[reg[rs] + C]);
                    
                    if(writeToRegZero(rt)){
                        reg[rt] = 0;
                    }
                    
                    current_pc = current_pc + 4;
                    break;
                    
                case 0x24://lbu(0, 1, 3)
                    rs = cut_rs(iimage[diff]);
                    rt = cut_rt(iimage[diff]);
                    C = cut_immediate_signed(iimage[diff]);
                    flag[1] = numOverflow(reg[rs], C);
                    flag[0] = writeToRegZero(rt);
                    flag[3] = (flag[1]) ? 1 : memoAdOverflow(reg[rs] + C, 1);
                    
                    if(flag[3] == 1){
                        break;
                    }
                    
                    D.checkCMP(reg[rs] + C, cycle);
                    
                    reg[rt] = combine_1_unsigned(dimage[reg[rs] + C]);
                    
                    if(writeToRegZero(rt)){
                        reg[rt] = 0;
                    }
                    
                    current_pc = current_pc + 4;
                    break;
                    
                case 0x2b://sw(1, 3, 4)
                    rs = cut_rs(iimage[diff]);
                    rt = cut_rt(iimage[diff]);
                    C = cut_immediate_signed(iimage[diff]);
                    flag[1] = numOverflow(reg[rs], C);
                    flag[3] = (flag[1]) ? 1 : memoAdOverflow(reg[rs] + C, 4);
                    flag[4] = dataMisaligned(reg[rs] + C, 4);
                    
                    if((flag[3] == 1) || (flag[4] == 1)){
                        break;
                    }
                    
                    D.checkCMP(reg[rs] + C, cycle);
                    
                    dimage[reg[rs] + C] = seperate_4(reg[rt])[0];
                    dimage[reg[rs] + C + 1] = seperate_4(reg[rt])[1];
                    dimage[reg[rs] + C + 2] = seperate_4(reg[rt])[2];
                    dimage[reg[rs] + C + 3] = seperate_4(reg[rt])[3];
                    
                    current_pc = current_pc + 4;
                    break;
                    
                case 0x29://sh(1, 3, 4)
                    rs = cut_rs(iimage[diff]);
                    rt = cut_rt(iimage[diff]);
                    C = cut_immediate_signed(iimage[diff]);
                    flag[1] = numOverflow(reg[rs], C);
                    flag[3] = (flag[1]) ? 1 : memoAdOverflow(reg[rs] + C, 2);
                    flag[4] = dataMisaligned(reg[rs] + C, 2);
                    
                    if((flag[3] == 1) || (flag[4] == 1)){
                        break;
                    }
                    
                    D.checkCMP(reg[rs] + C, cycle);
                    
                    dimage[reg[rs] + C] = seperate_2(reg[rt])[0];
                    dimage[reg[rs] + C + 1] = seperate_2(reg[rt])[1];
                    
                    current_pc = current_pc + 4;
                    break;
                    
                case 0x28://sb(1, 3)
                    rs = cut_rs(iimage[diff]);
                    rt = cut_rt(iimage[diff]);
                    C = cut_immediate_signed(iimage[diff]);
                    flag[1] = numOverflow(reg[rs], C);
                    flag[3] = (flag[1]) ? 1 : memoAdOverflow(reg[rs] + C, 1);
                    
                    if(flag[3] == 1){
                        break;
                    }
                    
                    D.checkCMP(reg[rs] + C, cycle);
                    
                    dimage[reg[rs] + C] = reg[rt];
                    
                    current_pc = current_pc + 4;
                    break;
                    
                case 0x0f://lui(0)
                    rs = cut_rs(iimage[diff]);
                    rt = cut_rt(iimage[diff]);
                    C = cut_immediate_unsigned(iimage[diff]);
                    flag[0] = writeToRegZero(rt);
                    
                    reg[rt] = C << 16;
                    
                    if(writeToRegZero(rt)){
                        reg[rt] = 0;
                    }
                    
                    current_pc = current_pc + 4;
                    break;
                    
                case 0x0c://andi(0)
                    rs = cut_rs(iimage[diff]);
                    rt = cut_rt(iimage[diff]);
                    C = cut_immediate_unsigned(iimage[diff]);
                    flag[0] = writeToRegZero(rt);
                    
                    reg[rt] = reg[rs] & C;
                    
                    if(writeToRegZero(rt)){
                        reg[rt] = 0;
                    }
                    
                    current_pc = current_pc + 4;
                    break;
                    
                case 0x0d://ori(0)
                    rs = cut_rs(iimage[diff]);
                    rt = cut_rt(iimage[diff]);
                    C = cut_immediate_unsigned(iimage[diff]);
                    flag[0] = writeToRegZero(rt);
                    
                    reg[rt] = reg[rs] | C;
                    
                    if(writeToRegZero(rt)){
                        reg[rt] = 0;
                    }
                    current_pc = current_pc + 4;
                    break;
                    
                case 0x0e://nori(0)
                    rs = cut_rs(iimage[diff]);
                    rt = cut_rt(iimage[diff]);
                    C = cut_immediate_unsigned(iimage[diff]);
                    flag[0] = writeToRegZero(rt);
                    
                    reg[rt] = ~(reg[rs] | C);
                    
                    if(writeToRegZero(rt)){
                        reg[rt] = 0;
                    }
                    current_pc = current_pc + 4;
                    break;
                    
                case 0x0a://slti(0)
                    rs = cut_rs(iimage[diff]);
                    rt = cut_rt(iimage[diff]);
                    C = cut_immediate_signed(iimage[diff]);
                    flag[0] = writeToRegZero(rt);
                    
                    reg[rt] = (reg[rs] < C) ? 1 : 0;
                    
                    if(writeToRegZero(rt)){
                        reg[rt] = 0;
                    }
                    
                    current_pc = current_pc + 4;
                    break;
                    
                case 0x04://beq(1)
                    rs = cut_rs(iimage[diff]);
                    rt = cut_rt(iimage[diff]);
                    C = cut_immediate_signed(iimage[diff]);
                    flag[1] = numOverflow(C * 4, 4);
                    
                    current_pc = (reg[rs] == reg[rt]) ? current_pc + C * 4 + 4 : current_pc + 4;
                    break;
                    
                case 0x05://bne(1)
                    rs = cut_rs(iimage[diff]);
                    rt = cut_rt(iimage[diff]);
                    C = cut_immediate_signed(iimage[diff]);
                    flag[1] = numOverflow(C * 4, 4);
                    
                    current_pc = (reg[rs] != reg[rt]) ? current_pc + C * 4 + 4 : current_pc + 4;
                    break;
                    
                case 0x07://bgtz(1)
                    rs = cut_rs(iimage[diff]);
                    C = cut_immediate_signed(iimage[diff]);
                    flag[1] = numOverflow(C * 4, 4);
                    
                    current_pc = (reg[rs] > 0) ? current_pc + C * 4 + 4 : current_pc + 4;
                    break;
                    
                    //=============J type=============
                case 0x02://j
                    C = cut_address(iimage[diff]);
                    C = C << 2;
                    current_pc = current_pc + 4;
                    current_pc = current_pc >> 28;
                    current_pc = current_pc << 28;
                    current_pc = current_pc | C;
                    break;
                    
                case 0x03://jal
                    C = cut_address(iimage[diff]);
                    C = C << 2;
                    current_pc = current_pc + 4;
                    reg[31] = current_pc;
                    current_pc = current_pc >> 28;
                    current_pc = current_pc << 28;
                    current_pc = current_pc | C;
                    break;
                    
                    //=============S type=============
                case 0x3F://halt
                    flag[5] = 1;
                    break;
                    
                default :
                    ;
            }
        }
        diff = (current_pc - pc) / 4;//每四個byte存一個Instruction
        /*cout << cycle << endl;
        cout << "DCache :" << endl;
        cout << "# hits : " << D.show_Cache_HIT() << endl;
        cout << "# misses :" << D.show_Cache_MISS() << endl;*/
        if(flag[0] == 1){
            flag[0] = 0;//開關還會用到需要關掉
        }
        if(flag[1] == 1){
            flag[1] = 0;
        }
        if(flag[2] == 1){
            flag[2] = 0;
        }
        if(flag[3] == 1){
            flag[5] = 1;
        }
        if(flag[4] == 1){
            flag[5] = 1;
        }
        if(flag[5] == 1){
            break;
        }
    }
    
    REPORT << "ICache :" << endl;
    REPORT << "# hits : " << I.show_Cache_HIT() << endl;
    REPORT << "# misses :" << I.show_Cache_MISS() << endl;
    REPORT << endl;
    
    REPORT << "DCache :" << endl;
    REPORT << "# hits : " << D.show_Cache_HIT() << endl;
    REPORT << "# misses :" << D.show_Cache_MISS() << endl;
    REPORT << endl;
    
    REPORT << "ITLB :" << endl;
    REPORT << "# hits : " << I.show_TLB_HIT() << endl;
    REPORT << "# misses :" << I.show_TLB_MISS() << endl;
    REPORT << endl;
    
    REPORT << "DTLB :" << endl;
    REPORT << "# hits : " << D.show_TLB_HIT() << endl;
    REPORT << "# misses :" << D.show_TLB_MISS() << endl;
    REPORT << endl;
    
    REPORT << "IPageTable :" << endl;
    REPORT << "# hits : " << I.show_PTE_HIT() << endl;
    REPORT << "# misses :" << I.show_PTE_MISS() << endl;
    REPORT << endl;
    
    REPORT << "DPageTable :" << endl;
    REPORT << "# hits : " << D.show_PTE_HIT() << endl;
    REPORT << "# misses :" << D.show_PTE_MISS() << endl;
    REPORT << endl;
    return 0;
}
