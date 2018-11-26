//
//  CacheManager.cpp
//  Lab4_CacheSimulator
//
//  Created by Benjamin C Flowers on 11/25/18.
//  Copyright © 2018 Benjamin C Flowers. All rights reserved.
//

#include "CacheManager.hpp"
using namespace std; 


CacheManager::CacheManager(const CacheConfig& cacheConfig, const DineroMatrix& dineroMatrix)
    :mDineroInstructionIndex(0),
    mDineroMatrix(dineroMatrix)
{
//    int A = 2;
//    int B = 16;
//    int C = 32768;
    int A = 1;
    int B = 32;
    int C = 8192;
    
    
    Section s = {A,B,C};
    
  
    
    switch (cacheConfig.replacementAlgorithm) {
        case eReplacementAlgorithm::LRU:
        {
            mpCacheSections= new CacheSectionLRU(s);
            break;
        }
        case eReplacementAlgorithm::RND:
        {
            mpCacheSections= new CacheSectionRND(s);
            break;
        }
        case eReplacementAlgorithm::NMRU:
        {
            mpCacheSections= new CacheSectionNMRU(s);
            break;
        }
        case eReplacementAlgorithm::FIFO:
        {
            mpCacheSections= new CacheSectionFIFO(s);
            break;
        }
    }
}

CacheManager::~CacheManager()
{}

// returns whether instruction was processed
bool CacheManager::ProcessInstruction()
{
    mpCacheSections->Replace();
    
    if (mDineroInstructionIndex>mDineroMatrix.data.size()-1)
        return false;
    
    std::pair<int,int> instruction = mDineroMatrix.data[mDineroInstructionIndex];
    
    auto instructionID=std::get<0>(instruction);
    auto address=std::get<1>(instruction);
    cout<<instructionID<<","<<address<<endl;
    
    auto partitionedAddress = mpCacheSections->PartitionAddress(address);
    
    // Delegate to appropriate cache section
    auto hit = mpCacheSections->ProcessSet(partitionedAddress);
    
    switch (instructionID) {
        case 0:
            mCacheStats.readHits += (hit ? 1 : 0);
            mCacheStats.readTotal++;
            break;
            
        case 1:
            mCacheStats.writeHits += (hit ? 1 : 0);
            mCacheStats.writeTotal++;
            break;
            
        case 2:
            mCacheStats.instrFetchHits += (hit ? 1 : 0);
            mCacheStats.instrFetchTotal++;
            break;
    }
    
    mDineroInstructionIndex++;
    return true;
}

void CacheManager::PrintStats()
{
    mpCacheSections->Replace();
}

