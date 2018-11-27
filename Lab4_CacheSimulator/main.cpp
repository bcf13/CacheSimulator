//
//  main.cpp
//  Lab4_CacheSimulator
//
//  Created by Benjamin C Flowers on 11/20/18.
//  Copyright © 2018 Benjamin C Flowers. All rights reserved.
//

#include <iostream>
#include "ParseArgs.hpp"
#include "CacheManager.hpp"
#include "CacheSection.hpp"

#define INTERVAL 100000


int main(int argc, const char * argv[]) {
    // insert code here...
    
    CacheConfig cacheConfig;
    DineroMatrix dineroMatrix;

    ParseArgs(cacheConfig,dineroMatrix);
    
    CacheManager cacheManager(cacheConfig,dineroMatrix);
    
    int instructionIndex = 0;
    while (cacheManager.ProcessInstruction())
    {
        if ((instructionIndex+1) % INTERVAL ==0)
        {
            std::cout<<instructionIndex<<std::endl;
            cacheManager.PrintStats();
        }
        
        instructionIndex++;
    }
    
    cacheManager.PrintStats(); 
    
    return 0;
}

