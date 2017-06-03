#ifndef CMP_hpp
#define CMP_hpp

#include <stdio.h>
class TLB{
public:
    TLB(){}
    TLB(int TLB_entry);
    bool checkTLB(int VPN);
    int getPPN(int VPN);
    void TLB_Update(int VPN, int PPN, int cycle, int mode);
    void HIT();
    void MISS();
    int showHIT();
    int showMISS();
private:
    bool *validBit;
    int *VPN;
    int *PPN;
    int *LUC;
    int size;
    int hitNum;
    int missNum;
};

class PTE{
public:
    PTE(){}
    PTE(int PTE_entry);
    bool checkPTE(int VPN);
    int getPPN(int VPN);
    void PTE_Update(int VPN, int PPN, int mode);
    void HIT();
    void MISS();
    int showHIT();
    int showMISS();
private:
    bool *validBit;
    int *PPN;
    int size;
    int hitNum;
    int missNum;
};

class Cache{
public:
    Cache(){}
    Cache(int Cache_entry, int setNum);
    bool checkCache(int PPN, int Page_offset, int pageSize, int blockSize, int setNum);
    void Cache_Update(int PPN, int Page_offset, int pageSize, int blockSize, int setNum, int mode);
    void HIT();
    void MISS();
    int showHIT();
    int showMISS();
private:
    bool **validBit;
    bool **MRU;
    int **tag;
    int size;
    int hitNum;
    int missNum;
};

class Memory{
public:
    Memory(){}
    Memory(int Memory_entry);
    int Memory_Update(int VPN, int cycle);
    bool isFull();
private:
    bool *validBit;
    int *LUC;
    int size;
};

class CMP{
public:
    CMP(){}
    CMP(int memorySize, int pageSize, int cacheSize, int blockSize, int setNum);
    void checkCMP(int VA, int cycle);
    int show_TLB_HIT();
    int show_TLB_MISS();
    int show_PTE_HIT();
    int show_PTE_MISS();
    int show_Cache_HIT();
    int show_Cache_MISS();
private:
    TLB TLB;
    PTE PTE;
    Cache Cache;
    Memory Memory;
    int memorySize;
    int pageSize;
    int cacheSize;
    int blockSize;
    int setNum;
};
#endif /* CMP_hpp */
