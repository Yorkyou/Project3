#include <algorithm>
#include <iostream>
#include "CMP.hpp"
using namespace std;
TLB::TLB(int TLB_entry){
    size = TLB_entry;
    hitNum = 0;
    missNum = 0;
    validBit = new bool[TLB_entry];
    fill(validBit, validBit + TLB_entry, 0);
    VPN = new int[TLB_entry];
    fill(VPN, VPN + TLB_entry, 0);
    PPN = new int [TLB_entry];
    fill(PPN, PPN + TLB_entry, 0);
    LUC = new int[TLB_entry];
    fill(LUC, LUC + TLB_entry, 0);
}

bool TLB::checkTLB(int VPN){
    for(int i = 0; i < size; i++){
        if((this->VPN[i] == VPN) && (validBit[i] == 1)){
            return true;
        }
    }
    return false;
}

int TLB::getPPN(int VPN){
    for(int i = 0; i < size; i++){
        if(this->VPN[i] == VPN){
            return PPN[i];
        }
    }
    return 0;
}

void TLB::TLB_Update(int VPN, int PPN, int cycle, int mode){
    int temp = 0;
    int minLUC = LUC[0];
    switch (mode) {
        case 0:
            for(int i = 0; i < size; i++){
                if(validBit[i] == 0){
                    temp = i;
                    break;
                }
                else{
                    if(LUC[i] < minLUC){
                        minLUC = LUC[i];
                        temp = i;
                    }
                }
            }
            this->validBit[temp] = 1;
            this->VPN[temp] = VPN;
            this->PPN[temp] = PPN;
            this->LUC[temp] = cycle;
            break;
            
        case 1:
            for(int i = 0; i < size; i++){
                if(this->PPN[i] == PPN){
                    validBit[i] = 0;
                }
            }
            for(int i = 0; i < size; i++){
                if(validBit[i] == 0){
                    temp = i;
                    break;
                }
                else{
                    if(LUC[i] < minLUC){
                        minLUC = LUC[i];
                        temp = i;
                    }
                }
            }
            this->validBit[temp] = 1;
            this->VPN[temp] = VPN;
            this->PPN[temp] = PPN;
            this->LUC[temp] = cycle;
            break;
            
        default:
            break;
    }
    for(int i = 0; i < size; i++){
        if(validBit[i] == 0){
            temp = i;
            break;
        }
        else{
            if(LUC[i] < minLUC){
                minLUC = LUC[i];
                temp = i;
            }
        }
    }
    this->validBit[temp] = 1;
    this->VPN[temp] = VPN;
    this->PPN[temp] = PPN;
    this->LUC[temp] = cycle;
}

void TLB::HIT(){
    hitNum++;
}

void TLB::MISS(){
    missNum++;
}

int TLB::showHIT(){
    return hitNum;
}

int TLB::showMISS(){
    return missNum;
}
//================================================================================================
PTE::PTE(int PTE_entry){
    size = PTE_entry;
    hitNum = 0;
    missNum = 0;
    validBit = new bool[PTE_entry];
    fill(validBit, validBit + PTE_entry, 0);
    PPN = new int [PTE_entry];
    fill(PPN, PPN + PTE_entry, 0);
}

bool PTE::checkPTE(int VPN){
    return (validBit[VPN] == true) ? true : false;
}

int PTE::getPPN(int VPN){
    if(validBit[VPN]){
        return PPN[VPN];
    }
    else{
        return 0;
    }
}

void PTE::PTE_Update(int VPN, int PPN, int mode){
    switch (mode) {
        case 0:
            this->validBit[VPN] = 1;
            this->PPN[VPN] = PPN;
            break;
        case 1:
            for(int i = 0; i < size; i++){
                if(this->PPN[i] == PPN){
                    validBit[i] = 0;
                }
            }
            this->validBit[VPN] = 1;
            this->PPN[VPN] = PPN;
            break;
        default:
            break;
    }
}

void PTE::HIT(){
    hitNum++;
}

void PTE::MISS(){
    missNum++;
}

int PTE::showHIT(){
    return hitNum;
}

int PTE::showMISS(){
    return missNum;
}
//================================================================================================
Cache::Cache(int Cache_entry, int setNum){
    size = Cache_entry;
    hitNum = 0;
    missNum = 0;
    validBit = new bool*[Cache_entry];
    for(int i = 0; i < Cache_entry; i++){
        validBit[i] = new bool[setNum];
        fill(validBit[i], validBit[i] + setNum, false);
    }
    tag = new int*[Cache_entry];
    for(int i = 0; i < Cache_entry; i++){
        tag[i] = new int[setNum];
        fill(tag[i], tag[i] + setNum, 0);
    }
    MRU = new bool*[Cache_entry];
    for(int i = 0; i < Cache_entry; i++){
        MRU[i] = new bool[setNum];
        fill(MRU[i], MRU[i] + setNum, false);
    }
}

bool Cache::checkCache(int PPN, int Page_offset, int pageSize, int blockSize, int setNum){
    int MRU_zeroNum = 0;
    int firstZero = 0;
    int blockNum = (PPN * pageSize + Page_offset) / blockSize;
    int index = blockNum % size;
    int tag = blockNum / size;
    for(int i = 0; i < setNum; i++){
        if(MRU[index][i] == 0){
            if(MRU_zeroNum == 0){
                firstZero = i;
            }
            MRU_zeroNum++;
        }
    }
    for(int i = 0; i < setNum; i++){
        if((this->tag[index][i] == tag) && (validBit[index][i] == 1)){
            if((firstZero == i) && (MRU_zeroNum == 1)){
                fill(MRU[index], MRU[index] + setNum, false);
            }
            MRU[index][i] = true;
            return true;
        }
    }
    return false;
}

