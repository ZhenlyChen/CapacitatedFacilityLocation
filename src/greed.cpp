#include <cstring>
#include <functional>
#include <iostream>
using namespace std;

static size_t minCost;
static int* minFacilityState;
static int* minCustomerState;
static int totalCustomerDemand;

namespace greedyFunc {
  void makeState(int* state, int current, int num, function<void(int*)> func);
  void EstimatedCost(int facilityCount, int customerCount, int facility[][2],
                   int customerDemand[], int** customerCost,
                   int* facilityState);
}

void greedyFunc::makeState(int* state, int current, int num, function<void(int*)> func) {
  if (current == num) {
    // int* newState = new int[num];
    // memcpy(newState, state, num * sizeof(int));
    func(state);
    // delete newState;
  } else {
    state[current] = 0;
    makeState(state, current + 1, num, func);
    state[current] = 1;
    makeState(state, current + 1, num, func);
  }
}

void greedyFunc::EstimatedCost(int facilityCount, int customerCount, int facility[][2],
                   int customerDemand[], int** customerCost,
                   int* facilityState) {
  int totalCap = 0;
  size_t currentCost = 0;
  int currentCap[facilityCount];
  for (int i = 0; i < facilityCount; i++) {
    currentCap[i] = facilityState[i] * facility[i][0];
    currentCost += facilityState[i] * facility[i][1];
    totalCap += currentCap[i];
  }
  if (totalCap < totalCustomerDemand) {
    return;
  }
  int customerState[customerCount];
  memset(customerState, 0, customerCount * sizeof(int));
  for (int i = 0; i < customerCount; i++) {
    int minIndex = -1;
    for (int j = 0; j < facilityCount; j++) {
      if (currentCap[j] - customerDemand[i] > 0) {
        if (minIndex == -1) {
          minIndex = j;
        } else if (customerCost[i][minIndex] > customerCost[i][j]) {
          minIndex = j;
        }
      }
    }
    if (minIndex == -1) {
      return;
    } else {
      currentCost += customerCost[i][minIndex];
      currentCap[minIndex] -= customerDemand[i];
      customerState[i] = minIndex;
    }
  }
  if (currentCost < minCost) {
    minCost = currentCost;
    memcpy(minFacilityState, facilityState, facilityCount * sizeof(int));
    memcpy(minCustomerState, customerState, customerCount * sizeof(int));
  }
}

void greed(int facilityCount, int customerCount, int facility[][2],
           int customerDemand[], int** customerCost) {
  minCost = -1;
  minFacilityState = new int[facilityCount];
  minCustomerState = new int[customerCount];

  int* facilityState = new int[facilityCount];
  memset(facilityState, 0, facilityCount * sizeof(int));

  // 总需求
  int totalCustomerDemand = 0;
  for (int i = 0; i < customerCount; i++) {
    totalCustomerDemand += customerDemand[i];
  }

  greedyFunc::makeState(facilityState, 0, facilityCount, [&](int* facilityState) -> void {
    greedyFunc::EstimatedCost(facilityCount, customerCount, facility, customerDemand,
                  customerCost, facilityState);
  });

  cout << minCost << endl;
  for (int i = 0; i < facilityCount; i++) {
    cout << minFacilityState[i] << " ";
  }
  cout << endl;
  for (int i = 0; i < customerCount; i++) {
    cout << minCustomerState[i] << " ";
  }
  cout << endl;
  delete minFacilityState;
  delete minCustomerState;
}
