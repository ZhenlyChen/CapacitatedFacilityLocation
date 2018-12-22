#include <chrono>
#include <cstdio>
#include <iostream>
#include "func.hpp"
using namespace std;
using namespace chrono;

int main(int argc, char const* argv[]) {
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
  int** customerCost = new int*[customerCount];
  for (int i = 0; i < customerCount; i++) {
    customerCost[i] = new int[facilityCount];
  }
  for (int j = 0; j < facilityCount; j++) {
    for (int i = 0; i < customerCount; i++) {
      scanf("%d.", &customerCost[i][j]);
    }
  }
  auto start = system_clock::now();
  // greed(facilityCount, customerCount, facility, customerDemand, customerCost);
  hillClimbing(facilityCount, customerCount, facility, customerDemand, customerCost);
  // randomGreedy(facilityCount, customerCount, facility, customerDemand, customerCost);
  // SA(facilityCount, customerCount, facility, customerDemand, customerCost);
  auto end = system_clock::now();
  auto duration = duration_cast<microseconds>(end - start);
  cout << "Spend: "
       << double(duration.count()) * microseconds::period::num /
              microseconds::period::den
       << "s" << endl;

  // start = system_clock::now();
  // randomGreedy(facilityCount, customerCount, facility, customerDemand,
  // customerCost); end = system_clock::now(); duration =
  // duration_cast<microseconds>(end - start); cout << "Spend: "
  //      << double(duration.count()) * microseconds::period::num /
  //             microseconds::period::den
  //      << "s" << endl;

  return 0;
}
