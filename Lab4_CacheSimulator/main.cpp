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
#include <vector>

#define INTERVAL 10

#define DEBUG 1
#define TEST_ID 2 // 0,2,3,10
#define REGRESSION 1


int main(int argc, const char * argv[]) {
    // insert code here...
    
    std::vector<int> test_ids={4};
    
    if (DEBUG)
        for (auto test_id : test_ids)
        {
            CacheConfig cacheConfig;
            DineroMatrix dineroMatrix;

            ParseArgs(cacheConfig,dineroMatrix,DEBUG,test_id);
            
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
            
            if (REGRESSION && DEBUG)
            {
                bool regressionResult=cacheManager.RegressionTest(test_id);
                
                std::cout<< "Regression Test # " << test_id << (regressionResult ? " PASSED" : " FAILED") << std::endl;
            }
            
        }
    
    return 0;
}