void Cache::Cache_Update(int PPN, int Page_offset, int pageSize, int blockSize, int setNum, int mode){
    int temp;
    int MRU_zeroNum;
    int firstZero;
    int blockNum;
    int index;
    int tag;
    switch (mode) {
        case 0:
            temp = -1;
            MRU_zeroNum = 0;
            firstZero = 0;
            blockNum = (PPN * pageSize + Page_offset) / blockSize;
            index = blockNum % size;
            tag = blockNum / size;
            for(int i = 0; i < setNum; i++){
                if(validBit[index][i] == 0){
                    temp = i;
                }
            }
            for(int i = 0; i < setNum; i++){
                if(MRU[index][i] == false){
                    if(MRU_zeroNum == 0){
                        firstZero = i;
                    }
                    MRU_zeroNum++;
                }
            }
            if(MRU_zeroNum == 1){
                if(temp == firstZero){
                    fill(MRU[index], MRU[index] + setNum, false);
                }
                MRU[index][temp] = true;
                validBit[index][temp] = 1;
                this->tag[index][temp] = tag;
            }
            else{
                MRU[index][firstZero] = true;
                validBit[index][firstZero] = 1;
                this->tag[index][firstZero] = tag;
            }
            break;
            
        case 1:
            blockNum = 0;
            index = 0;
            tag = 0;
            for(int i = 0; i < pageSize; i++){
                blockNum = (PPN * pageSize + i) / blockSize;
                index = blockNum % size;
                tag = blockNum / size;
                for(int j = 0; j < setNum; j++){
                    if((this->tag[index][j] == tag) && (validBit[index][i] == 1)){
                        validBit[index][i] = 0;
                        MRU[index][i] = 0;
                    }
                }
            }
            break;
            
        default:
            break;
    }
}

void Cache::HIT(){
    hitNum++;
}

void Cache::MISS(){
    missNum++;
}

int Cache::showHIT(){
    return hitNum;
}

int Cache::showMISS(){
    return missNum;
}
//================================================================================================
Memory::Memory(int Memory_entry){
    size = Memory_entry;
    validBit = new bool[Memory_entry];
    fill(validBit, validBit+Memory_entry, 0);
    LUC = new int[Memory_entry];
    fill(LUC, LUC+Memory_entry, 0);
}

int Memory::Memory_Update(int VPN, int cycle){
    int state = 0;
    int minLUC = LUC[0];
    int PPN = 0;
    for(int i = 0; i < size; i++){
        if(validBit[i] == 0){
            PPN = i;
            state = 1;
            break;
        }
        else{
            if(LUC[i] < minLUC){
                minLUC = LUC[i];
                PPN = i;
            }
        }
    }
    validBit[PPN] = 1;
    LUC[PPN] = cycle;
    return PPN;
}

bool Memory::isFull(){
    for(int i = 0; i < size; i++){
        if(validBit[i] == 0){
            return false;
        }
    }
    return true;
}
//================================================================================================
CMP::CMP(int memorySize, int pageSize, int cacheSize, int blockSize, int setNum){
    int PTE_entry = 1024 / pageSize;
    int TLB_entry = PTE_entry / 4;
    int Cache_entry = cacheSize / blockSize / setNum;
    int Memory_entry = memorySize / pageSize;
    TLB = *new TLB::TLB(TLB_entry);
    PTE = *new PTE::PTE(PTE_entry);
    Cache = *new Cache::Cache(Cache_entry, setNum);
    Memory = *new Memory::Memory(Memory_entry);
    this->memorySize = memorySize;
    this->pageSize = pageSize;
    this->cacheSize = cacheSize;
    this->blockSize = blockSize;
    this->setNum = setNum;
}

void CMP::checkCMP(int VA, int cycle){
    int PPN = 0;
    int VPN = VA / pageSize;
    int Page_offset = VA % pageSize;
    if(TLB.checkTLB(VPN)){//TLB hit（情況1）
        TLB.HIT();
        PPN = TLB.getPPN(VPN);
    }
    else{//TLB miss
        TLB.MISS();
        if(PTE.checkPTE(VPN)){//PTE hit（情況2）
            PTE.HIT();
            PPN = PTE.getPPN(VPN);
            TLB.TLB_Update(VPN, PPN, cycle, 0);
        }
        else{//PTE miss（情況3）
            PTE.MISS();
            PPN = Memory.Memory_Update(VPN, cycle);
            if(!Memory.isFull()){//state == （mem還有空位直接放）
                PTE.PTE_Update(VPN, PPN, 0);
                TLB.TLB_Update(VPN, PPN, cycle, 0);
            }
            else{//原本的PPN要清空
                PTE.PTE_Update(VPN, PPN, 1);
                TLB.TLB_Update(VPN, PPN, cycle, 1);
                Cache.Cache_Update(PPN, Page_offset, pageSize, blockSize, setNum, 1);
            }
        }
    }
    if(Cache.checkCache(PPN, Page_offset, pageSize, blockSize, setNum)){
        Cache.HIT();
    }
    else{
        Cache.MISS();
        Cache.Cache_Update(PPN, Page_offset, pageSize, blockSize, setNum, 0);
    }
}

int CMP::show_TLB_HIT(){
    return TLB.showHIT();
}

int CMP::show_TLB_MISS(){
    return TLB.showMISS();
}

int CMP::show_PTE_HIT(){
    return PTE.showHIT();
}

int CMP::show_PTE_MISS(){
    return PTE.showMISS();
}

int CMP::show_Cache_HIT(){
    return Cache.showHIT();
}

int CMP::show_Cache_MISS(){
    return Cache.showMISS();
}
//================================================================================================
