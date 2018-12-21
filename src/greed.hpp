#ifndef GREED_HPP
#define GREED_HPP

void greed(int facilityCount, int customerCount, int facility[][2],
           int customerDemand[], int** customerCost);
void randomGreedy(int facilityCount, int customerCount, int facility[][2],
           int customerDemand[], int** customerCost);
#endif
