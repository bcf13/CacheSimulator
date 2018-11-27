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


int main(int argc, const char * argv[]) {
    // insert code here...
    
    CacheConfig cacheConfig;
    DineroMatrix dineroMatrix;

    ParseArgs(cacheConfig,dineroMatrix);
    
    CacheManager cacheManager(cacheConfig,dineroMatrix);
    
    while (cacheManager.ProcessInstruction())
    {
        
    }
    
    cacheManager.PrintStats(); 
    
    return 0;
}

