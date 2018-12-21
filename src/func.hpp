#ifndef FUNC_HPP
#define FUNC_HPP

void greedy(int facilityCount, int customerCount, int facility[][2],
            int customerDemand[], int** customerCost);
void hillClimbing(int facilityCount, int customerCount, int facility[][2],
                  int customerDemand[], int** customerCost);
void GA(int facilityCount, int customerCount, int facility[][2],
        int customerDemand[], int** customerCost);
void SA(int facilityCount, int customerCount, int facility[][2],
        int customerDemand[], int** customerCost);
#endif
