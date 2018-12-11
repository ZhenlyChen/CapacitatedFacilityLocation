#include <cstdio>
#include <iostream>
#include "greed.hpp"
using namespace std;

int main(int argc, char const *argv[]) {
  int facilityCount, customerCount;
  cin >> facilityCount >> customerCount;
  int facility[facilityCount][2];
  for (int i = 0; i < facilityCount; i++) {
    cin >> facility[i][0] >> facility[i][1];
  }
  int customerDemand[customerCount];
  for (int i = 0; i < customerCount; i++) {
    scanf("%d.", &customerDemand[i]);
  }
  int customerCost[customerCount][facilityCount];
  for (int i = 0; i < customerCount; i++) {
    for (int j = 0; j < facilityCount; j++) {
      scanf("%d.", &customerCost[i][j]);
    }
  }
  hello();


  return 0;
}
